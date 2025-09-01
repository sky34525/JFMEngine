cmake_minimum_required(VERSION 3.20)

# 示例项目
project(JFMEngineExamples)

# 添加子目录

add_subdirectory(Simple3DDemo)
add_subdirectory(ModelRenderDemo)


# 集成渲染演示项目
add_executable(IntegratedRenderingDemo
        IntegratedRenderingDemo/IntegratedRenderingDemo.cpp
)

target_link_libraries(IntegratedRenderingDemo PRIVATE JFMEngine)
target_include_directories(IntegratedRenderingDemo PRIVATE
        ../Engine/Include
        ../ThirdParty/glad/include
        ../ThirdParty/glm
        ../ThirdParty/spdlog/include
)

# 设置所有示例的输出目录
set_target_properties(IntegratedRenderingDemo
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
