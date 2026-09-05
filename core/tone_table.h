#ifndef TONE_TABLE_H
#define TONE_TABLE_H

enum Tone
{
  TONE_NONE = -1,
  TONE_SAC = 0,   // á
  TONE_HUYEN = 1, // à
  TONE_HOI = 2,   // ả
  TONE_NGA = 3,   // ã
  TONE_NANG = 4,  // ạ
};

Tone tone_from_telex(char key);
Tone tone_from_vni(char key);

char32_t apply_tone_to_vowel(char32_t vowel, Tone tone);

#endif /* TONE_TABLE_H */