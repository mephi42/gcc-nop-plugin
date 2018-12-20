#include <map>
#include <string>
#include <utility>

#include <gcc-plugin.h>
#include <plugin-version.h>
#include <tree-pass.h>
#include <context.h>
#include <diagnostic-core.h>
#include <tree.h>

namespace {

class nop_pass : public rtl_opt_pass {
public:
  nop_pass (const pass_data &data,
            gcc::context *ctxt,
            struct plugin_name_args *plugin_info) :
      rtl_opt_pass (data, ctxt)
  {
    int i;

    for (i = 0; i < plugin_info->argc; i++)
      m_config[plugin_info->argv[i].key] = atoi (plugin_info->argv[i].value);
  }

  virtual unsigned int execute (function *fun)
  {
    Config::iterator it = m_config.find (function_name (fun));
    if (it == m_config.end ())
      return 0;
    inform (DECL_SOURCE_LOCATION(fun->decl), "adding a %d-byte nop", it->second);
    return 0;
  }

private:
  typedef std::map<std::string, int> Config;
  Config m_config;
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
  opt_pass *nop = new nop_pass (pass_data_nop, g, plugin_info);
  struct register_pass_info info = {
      nop, "final", 1, PASS_POS_INSERT_BEFORE
  };

  if (!plugin_default_version_check (version, &gcc_version))
    {
      error ("gcc_nop_plugin is not compatible with your gcc");
      return 1;
    }
  register_callback (
      plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &info);
  return 0;
}
