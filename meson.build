#
 # stm32-libopencm3-meson build
 # Copyright (C) 2017 Amitesh Singh
 #
 # This library is free software; you can redistribute it and/or
 # modify it under the terms of the GNU Lesser General Public
 # License as published by the Free Software Foundation; either
 # version 2.1 of the License, or (at your option) any later version.
 #
 # This library is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 # Lesser General Public License for more details.
 #
 # You should have received a copy of the GNU Lesser General Public
 # License along with this library;
 # if not, see <http://www.gnu.org/licenses/>.
 #


project('stm32f1-project', 'cpp')

compiler_obj = meson.get_compiler('cpp')

size = find_program('arm-none-eabi-size')
objcopy = find_program('arm-none-eabi-objcopy')
stinfo = find_program('st-info')
stflash = find_program('st-flash')

#define libopencm3 path here
libocm3Path = '/home/ami/repos/libopencm3'

#define cpu types here
cputype = 'cortex-m3'
cpudefine = 'STM32F1'
libocm3cputype = 'opencm3_stm32f1'

#Add your source files here
srcfiles = ['main.cpp']


libocm3IncPath = libocm3Path + '/include'
libocm3LibPath = libocm3Path + '/lib'
libocm3LdPath = libocm3Path + '/lib/stm32/f1'

# check for libopencm3 library
libocm3lib_obj = compiler_obj.find_library(libocm3cputype, required: true, dirs: libocm3LibPath)
if libocm3lib_obj.found() == true
   message('Found libopencm3 library at ' + libocm3Path)
else
   error('unable to find libopencm3 library')
endif

incdir = include_directories(libocm3IncPath)

stm32f1cargs = ['-Os',
                '-ggdb',
                '-mcpu=' + cputype,
                '-mthumb',
                '-msoft-float',
                '-std=gnu++14',
                '-Wall',
                '-Wshadow',
                '-ffunction-sections',
                '-fdata-sections',
                '-D' + cpudefine,
                '-fno-exceptions',
#'-I' + libocm3IncPath
                ]

stm32f1linkargs = ['-Os',
                   '-ggdb',
                   '-D' + cpudefine,
                   '-fdata-sections',
                   '-mcpu=' + cputype,
                   '-mthumb',
                   '-msoft-float',
                   '-lc',
                   '-flto',
                   #'-T../libopencm3_f1.ld',
                   '-T' + meson.current_source_dir() + '/libopencm3_f1.ld',
                   '--specs=nosys.specs',
                   '-nostartfiles',
                   '-Wl,--gc-sections',
                   '-L' + libocm3LibPath,
                   '-L' + libocm3LdPath,
#               '-l' + libocm3cputype,
                  ]

p = ['blink', srcfiles, stm32f1cargs, stm32f1linkargs]

exe = executable(p[0], p[1],
                 cpp_args: p[2],
                 link_args: p[3],
                 include_directories : incdir,
                 dependencies: libocm3lib_obj,
                 build_by_default: true)

run_target('hex', command: [objcopy, ['-Obinary', exe.full_path(),
           exe.full_path() + '.hex']], depends: exe)
run_target('size', command: [size, exe.full_path(), '-B'], depends: exe)

# STlink programmer related commands
run_target('probe', command: [stinfo, '--probe'])
run_target('upload', command: [stflash, 'write', exe.full_path() + '.hex', '0x08000000'], depends: exe)

if meson.is_cross_build()
   message('cross compiling for ' + cputype)
   message('     \n                  ninja - generates elf file.
                  ninja hex - generates hex file.
                  ninja upload - upload hex file to stm32 via stlink programmer.
                  ninja probe - probe stlink programmer.
                  ninja size - gives the summary of hex file size.
                  (C) Amitesh Singh ')
endif

