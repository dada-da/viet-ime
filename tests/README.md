# viet-ime — Tests

Bộ test cho `ime_core` và `libvietime`. Không dùng framework ngoài: runner là
`check.h` / `check.cpp`, điểm vào là `test_main.cpp`.

## Chạy

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build              # chạy mọi target test
./build/ime_tests                   # xem từng ca
```

Build `Debug` bật `-Werror` và, trên Linux/macOS, AddressSanitizer +
UndefinedBehaviorSanitizer cho mọi target. Một test đọc/ghi lố bộ nhớ sẽ
dừng ngay với báo cáo của ASan, kể cả khi kết quả so sánh vẫn đúng.

## Đọc kết quả

Mỗi ca in đúng một dòng. Dòng cuối là số đếm:

```
PASS  [TLX] go "tieengs"
FAIL  [TLX] go "saw"
    want: U+0073 U+0103 | să
    got:  U+0103 | ă
...
CO LOI: 727 pass, 1 fail, 728 tong
```

- Chuỗi được so theo byte UTF-8, tương đương so theo dãy codepoint. Khi đỏ,
  `check_str` in ra từng codepoint để thấy khác biệt mà mắt không thấy
  (ví dụ NFC `ế` U+1EBF so với NFD `ê` + U+0301).
- `ime_tests` thoát với mã **1** khi có ít nhất một ca đỏ, **0** khi tất cả
  xanh. `ctest` và CI dựa vào mã này.

## Các target

| Target          | Link với            | Nội dung                                           |
| --------------- | ------------------- | -------------------------------------------------- |
| `ime_tests`     | `ime_core` (tĩnh)   | Mọi test C++ bên dưới                              |
| `type_sentence` | `libvietime` (động) | Chương trình C thuần gõ `Tieengs Vieejt` qua API C |

`type_sentence` là DoD của Day 28: nó chứng minh API C đủ dùng mà không cần
chạm vào `ime_core`. Nó cũng kiểm hợp đồng `backspace_count`: preedit người
dùng nhìn thấy phải là đầu của phần được commit.

`api_compile_check.c` hiện **không** nằm trong CMake.

## Các file

| File                     | Kiểm                                                         |
| ------------------------ | ------------------------------------------------------------ |
| `test_utf8.cpp`          | Chuyển đổi UTF-8 / UTF-32, `utf8_char_count`, round-trip     |
| `test_normalize.cpp`     | `to_nfc` với đầu vào NFD, dấu mồ côi, idempotent             |
| `test_tone_table.cpp`    | `apply_tone_to_vowel`, `tone_from_telex`, `tone_from_vni`    |
| `test_syllable.cpp`      | `split_syllable`, `find_tone_position` (cả hai kiểu đặt dấu) |
| `test_case_map.cpp`      | `to_upper_viet` đối chiếu `case_table.h`                     |
| `test_key_processor.cpp` | Gõ trọn qua `KeyProcessor`; bất biến `char_count()` (G2)     |
| `test_telex.cpp`         | Vector Telex, lấy từ sheet TestCases                         |
| `test_vni.cpp`           | Vector VNI, gồm các ca chữ số phải đi thẳng                  |
| `test_uppercase.cpp`     | Mặt nạ chữ hoa                                               |
| `test_undo.cpp`          | Hoàn tác lặp phím, phím xoá dấu, backspace                   |
| `test_api_util.cpp`      | `copy_text` (cắt ở 63 byte, luôn ghi NUL)                    |

`case_table.h` là file **sinh ra** bằng `tools/gen_case_table.py` từ bảng
Unicode, được commit sẵn vào repo. Không sửa tay; sửa script rồi sinh lại.

## Thêm một ca

Một ca gõ phím là một dòng:

```cpp
check_telex("tieengs", "tiếng");
check_vni("tie6ng1", "tiếng");
check_telex_tone_placement("hoaf", "hoà", vietime::PLACEMENT_MODERN);
```

Nếu ca đến từ sheet TestCases, ghi mã ca ở cuối dòng (`// TC-019`) để hai
bên đối chiếu được.

Thêm một file test mới:

1. Viết `void run_xxx_tests()` trong `tests/test_xxx.cpp`.
2. Khai báo và gọi nó trong `test_main.cpp`.
3. Thêm file vào `add_executable(ime_tests ...)` trong `CMakeLists.txt`.

## Quy tắc hồi quy

Các quy tắc này có từ những bug thật đã tốn thời gian của project
(xem mục F trong `backlog.md`).

1. **Mọi bug có test trước khi sửa.** Viết ca tái hiện, chạy thấy **đỏ**, rồi
   mới sửa code, rồi thấy xanh. Một test chưa từng đỏ thì chưa chứng minh
   được là nó bắt được lỗi. Bug phát hiện ngoài bộ test cũng được thêm một
   dòng vào sheet TestCases; dòng đó không bao giờ bị xoá.
2. **Test khoá hành vi: sửa test trước, sửa code sau.** Một số test ghi lại
   hành vi _đã chốt_ chứ không phải hành vi "đúng" hiển nhiên, ví dụ
   `nam2024` → `nãm` (VNI), `aAn` → `ân`, `uo` + `w` luôn ra `ươ`, và ca 9
   của `copy_text`. Muốn đổi hành vi thì đổi test trước, có lý do ghi lại
   trong `backlog.md`.
3. **Test hàm thuần, không chỉ test đầu-cuối.** Hàng trăm ca gõ phím từng
   xanh trong khi `to_upper_viet` và `copy_text` sai, vì các ca đầu-cuối chỉ
   chạm vào một phần nhỏ đầu vào của chúng.
4. **Không gõ tay bảng dữ liệu.** Bảng lớn (chữ hoa/thường, bảng dấu) sinh
   bằng script từ nguồn chuẩn.
5. **Mọi thay đổi phải giữ `ctest` xanh ở build Debug**, tức là không cảnh
   báo (`-Werror`) và không báo cáo sanitizer.

## Chưa có (Day 33–36)

- Edge case và an toàn buffer qua API C (Day 33).
- Chạy valgrind trên bản build không sanitizer (Day 34).
- Fuzz 1 000 000 phím (Day 35).
- CI chạy suite mỗi lần push, và báo cáo coverage tự động (Day 36). Đo thủ
  công bằng gcov ngày 11/09: `core/` đạt 93% số dòng.
