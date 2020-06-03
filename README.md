# RapidBATCH

Programming language, compiler and platform.

## About

RapidBATCH was a scripting language targetting to Windows systems until 2015. In 2008, this successor version 6 of the fine running, last [officially released version 5.1](https://phorward.info/download/MUSEUM/jmksf/rb5/current/) of RapidBATCH was started, but never got to an end. There are several reasons why this, unfortunatelly, happened. (If you are really interested, more about the reasons why can be found [here](https://phorward.info/temp/der_drang_nach_perfektion.pdf),  written in german).

Anyway, this is the repository of the available and original RapidBATCH 6 source code in a state as it was in end of 2009, with some minor maintenance updates.

The current state of the source code here has been merged from various project states and ported to be compiled with the latest version of the [Phorward C/C++ Library](https://github.com/phorward/phorward) and the [UniCC parser generator](https://github.com/phorward/unicc), which are both dependencies. So it slightly works for now and (maybe) is a point to start.

Starting RapidBATCH, both the language and its vision behind, as an open source project, is both considered as a way to grow ideas and to maybe start a worthy successor of version 5.1.

## Building

This repository currently has only one simple Makefile that runs best when [libphorward](https://github.com/phorward/phorward) and [UniCC](https://github.com/phorward/unicc) are installed.

On Linux with gcc or clang, this will work:

```bash
$ git clone https://github.com/phorward/rapidbatch.git
$ cd rapidbatch
$ touch rb_comp.parser.?   # this avoids the requirement of UniCC
$ make
```

After that, invoke the interpreter like this

```bash
$./rb6 test/test1.rb
```

## Examples

Example programs can be found in `test/`.

## Contributions

Contributions, ideas, concepts and code is always welcome!

This is the 2017 ported, slightly running state of a project that stopped in 2009. There are many parts in these sources that are already reflected in the current [Phorward C/C++ Library](https://github.com/phorward/phorward) toolchain and are obsolete here, but need to be rewritten.

## Credits

RapidBATCH is developed and maintained by Jan Max Meyer, Phorward Software Technologies.

## License

This software is an open source project released under the terms and conditions of the MIT license. See the LICENSE file for more information.
