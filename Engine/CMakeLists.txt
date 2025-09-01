cmake_minimum_required(VERSION 3.20)

# 引擎库项目
project(JFMEngineLib)

# 收集所有源文件
file(GLOB_RECURSE ENGINE_SOURCES
    "Source/*.cpp"
    "Source/*.c"
)

file(GLOB_RECURSE LAYER_SOURCES
    "Layers/Source/*.cpp"
)

# 收集头文件
file(GLOB_RECURSE ENGINE_HEADERS
    "Include/*.h"
    "Include/*.hpp"
)

file(GLOB_RECURSE LAYER_HEADERS
    "Layers/Include/*.h"
)

# 添加第三方源文件
set(GLAD_SOURCES
    "../ThirdParty/glad/src/glad.c"
)

set(STB_SOURCES
    "../ThirdParty/stb/stb_image_impl.cpp"
)

# 创建引擎库
add_library(JFMEngine SHARED
    ${ENGINE_SOURCES}
    ${LAYER_SOURCES}
    ${GLAD_SOURCES}
    ${STB_SOURCES}
)

# 包含目录
target_include_directories(JFMEngine PUBLIC
    "Include"
    "Layers/Include"
    "../ThirdParty/glad/include"
    "../ThirdParty/glm"
    "../ThirdParty/stb"
)

# 编译定义
target_compile_definitions(JFMEngine PRIVATE
    JFMEngine_EXPORTS
    JFM_BUILD_DLL
    GLFW_INCLUDE_NONE
)

# Debug 模式下的定义
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(JFMEngine PRIVATE JFM_DEBUG)
endif()

# 编译选项
target_compile_options(JFMEngine PRIVATE
    -Wall -Wextra -Wpedantic
    -fcolor-diagnostics
)

# 查找并链接必要的库
find_package(glfw3 REQUIRED)
find_package(assimp QUIET)

target_link_libraries(JFMEngine PUBLIC
        glfw
        assimp::assimp
)

# macOS 框架
if(APPLE)
    target_link_libraries(JFMEngine PUBLIC
        "-framework OpenGL"
    )
endif()

# 设置库输出路径
set_target_properties(JFMEngine PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Debug
)

# 安装规则
install(TARGETS JFMEngine
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY Include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
