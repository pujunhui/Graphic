# CMake 实用指南

## CMake语法基础

### 变量和作用域规则

#### 变量定义语法

```cmake
set(<variable> <value>... [PARENT_SCOPE])
```

变量展开规则：

- `${VAR}` - 普通变量展开
- `$ENV{VAR}` - 环境变量展开  
- `$<EXPRESSION>` - 生成器表达式

#### 作用域传递公式

设当前作用域为 $S_i$，父作用域为 $S_{i-1}$：

$$
\text{Variable}_{S_{i-1}} = \begin{cases}
\text{Variable}_{S_i} & \text{if PARENT_SCOPE is set} \\
\text{unchanged} & \text{otherwise}
\end{cases}
$$

### 路径处理数学模型

#### 路径规范化算法

对于路径字符串 $P = p_1/p_2/.../p_n$，规范化函数为：

$$\text{normalize}(P) = \text{resolve}(\text{clean}(\text{absolute}(P)))$$

其中：

- $\text{absolute}(P)$：转换为绝对路径
- $\text{clean}(P)$：移除 `.` 和 `..` 组件
- $\text{resolve}(P)$：解析符号链接

## 收集源文件

### 文件匹配模式数学表示

#### GLOB模式语法

对于模式 $M$ 和文件集合 $F$，匹配函数为：

$$\text{GLOB}(M, F) = \{f \in F : \text{match}(f, M)\}$$

其中 `match` 函数遵循以下规则：

- `*` 匹配任意字符序列（除路径分隔符）
- `?` 匹配单个字符
- `[abc]` 匹配字符集中的任一字符
- `**` 递归匹配所有子目录

### 1. 收集src目录下的所有源文件

```cmake
aux_source_directory(src SOURCE_FILES)
```

### 2. 更灵活地控制文件匹配模式，可以使用 `file(GLOB ...)` 命令：

```cmake
file(GLOB SOURCE_FILES
    src/*.cpp
    src/*.c
)
```

### 3. 递归搜索：

```cmake
file(GLOB_RECURSE SOURCE_FILES
    src/*.cpp
    src/*.c
)
```

## 目标依赖关系数学模型

### 依赖图理论

CMake项目可以表示为有向无环图 $G = (V, E)$：

- 顶点集 $V$：所有目标（targets）
- 边集 $E$：依赖关系

对于目标 $t_i$，其依赖集合为：
$$\text{Dependencies}(t_i) = \{t_j : (t_i, t_j) \in E\}$$

构建顺序通过拓扑排序确定：
$$\text{BuildOrder} = \text{TopologicalSort}(G)$$

## 设置项目属性

### 编译器标志累积规则

设全局标志为 $F_g$，目标特定标志为 $F_t$，最终编译标志为：

$$F_{final} = F_g \cup F_t \cup F_{inheritance}$$

其中 $F_{inheritance}$ 表示从依赖目标继承的标志。

### 设置C++标准

```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

### 设置构建类型

```cmake
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
```

## 链接器依赖传播模型

### 传播规则数学表示

对于目标 $A$ 依赖于目标 $B$，属性传播规则为：

$$\text{Property}_A = \text{Property}_A \cup \begin{cases}
\text{Property}_B & \text{if visibility} = \text{PUBLIC} \\
\text{Property}_B & \text{if visibility} = \text{INTERFACE and consuming} \\
\emptyset & \text{if visibility} = \text{PRIVATE}
\end{cases}$$

### 传递性闭包
完整的依赖集合通过传递闭包计算：
$$\text{AllDeps}(t) = \bigcup_{i=0}^{\infty} \text{DirectDeps}^i(t)$$

## 添加依赖库

### 查找并链接系统库
```cmake
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    glfw
)
```

### 添加第三方库（使用FetchContent）
```cmake
include(FetchContent)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        0.9.9.8
)

FetchContent_MakeAvailable(glm)
target_link_libraries(${PROJECT_NAME} glm::glm)
```

## 生成器表达式数学模型

### 条件表达式语法
生成器表达式的通用形式：
$$\text{GenEx} = \$<\text{condition}:\text{true\_value}:\text{false\_value}>$$

求值函数：
$$\text{eval}(\text{GenEx}) = \begin{cases}
\text{true\_value} & \text{if condition is true} \\
\text{false\_value} & \text{if condition is false}
\end{cases}$$

### 常用生成器表达式
```cmake
# 配置相关
$<CONFIG:Debug>           # 当构建类型为Debug时为真
$<$<CONFIG:Debug>:value>  # Debug时展开为value

# 目标属性
$<TARGET_PROPERTY:target,property>

# 布尔运算
$<AND:condition1,condition2>
$<OR:condition1,condition2>
$<NOT:condition>
```

## 设置包含目录
```cmake
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
```

## 添加子目录
```cmake
add_subdirectory(src)
add_subdirectory(tests)
```

## 生成可执行文件和库

### 生成可执行文件
```cmake
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
```

### 生成静态库
```cmake
add_library(${PROJECT_NAME}_lib STATIC ${SOURCE_FILES})
```

### 生成动态库
```cmake
add_library(${PROJECT_NAME}_shared SHARED ${SOURCE_FILES})
```

## 条件编译
```cmake
if(WIN32)
    # Windows特定设置
    target_compile_definitions(${PROJECT_NAME} PRIVATE WINDOWS_BUILD)
elseif(UNIX)
    # Unix/Linux特定设置
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNIX_BUILD)
endif()
```

## 自定义编译选项
```cmake
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" OFF)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
```

## 安装规则
```cmake
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include
)
```
