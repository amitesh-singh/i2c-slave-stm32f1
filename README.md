# i2c slave based on stm32
This is an i2c slave based on stm32. This implements a simple math calculator
for doing addition, subtraction and multiplication of given two numbers.

## How to compile

Modify `meson.build` according to your needs.

`libocm3Path` - libopencm3 directory path

You need to issue `./build.sh` once.
 ```shell
$ ./build.sh
$ cd builddir
```
`ninja` - generates elf file.
```shell
$ ninja
```

`ninja hex` - generates hex file.
```shell
$ ninja hex
```
`ninja size` - gives the summary of hex file size.
```shell
$ ninja size
```
`ninja upload` - upload hex file to stm32 via stlink programmer.
```shell
$ ninja upload
```

Refer to [my blog](http://amitesh-singh.github.io/stm32/2018/01/07/making-i2c-slave-using-stm32f103.html) for more details.
