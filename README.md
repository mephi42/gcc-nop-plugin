# gcc-nop-plugin

gcc plugin, which inserts nop instructions.

# Usage

```
g++ \
    -fplugin=libgcc_nop_plugin.dylib \
    -fplugin-arg-libgcc_nop_plugin.dylib-_main=2 \
    test/hello.cpp
```

# Links

* https://gcc.gnu.org/onlinedocs/gccint/Plugins.html
