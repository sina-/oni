# Summary
Oni is an open-source game engine build from ground up using ECS and modern C++ mainly focused on building 2D multiplayer games, and it is easily extendible to 3D. The development is driven by the needs of a game I am developing using the engine. The API is undergoing rapid changes as I explore new techniques, but I'm hoping to get to a stable 1.0 API at some point, and as a result there isn't much documentation around the engine code at this point. However there is an example particle-editor (https://github.com/sina-/oni/tree/master/tools/oni-particle-editor) included that show cases the minimum setup needed to get the engine up and running. 

The following is the list of depenedncies needed to compile the engine.

# Linux Debian/Ubuntu
```
sudo apt install gcc-8 		 \
		 g++-8		 \
		 make		 \
		 cmake 		 \
		 libx11-dev	 \
		 xorg-dev	 \
                 libxrandr-dev   \
                 libxinerama-dev \
                 libxcursor-dev  \
                 libxi-dev       \
                 libxxf86vm-dev  \
                 mesa-common-dev

sudo apt install linux-headers-`uname -r`

wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
sudo apt-add-repository 'deb http://apt.llvm.org/buster/ llvm-toolchain-buster-9 main'
sudo apt-add-repository 'deb-src http://apt.llvm.org/buster/ llvm-toolchain-buster-9 main'
sudo apt update
sudo apt install clang-9
sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-9 100
sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-9 100
```
# Build the dependency sub-modules
## Build AntTweakBar
Linux
```
cd lib/anttweakbar
cd src
make -j 12
```
Windows
Use the solution file in `oni/lib/anttweakbar/src/AntTweakBar_VS2012.sln` and run `Debug`, this should generate the binaries under `oni/lib/anttweakbar/lib/debug`.

## Build GLFW
Linux
```
cd lib/glfw
mkdir build
cd build
cmake ..
make -j 12
```
Windows
Open the CMake project from visual studio under `oni/lib/glfw/` then `Build -> Build All`. By default this should build the binaries for `x64-Debug` under `oni/lib/glfw/out/build/x64-Debug/src`
## Build GLEW
Linux
```
cd lib/glew
cd auto
make -j 12
cd ..
make glew.lib -j 12
```
Windows
No action needed, pre-built binaries are bundled with the submodule under `oni/lib/glew/win/lib/Release/x64`
## Build Box2D
Linux
```
cd lib/box2d
./premake gmake
make -C Build -j 12
```
Windows
```
cd lib/box2d
./premake.exe vs2019
```
Open the solution from visual studio and then `Build -> Build Solution`
## Build Enet
```
cd lib/enet
mkdir build
cd build
cmake ..
make -j 12
```
## Build Freetype2
```
cd lib/freetype2
mkdir build-cmake
cd build-cmake
cmake ..
make -j 12
```
## Build Freeimage
```
cd lib/freeimage
# add -Wno-narrowing to CXX flags of the paltform you are compiling for, e.g., Makefile.gnu
CC=clang CXX=clang++ make -j 8
```
# Windows
```
*To be added*
```
# Note on OpenGL drivers on Linux
If mesa installation doesn't work, try https://askubuntu.com/questions/61396/how-do-i-install-the-nvidia-drivers

# Profiling
## CPU
```
valgrind --tool=callgrind --callgrind-out-file=callgrind.out ./oni-core-demo
kcachegrind callgrind.out
```

## CPU Cache
```
valgrind --tool=cachegrind --cachegrind-out-file=cachegrind.out ./oni-core-demo
kcachegrind cachegrind.out
```

## Time measurement using gpprof
```
CPUPROFILE=$(pwd)/cpu.profile CPUPROFILE_FREQUENCY=10000 CPUPROFILE_REALTIME=1 LD_PRELOAD=/usr/lib/libprofiler.so ./oni-core-demo
google-pprof --callgrind ./oni-core-demo cpu.profile > gpprof.callgrind.out
kcachegrind gpprof.callgrind.out

```

## Using CCache
`apt install ccache`
and use `usr/lib/ccache/gcc` and `usr/lib/ccache/g++` as the compilers for the project

