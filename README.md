# RapidBATCH

(Former) programming language, compiler and platform.

## About

In the past, RapidBATCH was a scripting language targetting to Windows systems. In 2008, the successor version 6 of the great running version 5.1 of RapidBATCH was started, but never got to an end. There are several reasons why this, unfortunatelly, happened. (If you are really interested, more about the reasons why can be found [here](https://phorward.info/temp/der_drang_nach_perfektion.pdf),  written in german).

Anyway, this is the repository of the available and original RapidBATCH 6 source code in a state as it was in end of 2009. After that, nothing more happened.

The current 2017 state of the source code has been merged from various project states and ported to be compiled with the latest version of [libphorward](https://github.com/phorward/phorward) and [UniCC](https://github.com/phorward/unicc), which are both dependencies. So it slightly works for now and (maybe) is a point to start.

Starting RapidBATCH, both the language and its vision behind, as an open source project, is both considered as a way to grow ideas and to maybe start a worthy successor of the [last officially released version](https://phorward.info/download/MUSEUM/jmksf/rb5/current/).

## Building

This repository currently has only one simple Makefile that runs best when [libphorward](https://github.com/phorward/phorward) and [UniCC](https://github.com/phorward/unicc) are installed.

On Linux with gcc or clang, this will work:

```bash
# setup a project dir
mkdir dev
cd dev

# libphorward
git clone https://github.com/phorward/phorward.git
cd phorward
make -f Makefile.gnu
cd ..

# unicc
git clone https://github.com/phorward/unicc.git
cd unicc
make -f Makefile.gnu
cd ..

# rapidbatch
git clone https://github.com/phorward/rapidbatch.git
cd rapidbatch
make

# Run test file
./rb6 test/test1.rb
```

## Examples

Example programs are in test/. Some of the programs there run, some are freezing, so there is still much work to do.

## Contributions

Contributions, ideas, concepts and code is always welcome!

This is the 2017 ported, slightly running state of a project that stopped in 2009. There are many parts in these sources that are already reflected in the current [libphorward](https://github.com/phorward/phorward) toolchain and are obsolete here, but have to be rewritten. This can be a good place to start from. These are array processing functions, linked lists, hash tables (the symbol table, for example!) and many of the string and regex processing function.

But anyway, its better to have it saved here GitHub and released to the public, instead of forgetting it on an encrypted, old device where somebody in some time will forget the last password to access it.

Ideas will grow, maybe new ways will start. The playground is open!

## Credits

RapidBATCH is developed and maintained by Jan Max Meyer, Phorward Software Technologies.

Some other projects by the author are:

- [libphorward](https://github.com/phorward/phorward), a free toolkit for parser development, lexical analysis, regular expressions and more.
- [UniCC](https://github.com/phorward/unicc), a universal, target-language independent LALR(1) parser generator.
- [pynetree](https://github.com/phorward/pynetree), a light-weight parsing toolkit written in pure Python.
- [JS/CC](https://jscc.brobston.com), the JavaScript parser generator.

## License

This software is an open source project released under the terms and conditions of the MIT license. See the LICENSE file for more information.
