// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Sinh tự động bởi tools/gen_case_table.py — không sửa tay.
// Dữ liệu ánh xạ hoa/thường lấy từ Unicode Character Database
// (https://www.unicode.org/Public/UCD/latest/), Unicode License v3.

#ifndef CASE_TABLE_H
#define CASE_TABLE_H

// Đây là DỮ LIỆU ĐỐI CHỨNG cho test, không phải bảng để to_upper_viet tra.

struct CasePair
{
  char32_t lower;
  char32_t upper;
};

static const CasePair kCaseTable[] = {
    {U'\u0061', U'\u0041'}, // a -> A
    {U'\u00E1', U'\u00C1'}, // á -> Á
    {U'\u00E0', U'\u00C0'}, // à -> À
    {U'\u1EA3', U'\u1EA2'}, // ả -> Ả
    {U'\u00E3', U'\u00C3'}, // ã -> Ã
    {U'\u1EA1', U'\u1EA0'}, // ạ -> Ạ
    {U'\u0103', U'\u0102'}, // ă -> Ă
    {U'\u1EAF', U'\u1EAE'}, // ắ -> Ắ
    {U'\u1EB1', U'\u1EB0'}, // ằ -> Ằ
    {U'\u1EB3', U'\u1EB2'}, // ẳ -> Ẳ
    {U'\u1EB5', U'\u1EB4'}, // ẵ -> Ẵ
    {U'\u1EB7', U'\u1EB6'}, // ặ -> Ặ
    {U'\u00E2', U'\u00C2'}, // â -> Â
    {U'\u1EA5', U'\u1EA4'}, // ấ -> Ấ
    {U'\u1EA7', U'\u1EA6'}, // ầ -> Ầ
    {U'\u1EA9', U'\u1EA8'}, // ẩ -> Ẩ
    {U'\u1EAB', U'\u1EAA'}, // ẫ -> Ẫ
    {U'\u1EAD', U'\u1EAC'}, // ậ -> Ậ
    {U'\u0065', U'\u0045'}, // e -> E
    {U'\u00E9', U'\u00C9'}, // é -> É
    {U'\u00E8', U'\u00C8'}, // è -> È
    {U'\u1EBB', U'\u1EBA'}, // ẻ -> Ẻ
    {U'\u1EBD', U'\u1EBC'}, // ẽ -> Ẽ
    {U'\u1EB9', U'\u1EB8'}, // ẹ -> Ẹ
    {U'\u00EA', U'\u00CA'}, // ê -> Ê
    {U'\u1EBF', U'\u1EBE'}, // ế -> Ế
    {U'\u1EC1', U'\u1EC0'}, // ề -> Ề
    {U'\u1EC3', U'\u1EC2'}, // ể -> Ể
    {U'\u1EC5', U'\u1EC4'}, // ễ -> Ễ
    {U'\u1EC7', U'\u1EC6'}, // ệ -> Ệ
    {U'\u0069', U'\u0049'}, // i -> I
    {U'\u00ED', U'\u00CD'}, // í -> Í
    {U'\u00EC', U'\u00CC'}, // ì -> Ì
    {U'\u1EC9', U'\u1EC8'}, // ỉ -> Ỉ
    {U'\u0129', U'\u0128'}, // ĩ -> Ĩ
    {U'\u1ECB', U'\u1ECA'}, // ị -> Ị
    {U'\u006F', U'\u004F'}, // o -> O
    {U'\u00F3', U'\u00D3'}, // ó -> Ó
    {U'\u00F2', U'\u00D2'}, // ò -> Ò
    {U'\u1ECF', U'\u1ECE'}, // ỏ -> Ỏ
    {U'\u00F5', U'\u00D5'}, // õ -> Õ
    {U'\u1ECD', U'\u1ECC'}, // ọ -> Ọ
    {U'\u00F4', U'\u00D4'}, // ô -> Ô
    {U'\u1ED1', U'\u1ED0'}, // ố -> Ố
    {U'\u1ED3', U'\u1ED2'}, // ồ -> Ồ
    {U'\u1ED5', U'\u1ED4'}, // ổ -> Ổ
    {U'\u1ED7', U'\u1ED6'}, // ỗ -> Ỗ
    {U'\u1ED9', U'\u1ED8'}, // ộ -> Ộ
    {U'\u01A1', U'\u01A0'}, // ơ -> Ơ
    {U'\u1EDB', U'\u1EDA'}, // ớ -> Ớ
    {U'\u1EDD', U'\u1EDC'}, // ờ -> Ờ
    {U'\u1EDF', U'\u1EDE'}, // ở -> Ở
    {U'\u1EE1', U'\u1EE0'}, // ỡ -> Ỡ
    {U'\u1EE3', U'\u1EE2'}, // ợ -> Ợ
    {U'\u0075', U'\u0055'}, // u -> U
    {U'\u00FA', U'\u00DA'}, // ú -> Ú
    {U'\u00F9', U'\u00D9'}, // ù -> Ù
    {U'\u1EE7', U'\u1EE6'}, // ủ -> Ủ
    {U'\u0169', U'\u0168'}, // ũ -> Ũ
    {U'\u1EE5', U'\u1EE4'}, // ụ -> Ụ
    {U'\u01B0', U'\u01AF'}, // ư -> Ư
    {U'\u1EE9', U'\u1EE8'}, // ứ -> Ứ
    {U'\u1EEB', U'\u1EEA'}, // ừ -> Ừ
    {U'\u1EED', U'\u1EEC'}, // ử -> Ử
    {U'\u1EEF', U'\u1EEE'}, // ữ -> Ữ
    {U'\u1EF1', U'\u1EF0'}, // ự -> Ự
    {U'\u0079', U'\u0059'}, // y -> Y
    {U'\u00FD', U'\u00DD'}, // ý -> Ý
    {U'\u1EF3', U'\u1EF2'}, // ỳ -> Ỳ
    {U'\u1EF7', U'\u1EF6'}, // ỷ -> Ỷ
    {U'\u1EF9', U'\u1EF8'}, // ỹ -> Ỹ
    {U'\u1EF5', U'\u1EF4'}, // ỵ -> Ỵ
    {U'\u0064', U'\u0044'}, // d -> D
    {U'\u0111', U'\u0110'}, // đ -> Đ
};

static const int kCaseTableSize = 74;

#endif /* CASE_TABLE_H */