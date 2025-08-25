//
// JFMEngine.h - 主引擎头文件
// 这是客户端应用程序需要包含的唯一头文件
//

#pragma once

// 核心组件
#include "Core/Application.h"
#include "Core/Layer.h"
#include "Core/LayerStack.h"
#include "Core/Core.h"

// 事件系统
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

// 输入系统
#include "Input/KeyCodes.h"

// 平台抽象
#include "Platform/Window.h"

// 渲染系统
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Buffer.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Camera.h"
#include "Renderer/CameraController.h"
#include "Renderer/Texture.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/GeometryGenerator.h"



// 光照系统
#include "Renderer/Light.h"
#include "Renderer/Material.h"
#include "Renderer/LightingRenderer.h"
#include "Renderer/Mesh.h"
#include "Renderer/LightingManager.h"

// ECS系统 - 新添加
#include "ECS/ECS.h"
#include "ECS/Entity.h"
#include "ECS/Component.h"
#include "ECS/Components.h"
#include "ECS/System.h"
#include "ECS/World.h"
#include "ECS/Systems/TransformSystem.h"
#include "ECS/Systems/RenderSystem.h"

// 现代化场景管理 - 新添加
#include "Scene/Scene.h"
#include "Scene/SceneFactory.h"

// 工具类
#include "Utils/Log.h"

// 物理系统
#include "Physics/AllPhysics.h"



// 入口点 - 最后包含
#include "Core/EntryPoint.h"

// GLM数学库 - 用于向量和矩阵运算
#include</Users/kw/JFMEngine_New/ThirdParty/glm/glm/gtc/matrix_transform.hpp>

// 为客户端提供的命名空间
namespace JFMEngine {
    // 重新导出主要类型，方便使用
    using Application = JFM::Application;
    using Layer = JFM::Layer;
    using LayerStack = JFM::LayerStack;


    // 渲染系统类型
    using Renderer = JFM::Renderer;
    using RenderCommand = JFM::RenderCommand;
    using Shader = JFM::Shader;
    using ShaderLibrary = JFM::ShaderLibrary;
    using VertexBuffer = JFM::VertexBuffer;
    using IndexBuffer = JFM::IndexBuffer;
    using VertexArray = JFM::VertexArray;
    using BufferLayout = JFM::BufferLayout;
    using BufferElement = JFM::BufferElement;
    using ShaderDataType = JFM::ShaderDataType;
}
