// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstdio>
#include <cstring>

#include "ime_api.h"

int main(int argc, char **argv)
{
  int arg = 1;
  VietimeInputMethod method = VIETIME_METHOD_TELEX;

  if (arg < argc && std::strcmp(argv[arg], "--vni") == 0)
  {
    method = VIETIME_METHOD_VNI;
    arg++;
  }

  if (arg >= argc)
  {
    std::fprintf(stderr, "dung: type_keys [--vni] <chuoi_phim>\n");
    return 2;
  }

  vietime_ctx *ctx = vietime_create();
  if (ctx == nullptr)
  {
    std::fprintf(stderr, "vietime_create that bai\n");
    return 1;
  }
  vietime_set_method(ctx, method);

  const char *keys = argv[arg];
  std::printf("phim: %s\n", keys);

  for (const char *p = keys; *p != '\0'; p++)
  {
    VietimeKeyResult r = vietime_process_key(ctx, static_cast<uint32_t>(
                                                      static_cast<unsigned char>(*p)));
    std::printf("  '%c' -> bs=%zu committed=%d consumed=%d text=[%.*s]\n",
                *p, r.backspace_count, r.text_committed, r.key_consumed,
                static_cast<int>(r.text_length), r.text);
  }

  VietimeKeyResult f = vietime_flush(ctx);
  std::printf("flush -> [%.*s]\n", static_cast<int>(f.text_length), f.text);

  vietime_destroy(ctx);
  return 0;
}