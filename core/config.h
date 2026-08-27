#ifndef VIETIME_CONFIG_H
#define VIETIME_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif
  typedef struct
  {
    char method[16];
    int enabled;
  } Config;

  int config_save(const char *path, const Config *c);
  int config_load(const char *path, Config *c);

#ifdef __cplusplus
}
#endif

#endif