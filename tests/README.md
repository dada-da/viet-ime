# viet-ime - Test suite

## Bố cục

Hai binary test, phân chia theo **cái được test**, không theo chủ đề.

### `ime_tests` - link `ime_core` tĩnh

Test logic bên trong core. Không đụng tới `libvietime`.

| File                     | Phạm vi                        |
| ------------------------ | ------------------------------ |
| `test_utf8.cpp`          | mã hoá/giải mã UTF-8           |
| `test_normalize.cpp`     | NFC/NFD                        |
| `test_tone_table.cpp`    | bảng tra dấu                   |
| `test_key_processor.cpp` | vòng đời buffer, preedit       |
| `test_syllable.cpp`      | tách âm tiết                   |
| `test_telex.cpp`         | luật Telex                     |
| `test_vni.cpp`           | luật VNI                       |
| `test_uppercase.cpp`     | mặt nạ chữ hoa                 |
| `test_case_map.cpp`      | `to_upper_viet` theo bảng sinh |
| `test_undo.cpp`          | hoàn tác lặp phím              |
| `test_api_util.cpp`      | `copy_text` và tiện ích API    |
| `test_unicode.cpp`       | so sánh theo code point        |

### `api_tests` - link `libvietime` động

Test bề mặt C API công khai qua ranh giới thư viện: `test_edge_case.cpp`.

### `type_sentence` - test C thuần

Xác nhận một chương trình **C** (không phải C++) link được vào
`libvietime` và gõ hết một câu. Đây là test canh ABI: hỏng khi header
rò rỉ thứ gì đó chỉ C++ hiểu.

### `fuzz_keys` - fuzzer bất biến

Sinh phím có trọng số, dựng một tài liệu mô phỏng, kiểm tra bất biến sau
**mỗi** lời gọi API.

**không** kiểm tính đúng của nội dung. Gõ `tieengs` ra `tiếng` hay ra
`tiengs` thì mọi bất biến đều xanh như nhau. `fuzz_keys` xanh không phải
bằng chứng engine đúng - đó là việc của các test theo bảng ở trên.

Bất biến đang kiểm:

| #   | Nội dung                                                            |
| --- | ------------------------------------------------------------------- |
| BB1 | `error` thuộc enum đã định nghĩa                                    |
| BB2 | `text_committed`, `key_consumed` đúng là 0 hoặc 1                   |
| BB3 | `text_length < VIETIME_MAX_TEXT_BYTES`                              |
| BB4 | `text[text_length] == '\0'`                                         |
| BB5 | `text` là UTF-8 hợp lệ (bộ giải mã độc lập, không dùng `core/utf8`) |
| BB6 | đầu ra ở dạng NFC, không có dấu tổ hợp rời                          |
| BB7 | `backspace_count` không ăn sang chữ có sẵn của ứng dụng             |
| BB8 | `backspace_count` ≤ độ dài preedit trước lời gọi                    |
| BB9 | preedit ≤ `VIETIME_MAX_CODE_POINT`                                  |

Fuzzer in seed **trước** vòng lặp, không phải khi bắt được lỗi. Crash
nghĩa là không bao giờ tới được dòng in ở cuối, và một fuzzer không tái
hiện được crash của chính nó thì vô dụng.

Replay: `./fuzz_keys <seed>` hoặc `./fuzz_keys <seed> <số lượt>`.

## Chạy

```
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

Một test riêng: `ctest --test-dir build -R fuzz --output-on-failure`

Cấu hình khác:

| Mục đích                              | Flag                                           |
| ------------------------------------- | ---------------------------------------------- |
| Sanitizer (mặc định, chỉ Linux/macOS) | `-DVIETIME_SANITIZE=ON`                        |
| Cho valgrind                          | `-DVIETIME_SANITIZE=OFF`                       |
| Coverage                              | `-DVIETIME_SANITIZE=OFF -DVIETIME_COVERAGE=ON` |

Sanitizer và coverage loại trừ nhau - CMake báo lỗi nếu bật cả hai. ASan
làm méo số đếm dòng của gcov.

## CI

| Job                      | Chạy khi                | Chặn merge |
| ------------------------ | ----------------------- | ---------- |
| `test (linux-gcc)`       | mọi push                | có         |
| `test (windows-ucrt64)`  | mọi push                | có         |
| `asan + ubsan + lsan`    | mọi push                | có         |
| `valgrind memcheck`      | mọi push                | có         |
| `coverage (core >= 80%)` | mọi push                | có         |
| `fuzz 1e6 (nightly)`     | 02:00 UTC, hoặc gọi tay | **không**  |

`fuzz-long` không chặn merge vì chạy theo lịch chứ không theo commit -
lỗi tìm ra thuộc về code đã merge từ trước. Đỏ ở đó thì mở issue, không
revert mù.

Ngưỡng coverage 80% chỉ tính `core/`, loại `tests/` và `core/main.cpp`.
Gộp `tests/` vào sẽ cho một con số cao và vô nghĩa.

## Chính sách hồi quy

### Mỗi bug đẻ ra một test, trước khi sửa

Thứ tự bắt buộc:

1. Viết ca test mô tả hành vi **mong muốn**
2. Chạy - phải **đỏ**
3. Sửa code
4. Chạy - phải xanh

### Áp dụng cho cả bug của công cụ test

Luật này không chỉ phủ `core/` và `libvietime`. Phủ cả `tests/`,
`CMakeLists.txt` và `.github/workflows/`.

### Đổi hành vi thì sửa test trước, sửa code sau

Một số test đang **khoá hành vi hiện tại**, không phải khẳng định hành vi
đúng. Chúng tồn tại để không ai đổi hành vi một cách vô tình.

Khi đổi có chủ đích: sửa test trước, xem kết quả đỏ, rồi mới sửa code. Không
bao giờ sửa code trước rồi chỉnh test cho khớp.

### Test hàm thuần, không chỉ test đầu-cuối

Test đầu-cuối bỏ sót lỗi mà test hàm thuần bắt được ngay. Ví dụ đã gặp:
`to_upper_viet` sai trong khi 293 test đầu-cuối vẫn xanh, vì chúng chỉ
chạm 12 trong 74 ký tự và 12 cái đó tình Flag không lộ lỗi.

Thêm hàm nội bộ nào có bảng tra hoặc có nhánh, viết test riêng cho nó.

### Bảng dữ liệu phải sinh bằng script, không gõ tay

74 cặp thường/hoa trong `tests/case_table.h` sinh từ bảng Unicode bằng
`tools/gen_case_table.py`. Gõ tay một bảng 74 dòng đúng là việc mà bảng
sinh ra để tránh.

### Bật cảnh báo, và đọc chúng

Build Debug dùng `-Wall -Wextra -Wshadow -Werror`. Ba Flag này đã bắt được
lỗi thật mà test không bắt:

- `-Wshadow` bắt lỗi che biến trong `copy_text` khiến hàm luôn trả 1.
  `-Wall -Wextra` im lặng về test.
- GCC không cảnh báo biến `std::string` không dùng (chỉ cảnh báo với
  kiểu có hàm dựng tầm thường như `int`), nên hai dòng chết trong
  `process_key` lọt qua `-Werror`.

Đừng gỡ `-Werror` để build cho qua. Cảnh báo mới trên GCC phiên bản khác
là CI đang làm đúng việc.

## Thêm ca test

Ca Telex hoặc VNI mới là thêm **một dòng** vào bảng trong
`test_telex.cpp` / `test_vni.cpp`. Nếu phải viết hơn một dòng, cấu trúc
bảng đang sai chỗ nào đó - sửa bảng, đừng viết ca đặc biệt.

Ca cho hành vi API công khai vào `test_edge_case.cpp`.

Bất biến mới cho fuzzer vào `check_invariants()` trong `fuzz_keys.cpp`,
và **phải kèm một lần phá để xác nhận**: sửa một dòng cho bất biến đó
sai, chạy, thấy test đỏ, rồi hoàn tác.
