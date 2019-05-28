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

For C++ code, use non-mangled names.

# CMake settings

* `CMAKE_TARGET_C_COMPILER`: compiler to build `gcc-nop-plugin` for.
* `SUFFIX`: library name suffix. Useful when building multiple
  `gcc-nop-plugin` versions for different compilers. Do not use `-`
  character: this confuses the gcc command line parser.

# Links

* https://gcc.gnu.org/wiki/plugins
