![logo](logo.png)

C++ implementation of a simple programming language. Designed to be easily extendable with C++.

## Install

Use GitHub releases to install the [newest build](https://github.com/Juicestus/eXTendable/releases/tag/v1.0.2). Download the zipped installer for your operating system and open the installer.
After the installation is complete, open CMD on Windows or Terminal on Mac and type `xt`.
If you get the message `Usage: xt <file.xt>`, the installation was successful and you can use the compiler.

## Syntax

Very similar to JavaScript. Full documentation coming soon!

### Hightlighting in VSCode

Open the command palette and run "Change Language Mode". Select "Configure File Association" and select JavaScript. This is the temporary solution until I release a VSCode extension.

I am also coming out with a VIM plugin as well.

## Compile

Use Make. Designed for UNIX. Windows compatability is an afterthought.

To compile the language with Make, run ```make```.

To run some example code using the binary, run ```bin/xt examples/helloworld.xt```.

## Contributing

Use pull requests. Format your code using clang-format as specified in ```.clang-format```.
