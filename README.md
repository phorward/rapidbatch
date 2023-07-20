# RapidBATCH

A general-purpose scripting language platform for automation.

## About

RapidBATCH is a general-purpose scripting language. From 2001 to 2015, it targetted Windows systems as primary platform, and was sold commercially as shareware.

In 2008, this successor version 6 of RapidBATCH was started, but never got to an end. There are several reasons why this, unfortunatelly, happened.

So, this is the repository of the available and original RapidBATCH 6 source code in a state as it was in end of 2009, with some minor maintenance updates. 

The repository also contains all stuff relating the last, officially released version 5.1 and a planned version 5.2 of RapidBATCH, see [museum](museum) folder for details.

## Building

This repository currently has only one simple Makefile 

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

This project is not actively maintained anymore.<br>
Feel free to fork and extend in on your own, if wanted.

## Legacy version

Any stuff relating the old version RapidBATCH 5 can be found in the [museum](museum)-folder.

## Credits

RapidBATCH was developed, maintained and distributed by Jan Max Meyer, Phorward Software Technologies (formerly known as J.M.K S.F. Software Technologies).

## License

The stuff in this repository is released under the terms and conditions of the MIT license. See the LICENSE file for more information.
