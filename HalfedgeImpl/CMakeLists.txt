cmake_minimum_required(VERSION 3.15)
project(HalfedgeImpl)

set(CMAKE_CXX_STANDARD 14)

add_executable(HalfedgeImpl main.cpp PolyMesh.h PolyMesh.cpp IOManager.h IOManager.cpp)