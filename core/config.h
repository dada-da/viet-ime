#ifndef VIETIME_CONFIG_H
#define VIETIME_CONFIG_H

typedef struct
{
  char method[16];
  int enabled;
} Config;

int config_save(const char *path, const Config *c);
int config_load(const char *path, Config *c);

#endif