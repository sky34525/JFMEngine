# ModelRenderDemo - 模型渲染演示
cmake_minimum_required(VERSION 3.20)

# 创建可执行文件
add_executable(ModelRenderDemo
    ModelRenderDemo.cpp
)

# 链接JFM引擎
target_link_libraries(ModelRenderDemo PRIVATE JFMEngine)

# 包含目录
target_include_directories(ModelRenderDemo PRIVATE
    ../../Engine/Include
    ../../ThirdParty/glad/include
    ../../ThirdParty/glm
    ../../ThirdParty/spdlog/include
    ../../ThirdParty/assimp/include
)

# 设置输出目录
set_target_properties(ModelRenderDemo
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# 设置工作目录，确保能找到资源文件
if(WIN32)
    set_target_properties(ModelRenderDemo
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()