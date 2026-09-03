#ifndef NORMALIZE_H
#define NORMALIZE_H

#include <string>

// Chuyển chuỗi UTF-8 bất kỳ về NFC.
// GIỚI HẠN: chỉ xử lý phạm vi tiếng Việt. Dấu tổ hợp của ngôn ngữ khác được giữ nguyên
std::string to_nfc(const std::string &s);

#endif /* NORMALIZE_H */