#include "check.h"
#include "ime_api.h"
#include "diff_apply.h"
#include <string>

namespace
{

  void pop_codepoints(std::string &doc, unsigned n)
  {
    while (n-- > 0 && !doc.empty())
    {
      size_t i = doc.size() - 1;
      while (i > 0 && (static_cast<unsigned char>(doc[i]) & 0xC0) == 0x80)
        --i;
      doc.erase(i);
    }
  }

  struct Docs
  {
    std::string naive, bridge, shown;
  };

  void apply_both(Docs &d, const VietimeKeyResult &r, char raw_key,
                  const std::string &label)
  {
    const std::string text(r.text, r.text_length);

    pop_codepoints(d.naive, r.backspace_count);
    d.naive += text;

    vietime_hook::Step st = vietime_hook::plan_step(
        d.shown, r.backspace_count, text);
    pop_codepoints(d.bridge, st.diff.backspaces);
    d.bridge += st.diff.insert;
    d.shown = st.shown;

    if (raw_key && !r.key_consumed)
    {
      d.naive += raw_key;
      d.bridge += raw_key;
    }

    check_eq(d.bridge, d.naive, label + " doc");
    check_eq(st.desync, false, label + " desync");
  }

  void run(const std::string &name, const std::string &keys)
  {
    vietime_ctx *ctx = vietime_create();

    Docs d;
    for (size_t i = 0; i < keys.size(); ++i)
    {
      VietimeKeyResult r = vietime_process_key(ctx, keys[i]);
      apply_both(d, r, keys[i], name + " #" + std::to_string(i + 1));
    }
    VietimeKeyResult r = vietime_flush(ctx);
    apply_both(d, r, 0, name + " flush");

    vietime_destroy(ctx);
  }

} // namespace

void test_hook_apply()
{
  run("lap a 64", std::string(64, 'a'));
  run("tieng viet", "tieengs vieejt");
  run("nguoi", "nguoiwf");
  run("tab giua", "tieeng\tvieejt"); // phím ngoài 0x20..0x7E
  run("vuot tran roi cach", std::string(64, 'a') + " tieengs");
}