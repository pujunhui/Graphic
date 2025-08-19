# 收集src目录下的所有源文件
```
aux_source_directory(src SOURCE_FILES)
```

# 更灵活地控制文件匹配模式，可以使用 `file(GLOB ...)` 命令：
```
file(GLOB SOURCE_FILES 
    src/*.cpp
    src/*.c
)
```

# 递归搜索：
```
file(GLOB_RECURSE SOURCE_FILES 
    src/*.cpp 
    src/*.c
)
```