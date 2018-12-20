#include <map>
#include <string>
#include <utility>

#include <gcc-plugin.h>
#include <plugin-version.h>
#include <tree-pass.h>
#include <context.h>
#include <diagnostic-core.h>
#include <tree.h>
#include <rtl.h>

namespace {

class nop_pass : public rtl_opt_pass {
public:
  nop_pass (const pass_data &data,
            gcc::context *ctxt,
            struct plugin_name_args *plugin_info) :
      rtl_opt_pass (data, ctxt)
  {
    for (int i = 0; i < plugin_info->argc; i++)
      m_config[plugin_info->argv[i].key] = atoi (plugin_info->argv[i].value);
  }

  unsigned int execute (function *fun) override
  {
    auto it = m_config.find (function_name (fun));
    if (it == m_config.end ())
      return 0;
    location_t locus = DECL_SOURCE_LOCATION(fun->decl);
    inform (locus, "prepending a %d-byte nop", it->second);
    char code[128];
    snprintf (code, sizeof (code), ".fill %d,1,0x90\n", it->second);
    auto rtx = gen_rtx_ASM_INPUT_loc (VOIDmode,
                                      ggc_strdup (code),
                                      locus);
    MEM_VOLATILE_P (rtx) = 1;
    emit_insn_at_entry (rtx);
    return 0;
  }

private:
  std::map<std::string, int> m_config;
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
  auto nop = new nop_pass (pass_data_nop, g, plugin_info);
  struct register_pass_info info = {
      nop, "pro_and_epilogue", 1, PASS_POS_INSERT_AFTER
  };
  register_callback (plugin_info->base_name,
                     PLUGIN_PASS_MANAGER_SETUP, nullptr, &info);
  return 0;
}
