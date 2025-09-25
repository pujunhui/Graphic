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

add_executable(01_drawPoint "01_drawPoint.cpp")
target_link_libraries(01_drawPoint app gpu_old)

add_executable(02_drawLine "02_drawLine.cpp")
target_link_libraries(02_drawLine app gpu_old)

add_executable(03_testMath "03_testMath.cpp")

add_executable(04_drawTriangle "04_drawTriangle.cpp")
target_link_libraries(04_drawTriangle app gpu_old)

add_executable(05_drawImage "05_drawImage.cpp")
target_link_libraries(05_drawImage app gpu_old)

add_executable(06_uvTexture "06_uvTexture.cpp")
target_link_libraries(06_uvTexture app gpu_old)

add_executable(07_bilinear "07_bilinear.cpp")
target_link_libraries(07_bilinear app gpu_old)

add_executable(08_textureWrap "08_textureWrap.cpp")
target_link_libraries(08_textureWrap app gpu_old)

add_executable(09_transform "09_transform.cpp")
target_link_libraries(09_transform app gpu_old)

add_executable(10_vao "10_vao.cpp")
target_link_libraries(10_vao app gpu)

add_executable(11_draw "11_draw.cpp")
target_link_libraries(11_draw app gpu)

add_executable(Graphic "main.cpp")
target_link_libraries(Graphic app gpu)

add_executable(TestLight "testLight.cpp")
target_link_libraries(TestLight app gpu)

