#!/bin/sh

# target for this os
export TARGET=i686-elf
# prefix to locally install our toolchain
export PREFIX="$HOME/Tmp/toolchain-$TARGET"
# adjust PATH for build
export PATH="$PREFIX/bin:$PATH"
