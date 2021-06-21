# `mtfind2(1)`
![version: 0.2.0](https://img.shields.io/badge/version-0.2.0-blue.svg)
![license: GNU GPL v2](https://img.shields.io/badge/license-GNU_GPL_v2-brightgreen.svg)
![works: in my machine](https://img.shields.io/badge/works-in_my_machine-brightgreen.svg)

## Build Instructions
1. Clone this repository
2. Make sure you've installed common build utilities (`make`, `g++` required)
3. Make sure your compiler is compatible with C++20.

### GNU Make
```shell
git clone https://github.com/anpep/mtfind2
cd mtfind2
make
./mtfind2
```

### CMake
```shell
git clone https://github.com/anpep/mtfind2
cd mtfind2
mkdir out && cd out
cmake -G "Unix Makefiles" .. && make
```

## Open-source code
`mtfind2(1)` is licensed under the GNU General Public License v2.

```
mtfind2(1) -- Multi-thread find utility: the sequel that nobody needs
Copyright (c) 2021 Ángel Pérez <angel.perez7@alu.uclm.es>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
```