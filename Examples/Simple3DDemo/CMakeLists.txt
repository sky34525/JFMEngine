cmake_minimum_required(VERSION 3.20)

project(Simple3DDemo)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加可执行文件
add_executable(Simple3DDemo Simple3DDemo.cpp)

# 链接JFMEngine库
target_link_libraries(Simple3DDemo PRIVATE JFMEngine)

# 设置包含目录
target_include_directories(Simple3DDemo PRIVATE
    ${CMAKE_SOURCE_DIR}/Engine/Include
    ${CMAKE_SOURCE_DIR}/ThirdParty/glad/include
    ${CMAKE_SOURCE_DIR}/ThirdParty/glm
)

# 设置输出目录
set_target_properties(Simple3DDemo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
