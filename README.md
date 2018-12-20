# gcc-nop-plugin

gcc plugin, which inserts nop instructions.

# Usage

```
g++ \
    -fplugin=libgcc_nop_plugin.dylib \
    -fplugin-arg-libgcc_nop_plugin.dylib-main=2 \
    test/hello.cpp
```

# Links

* https://gcc.gnu.org/wiki/plugins
