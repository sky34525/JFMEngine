# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 构建系统

JFMEngine 使用 CMake 构建系统，支持 Debug 和 Release 模式。

### 主要构建命令
```bash
# 构建项目 (Debug模式，默认)
./Scripts/build.sh

# 构建 Release 模式
./Scripts/build.sh Release

# 清理构建文件
./Scripts/clean.sh

# 手动 CMake 配置
cd Build/Debug && cmake -DCMAKE_BUILD_TYPE=Debug ../..

# 编译 (在构建目录中)
make -j$(nproc)
```

### 运行示例程序
```bash
# 在构建完成后运行示例
cd Build/Debug && ./IntegratedRenderingDemo
cd Build/Debug && ./Simple3DDemo  
cd Build/Debug && ./AnimalAnimationDemo
```

## 代码架构

JFMEngine 是一个现代化的 C++20 游戏引擎，采用分层架构设计：

### 核心架构组件
- **Application**: 引擎主应用程序类，管理整个引擎生命周期
- **Layer/LayerStack**: 分层系统，支持多个渲染层和逻辑层的堆栈管理
- **EventSystem**: 事件驱动架构，处理窗口、输入和自定义事件
- **ECS (Entity Component System)**: 现代化的实体组件系统，支持高性能的游戏对象管理
- **Scene Management**: 场景管理系统，支持3D场景的创建和管理

### 渲染系统架构
- **Renderer3D**: 3D渲染器，支持现代OpenGL渲染管线
- **LightingSystem**: 光照管理，支持多种光源类型和材质系统
- **Camera**: 摄像机系统，支持透视和正交投影
- **Mesh/Model**: 3D模型加载和渲染，集成 Assimp 库

### 关键目录结构
- `Engine/Include/JFMEngine/`: 公共头文件接口
- `Engine/Source/`: 引擎核心实现代码
- `Examples/`: 示例应用程序
- `ThirdParty/`: 外部依赖库 (GLM, GLAD, STB, Assimp)

### 依赖管理
引擎依赖以下主要库：
- **GLFW**: 窗口和输入管理
- **OpenGL**: 图形渲染API  
- **GLM**: 数学库 (向量、矩阵运算)
- **Assimp**: 3D模型加载
- **STB**: 图像加载

### 入口点模式
客户端应用程序需要实现 `JFM::CreateApplication()` 函数来创建自定义应用程序类，引擎会自动管理主循环。

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

### 测试
当前项目包含测试目录结构 (`Tests/Core`, `Tests/Events`, `Tests/Platform`)，但测试构建需要通过 `-DBUILD_TESTS=ON` CMake 选项启用。
- 日志需要记录渲染出的顶点属性