// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <algorithm>
#include <iterator>  
#include "rime_table.h"

namespace vietime
{
  namespace
  {
    // Cụm nguyên âm tiếng Việt (âm đệm + âm chính + âm cuối phần nguyên âm).
    // KHÔNG gồm phụ âm cuối.
    // xem tests/test_rime_table.cpp.
    const std::u32string_view kVowelClusters[] = {
        // 1 ký tự
        U"a", U"ă", U"â", U"e", U"ê", U"i",
        U"o", U"ô", U"ơ", U"u", U"ư", U"y",

        // 2 ký tự
        U"ai", U"ao", U"au", U"ay",
        U"âu", U"ây",
        U"eo",
        U"êu",
        U"ia", U"iê", U"iu",
        U"oa", U"oă", U"oe", U"oi", U"oo",
        U"ôi",
        U"ơi",
        U"ua", U"uâ", U"uê", U"ui", U"uô", U"uơ", U"uy",
        U"ưa", U"ưi", U"ươ", U"ưu",
        U"yê",

        // 3 ký tự
        U"iêu", U"oai", U"oay", U"oeo",
        U"uây", U"uôi", U"uya", U"uyê", U"uyu",
        U"ươi", U"ươu", U"yêu",
    };
  } // namespace

  bool is_vowel_cluster_prefix(std::u32string_view cluster)
  {
    if (cluster.empty())
      return true;

    auto valid_vowel_cluster = std::find(std::begin(kVowelClusters), std::end(kVowelClusters), cluster);

    return valid_vowel_cluster != std::end(kVowelClusters);
  }
} // namespace vietime