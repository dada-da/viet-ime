#include "ime_api.h"
#include "key_processor.h"
#include "api_util.h"
#include <string>

struct vietime_ctx
{
  vietime::KeyProcessor kp;
};

extern "C" vietime_ctx *vietime_create(void)
{
  try
  {
    return new vietime_ctx();
  }
  catch (...)
  {
    return nullptr;
  }
}

extern "C" int32_t vietime_destroy(vietime_ctx *ctx)
{
  if (ctx != nullptr)
  {
    delete ctx;
    return VIETIME_OK;
  }

  return VIETIME_NULL_POINTER;
}

extern "C" VietimeKeyResult vietime_process_key(vietime_ctx *ctx, uint32_t c)
{
  VietimeKeyResult key_result = {};

  try
  {
    if (ctx == nullptr)
    {
      key_result.error = VIETIME_NULL_POINTER;

      return key_result;
    }

    key_result.backspace_count = ctx->kp.char_count();

    vietime::KeyResult result = ctx->kp.handle_key(c);

    key_result.key_consumed = result.consumed;

    if (result.has_commit)
    {
      key_result.text_committed = true;
      key_result.backspace_count = 0;

      size_t length = vietime::copy_text(key_result.text, result.commit_text);

      key_result.text_length = length;

      return key_result;
    }

    std::string text_result = ctx->kp.preedit();
    size_t length = vietime::copy_text(key_result.text, text_result);

    key_result.text_length = length;

    return key_result;
  }
  catch (...)
  {
    key_result.error = VIETIME_UNKNOWN;

    return key_result;
  }
}

extern "C" VietimeKeyResult vietime_reset(vietime_ctx *ctx)
{
  VietimeKeyResult key_result = {};

  if (ctx == nullptr)
  {
    key_result.error = VIETIME_NULL_POINTER;

    return key_result;
  }

  try
  {
    if (!ctx->kp.empty())
    {
      key_result.backspace_count = ctx->kp.char_count();
      ctx->kp.reset();
      key_result.key_consumed = false;
    }

    return key_result;
  }
  catch (...)
  {
    key_result.error = VIETIME_UNKNOWN;

    return key_result;
  }
}

extern "C" int32_t vietime_set_method(vietime_ctx *ctx, VietimeInputMethod m)
{
  try
  {
    if (ctx == nullptr)
    {
      return VIETIME_NULL_POINTER;
    }

    vietime::InputMethod method = vietime::METHOD_TELEX;

    if (m == VIETIME_METHOD_VNI)
    {
      method = vietime::METHOD_VNI;
    }

    ctx->kp.set_method(method);

    return VIETIME_OK;
  }
  catch (...)
  {
    return VIETIME_UNKNOWN;
  }
}

extern "C" int32_t vietime_set_tone_placement(vietime_ctx *ctx, VietimeTonePlacement p)
{
  try
  {
    if (ctx == nullptr)
    {
      return VIETIME_NULL_POINTER;
    }

    vietime::TonePlacement placement = vietime::PLACEMENT_MODERN;

    if (p == VIETIME_PLACEMENT_CLASSIC)
    {
      placement = vietime::PLACEMENT_CLASSIC;
    }

    ctx->kp.set_tone_placement(placement);

    return VIETIME_OK;
  }
  catch (...)
  {
    return VIETIME_UNKNOWN;
  }
}

extern "C" uint32_t vietime_get_version(void)
{
  return VIETIME_ABI_VERSION;
}

extern "C" VietimeKeyResult vietime_backspace(vietime_ctx *ctx)
{
  VietimeKeyResult key_result = {};

  if (ctx == nullptr)
  {
    key_result.error = VIETIME_NULL_POINTER;

    return key_result;
  }

  try
  {
    if (!ctx->kp.empty())
    {
      key_result.backspace_count = ctx->kp.char_count();
      key_result.key_consumed = true;
      key_result.text_committed = false;
      ctx->kp.backspace();

      std::string result = ctx->kp.preedit();

      size_t length = vietime::copy_text(key_result.text, result);

      key_result.text_length = length;
    }

    return key_result;
  }
  catch (...)
  {
    key_result.error = VIETIME_UNKNOWN;

    return key_result;
  }
}

extern "C" VietimeKeyResult vietime_flush(vietime_ctx *ctx)
{
  VietimeKeyResult key_result = {};

  if (ctx == nullptr)
  {
    key_result.error = VIETIME_NULL_POINTER;

    return key_result;
  }

  try
  {
    if (ctx->kp.empty())
    {
      key_result.text_committed = false;

      return key_result;
    }

    std::string result = ctx->kp.commit(); // commit() đã tự dọn buffer
    key_result.text_committed = true;
    size_t length = vietime::copy_text(key_result.text, result);
    key_result.text_length = length;

    return key_result;
  }
  catch (...)
  {
    key_result.error = VIETIME_UNKNOWN;

    return key_result;
  }
}