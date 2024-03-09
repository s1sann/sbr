cmake_minimum_required (VERSION 3.1)

project ("RayTracingInCornellBox")

set(INC_DIR D:/third/include)
set(LINK_DIR D:/third/lib)

include_directories(${INC_DIR})

link_directories(${LINK_DIR})

add_executable (RayTracingInCornellBox "main.cpp" "svpng.inc" "Scene.h" "Scene.cpp")


