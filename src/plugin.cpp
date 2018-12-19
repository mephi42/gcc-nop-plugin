#include <iostream>
#include <map>
#include <string>
#include <utility>

#include <gcc-plugin.h>
#include <plugin-version.h>

int plugin_is_GPL_compatible;

int
plugin_init (struct plugin_name_args *plugin_info,
             struct plugin_gcc_version *version)
{
  int i;
  std::map<std::string, std::string> config;

  if (!plugin_default_version_check (version, &gcc_version))
    return 1;
  for (i = 0; i < plugin_info->argc; i++)
    config[plugin_info->argv[i].key] = plugin_info->argv[i].value;
  return 0;
}
