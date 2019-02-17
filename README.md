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
sudo apt-add-repository 'deb http://apt.llvm.org/stretch/ llvm-toolchain-stretch-7 main'
sudo apt-add-repository 'deb-src http://apt.llvm.org/stretch/ llvm-toolchain-stretch-7 main'
sudo apt update
sudo apt install clang-7
sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-7 100
sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-7 100
```
# Build the dependency sub-modules
## Build GLFW
```
cd lib/glfw
mkdir build
cd build
cmake ..
make
```
## Build GLEW
```
cd lib/glew
cd auto
make
cd ..
make glew.lib
```
## Build Box2D
```
cd lib/box2d
./premake gmake
make -C Build
```
## Build Enet
```
cd lib/enet
mkdir build
cd build
cmake ..
make
```
## Build Freetype2
```
cd lib/freetype2
mkdir build-cmake
cd build-cmake
cmake ..
make
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

