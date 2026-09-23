// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "check.h"
#include "rime_table.h"

#include <string>
#include <string_view>

namespace
{
  // Cụm phải được chấp nhận. Gồm cả trạng thái gõ dở (`uy` trong `nguyeen`).
  const std::u32string_view kAccept[] = {
      U"a", U"ă", U"â", U"e", U"ê", U"i", U"o", U"ô", U"ơ", U"u", U"ư", U"y",
      U"ai", U"ao", U"au", U"ay", U"âu", U"ây", U"eo", U"êu",
      U"ia", U"iê", U"iu", U"oa", U"oă", U"oe", U"oi", U"ôi", U"ơi",
      U"ua", U"uâ", U"uê", U"ui", U"uô", U"uy", U"ưa", U"ưi", U"ươ", U"ưu",
      U"yê", U"iêu", U"oai", U"oay", U"uây", U"uôi", U"uya", U"uyê",
      U"ươi", U"ươu", U"yêu",
  };

  // Cụm phải bị từ chối. Mỗi dòng kèm lý do vì sao nó tới được đây.
  const std::u32string_view kReject[] = {
      U"ae",   // `aeae`: biến `a` thành `â` cho ra `âe`, phải hoàn tác
      U"âe",
      U"âê",   // `aaee`
      U"ea",
      U"uă",   // `muaw`: ứng viên gần nhất sai, phải quét tiếp để ra `ưa`
      U"ăa",
      U"êo",   // khác `eo`
      U"ôa",
      U"ưô",
      U"ooo",
      U"iee",  // dạng thô giữa chừng, chưa biến
      U"uoi",
  };

  // Bảng đóng với tiền tố: mọi tiền tố của mục được chấp nhận cũng phải được
  // chấp nhận. Nhờ vậy thân hàm chỉ cần kiểm thành viên.
  void check_prefix_closed()
  {
    for (std::u32string_view s : kAccept)
    {
      for (std::size_t n = 1; n < s.size(); ++n)
      {
        std::u32string_view p = s.substr(0, n);
        check_eq(vietime::is_vowel_cluster_prefix(p), true,
                 "prefix-closed: tien to cua mot cum hop le");
      }
    }
  }
} // namespace

void test_rime_table()
{
  for (std::u32string_view s : kAccept)
    check_eq(vietime::is_vowel_cluster_prefix(s), true, "accept");

  for (std::u32string_view s : kReject)
    check_eq(vietime::is_vowel_cluster_prefix(s), false, "reject");

  check_eq(vietime::is_vowel_cluster_prefix(U""), true, "rong: chua co nguyen am");

  check_prefix_closed();
}