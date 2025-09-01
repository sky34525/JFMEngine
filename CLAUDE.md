# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 构建系统

JFMEngine 使用 CMake 构建系统，支持 Debug 和 Release 模式。

### 主要构建命令
```bash
# 创建构建目录并配置 (Debug模式)
mkdir -p Build/Debug && cd Build/Debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..

# 构建项目
make -j$(sysctl -n hw.ncpu)

# Release 模式构建
mkdir -p Build/Release && cd Build/Release  
cmake -DCMAKE_BUILD_TYPE=Release ../..
make -j$(sysctl -n hw.ncpu)

# 清理构建文件
rm -rf Build/
```

### 运行示例程序
```bash
# 在构建完成后运行示例
cd Build/Debug && ./IntegratedRenderingDemo
cd Build/Debug && ./Simple3DDemo  
cd Build/Debug && ./AnimalAnimationDemo
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

### 新增功能模块
- **Skybox**: 天空盒渲染系统
- **Shadow**: 阴影渲染系统
- **ParticleSystem**: 粒子效果系统
- **PostProcessing**: 后处理效果管线
- **Terrain**: 地形渲染系统
- **AudioSystem**: 完善的音频管理系统

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
客户端应用程序需要实现 `JFM::CreateApplication()` 函数来创建自定义应用程序类。现代化的应用程序通过继承 `Layer` 类实现：

```cpp
class RenderDemoLayer : public Layer {
public:
    virtual void OnAttach() override { /* 初始化逻辑 */ }
    virtual void OnRender() override { /* 渲染逻辑 */ }
    virtual void OnUpdate(float deltaTime) override { /* 更新逻辑 */ }
};
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

### 架构演进说明
引擎已从复杂的 ECS (Entity Component System) 架构转向更简化的 Layer-based 架构：
- **简化性**: 降低了学习曲线和使用复杂度
- **模块化**: 清晰的功能模块划分
- **扩展性**: Layer 系统便于添加新功能
- **性能**: 减少抽象层级，提高渲染性能

### 测试
当前项目包含测试目录结构，测试构建需要通过 `-DBUILD_TESTS=ON` CMake 选项启用。