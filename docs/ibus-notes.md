## 1. Đường đi của một phím

### Chiều đi: bàn phím → engine

1. Kernel nhận sự kiện phần cứng và đẩy lên máy chủ hiển thị.

2. **X11** — X server gửi `KeyPress` tới cửa sổ đang focus. Toolkit của
   ứng dụng (GTK, Qt) nạp một _IM module_ (`ibus-gtk3`, `ibus-qt`) chặn
   sự kiện **trước khi** widget nhận, rồi chuyển qua D-Bus tới
   ibus-daemon. Ứng dụng không có IM module phải dùng XIM — giao thức
   cũ, hạn chế nhiều hơn.

   **Wayland** — ứng dụng khai báo qua `text-input-v3` rằng nó nhận
   text input. Compositor giữ phím lại và chuyển cho IME qua
   `input-method-v2`. Ứng dụng không thấy phím gốc.

3. ibus-daemon tra input context đang focus, tìm engine đang active,
   phát signal `process-key-event` qua D-Bus.

4. Engine vietime nhận `(keyval, keycode, state)`:
   - bỏ qua sự kiện nhả phím (`state & IBUS_RELEASE_MASK`)
   - đổi `keyval` (keysym) sang codepoint Unicode
   - gọi `vietime_process_key()`

### Chiều về: engine → màn hình

5. Theo `text_committed` trong `VietimeKeyResult`:
   - `0` → `ibus_engine_update_preedit_text()` — chữ gạch chân, chưa chốt
   - `1` → `ibus_engine_commit_text()` — chữ vào hẳn tài liệu

6. ibus-daemon chuyển ngược qua D-Bus, tới IM module (X11) hoặc
   compositor (Wayland).

7. Toolkit vẽ preedit trong widget, hoặc chèn chữ đã commit.

8. Giá trị `gboolean` trả về quyết định ứng dụng có thấy phím gốc hay
   không — ánh xạ từ `key_consumed`.

### Note:

Engine không bao giờ chạm vào cửa sổ ứng dụng, không tự vẽ, không tự
chèn chữ. Chỉ nhận phím và trả kết quả; mọi thứ đi ngược lại đúng
đường đã đến.

## 2. VietimeKeyResult trong wrapper IBus

### Khung của hàm

```c
static gboolean
vietime_ibus_process_key(IBusEngine *engine,
                         guint keyval, guint keycode, guint state)
{
    /* 1. Lọc sự kiện nhả phím */
    if (state & IBUS_RELEASE_MASK)
        return FALSE;

    /* 2. keysym -> codepoint Unicode */
    guint32 cp = /* xem "keyval không phải codepoint" bên dưới */;

    VietimeKeyResult r = vietime_process_key(ctx, cp);

    /* 3. Chặn lỗi */
    if (r.error != VIETIME_OK)
        return FALSE;

    /* 4. Chọn hàm hiển thị */
    IBusText *t = ibus_text_new_from_string(r.text);
    if (r.text_committed)
        ibus_engine_commit_text(engine, t);
    else
        ibus_engine_update_preedit_text_with_mode(engine, t, cursor_pos, TRUE, IBUS_ENGINE_PREEDIT_COMMIT);

    /* 5. Trả quyền cho ứng dụng hay không */
    return r.key_consumed ? TRUE : FALSE;
}
```

### Vai trò từng trường

| Trường            | Vai trò trong wrapper IBus                                                                                                                                             |
| ----------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `error`           | Chặn lỗi ở bước 3. Khác `VIETIME_OK` thì không vẽ gì, trả `FALSE` để phím đi thẳng về ứng dụng.                                                                        |
| `text`            | Nội dung truyền vào `ibus_text_new_from_string()`. Dùng cho cả hai nhánh ở bước 4.                                                                                     |
| `text_committed`  | Rẽ nhánh ở bước 4. `1` → `commit_text` (chữ vào hẳn tài liệu). `0` → `update_preedit_text` (chữ gạch chân, chưa chốt).                                                 |
| `key_consumed`    | Giá trị trả về của hàm. `TRUE` = engine đã nuốt phím. `FALSE` = ứng dụng nhận phím gốc.                                                                                |
| `text_length`     | Không dùng. `text` đã NUL-terminated nên `ibus_text_new_from_string()` tự đo. Giữ lại cho wrapper nền tảng khác.                                                       |
| `backspace_count` | **Không dùng.** IBus theo mô hình thay thế nguyên khối: engine gửi toàn bộ preedit mới, IBus tự dọn cái cũ. Trường này dành cho wrapper hook + SendInput trên Windows. |

### Lưu ý

**Sự kiện nhả phím.** Mỗi lần nhấn sinh ra hai sự kiện: nhấn và nhả.
Không lọc `IBUS_RELEASE_MASK` là mỗi phím qua engine hai lần, và
`tieengs` thành `ttiieeeennggss`.

**`keyval` là keysym, không phải codepoint.** `vietime_process_key()`
nhận codepoint Unicode. Với chữ cái ASCII hai thứ trùng số nhau
(`IBUS_KEY_a` = 0x61 = `U+0061`), nên bỏ qua bước đổi vẫn chạy được với
Telex và miss lỗi. Dùng `ibus_keyval_to_unicode()`.

Về chọn `keyval` hay `keycode`: IME cần một bố cục bàn phím cụ thể thì
dùng `keycode` (Chewing, Cangjie, Wubi cần QWERTY en-US); IME ít nhạy với
bố cục dùng `keyval` (pinyin). Telex và VNI ánh xạ theo _chữ cái_ chứ
không theo _vị trí phím_, nên dùng `keyval` — người dùng Dvorak vẫn gõ
được bình thường.

**`IBusText` là floating object.** `ibus_engine_commit_text()` gọi
`_g_object_unref_if_floating(text)` bên trong. Tạo xong truyền vào là hết
việc — tự free thêm là double free. Muốn giữ lại thì
`g_object_ref_sink()` trước.

### Hai quyết định behaviour

**1. Mất focus khi đang gõ dở.**

`ibus_engine_update_preedit_text()` là wrapper mỏng của
`ibus_engine_update_preedit_text_with_mode()` với `IBUS_ENGINE_PREEDIT_CLEAR`,
nghĩa là preedit **bị vứt** khi mất focus. Người dùng gõ `tieen` rồi bấm
sang cửa sổ khác sẽ mất chữ.

Bản `_with_mode` cho chọn `IBUS_ENGINE_PREEDIT_COMMIT` để commit thay vì
vứt. UniKey commit.

Quyết định: commit

**2. Giá trị trả về cho sự kiện nhả phím.**

Trả `FALSE` thì ứng dụng nhận sự kiện nhả. Trả `TRUE` thì không.
ibus-chewing trả `TRUE`. Thường không khác biệt với gõ chữ thường,
nhưng có thể ảnh hưởng tới phím tắt của ứng dụng.

Quyết định: FALSE

### Chưa xác minh

Mọi thứ trên viết từ tài liệu và source của IBus, chưa chạy thử. Khi có
Linux, kiểm lại bằng `ibus-daemon -vx` và sửa phần nào sai.

Source:

- https://valadoc.org/ibus-1.0/IBus.Engine.html
- https://github.com/ibus/ibus/blob/main/src/ibusengine.c

## 3. IBus và Fcitx5

### Khác biệt cốt lõi

Khác biệt quan trọng nhất không phải API, mà là **code chạy ở đâu**.

|                  | IBus                                        | Fcitx5                                    |
| ---------------- | ------------------------------------------- | ----------------------------------------- |
| Hình thái engine | tiến trình riêng, đăng ký qua component XML | addon `.so` nạp vào tiến trình fcitx5     |
| Giao tiếp        | D-Bus (liên tiến trình)                     | gọi hàm trực tiếp (cùng tiến trình)       |
| Ngôn ngữ         | C với GObject                               | C++                                       |
| Crash trong core | chết engine, fcitx/app còn sống             | **chết cả fcitx5**, mất IME toàn hệ thống |
| Mặc định ở       | GNOME, Ubuntu, Fedora                       | KDE Plasma, Arch                          |

Fcitx5 hợp với dự án hơn về ngôn ngữ — core đã là C++, không phải bọc
qua GObject. Nhưng nó cũng khắt khe hơn: một lỗi bộ nhớ trong
`libvietime` làm sập toàn bộ IME của người dùng chứ không chỉ engine.

### Quyết định: làm IBus trước

Lý do chính là Wayland, không phải thị phần.

GNOME chỉ hỗ trợ `text-input-v3` và **không** hỗ trợ giao thức
`input-method` của Wayland — thứ mà frontend Wayland của fcitx5 cần để
nói chuyện với compositor và hiển thị popup. Trên GNOME Wayland,
fcitx5 phải dựa vào extension phụ (kimpanel) cho popup, còn IBus chạy
thẳng vì gnome-shell nhúng sẵn.

Vì GNOME Wayland là cấu hình mặc định của Ubuntu và Fedora, IBus là
đường duy nhất chạy trơn ở đó. Fcitx5 phục vụ KDE và người dùng tự cấu
hình — vẫn đáng làm, nhưng làm sau.

### Hệ quả: phần dùng lại được

Hai wrapper gọi cùng một `libvietime` qua cùng một C API. Việc thật sự
phải viết lại chỉ là:

- ánh xạ keysym sang codepoint
- gọi hàm hiển thị preedit / commit của từng nền tảng
- đăng ký engine với hệ thống

Toàn bộ logic biến đổi không đụng tới. Wrapper thứ hai rẻ hơn wrapper
thứ nhất đáng kể — miễn là wrapper thứ nhất không để logic rò rỉ vào
trong nó.

**Bất biến cần giữ:** không có luật tiếng Việt nào được nằm trong
`platform/`. Thấy mình viết `if (key == 's')` trong file wrapper là dấu
hiệu logic đang rò ra khỏi core.
