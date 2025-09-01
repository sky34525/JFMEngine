# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 构建系统

JFMEngine 使用 CMake 3.20+ 构建系统，支持 Debug 和 Release 模式。

### 依赖安装 (macOS)
```bash
brew install cmake glfw assimp
```

### 主要构建命令
```bash
# 使用VS Code CMake Tools 扩展（推荐）
# Cmd+Shift+P -> CMake: Configure
# Cmd+Shift+P -> CMake: Build

# 或使用命令行
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --parallel $(sysctl -n hw.ncpu)

# Ninja 构建系统（更快）
cmake -GNinja -DCMAKE_BUILD_TYPE=Debug ..
ninja

# 清理构建文件
rm -rf build/ Build/
```

### 运行示例程序
```bash
# 从构建目录运行（重要：从构建目录运行确保资源路径正确）
cd build && ./IntegratedRenderingDemo
cd build && ./Simple3DDemo  
cd build && ./ModelRenderDemo

# 或使用VS Code调试配置（F5）
```

### 测试构建
```bash
cmake -DBUILD_TESTS=ON ..
cmake --build .
```

## 代码架构

JFMEngine 是一个现代化的 C++20 游戏引擎，采用 Layer-based 分层架构设计：

### 核心架构组件
- **Application**: 引擎主应用程序类，管理整个引擎生命周期
- **Layer/LayerStack**: 分层系统核心，所有功能作为独立 Layer 实现
- **EventSystem**: 事件驱动架构，处理窗口、输入和自定义事件
- **Scene System**: 新的场景管理系统，支持场景对象和层次变换
- **SceneManager**: 单例模式的场景管理器

### 渲染系统架构
- **Renderer3D**: 现代化3D渲染器，支持完整的渲染管线
- **LightingManager**: 单例光照管理系统，统一处理所有光照计算
- **GeometryGenerator**: 内置几何体生成器（立方体、球体、平面等）
- **Camera/CameraController**: 摄像机系统，支持透视投影和控制器
- **Material**: 增强的材质系统，支持多种渲染属性
- **Mesh/Model**: 3D模型加载和渲染，集成 Assimp 库

### 可用示例程序
- **IntegratedRenderingDemo**: 综合渲染演示，展示光照、材质、模型加载
- **Simple3DDemo**: 简单3D场景演示
- **ModelRenderDemo**: 3D模型渲染演示，使用Assimp加载模型

### 关键目录结构
- `Engine/Include/JFMEngine/`: 按功能模块组织的公共头文件接口
  - `Core/`: 应用程序核心、事件系统、内存管理
  - `Renderer/`: 完整的渲染管线、光照、材质系统
  - `Physics/`: 物理系统
  - `Audio/`: 音频系统
  - `Effects/`: 特效系统（粒子等）
  - `Input/`: 输入管理
  - `Platform/`: 平台抽象层
- `Engine/Source/`: 引擎核心实现代码
- `Examples/`: Layer-based 示例应用程序
- `ThirdParty/`: 外部依赖库 (GLM, GLAD, STB, Assimp, EnTT)

### 依赖管理
引擎依赖以下主要库：
- **GLFW**: 窗口和输入管理
- **OpenGL**: 图形渲染API  
- **GLM**: 数学库 (向量、矩阵运算)
- **Assimp**: 3D模型加载
- **STB**: 图像加载
- **EnTT**: 现代C++ ECS库（预备用于未来架构）

使用系统包管理器安装依赖（macOS 使用 Homebrew）

### Layer-based 应用架构
客户端应用程序需要实现 `JFM::CreateApplication()` 函数来创建自定义应用程序类。Layer-based 应用通过继承 `Layer` 类实现：

```cpp
class RenderDemoLayer : public Layer {
public:
    virtual void OnAttach() override { 
        // 初始化渲染资源、相机、材质等
        m_CameraController = std::make_shared<CameraController>(...);
    }
    virtual void OnRender() override { 
        // 渲染3D对象、应用光照、处理材质
        Renderer3D::BeginScene(m_CameraController->GetCamera());
        // ... 渲染调用
        Renderer3D::EndScene();
    }
    virtual void OnUpdate(float deltaTime) override { 
        // 更新相机位置、动画状态、物理模拟
        m_CameraController->OnUpdate(deltaTime);
    }
    virtual void OnEvent(Event& event) override {
        // 处理输入事件、窗口事件
        m_CameraController->OnEvent(event);
    }
};
```

### 客户端应用程序入口
```cpp
#include <JFMEngine/JFMEngine.h>

class MyApp : public JFM::Application {
public:
    MyApp() {
        PushLayer(std::make_shared<RenderDemoLayer>());
    }
};

// 必须实现此函数
JFM::Application* JFM::CreateApplication() {
    return new MyApp();
}
```

### 光照系统使用
统一的光照管理通过单例模式实现：
```cpp
LightingManager::GetInstance().SetupDefaultLighting();
LightingManager::GetInstance().ApplyLighting(shader, cameraPos);
```

### 几何体生成
内置几何体生成器支持程序化创建：
```cpp
auto cubeGeometry = GeometryGenerator::CreateCube(1.0f);
auto sphereGeometry = GeometryGenerator::CreateSphere(0.5f, 20, 20);
```

### 调试和故障排除

#### 常见渲染问题
- **黑屏问题**: 通常由相机投影模式错误引起，确保 3D 应用使用透视投影而非正交投影
- **着色器制服变量**: 在 OpenGLShader.cpp 中已增强制服变量验证，会在控制台输出未找到的制服变量警告
- **相机控制**: CameraController 的键盘事件处理目前为空实现，需要根据具体需求补充

#### 渲染管线调试
- 可通过创建简化测试着色器来隔离渲染问题
- BasicCube.glsl 着色器已与 LightingManager 接口保持兼容
- 顶点绑定日志已从 OpenGLVertexArray.cpp 和 OpenGLBuffer.cpp 中移除以减少噪音

### 坐标系统
引擎采用右手坐标系：
- +X轴：右方向 (Right)
- +Y轴：上方向 (Up)  
- +Z轴：前方向 (Forward，朝向观察者)

### 相机系统详解
- **Camera 类**: 透视投影相机，支持 FOV、宽高比、近远裁剪面设置
- **CameraController 类**: 相机控制器，处理输入事件和相机移动
  - 构造函数使用透视投影参数: `Camera(45.0f, aspectRatio, 0.1f, 1000.0f)`
  - 键盘移动控制需要自行实现 OnKeyPressed/OnKeyReleased 方法
  - 默认相机位置: `(0.0f, 0.0f, 3.0f)`, Yaw: -90°, Pitch: 0°

### 开发工作流程

#### 添加新功能
1. **渲染功能**: 在 `Engine/Source/Renderer/` 中添加实现，头文件放入 `Engine/Include/JFMEngine/Renderer/`
2. **核心功能**: 在 `Engine/Source/Core/` 中添加实现
3. **示例程序**: 在 `Examples/` 中创建新目录，添加 CMakeLists.txt

#### 调试渲染问题
1. **检查相机设置**: 确保透视投影参数正确，位置合理
2. **验证制服变量**: OpenGLShader 会输出未找到的制服变量警告
3. **着色器调试**: 创建简化的测试着色器隔离问题
4. **VAO绑定**: 确保VAO绑定后正确解绑

#### 性能注意事项
- 使用 `GeometryGenerator` 缓存几何体数据
- `LightingManager` 是单例，避免重复初始化
- 批量渲染相同材质的对象
- 在Release模式下测试最终性能

### 架构演进说明
引擎从 ECS 架构转向 Layer-based 架构：
- **简化性**: 降低学习曲线，更直观的代码结构
- **专注性**: Layer 系统专注于特定功能模块
- **调试友好**: 更容易追踪问题和性能瓶颈
- **渐进式**: 可以逐步添加复杂功能而不影响核心架构

### 重要实现细节
- **内存管理**: 使用智能指针，避免裸指针
- **资源加载**: 支持相对路径资源加载，从构建目录运行程序
- **事件传播**: 事件系统支持Layer级别的事件处理和传播控制