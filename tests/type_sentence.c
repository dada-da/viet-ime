// SPDX-FileCopyrightText: 2026 Cao Duc Anh <anhcd.151635@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ime_api.h"

#include <stdio.h>
#include <string.h>

#define SCREEN_BYTES 256

struct screen
{
  char doc[SCREEN_BYTES];
  char pre[SCREEN_BYTES];
};

static int drop_last(char *s, size_t n)
{
  size_t len = strlen(s);

  while (n > 0)
  {
    if (len == 0)
      return 0;
    do
      len--;
    while (len > 0 && ((unsigned char)s[len] & 0xC0) == 0x80);
    n--;
  }
  s[len] = '\0';
  return 1;
}

static int append(char *dst, const char *src, size_t n)
{
  size_t len = strlen(dst);

  if (len + n + 1 > SCREEN_BYTES)
    return 0;
  memcpy(dst + len, src, n);
  dst[len + n] = '\0';
  return 1;
}


static int commit_to_doc(struct screen *sc, const char *text, size_t n)
{
  size_t pre_len = strlen(sc->pre);

  if (pre_len > n || memcmp(sc->pre, text, pre_len) != 0)
  {
    fprintf(stderr, "preedit \"%s\" khong khop phan commit \"%.*s\"\n",
            sc->pre, (int)n, text);
    return 0;
  }
  sc->pre[0] = '\0';
  return append(sc->doc, text, n);
}

static int apply(struct screen *sc, const VietimeKeyResult *r, char key)
{
  if (r->error != VIETIME_OK)
    return 0;

  if (!r->key_consumed)
    return append(sc->doc, &key, 1);

  if (r->text_committed)
    return commit_to_doc(sc, r->text, r->text_length);

  if (!drop_last(sc->pre, r->backspace_count))
    return 0;
  return append(sc->pre, r->text, r->text_length);
}

int main(void)
{
  const char *keys = "Tieengs Vieejt";
  const char *want = "Tiếng Việt";
  struct screen sc;
  vietime_ctx *ctx;
  VietimeKeyResult r;
  const char *k;

  sc.doc[0] = '\0';
  sc.pre[0] = '\0';

  ctx = vietime_create();
  if (ctx == NULL)
  {
    fprintf(stderr, "vietime_create tra NULL\n");
    return 1;
  }

  for (k = keys; *k != '\0'; ++k)
  {
    r = vietime_process_key(ctx, (uint32_t)(unsigned char)*k);
    if (!apply(&sc, &r, *k))
    {
      fprintf(stderr, "vi pham hop dong o phim '%c'\n", *k);
      vietime_destroy(ctx);
      return 1;
    }
  }

  r = vietime_flush(ctx);
  vietime_destroy(ctx);
  if (r.text_committed && !commit_to_doc(&sc, r.text, r.text_length))
    return 1;

  append(sc.doc, sc.pre, strlen(sc.pre));

  printf("go:   %s\nra:   %s\ncan:  %s\n", keys, sc.doc, want);

  if (strcmp(sc.doc, want) != 0)
  {
    printf("FAIL\n");
    return 1;
  }

  printf("PASS\n");
  return 0;
}