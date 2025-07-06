#需求的最低cmake程序版本
cmake_minimum_required(VERSION 3.12)

#本工程的名称
project(Graphic)

#本工程支持的C++版本
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")

#将assets目录下的文件拷贝到可执行目录下
file(GLOB ASSETS "./assets")
file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(application)
add_subdirectory(gpu)
add_subdirectory(gpu_old)

#本工程所有cpp文件编译链接，生成exe

add_executable(DrawPoint "01_drawPoint.cpp")
target_link_libraries(DrawPoint app gpu_old)

add_executable(DrawLine "02_drawLine.cpp")
target_link_libraries(DrawLine app gpu_old)

add_executable(TestMath "03_testMath.cpp")

add_executable(DrawTriangle "04_drawTriangle.cpp")
target_link_libraries(DrawTriangle app gpu_old)

add_executable(DrawImage "05_drawImage.cpp")
target_link_libraries(DrawImage app gpu_old)

add_executable(UVTexture "06_UVTexture.cpp")
target_link_libraries(UVTexture app gpu_old)

add_executable(Graphic "main.cpp")
target_link_libraries(Graphic app gpu)

add_executable(TestLight "testLight.cpp")
target_link_libraries(TestLight app gpu)

