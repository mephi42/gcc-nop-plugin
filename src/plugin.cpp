#include <memory>
#include <map>
#include <string>

#include <gcc-plugin.h>
#include <plugin-version.h>
#include <tree-pass.h>
#include <context.h>
#include <diagnostic-core.h>
#include <tree.h>
#include <rtl.h>

#if defined(ix86_isa_flags)
#define nop_pass_i386
#elif defined(s390_arch_flags)
#define nop_pass_s390
#elif defined(aarch64_isa_flags)
#define nop_pass_aarch64
#else
#error Unsupported architecture
#endif

namespace {

class nop_pass_config {
public:
  explicit nop_pass_config (std::map<std::string, unsigned long> nop_counts) :
      m_nop_counts (std::move (nop_counts))
  {
  }

  static std::unique_ptr<nop_pass_config>
  parse (struct plugin_name_args *plugin_info)
  {
    std::map<std::string, unsigned long> nop_counts;
    for (int i = 0; i < plugin_info->argc; i++)
      {
        char *str = plugin_info->argv[i].value;
        char *endptr;
        unsigned long count = std::strtoul (str, &endptr, 0);

        if (*endptr)
          {
            error ("invalid nop size: %s", str);
            return std::unique_ptr<nop_pass_config> ();
          }
#if defined(nop_pass_s390)
        if (count & 1)
          {
            error ("invalid nop size: %s, s390 insns are even-sized", str);
            return std::unique_ptr<nop_pass_config> ();
          }
#elif defined(nop_pass_aarch64)
        if (count & 3)
          {
            error ("invalid nop size: %s, aarch64 insns take 4 bytes", str);
            return std::unique_ptr<nop_pass_config> ();
          }
#endif
        nop_counts[plugin_info->argv[i].key] = count;
      }
    return std::unique_ptr<nop_pass_config> (
        new nop_pass_config (std::move (nop_counts)));
  }

  const std::map<std::string, unsigned long> m_nop_counts;
};

class nop_pass : public rtl_opt_pass {
public:
  nop_pass (const pass_data &data,
            gcc::context *ctxt,
            std::unique_ptr<nop_pass_config> config) :
      rtl_opt_pass (data, ctxt),
      m_config (std::move (config))
  {
  }

  unsigned int execute (function *fun) override
  {
    auto it = m_config->m_nop_counts.find (function_name (fun));
    if (it == m_config->m_nop_counts.end ())
      return 0;
    location_t locus = DECL_SOURCE_LOCATION(fun->decl);
    inform (locus, "prepending a %lu-byte nop", it->second);
    char code[128];
#if defined(nop_pass_i386)
    snprintf (code, sizeof (code), ".fill %lu,1,0x90\n", it->second);
#elif defined(nop_pass_s390)
    snprintf (code, sizeof (code), ".fill %lu,1,0x07\n", it->second);
#elif defined(nop_pass_aarch64)
    snprintf (code, sizeof (code),
              ".fill %lu,4,0xd503201f\n", it->second >> 2);
#else
#error Unsupported architecture
#endif
    auto rtx = gen_rtx_ASM_INPUT_loc (VOIDmode,
                                      ggc_strdup (code),
                                      locus);
    MEM_VOLATILE_P (rtx) = 1;
    emit_insn_at_entry (rtx);
    return 0;
  }

private:
  std::unique_ptr<nop_pass_config> m_config;
};

const pass_data pass_data_nop =
    {
        RTL_PASS, /* type */
        "nop", /* name */
        OPTGROUP_NONE, /* optinfo_flags */
        TV_NONE, /* tv_id */
        0, /* properties_required */
        0, /* properties_provided */
        0, /* properties_destroyed */
        0, /* todo_flags_start */
        0, /* todo_flags_finish */
    };

}

int plugin_is_GPL_compatible;

int
plugin_init (struct plugin_name_args *plugin_info,
             struct plugin_gcc_version *version)
{
  if (!plugin_default_version_check (version, &gcc_version))
    {
      error ("gcc_nop_plugin is not compatible with your gcc");
      return 1;
    }
  auto config = nop_pass_config::parse (plugin_info);
  auto nop = new nop_pass (pass_data_nop, g, std::move (config));
  struct register_pass_info info = {
      nop, "pro_and_epilogue", 1, PASS_POS_INSERT_AFTER
  };
  register_callback (plugin_info->base_name,
                     PLUGIN_PASS_MANAGER_SETUP, nullptr, &info);
  return 0;
}
