# Toy operating system

This is a toy 32bit operating system based on the [x86 bare bones](http://wiki.osdev.org/Bare_bones) series written in C11 and some assembler.
This OS is a learning exercise to myself and reference to others.

## Build

### Toolchain

You will need to setup a cross compiler toolchain for `gcc` and `binutils` in `$HOME/Tmp/toolchain-i686`.

First setup the environment so `$TARGET`,`$PATH` and `$PREFIX` are initilized.

```
source ./setupenv.sh
```

Then from the same shell run:

```
mkdir -p third_party
cd third_party
wget ftp://ftp.fu-berlin.de/unix/gnu/binutils/binutils-2.27.tar.bz2
atool -x binutils-2.27.tar.bz2
cd bintuitl-2.27
mkdir build
cd build
../configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls
make -j`nproc`
make install
```

This will compile and install binutils for the target platfrom i686. To compile and install GCC 6.2 run the following:

```
mkdir -p third_party
cd third_party
wget ftp://ftp.fu-berlin.de/unix/languages/gcc/releases/gcc-6.2.0/gcc-6.2.0.tar.bz2
atool -x gcc-6.2.0.tar.bz2
cd gcc-6.2.0
mkdir build
cd build
../configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make -j`nproc` all-gcc
make -j`nproc` all-target-libgcc
make install-gcc
make install-target-libgcc
```

### Compile the Kernel

After you have setup the toolchain compile the kernel using:

```
source ./setupenv.sh
make
```

You can launch the kernel using `make qemu` if you have QEMU for `i686` installed.

To compile and run test kernels run:

```
make tests
```

## Things this OS does not have

- A filesystem
- Device drivers (other then VGA maybe)

## Goals

- [x] Simple VGA output
- [x] Virtual memory
- [ ] Task management

## Open questions

- [ ] Effort to port to ARMv7
- [ ] Effort to port to x86_64

