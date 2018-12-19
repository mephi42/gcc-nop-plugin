#include <gcc-plugin.h>
#include <plugin-version.h>

int plugin_is_GPL_compatible;

int
plugin_init (struct plugin_name_args *plugin_info,
             struct plugin_gcc_version *version)
{
  (void) plugin_info;
  if (!plugin_default_version_check (version, &gcc_version))
    return 1;
  return 0;
}
