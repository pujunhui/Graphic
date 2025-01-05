#需求的最低cmake程序版本
cmake_minimum_required(VERSION 3.12)

#本工程的名称
project(Graphic)

#本工程支持的C++版本
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")

file(GLOB copyResources "./assets")
file(COPY ${copyResources} DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(application)
add_subdirectory(gpu)

#本工程所有cpp文件编译链接，生成exe
add_executable(Graphic "main.cpp")

add_executable(testMath "testMath.cpp")

add_executable(testLight "testLight.cpp")

target_link_libraries(Graphic app gpu)

target_link_libraries(testLight app gpu)
