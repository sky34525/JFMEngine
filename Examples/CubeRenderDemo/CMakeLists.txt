# CubeRenderDemo - 立方体渲染演示

# 设置项目名称
set(TARGET_NAME CubeRenderDemo)

# 收集源文件
file(GLOB_RECURSE SOURCES "*.cpp" "*.h")

# 创建可执行文件
add_executable(${TARGET_NAME} ${SOURCES})

# 链接引擎库
target_link_libraries(${TARGET_NAME} JFMEngine)

# 设置包含目录
target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/Engine/Include
    ${CMAKE_SOURCE_DIR}/ThirdParty/glfw/include
    ${CMAKE_SOURCE_DIR}/ThirdParty/glad/include
    ${CMAKE_SOURCE_DIR}/ThirdParty/glm
)

# 设置输出目录
set_target_properties(${TARGET_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# 复制资源文件
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Engine/Assets
        $<TARGET_FILE_DIR:${TARGET_NAME}>/Assets
)