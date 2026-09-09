// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VIETIME_IME_API_H
#define VIETIME_IME_API_H

#if defined(_WIN32) || defined(__CYGWIN__)
#ifdef VIETIME_EXPORTS
#define VIETIME_API __declspec(dllexport)
#else
#define VIETIME_API __declspec(dllimport)
#endif

#else
#if defined(__GNUC__) && __GNUC__ >= 4
#define VIETIME_API __attribute__((visibility("default")))
#else
#define VIETIME_API
#endif
#endif

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define VIETIME_MAX_TEXT_BYTES 64
#define VIETIME_ABI_VERSION 1

  typedef enum
  {
    VIETIME_METHOD_TELEX,
    VIETIME_METHOD_VNI,
  } VietimeInputMethod;

  typedef enum
  {
    VIETIME_PLACEMENT_CLASSIC,
    VIETIME_PLACEMENT_MODERN,
  } VietimeTonePlacement;

  typedef enum
  {
    VIETIME_OK = 0,
    VIETIME_NULL_POINTER = -1,
    VIETIME_UNKNOWN = -2,
  } VietimeErrorCode;

  typedef struct
  {
    /* Số ký tự người gọi phải xoá lùi trước khi chèn `text`.
     *
     * ĐƠN VỊ: codepoint Unicode — KHÔNG phải byte, KHÔNG phải cluster.
     *
     * Trên Windows con số này dùng thẳng được cho UTF-16: mọi ký tự tiếng
     * Việt đều nằm dưới U+FFFF (cao nhất là 'ỹ' U+1EF9), kể cả dạng tổ hợp
     * NFD, nên không có cặp thay thế (surrogate pair) và 1 codepoint luôn
     * bằng 1 UTF-16 code unit.
     *
     * ĐIỀU KIỆN: trường này chỉ đếm những ký tự
     * do chính libvietime sinh ra ở các lần gọi trước và vẫn còn nguyên
     * trước con trỏ. Nó không bao giờ được đếm sang chữ có sẵn của ứng
     * dụng. Nếu một codepoint >= U+10000 (emoji, chữ Hán) lọt vào
     * phần được đếm, Windows sẽ xoá lẻ nửa cặp thay thế và để lại một
     * code unit lẻ trong text của người dùng.
     */
    size_t backspace_count;
    char text[VIETIME_MAX_TEXT_BYTES]; // mã hoá UTF-8, dài 64 bytes nhưng thực tế chỉ có 63, chừa lại 1 byte cho NUL-terminate
    size_t text_length;                // đếm byte
    int32_t error;
    int32_t text_committed; // 1 là rồi 0 là chưa
    int32_t key_consumed;   // 1 là rồi 0 là chưa
  } VietimeKeyResult;

  typedef struct vietime_ctx vietime_ctx;

  VIETIME_API vietime_ctx *vietime_create(void);
  VIETIME_API void vietime_destroy(vietime_ctx *ctx);
  VIETIME_API VietimeKeyResult vietime_process_key(vietime_ctx *ctx, uint32_t c);
  VIETIME_API void vietime_reset(vietime_ctx *ctx);
  VIETIME_API void vietime_set_method(vietime_ctx *ctx, VietimeInputMethod m);
  VIETIME_API void vietime_set_tone_placement(vietime_ctx *ctx, VietimeTonePlacement p);
  VIETIME_API uint32_t vietime_get_version(void);
  VIETIME_API VietimeKeyResult vietime_backspace(vietime_ctx *ctx); // xoá một ký tự trong preedit
  VIETIME_API VietimeKeyResult vietime_flush(vietime_ctx *ctx);     // trả về phần chưa commit trước khi reset

#ifdef __cplusplus
}
#endif

#endif