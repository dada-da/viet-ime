# viet-ime — Test report

## Memory safety (Day 34)

Kiểm tra bộ nhớ chạy trên GitHub Actions, không chạy trên máy dev.
Lý do: toolchain MinGW/UCRT64 không có `libasan`, và LeakSanitizer không
hỗ trợ Windows trên bất kỳ toolchain nào. Xem "Giới hạn theo nền tảng".

### environment

|                    | ASan + UBSan + LSan        | valgrind memcheck   |
| ------------------ | -------------------------- | ------------------- |
| Job CI             | `asan + ubsan + lsan`      | `valgrind memcheck` |
| Runner             | `ubuntu-latest`            | `ubuntu-latest`     |
| Compiler           | `<GCC x.y.z — lấy từ log>` | `<GCC x.y.z>`       |
| Build type         | Debug                      | Debug               |
| `VIETIME_SANITIZE` | ON                         | OFF                 |
| valgrind           | —                          | `<phiên bản>`       |

Hai build **phải tách rời**. ASan và valgrind đều thay thế allocator;
chạy chung cho kết quả rác hoặc crash.

### Flag

Build sanitizer:

```
-fsanitize=address,undefined -fno-sanitize-recover=all
-fno-omit-frame-pointer -g
```

`-fno-sanitize-recover=all` là bắt buộc. Mặc định UBSan in
`runtime error:` ra stderr rồi **chạy tiếp**, và tiến trình vẫn thoát
với mã 0 — ctest sẽ báo xanh trong khi UBSan vừa tìm được lỗi thật.

Biến environment khi chạy:

```
ASAN_OPTIONS=detect_leaks=1:abort_on_error=1:detect_stack_use_after_return=1
UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1
LSAN_OPTIONS=report_objects=1
```

`detect_leaks=1` phải ghi rõ vì LeakSanitizer tắt mặc định trên macOS.

Build valgrind: sạch sanitizer, `-O0 -g`. Lệnh:

```
valgrind --leak-check=full --show-leak-kinds=definite,indirect \
         --track-origins=yes --error-exitcode=1 \
         --errors-for-leak-kinds=definite,indirect <binary>
```

### Xác nhận instrumentation

Một bước riêng trong job kiểm rằng Flag sanitizer thật sự tới mọi target,
kể cả `ime_core`:

```
grep -c 'fno-sanitize-recover' build-san/compile_commands.json
```

Bước này tồn tại vì dự án dùng `target_compile_options` cho từng target
thay vì `add_compile_options` toàn cục. Nếu Flag chỉ tới binary test mà
không tới `ime_core`, suite vẫn chạy, vẫn xanh, và mọi lỗi trong core vô
hình.

Số file được instrument: 30

### Phát hiện

| #   | Công cụ | Test | Loại lỗi | File:dòng | Nguyên nhân | Commit sửa |
| --- | ------- | ---- | -------- | --------- | ----------- | ---------- |
|     |         |      |          |           |             |            |

### Suppression

`still reachable` từ khởi tạo `std::locale` của libstdc++ là bình
thường và không được tính là leak. Flag `--show-leak-kinds=definite,indirect`
đã loại nó ra.

### Kết quả

- ASan + UBSan + LSan: `<sạch / 0 phát hiện>` — run `https://github.com/dada-da/viet-ime/actions/runs/34870859555/job/104066090084`
- valgrind memcheck: `3,077 allocs, 3,077 frees, 7,778,986 bytes allocated` — run `https://github.com/dada-da/viet-ime/actions/runs/34870859555/job/104066089744`
- Số test chạy dưới sanitizer: `843` (`core`, `api`, `c_sentence`, `fuzz`)

### Giới hạn theo nền tảng

| Nền tảng                   | ASan  | UBSan | LSan                | valgrind |
| -------------------------- | ----- | ----- | ------------------- | -------- |
| Linux (GCC)                | có    | có    | có                  | có       |
| Windows MSYS2 UCRT64 (GCC) | không | không | không               | không    |
| Windows MSYS2 CLANG64      | có    | có    | **không**           | không    |
| macOS ARM64                | có    | có    | không (Apple Clang) | không    |

Hệ quả: **DoD Day 34 chỉ thoả được trên Linux.** Sanitizer cho mingw-w64
không tồn tại (`libasan` không được build cho target này); LeakSanitizer
không hỗ trợ Windows; valgrind dừng hỗ trợ macOS ở 10.13 x86_64.

`CMakeLists.txt` đã chặn sẵn bằng
`if(NOT MINGW AND NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")` — bỏ điều
kiện đó ra thì build chết ở bước link với `cannot find -lasan`.

Trên Windows, lỗi bộ nhớ chỉ hiện ra dưới dạng mã thoát thô của hệ điều
hành. Ví dụ đã gặp: `0xC0000374` (`STATUS_HEAP_CORRUPTION`) — không tên
file, không số dòng, không biết vùng nhớ nào bị đụng.

### Cách chạy lại

```
cmake -S . -B build-san -G Ninja -DCMAKE_BUILD_TYPE=Debug -DVIETIME_SANITIZE=ON
cmake --build build-san
ASAN_OPTIONS=detect_leaks=1:abort_on_error=1 ctest --test-dir build-san --output-on-failure
```

```
cmake -S . -B build-vg -G Ninja -DCMAKE_BUILD_TYPE=Debug -DVIETIME_SANITIZE=OFF
cmake --build build-vg
valgrind --leak-check=full --error-exitcode=1 ./build-vg/ime_tests
```
