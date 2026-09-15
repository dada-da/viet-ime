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

#define VIETIME_ABI_VERSION 1
#define VIETIME_MAX_TEXT_BYTES 97
/* Trần độ dài preedit, tính bằng codepoint. Chạm trần thì libvietime
 * commit phần đang gõ dở và nhường phím gây tràn lại cho ứng dụng — xem
 * REQUIRE áp dụng kết quả bên dưới. Hệ quả: đúng một phím đi thẳng về
 * ứng dụng không qua bộ biến đổi, nên "aa" nằm hai bên ranh giới đó
 * không hợp thành "â".
 *
 * Phím nằm ngoài 0x20..0x7E (Enter, Tab, Escape, chữ không phải ASCII)
 * đi cùng đường đó: có preedit thì commit rồi nhường phím; preedit rỗng
 * thì chỉ nhường phím (mọi trường bằng 0, key_consumed = 0).
 *
 * Hệ số 3 trong static_assert ở ime_api.cpp là số byte UTF-8 tối đa của
 * một ký tự tiếng Việt NFC (ví dụ 'ệ' U+1EC7). */
#define VIETIME_MAX_CODE_POINT 32

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
    VIETIME_INVALID_KEY = -3,
  } VietimeErrorCode;

  /* YÊU CẦU ÁP DỤNG KẾT QUẢ — đọc trước khi viết wrapper.
   *
   * Mỗi lần gọi có thể yêu cầu function làm tối đa ba việc. Chúng phải
   * được làm ĐÚNG THỨ TỰ NÀY:
   *
   *   1. xoá lùi `backspace_count` codepoint
   *   2. chèn `text_length` byte đầu của `text`
   *   3. nếu `key_consumed == 0`: trả phím thô về cho ứng dụng tự xử lý
   *
   * Bước 2 và bước 3 CÙNG XẢY RA trong hai trường hợp, khi preedit
   * không rỗng: preedit chạm trần VIETIME_MAX_CODE_POINT, hoặc phím nằm
   * ngoài 0x20..0x7E. libvietime commit phần đang gõ dở rồi nhường
   * phím đó lại. Làm bước 3 trước bước 2 sẽ cho chuỗi đảo thứ tự, và
   * không có mã lỗi nào báo — gõ 32 chữ rồi 'x' sẽ ra "x" đứng trước 32
   * chữ đó.
   *
   * ĐỪNG viết `if (!key_consumed) return;` — nhánh đó bỏ mất
   * `text` đi kèm.
   *
   * Muốn một phím (ví dụ Escape) HUỶ chữ đang gõ thay vì commit: gọi
   * vietime_reset TRƯỚC, rồi mới đưa phím đó vào vietime_process_key.
   *
   * Phím ranh giới từ (dấu cách, dấu câu) khi preedit rỗng cũng chỉ nhường phím; khi có preedit thì libvietime nhận phím và text = preedit + phím đó.
   */
  typedef struct
  {
    /* Số ký tự wrapper phải xoá lùi trước khi chèn `text`.
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
     *
     * backspace_count có nghĩa với MỌI kết quả, kể cả khi text_committed = 1.
     * Nó luôn là số codepoint do libvietime sinh ra trước con trỏ cần xoá
     * trước khi chèn `text`.
     *
     * Wrapper nào ÁP DỤNG trường này phụ thuộc vào mô hình gửi chữ:
     *
     *   IBus, TSF  — BỎ QUA. Nền tảng tự dọn preedit; wrapper chỉ chọn giữa
     *                update_preedit_text (text_committed = 0) và
     *                commit_text (text_committed = 1).
     *   SendInput  — PHẢI dùng. Preedit cũ đang nằm thật trong tài liệu và
     *                không ai xoá hộ.
     *
     * DẠNG ĐẦY ĐỦ: backspace_count luôn bằng TOÀN BỘ số codepoint
     * libvietime đang có trước con trỏ, và `text` luôn là chuỗi ĐẦY ĐỦ
     * (preedit mới, hoặc chuỗi commit). libvietime không bao giờ trả phần
     * chênh lệch. Wrapper SendInput muốn gửi ít phím hơn thì tự so chuỗi
     * cũ với `text` và chỉ gửi phần đuôi khác nhau.
     *
     * Trường này không bao giờ đếm sang chữ có sẵn của ứng dụng.
     */
    size_t backspace_count;
    char text[VIETIME_MAX_TEXT_BYTES]; // mã hoá UTF-8, dài 97 bytes nhưng thực tế chỉ có 96, chừa lại 1 byte cho NUL-terminate
    size_t text_length;                // đếm byte
    int32_t error;
    int32_t text_committed; // 1 là rồi 0 là chưa
    int32_t key_consumed;   // 1 là rồi 0 là chưa
  } VietimeKeyResult;

  typedef struct vietime_ctx vietime_ctx;

  VIETIME_API vietime_ctx *vietime_create(void);
  VIETIME_API int32_t vietime_destroy(vietime_ctx *ctx);
  VIETIME_API VietimeKeyResult vietime_process_key(vietime_ctx *ctx, uint32_t c);
  VIETIME_API VietimeKeyResult vietime_reset(vietime_ctx *ctx); // bỏ đi toàn bộ ký tự trong preedit chứ không commit, backspace_count là số ký tự cần xoá. Muốn giữ lại text thì gọi vietime_flush trước
  VIETIME_API int32_t vietime_set_method(vietime_ctx *ctx, VietimeInputMethod m);
  VIETIME_API int32_t vietime_set_tone_placement(vietime_ctx *ctx, VietimeTonePlacement p);
  VIETIME_API uint32_t vietime_get_version(void);
  VIETIME_API VietimeKeyResult vietime_backspace(vietime_ctx *ctx); // xoá một ký tự trong preedit
  VIETIME_API VietimeKeyResult vietime_flush(vietime_ctx *ctx);     // trả về phần chưa commit trước khi reset

#ifdef __cplusplus
}
#endif

#endif