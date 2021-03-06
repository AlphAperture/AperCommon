# λcommon

[![Website](https://img.shields.io/badge/website-%CE%BBcommon-9B599A.svg?style=flat-square)](https://aperlambda.github.io/libraries/lambdacommon/)
![C++](https://img.shields.io/badge/language-C++17-9B599A.svg?style=flat-square)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg?style=flat-square)](https://raw.githubusercontent.com/AperLambda/lambdacommon/master/LICENSE)
[![Version](https://img.shields.io/github/release/AperLambda/lambdacommon.svg?style=flat-square)](https://github.com/AperLambda/lambdacommon/releases)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/981c6234700d413297724d7d759e2dec)](https://www.codacy.com/app/LambdAurora/lambdacommon?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=AperLambda/lambdacommon&amp;utm_campaign=Badge_Grade)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/AperLambda/lambdacommon.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/AperLambda/lambdacommon/context:cpp)
[![Build Status](https://travis-ci.org/AperLambda/lambdacommon.svg?branch=master)](https://travis-ci.org/AperLambda/lambdacommon/)
[![Build status](https://ci.appveyor.com/api/projects/status/aufclhjsvecva6mk?svg=true)](https://ci.appveyor.com/project/LambdAurora/lambdacommon)
![Downloads](https://img.shields.io/github/downloads/AperLambda/lambdacommon/total.svg?style=flat-square)
[![GitHub issues](https://img.shields.io/github/issues/AperLambda/lambdacommon.svg?style=flat-square)](https://github.com/AperLambda/lambdacommon/issues/)

A library written in C++ with common features.

Features: 
 - OS detection.
 - System information (username, user directory, CPU name, memory usage, etc...).
 - Terminal manipulation.
 - Resources management.
 - Basic string manipulation.
 - Basic maths utilities.
 - Graphics:
    * Color manipulation.
    * Very basic scene system.
 - Files manipulation.
 - URI manipulation.
 - and more!

[![Packaging status](https://repology.org/badge/vertical-allrepos/lambdacommon.svg)](https://repology.org/metapackage/lambdacommon)

## Compatibility table

| OS          |  Compatibility |
|-------------|:--------------:|
| Windows     |        ✔       |
| Mac OSX     |        ✔       |
| Linux       |        ✔       |
| Android     |        ⚠       |
| iOS         |        ❌       |
| WebAssembly |        ⚠       |
| FreeBSD     |        ✔       |
| OpenBSD     | ⚠ Not tested ⚠ |
| NetBSD      | ⚠ Not tested ⚠ |
| DragonFly   | ⚠ Not tested ⚠ |
| Solaris     |  ❌ Not tested  |
| Haiku       |        ❌       |

✔: Supported
⚠: Partial support / Not tested
❌: Not working

on: 
 - i686
 - amd64
 - ARMv7
 - ARMv8
 - riscv32
 - riscv64

## Quick example

```cpp
#include <lambdacommon/system/system.h>

using namespace std;
namespace sys = lambdacommon::system;
namespace term = lambdacommon::terminal;

int main()
{
  term::setup();
  term::set_title("λcommon - example");

  cout << "Starting lambdacommon example with " << term::CYAN << "lambdacommon" << term::RESET << " v" << lambdacommon::get_version() << endl;
  cout << endl;
  cout << "Hello " << term::LIGHT_YELLOW << sys::get_user_name() << term::RESET << endl;
  // Sleep for 250ms
  sys::sleep(250);
  cout << endl << "I like your CPU, it is " << term::LIGHT_GREEN << sys::get_cpu_name() << term::RESET << " right?" << endl;
  sys::sleep(1000);
  // Clear all the content of the terminal.
  term::clear();
  cout << "Because I like my creators I want you to see their website!" << endl;
  sys::open_uri("https://aperlambda.github.io/");
  return 0;
}
```

## Installation

### Windows

Download the latest ZIP file of, the compiled binaries and the headers for Windows on [GitHub](https://github.com/AperLambda/lambdacommon/releases/latest). Or install from source.

### Mac OSX

Please install from source.

### Linux

#### ArchLinux (and variants)

Just install [lambdacommon](https://aur.archlinux.org/packages/lambdacommon/) from AUR

#### Others

Please install from source. 

### Android

#### As an app dependency

Include the source files in your project.

#### Termux

Please install from source.

The CMake command will change a bit, please use `cmake -DCMAKE_INSTALL_PREFIX="${PREFIX}" -DCMAKE_BUILD_TYPE="Release" ..` instead.

### BSD systems

Install from source. Other than FreeBSD may not work correctly.

### From source

Build the sources with CMake and make and install with `make install`, and keep the install manifest to allow the uninstallation with `make uninstall`. 

## Use in CMake

Use `Findlambdacommon.cmake` in [LambdaCMakeModules](https://github.com/LambdAurora/lcmm.git) to find λcommon on your computer.
