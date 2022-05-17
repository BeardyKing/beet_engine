# Beet Engine

### Requirements

- [CMake - Download](https://cmake.org/download/)
- Windows x64

### Optional
- [Ninja - Download](https://github.com/ninja-build/ninja/releases)

## Clone

```
git clone --recursive https://github.com/BeardyKing/beet_engine.git
cd beet_engine
```

## Build

### CMake Ninja

```
# Path to ninja.exe will need to be a environment variable
# using developer powershell 
# in the root of the repository
mkdir cmake-build-debug
cd cmake-build-debug
cmake -G Ninja ..
cmake --build . --parallel
```

### CLion - *recomeneded*

```
Debug/Release

- Toolchain : Visual Studio
- Generator : Ninja

Toolchains

- Compiler : MSVC 17.x
- Architecture : amd64
```

### CMake Visual Studio

```sh
# In the root of the repository
mkdir cmake-build-debug
cd cmake-build-debug
cmake -G "Visual Studio 17 2022" -A x64 ..
#cmake -G "Visual Studio 17 2019" -A x64 ..
# set "beet" as startup project
```

## License
- Licensed under the MIT license, see [LICENSE.txt](https://github.com/BeardyKing/beet_engine/blob/main/LICENSE) for details.
