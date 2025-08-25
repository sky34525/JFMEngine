# JFMEngine

一个现代化的C++游戏引擎，采用分层架构和事件驱动设计。

采用右手坐标系：

+X轴：右方向 (Right)
+Y轴：上方向 (Up)
+Z轴：前方向 (Forward，朝向观察者)


## 🌟 特性

- **分层架构** - 灵活的图层系统，支持UI层、游戏层、调试层等
- **事件驱动** - 高性能的事件系统，支持多线程处理
- **现代C++** - 基于C++20标准，使用现代C++特性
- **跨平台** - 支持Windows、macOS和Linux
- **模块化设计** - 清晰的模块划分，易于扩展

## 📁 项目结构

```
JFMEngine/
├── Engine/           # 引擎核心代码
│   ├── Include/      # 公共头文件
│   ├── Source/       # 引擎源码
│   └── Layers/       # 内置图层
├── Examples/         # 示例项目
│   ├── Sandbox/      # 基础示例
│   ├── LayerDemo/    # 图层演示
│   └── WindowTest/   # 窗口测试
├── ThirdParty/       # 第三方库
├── Build/            # 构建输出
└── Scripts/          # 构建脚本
```

## 🛠️ 构建要求

- **编译器**: 支持C++20的编译器 (GCC 10+, Clang 12+, MSVC 2019+)
- **CMake**: 3.20或更高版本
- **GLFW**: 3.3或更高版本
- **OpenGL**: 3.3或更高版本

## 🚀 快速开始

### 1. 克隆项目
```bash
git clone <repository-url>
cd JFMEngine
```

### 2. 安装依赖 (macOS)
```bash
brew install cmake glfw
```

### 3. 构建项目
```bash
# Debug模式 (默认)
./Scripts/build.sh

# Release模式
./Scripts/build.sh Release
```

### 4. 运行示例
```bash
cd Build/Debug/Examples
./Sandbox        # 基础示例
./LayerDemo      # 图层演示
./WindowTest     # 窗口测试
```

## 📖 使用示例

创建一个简单的应用程序：

```cpp
#include <JFMEngine/JFMEngine.h>

class MyApp : public JFM::Application
{
public:
    MyApp()
    {
        // 添加自定义图层
        auto layer = std::make_shared<MyLayer>();
        PushLayer(layer);
    }
};

JFM::Application* JFM::CreateApplication()
{
    return new MyApp();
}
```

## 🏗️ 架构设计

### 核心组件
- **Application**: 应用程序主类
- **Layer/LayerStack**: 图层系统
- **EventSystem**: 事件处理系统
- **Window**: 窗口抽象层

### 事件系统
支持多种事件类型：
- 窗口事件 (关闭、调整大小)
- 键盘事件 (按键按下/释放)
- 鼠标事件 (移动、点击、滚轮)

### 图层系统
- **OnAttach/OnDetach**: 图层生命周期
- **OnUpdate**: 每帧更新
- **OnEvent**: 事件处理
- **OnImGuiRender**: 渲染逻辑

## 🤝 贡献

欢迎提交Issue和Pull Request！

## 📄 许可证

[MIT License](LICENSE)
