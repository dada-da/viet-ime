#include <stdio.h>
#include <string.h>

#include "config.h"

int config_save(const char *path, const Config *c)
{
  FILE *f = fopen(path, "w");

  if (f == NULL)
  {
    perror("fopen");

    return 0;
  }

  fprintf(f, "method=%s\n", c->method);
  fprintf(f, "enabled=%d\n", c->enabled);

  fclose(f);

  return 1;
}

int config_load(const char *path, Config *c)
{
  FILE *f = fopen(path, "r");

  if (f == NULL)
  {
    perror("fopen");

    return 0;
  }

  char line[256];

  while (fgets(line, sizeof(line), f) != NULL)
  {
    char extract_method[17];
    int extract_enabled;

    if (sscanf(line, "method=%16s", extract_method) == 1)
    {
      strcpy(c->method, extract_method);
    }
    else if (sscanf(line, "enabled=%d", &extract_enabled) == 1)
    {
      c->enabled = extract_enabled;
    }
  }

  return 1;
}