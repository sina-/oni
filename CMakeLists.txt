# Partly inspired by https://stackoverflow.com/a/8304850
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(oni C CXX)

set(CMAKE_CXX_STANDARD 17)
set(Oni_VERSION_MAJOR 0)
set(Oni_VERSION_MINOR 1)

subdirs(src)