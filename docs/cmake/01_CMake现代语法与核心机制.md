# CMake 现代语法与核心机制系统教程

CMake 是现代 C/C++ 跨平台工程构建的事实标准。本教程全面系统地梳理现代 CMake（3.12+ 乃至 3.20+）的核心语法、数学/逻辑模型、Target-Centric 设计哲学与工程依赖管理，方便图形学及 C++ 开发者系统学习与查阅。

---

## 0. CMake 极速起步与标准工作流

### 0.1 最小可运行示例
新建项目目录并在其中创建 `CMakeLists.txt` 与 `main.cpp`：
```cmake
cmake_minimum_required(VERSION 3.12)
project(HelloWorld LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(HelloWorld main.cpp)
```

### 0.2 标准跨平台构建命令行工作流
现代 CMake 推荐**源码外构建（Out-of-Source Build）**，避免生成的大量构建中间文件污染源代码目录：

```bash
# 1. 配置与生成阶段 (Configure & Generate)
# -B 指定构建目录，-S 指定源码目录（默认当前目录）
cmake -B build -S .

# 2. 编译阶段 (Build)
# 自动调用对应平台的构建工具（如 MSVC MSBuild、Ninja、GNU Make），跨平台命令完全统一
cmake --build build --config Release

# 3. 运行产物
./build/Release/HelloWorld.exe    # Windows MSVC
./build/HelloWorld                # Linux / macOS Ninja
```

### 0.3 切换生成器（Generator）
- 使用 Visual Studio：`cmake -B build -G "Visual Studio 17 2022" -A x64`
- 使用极速构建工具 Ninja：`cmake -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release`

---

## 1. CMake 变量系统与作用域模型

### 1.1 变量定义与展开
```cmake
set(MY_VAR "Hello CMake")    # 普通变量定义
set(MY_LIST a b c d)         # 列表变量（分号分隔的分词字符串 "a;b;c;d"）
```
- **普通变量展开**：`${MY_VAR}`
- **环境变量读取/设置**：`$ENV{PATH}`、`set(ENV{MY_ENV} "value")`
- **缓存变量（Cache Variable）**：存储在 `CMakeCache.txt` 中，通常用于用户配置选项：
  ```cmake
  set(ENABLE_DEBUG ON CACHE BOOL "Enable debug logging")
  option(BUILD_TESTS "Build test suite" ON) # option 是 bool 缓存变量的简写
  ```

### 1.2 作用域传递规则（Scope）
CMake 的作用域是树状继承的：
- 父目录调用 `add_subdirectory(child)` 时，子目录会**复制一份**当前父作用域的变量。
- 子目录内部修改变量默认**仅在当前作用域生效**，不会影响父目录。
- 若需向父作用域传递变量，必须显式添加 `PARENT_SCOPE`：
  ```cmake
  set(RESULT_LIB "MyEngine" PARENT_SCOPE)
  ```

---

## 2. 现代 CMake 的核心：Target-Centric（面向目标）设计

在现代 CMake（3.0+）中，**严禁滥用全局命令**（如 `include_directories()`, `link_directories()`, `add_definitions()` 等），因为它们会污染工程树下的所有目标，导致模块耦合。
现代 CMake 的核心法则是：**一切围绕目标（Target）展开**。

### 2.1 目标类型
```cmake
add_executable(my_game main.cpp)            # 可执行文件目标
add_library(math_engine STATIC math.cpp)    # 静态库目标 (.lib / .a)
add_library(renderer SHARED render.cpp)     # 动态库目标 (.dll / .so)
add_library(header_only INTERFACE)          # 纯头文件接口库 (如 glm, stb_image)
```

### 2.2 属性可见性（Visibility Specifiers）
现代 CMake 通过三种可见性限定词控制头文件搜索路径、编译标志和依赖库的传播：

| 限定词 | 当前目标自身使用？ | 依赖此目标的使用者（Consumer）继承？ | 典型场景 |
| :--- | :---: | :---: | :--- |
| **`PRIVATE`** | **是** | **否** | 内部实现私有的头文件与依赖库（如内部引用的算法库） |
| **`INTERFACE`**| **否** | **是** | 纯头文件库、向外部调用者暴露的 API 头文件与公共宏 |
| **`PUBLIC`**   | **是** | **是** | 既在目标内部代码中使用，又在其暴露的头文件函数签名中使用 |

#### 属性传播数学模型
设目标 $A$ 依赖于目标 $B$（`target_link_libraries(A PUBLIC/PRIVATE B)`）：
$$\text{Property}_A \longleftarrow \text{Property}_A \cup \begin{cases} 
\text{Property}_B, & \text{if Visibility}(B) \in \{\text{PUBLIC}, \text{INTERFACE}\} \\
\emptyset, & \text{if Visibility}(B) = \text{PRIVATE}
\end{cases}$$
传递性闭包计算：所有使用者递归继承其直接依赖与间接依赖的 `PUBLIC` / `INTERFACE` 属性集合。

---

## 3. 包含路径、编译选项与条件编译

### 3.1 目标级包含目录（Include Directories）
```cmake
target_include_directories(renderer
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
```

### 3.2 编译器标志与 C++ 标准
```cmake
# 针对特定目标配置 C++ 标准
set_target_properties(renderer PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# 目标级编译宏
target_compile_definitions(renderer
    PUBLIC
        GRAPHIC_VERSION_MAJOR=1
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG_MODE=1>
)
```

### 3.3 操作系统与平台条件分支
```cmake
if(WIN32)
    target_compile_definitions(my_app PRIVATE OS_WINDOWS)
    target_compile_options(my_app PRIVATE /utf-8 /W4)
elseif(APPLE)
    target_compile_definitions(my_app PRIVATE OS_MACOS)
elseif(UNIX)
    target_compile_definitions(my_app PRIVATE OS_LINUX)
    target_compile_options(my_app PRIVATE -Wall -Wextra -O3)
endif()
```

---

## 4. 生成器表达式（Generator Expressions）

生成器表达式在 **构建阶段（Build Time，即实际编译执行时）** 求值，而非 CMake 配置阶段。通用语法为：
$$\$<CONDITION:TRUE\_VALUE:FALSE\_VALUE>$$

### 4.1 常用生成器表达式分类
1. **多配置构建类型（Debug/Release 区分）**：
   - `$<CONFIG:Debug>`：当当前处于 Debug 构建时返回 1。
   - `$<$<CONFIG:Debug>:-g>`：仅在 Debug 模式下启用调试符号。
   - `$<$<CONFIG:Release>:-O3>`：仅在 Release 模式下启用最高级优化。
2. **接口解耦**：
   - `$<BUILD_INTERFACE:...>`：源码工程构建期间生效。
   - `$<INSTALL_INTERFACE:...>`：安装打包后供第三方项目引用时生效。
3. **布尔逻辑**：
   - `$<AND:cond1,cond2>`
   - `$<NOT:cond>`

---

## 5. 第三方依赖管理体系

在大型项目中，如何优雅地集成第三方库（如 Assimp、GLFW、GLM、stb_image）？

### 5.1 `find_package`（查找系统已安装库）
```cmake
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

target_link_libraries(my_app PRIVATE OpenGL::GL Threads::Threads)
```

### 5.2 `FetchContent`（自动从 Git 仓库拉取源码）
现代 CMake 推荐的最佳实践，构建时自动克隆源码并无缝转换为子项目：
```cmake
include(FetchContent)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        0.9.9.8
)

FetchContent_MakeAvailable(glm)
target_link_libraries(my_app PRIVATE glm::glm)
```

### 5.3 导入外部预编译二进制库（`IMPORTED`）
对于未提供 CMake 源码或二进制打包的第三方库（如预编译好的静态库与动态库），声明导入目标：
```cmake
add_library(assimp STATIC IMPORTED)
set_target_properties(assimp PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/assimp/lib/assimp-vc143-mtd.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/assimp/include"
)
target_link_libraries(my_app PRIVATE assimp)
```

---

## 6. 构建类型与安装导出

### 6.1 构建类型控制
```cmake
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
```
- `Debug`：无优化，包含全量调试符号。
- `Release`：最高编译器优化（`/O2` 或 `-O3`），去除断言与符号。
- `RelWithDebInfo`：开启优化的同时保留调试符号（排查线上性能瓶颈首选）。

### 6.2 规则安装与打包（`install`）
```cmake
install(TARGETS my_app renderer
    RUNTIME DESTINATION bin        # exe 和 dll
    LIBRARY DESTINATION lib        # so 动态库
    ARCHIVE DESTINATION lib        # a 和 lib 静态库
)

install(DIRECTORY include/ DESTINATION include) # 安装公共头文件
```
通过上述标准命令，执行 `cmake --install .` 即可自动将工程产物组织为可交付分发的规范目录结构。
