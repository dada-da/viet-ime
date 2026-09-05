#include <string>
#include "key_processor.h"
#include "check.h"

static void check_typing(const char *keys, const std::string &want)
{
  KeyProcessor kp;
  for (const char *k = keys; *k; ++k)
    kp.handle_key(*k);
  check_str(kp.preedit(), want, std::string("go \"") + keys + "\"");
}

void run_pipeline_tests()
{
  check_typing("nguwowif", "người");
  check_typing("nguoiwf", "người");
  check_typing("tieengs", "tiếng");
  check_typing("tienges", "tiếng");
  check_typing("toans", "toán");
  check_typing("tosan", "toán"); // thanh gõ giữa chừng
  check_typing("hoaf", "hòa");
  check_typing("cuar", "của");
  check_typing("quas", "quá"); // không được ra "qúa"
  check_typing("dduwowcj", "được");
  check_typing("dduocwj", "được");
  check_typing("tuooir", "tuổi");
  check_typing("toansj", "toạn"); // thanh mới đè thanh cũ
  check_typing("toanss", "toan"); // gõ lại = bỏ dấu
  check_typing("ngoaif", "ngoài");
  check_typing("khoongs", "khống"); // oo tìm ngược, không đụng luật cặp
  check_typing("quoocs", "quốc");   // ngoại lệ qu: u ở âm đầu, oo áp lên o
  check_typing("", "");
}