# Initial file for Microsoft Visual Studio 2019 Cmake
#	cmake version 3.17.20032601-MSVC_2
#	CMake suite maintained and supported by Kitware (kitware.com/cmake).
#
# cmake -AWin32
# cmake -Ax64

cmake_minimum_required(VERSION 2.8.9)
project (turbohist)
set(SOURCES turbohist.c )
# file(GLOB SOURCES "*.c")
 
add_executable(turbohist ${SOURCES})
