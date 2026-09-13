#include "ime_api.h"

int main(void)
{
  uint32_t c = 0x75;
  VietimeKeyResult result;
  vietime_ctx *ctx;

  ctx = vietime_create();

  result = vietime_process_key(ctx, c);

  vietime_reset(ctx);

  vietime_set_method(ctx, VIETIME_METHOD_TELEX);

  vietime_set_tone_placement(ctx, VIETIME_PLACEMENT_MODERN);

  vietime_backspace(ctx);

  vietime_flush(ctx);

  vietime_destroy(ctx);

  if (VIETIME_ABI_VERSION != vietime_get_version())
  {
    return 1;
  }

  if (result.error)
  {
    return result.error;
  }

  return 0;
}