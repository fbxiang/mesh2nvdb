cmake_minimum_required(VERSION 3.26)
project(mesh2nvdb LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-fpermissive")

if(MSVC)
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/W4>" "$<$<CONFIG:Release>:$<$<COMPILE_LANGUAGE:CXX>:/O2>>")
else()
    add_compile_options(
        "$<$<CONFIG:Debug>:-g3>" "$<$<CONFIG:Debug>:-O0>"
        "$<$<CONFIG:Release>:-O3>")
endif()

add_compile_definitions("$<$<CONFIG:DEBUG>:_DEBUG>")
add_compile_definitions("$<$<NOT:$<CONFIG:Debug>>:NDEBUG>")

include(zlib)
include(boost)
include(openvdb)

find_package (Python3 COMPONENTS Development.SABIModule Numpy)

file(GLOB_RECURSE SRC src/*.cpp)
Python3_add_library(_C MODULE USE_SABI 3 ${SRC})
target_link_libraries(_C PRIVATE openvdb_static)
target_include_directories(_C PRIVATE ${openvdb_SOURCE_DIR}/nanovdb)
