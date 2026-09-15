#include "ime_api.h"
#include "key_processor.h"
#include "api_util.h"
#include <string>

static_assert(VIETIME_MAX_CODE_POINT * 3 < VIETIME_MAX_TEXT_BYTES,
              "preedit co the tran text[]");

static_assert(vietime::kDefaultMaxLen == VIETIME_MAX_CODE_POINT,
              "tran trong core lech voi tran cong bo ra C API");

struct vietime_ctx
{
  vietime::KeyProcessor kp = vietime::KeyProcessor(VIETIME_MAX_CODE_POINT);
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

    if (c == 0)
    {
      key_result.error = VIETIME_INVALID_KEY;

      return key_result;
    }

    key_result.backspace_count = ctx->kp.char_count();

    if (c < 0x20 || c > 0x7E || ctx->kp.char_count() >= VIETIME_MAX_CODE_POINT)
    {
      if (!ctx->kp.empty())
      {
        std::string text_result = ctx->kp.commit();
        size_t length = vietime::copy_text(key_result.text, text_result);
        key_result.text_length = length;
        key_result.text_committed = true;
      }

      key_result.key_consumed = false;

      return key_result;
    }

    vietime::KeyResult result = ctx->kp.handle_key(c);

    key_result.key_consumed = result.consumed;

    if (!key_result.key_consumed)
    {
      key_result.backspace_count = 0;

      return key_result;
    }

    if (result.has_commit)
    {
      key_result.text_committed = true;

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

    key_result.backspace_count = ctx->kp.char_count();
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