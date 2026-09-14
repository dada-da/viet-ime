// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
//
// Sinh tu dong boi tools — khong sua tay.
//
// Cot std_nfc lay tu unicodedata cua Python (bang du lieu Unicode
// chinh thuc), khong go tay. Cot want la ky vong cho vietime::to_nfc,
// do nguoi viet dien trong CASES cua script.
//
// Unicode 15.0.0

#ifndef VIETIME_TESTS_NFC_TABLE_H
#define VIETIME_TESTS_NFC_TABLE_H

struct NfcCase
{
  const char *in;
  const char *want;     // vietime::to_nfc tra ve cai nay
  const char *std_nfc;  // NFC chuan tra ve cai kia
  const char *name;
};

static const NfcCase kNfcCases[] = {
    // U+0065 U+0301 U+0302                     LECH
    //   to_nfc    U+1EBF
    //   NFC chuan U+00E9 U+0302
    {"\x65" "\xCC" "\x81" "\xCC" "\x82", "\xE1" "\xBA" "\xBF", "\xC3" "\xA9" "\xCC" "\x82",
     "A: e + sac + mu"},
    // U+0061 U+0301 U+0306                     LECH
    //   to_nfc    U+1EAF
    //   NFC chuan U+00E1 U+0306
    {"\x61" "\xCC" "\x81" "\xCC" "\x86", "\xE1" "\xBA" "\xAF", "\xC3" "\xA1" "\xCC" "\x86",
     "A: a + sac + trang"},
    // U+0065 U+0302 U+0301                     khop
    //   to_nfc    U+1EBF
    //   NFC chuan U+1EBF
    {"\x65" "\xCC" "\x82" "\xCC" "\x81", "\xE1" "\xBA" "\xBF", "\xE1" "\xBA" "\xBF",
     "A: e + mu + sac (dung thu tu NFD)"},
    // U+0061 U+0306 U+0301                     khop
    //   to_nfc    U+1EAF
    //   NFC chuan U+1EAF
    {"\x61" "\xCC" "\x86" "\xCC" "\x81", "\xE1" "\xBA" "\xAF", "\xE1" "\xBA" "\xAF",
     "A: a + trang + sac (dung thu tu NFD)"},
    // U+0065 U+0301 U+0300                     LECH
    //   to_nfc    U+0065 U+0301 U+0300
    //   NFC chuan U+00E9 U+0300
    {"\x65" "\xCC" "\x81" "\xCC" "\x80", "\x65" "\xCC" "\x81" "\xCC" "\x80", "\xC3" "\xA9" "\xCC" "\x80",
     "B: hai dau thanh"},
    // U+0061 U+0302 U+0306                     LECH
    //   to_nfc    U+0061 U+0302 U+0306
    //   NFC chuan U+00E2 U+0306
    {"\x61" "\xCC" "\x82" "\xCC" "\x86", "\x61" "\xCC" "\x82" "\xCC" "\x86", "\xC3" "\xA2" "\xCC" "\x86",
     "B: hai dau phu"},
    // U+0065 U+0302 U+0301 U+0300              LECH
    //   to_nfc    U+0065 U+0302 U+0301 U+0300
    //   NFC chuan U+1EBF U+0300
    {"\x65" "\xCC" "\x82" "\xCC" "\x81" "\xCC" "\x80", "\x65" "\xCC" "\x82" "\xCC" "\x81" "\xCC" "\x80", "\xE1" "\xBA" "\xBF" "\xCC" "\x80",
     "B: mu + hai thanh"},
    // U+0061 U+0301 U+0301                     LECH
    //   to_nfc    U+0061 U+0301 U+0301
    //   NFC chuan U+00E1 U+0301
    {"\x61" "\xCC" "\x81" "\xCC" "\x81", "\x61" "\xCC" "\x81" "\xCC" "\x81", "\xC3" "\xA1" "\xCC" "\x81",
     "B: hai dau thanh giong het nhau"},
    // U+006E U+0303                            LECH
    //   to_nfc    U+006E U+0303
    //   NFC chuan U+00F1
    {"\x6E" "\xCC" "\x83", "\x6E" "\xCC" "\x83", "\xC3" "\xB1",
     "C: n + nga"},
    // U+0063 U+0327                            LECH
    //   to_nfc    U+0063 U+0327
    //   NFC chuan U+00E7
    {"\x63" "\xCC" "\xA7", "\x63" "\xCC" "\xA7", "\xC3" "\xA7",
     "C: c + moc duoi"},
    // U+0062 U+0302 U+0301                     khop
    //   to_nfc    U+0062 U+0302 U+0301
    //   NFC chuan U+0062 U+0302 U+0301
    {"\x62" "\xCC" "\x82" "\xCC" "\x81", "\x62" "\xCC" "\x82" "\xCC" "\x81", "\x62" "\xCC" "\x82" "\xCC" "\x81",
     "C: b + mu + sac"},
    // U+0061 U+0323 U+0302                     khop
    //   to_nfc    U+1EAD
    //   NFC chuan U+1EAD
    {"\x61" "\xCC" "\xA3" "\xCC" "\x82", "\xE1" "\xBA" "\xAD", "\xE1" "\xBA" "\xAD",
     "D: a + nang + mu (220/230)"},
    // U+0061 U+0302 U+0323                     khop
    //   to_nfc    U+1EAD
    //   NFC chuan U+1EAD
    {"\x61" "\xCC" "\x82" "\xCC" "\xA3", "\xE1" "\xBA" "\xAD", "\xE1" "\xBA" "\xAD",
     "D: a + mu + nang (230/220)"},
    // U+0075 U+031B U+0300                     khop
    //   to_nfc    U+1EEB
    //   NFC chuan U+1EEB
    {"\x75" "\xCC" "\x9B" "\xCC" "\x80", "\xE1" "\xBB" "\xAB", "\xE1" "\xBB" "\xAB",
     "D: u + sung + huyen (216/230)"},
    // U+0075 U+0300 U+031B                     khop
    //   to_nfc    U+1EEB
    //   NFC chuan U+1EEB
    {"\x75" "\xCC" "\x80" "\xCC" "\x9B", "\xE1" "\xBB" "\xAB", "\xE1" "\xBB" "\xAB",
     "D: u + huyen + sung (230/216)"},
    // U+006F U+0302 U+0323                     khop
    //   to_nfc    U+1ED9
    //   NFC chuan U+1ED9
    {"\x6F" "\xCC" "\x82" "\xCC" "\xA3", "\xE1" "\xBB" "\x99", "\xE1" "\xBB" "\x99",
     "D: o + mu + nang (230/220)"},
    // U+006F U+0323 U+0302                     khop
    //   to_nfc    U+1ED9
    //   NFC chuan U+1ED9
    {"\x6F" "\xCC" "\xA3" "\xCC" "\x82", "\xE1" "\xBB" "\x99", "\xE1" "\xBB" "\x99",
     "D: o + nang + mu (220/230)"},
    // (rong)                                   khop
    //   to_nfc    (rong)
    //   NFC chuan (rong)
    {"", "", "",
     "E: chuoi rong"},
    // U+1EBF                                   khop
    //   to_nfc    U+1EBF
    //   NFC chuan U+1EBF
    {"\xE1" "\xBA" "\xBF", "\xE1" "\xBA" "\xBF", "\xE1" "\xBA" "\xBF",
     "E: da la NFC"},
    // U+0301                                   khop
    //   to_nfc    U+0301
    //   NFC chuan U+0301
    {"\xCC" "\x81", "\xCC" "\x81", "\xCC" "\x81",
     "E: dau mo coi"},
    // U+0301 U+0061                            khop
    //   to_nfc    U+0301 U+0061
    //   NFC chuan U+0301 U+0061
    {"\xCC" "\x81" "\x61", "\xCC" "\x81" "\x61", "\xCC" "\x81" "\x61",
     "E: dau mo coi dau chuoi"},
    // U+0064 U+0335                            khop
    //   to_nfc    U+0064 U+0335
    //   NFC chuan U+0064 U+0335
    {"\x64" "\xCC" "\xB5", "\x64" "\xCC" "\xB5", "\x64" "\xCC" "\xB5",
     "E: d + gach ngang khong phai d-stroke"},
    // U+0068 U+0065 U+006C U+006C U+006F       khop
    //   to_nfc    U+0068 U+0065 U+006C U+006C U+006F
    //   NFC chuan U+0068 U+0065 U+006C U+006C U+006F
    {"\x68" "\x65" "\x6C" "\x6C" "\x6F", "\x68" "\x65" "\x6C" "\x6C" "\x6F", "\x68" "\x65" "\x6C" "\x6C" "\x6F",
     "E: ASCII"},
    // U+0054 U+0069 U+1EBF U+006E U+0067 U+0020 U+0056 U+0069 U+1EC7 U+0074 khop
    //   to_nfc    U+0054 U+0069 U+1EBF U+006E U+0067 U+0020 U+0056 U+0069 U+1EC7 U+0074
    //   NFC chuan U+0054 U+0069 U+1EBF U+006E U+0067 U+0020 U+0056 U+0069 U+1EC7 U+0074
    {"\x54" "\x69" "\xE1" "\xBA" "\xBF" "\x6E" "\x67" "\x20" "\x56" "\x69" "\xE1" "\xBB" "\x87" "\x74", "\x54" "\x69" "\xE1" "\xBA" "\xBF" "\x6E" "\x67" "\x20" "\x56" "\x69" "\xE1" "\xBB" "\x87" "\x74", "\x54" "\x69" "\xE1" "\xBA" "\xBF" "\x6E" "\x67" "\x20" "\x56" "\x69" "\xE1" "\xBB" "\x87" "\x74",
     "E: cau NFC"},
};

// 24 ca, trong do 8 ca lech khoi NFC chuan.
// Xem docs/unicode-notes.md de biet vi sao moi cho lech la co chu dich.

#endif /* VIETIME_TESTS_NFC_TABLE_H */