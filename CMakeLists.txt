cmake_minimum_required(VERSION 3.20)
project(JFMEngine VERSION 1.0.0 LANGUAGES CXX C)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Build/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Build/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Build/${CMAKE_BUILD_TYPE})

# 全局编译选项
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 查找依赖
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp QUIET)
if(NOT assimp_FOUND)
    message(WARNING "Assimp not found. Install with: brew install assimp")
endif()

# 添加子项目
add_subdirectory(Engine)
add_subdirectory(Examples)

# 可选：添加测试
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    add_subdirectory(Tests)
endif()

# 安装配置
install(DIRECTORY Engine/Include DESTINATION include)
install(TARGETS JFMEngine DESTINATION lib)

# 创建配置文件
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "JFMEngineConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
