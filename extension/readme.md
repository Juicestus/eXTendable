# Extension

An example eXTendable C++ extension pacakge (the primary feature of eXTendable C++).

## Makefile

The Makefile included in this directory is the Makefile used for all eXTendable C++ extension packages.
Run `make` to build the library to `./bin/pkg.so`, the file that all extendable packages are read from.

## xtpkg

The `xtpkg` file is an index of all the functions exported from the package.
It includes the declaration of how the function is called within a `.xt` file, and the name of the C++ function callback it maps to.

```txt
<decl>:<func>;
test():builtinTest;
print():cXTPrint;
```

## Header files

The header files in `../include` must be accessed by your library source files.