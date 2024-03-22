cmake_minimum_required(VERSION 3.1)

project(Fractal)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(INC_DIR "D:/cpp/SDL2/include")
set(LINK_DIR "D:/cpp/SDL2/lib/x64")

include_directories(${INC_DIR})
link_directories(${LINK_DIR})

add_executable(Fractal "Fractal.cpp")

target_link_libraries(Fractal PRIVATE SDL2 SDL2main)

add_custom_command(TARGET Fractal POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
                   "${LINK_DIR}/SDL2.dll"  
                   $<TARGET_FILE_DIR:Fractal>)

