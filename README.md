# gcc-nop-plugin

gcc plugin, which inserts nop instructions.

# Usage

```
gcc \
    -o test/hello \
    -fplugin=./libgcc_nop_plugin.so \
    -fplugin-arg-libgcc_nop_plugin-main=2 \
    test/hello.c
```

# Links

* https://gcc.gnu.org/wiki/plugins
