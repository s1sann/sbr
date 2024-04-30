cmake_minimum_required (VERSION 3.1)

project ("BaiscRayTracing")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(INC_DIR D:/third/include)
set(LINK_DIR D:/third/lib)

include_directories(${INC_DIR})

link_directories(${LINK_DIR})

add_executable (BasicRayTracing "main.cpp"  "Scene.h" "Scene.cpp" "geometry.h")


