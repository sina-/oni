# Windows (Does not work at the moment)
```
mkdir build
cd build 
cmake -G "Visual Studio 15 2017" ..
```

# Linux Debian/Ubuntu
```
sudo apt install libgl1-mesa-dev libglu1-mesa-dev libglfw3-dev libglew-dev
```
# Note on OpenGL drivers on Linux
If mesa installation doesn't work, try https://askubuntu.com/questions/61396/how-do-i-install-the-nvidia-drivers

# Profiling
## CPU
```
valgrind --tool=callgrind --callgrind-out-file=valgrind.out ./oni-core-demo
kcachegrind valigrind.out
```
## Time measurement using gpprof
```
CPUPROFILE=$(pwd)/cpu.profile CPUPROFILE_FREQUENCY=10000 CPUPROFILE_REALTIME=1 LD_PRELOAD=/usr/lib/libprofiler.so ./oni-core-demo
google-pprof --callgrind ./oni-core-demo cpu.profile > gpprof.callgrind.out
kcachegrind ggprof.callgrind.out

```