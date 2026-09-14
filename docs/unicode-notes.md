# Unicode trong viet-ime

Core IME làm việc hoàn toàn bằng UTF-8 ở dạng NFC. Mọi input và output đều được
chuẩn hoá tại ranh giới nhập dữ liệu ngoài, không chạy trong vòng xử lý phím.

Tài liệu này ghi lại `vietime::to_nfc` **lệch khỏi NFC chuẩn ở đâu và vì sao**.
Ba chỗ lệch dưới đây đều là quyết định có chủ đích, đều có test riêng.

---

## 1. Lớp kết hợp và quy tắc đổi chỗ

Mỗi dấu kết hợp mang một số gọi là lớp kết hợp (canonical combining class, ccc):

| Dấu    | ccc | Tên                              |
| ------ | --- | -------------------------------- |
| U+031B | 216 | COMBINING HORN (móc)             |
| U+0323 | 220 | COMBINING DOT BELOW (nặng)       |
| U+0300 | 230 | COMBINING GRAVE ACCENT (huyền)   |
| U+0301 | 230 | COMBINING ACUTE ACCENT (sắc)     |
| U+0302 | 230 | COMBINING CIRCUMFLEX ACCENT (mũ) |
| U+0303 | 230 | COMBINING TILDE (ngã)            |
| U+0306 | 230 | COMBINING BREVE (trăng)          |
| U+0309 | 230 | COMBINING HOOK ABOVE (hỏi)       |

Bước sắp xếp chính tắc của Unicode (UAX #15) sắp các dấu theo ccc tăng dần,
nhưng **chỉ đổi chỗ hai dấu khi ccc của chúng khác nhau**. Hai dấu cùng lớp giữ
nguyên thứ tự — đây là bảo đảm ổn định của Unicode, không phải tuỳ chọn cài đặt.

Hệ quả cho tiếng Việt: dấu móc và dấu nặng nằm khác lớp so với mọi dấu khác, nên thứ tự
của chúng không quan trọng. Nhưng **mũ, trăng và năm dấu thanh đều nằm chung lớp
230**, nên thứ tự giữa chúng thì quan trọng — với Unicode.

---

## 2. Chỗ lệch A: `to_nfc` bỏ qua thứ tự dấu

`to_nfc` không sắp xếp theo ccc. Nó gom mọi dấu đứng sau một ký tự nền, phân loại
thành _một_ dấu phụ và _một_ dấu thanh, rồi ghép theo thứ tự cố định
nền → dấu phụ → thanh. Thứ tự trong đầu vào không ảnh hưởng gì.

| Đầu vào           | ccc      | `to_nfc`   | NFC chuẩn     |          |
| ----------------- | -------- | ---------- | ------------- | -------- |
| `a` U+0323 U+0302 | 220, 230 | U+1EAD `ậ` | U+1EAD        | khớp     |
| `a` U+0302 U+0323 | 230, 220 | U+1EAD `ậ` | U+1EAD        | khớp     |
| `u` U+031B U+0300 | 216, 230 | U+1EEB `ừ` | U+1EEB        | khớp     |
| `u` U+0300 U+031B | 230, 216 | U+1EEB `ừ` | U+1EEB        | khớp     |
| `o` U+0302 U+0323 | 230, 220 | U+1ED9 `ộ` | U+1ED9        | khớp     |
| `e` U+0302 U+0301 | 230, 230 | U+1EBF `ế` | U+1EBF        | khớp     |
| `a` U+0306 U+0301 | 230, 230 | U+1EAF `ắ` | U+1EAF        | khớp     |
| `e` U+0301 U+0302 | 230, 230 | U+1EBF `ế` | U+00E9 U+0302 | **LỆCH** |
| `a` U+0301 U+0306 | 230, 230 | U+1EAF `ắ` | U+00E1 U+0306 | **LỆCH** |

Sáu dòng đầu khớp chuẩn chỉ vì ccc khác nhau nên Unicode tự đổi chỗ hộ. Hai dòng
cuối lệch vì ccc bằng nhau nên Unicode giữ nguyên thứ tự, còn `to_nfc` thì không.

**Vì sao chọn bỏ qua thứ tự.** NFD là thuật toán tất định: chuẩn hoá `ế` luôn cho
`U+0065 U+0302 U+0301`, mũ trước thanh sau, không bao giờ ngược lại. Nên dãy
`e` + sắc + mũ không thể là đầu ra của bất kỳ bộ chuẩn hoá nào — nó chỉ đến từ
chuỗi dựng bằng tay, từ phép nối chuỗi sai chỗ, hoặc từ một bộ sinh có bug. Với
đầu vào hỏng như vậy, trả `é` + mũ là đẩy một thứ không nằm trong 134 dạng chữ
tiếng Việt xuống cho `char_count()` và `backspace_count` đếm thành 2 codepoint.
Trả `ế` là đoán đúng ý người viết dữ liệu.

Lựa chọn này cũng nhất quán với chính bộ gõ: gõ `tiengs` hay `tiesng` đều ra
`tiếng`. Bỏ qua thứ tự dấu là cách mọi bộ gõ tiếng Việt hành xử ở tầng phím bấm;
`to_nfc` chỉ áp cùng nguyên tắc ấy ở tầng văn bản.

---

## 3. Chỗ lệch B: gặp dấu trùng lớp thì bỏ nguyên cụm

Khi một ký tự nền có **hai dấu phụ** hoặc **hai dấu thanh**, `to_nfc` không ghép
gì cả và trả nguyên cụm về. NFC chuẩn thì ghép được bao nhiêu ghép bấy nhiêu rồi
để phần thừa lại.

| Đầu vào                                   | `to_nfc`   | NFC chuẩn     |
| ----------------------------------------- | ---------- | ------------- |
| `e` U+0301 U+0300 (hai thanh)             | giữ nguyên | U+00E9 U+0300 |
| `a` U+0302 U+0306 (hai dấu phụ)           | giữ nguyên | U+00E2 U+0306 |
| `e` U+0302 U+0301 U+0300 (mũ + hai thanh) | giữ nguyên | U+1EBF U+0300 |

Ở đây `to_nfc` **chặt hơn** chuẩn, không phải dễ dãi hơn. Lý do: một ký tự tiếng
Việt mang tối đa một dấu phụ và một dấu thanh. Nhiều hơn thế nghĩa là chuỗi không
phải tiếng Việt, và ghép một nửa sẽ tạo ra thứ vừa không phải tiếng Việt vừa
không còn giống đầu vào. Giữ nguyên để tầng trên còn nhận ra mà xử lý.

---

## 4. Chỗ lệch C: ký tự nền ngoài tiếng Việt không được đụng tới

| Đầu vào           | `to_nfc`   | NFC chuẩn  |
| ----------------- | ---------- | ---------- |
| `n` U+0303        | giữ nguyên | U+00F1 `ñ` |
| `c` U+0327        | giữ nguyên | U+00E7 `ç` |
| `b` U+0302 U+0301 | giữ nguyên | giữ nguyên |

`to_nfc` chỉ biết bảng ghép của tiếng Việt. Chữ của ngôn ngữ khác đi qua nguyên
vẹn, kể cả khi Unicode có dạng dựng sẵn cho chúng. Đây là giới hạn đã biết, không
phải bug: dự án không nhúng bảng ghép đầy đủ của Unicode.

---

## 5. Những chỗ khớp chuẩn

- Chuỗi rỗng → chuỗi rỗng.
- Đầu vào đã là NFC → không đổi (idempotent).
- Dấu mồ côi không có ký tự nền (`U+0301` đứng đầu chuỗi) → giữ nguyên, không
  crash.
- `đ` U+0111 không có dạng phân tách nên không bao giờ bị đụng tới.
- `d` + U+0335 (gạch ngang chồng) **không** tương đương `đ` và không được ghép
  thành `đ`. Unicode cũng vậy.

---

## 6. Hệ quả khi dùng

`to_nfc` **không** thay thế được một bộ chuẩn hoá NFC đầy đủ. Nếu về sau cần NFC
đúng chuẩn cho văn bản đa ngôn ngữ, dùng ICU (`unorm2_getNFCInstance`) chứ đừng
mở rộng function này.

Ba nhóm lệch ở mục 2, 3, 4 là **toàn bộ** chỗ khác biệt đã biết, và đều có test
trong `tests/test_normalize.cpp`. Muốn đổi bất kỳ hành vi nào: **sửa test trước,
sửa code sau.**

---

## 7. Vì sao core dùng NFC chứ không dùng NFD

- Mọi ký tự tiếng Việt đều có dạng dựng sẵn, nên NFC luôn cho 1 codepoint mỗi ký
  tự. `char_count()` vì thế trả đúng số ký tự người dùng nhìn thấy.
- `backspace_count` tính theo codepoint. Nếu preedit là NFD, `ế` sẽ thành 3
  codepoint và ứng dụng sẽ xoá lẹm 2 ký tự.
- Mọi ký tự tiếng Việt đều nằm dưới U+FFFF (cao nhất là `ỹ` U+1EF9), nên trên
  Windows 1 codepoint luôn bằng 1 UTF-16 code unit, không có cặp thay thế.
- Dạng dựng sẵn tối đa 3 byte UTF-8; con số này là hệ số 3 trong `static_assert`
  ở `core/ime_api.cpp`.
