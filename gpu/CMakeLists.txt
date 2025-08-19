#递归将本文件夹下所有cpp放到GPU中
file(GLOB_RECURSE GPU ./  *.cpp)

#将GPU中所有cpp编译为gpu这个lib库
add_library(gpu ${GPU})
