#include <stdio.h>
#include <string.h>

#include "telex.h"

static TelexRule rules[] = {
    {'a', 'a', "â"},
    {'a', 'w', "ă"},
    {'e', 'e', "ê"},
    {'o', 'o', "ô"},
    {'o', 'w', "ơ"},
    {'u', 'w', "ư"},
    {'d', 'd', "đ"},
};

static ToneRule tone_rules[] = {
    {'s', "sac"},
    {'f', "huyen"},
    {'x', "nga"},
    {'r', "hoi"},
    {'j', "nang"},
};

const char *telex_lookup(char base, char mod)
{
  int rules_length = sizeof(rules);

  for (int i = 0; i < rules_length; i++)
  {
    if (rules[i].base == base && rules[i].mod == mod)
    {
      return rules[i].result;
    }
  }

  return NULL;
}

const char *tone_lookup(char key)
{
  int rules_length = sizeof(tone_rules);

  for (int i = 0; i < rules_length; i++)
  {
    if (tone_rules[i].key == key)
    {
      return tone_rules[i].name;
    }
  }

  return NULL;
}

void print_key_event(const KeyEvent *e)
{
  printf("Key pressed: %d", e->keycode);
}