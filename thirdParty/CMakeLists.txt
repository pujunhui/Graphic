cmake_minimum_required(VERSION 3.12)

# 设置第三方库的路径
set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# 创建 assimp 库
add_library(assimp STATIC IMPORTED GLOBAL)
set_target_properties(assimp PROPERTIES
    IMPORTED_LOCATION "${THIRD_PARTY_DIR}/assimp/lib/assimp-vc143-mtd.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${THIRD_PARTY_DIR}/assimp/include"
)
