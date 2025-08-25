//
// Created by kw on 25-8-3.
//
#include "glad/glad.h"
#include"JFMEngine/JFMEngine.h"


namespace JFM {
    class RenderDemoLayer : public Layer {
        virtual void OnAttach() override {
            JFM_INFO("=== OnAttach START ===");

            // 初始化着色器 - 使用简单的立方体着色器而不是Animal.glsl
            JFM_INFO("Attempting to create shader...");
            m_Shader = Shader::Create("/Users/kw/JFMEngine_New/Engine/Assets/Shaders/BasicCube.glsl");

            // 检查着色器是否成功创建
            if (!m_Shader) {
                JFM_ERROR("Failed to create animal shader!");
                return;
            }
            JFM_INFO("Shader created successfully");


            // 设置默认光照场景 - 使用LightingManager统一管理
            JFM_INFO("Setting up lighting...");
            LightingManager::GetInstance().SetupDefaultLighting();
            JFM_INFO("Lighting setup completed");


            JFM_INFO("=== OnAttach COMPLETED ===");

            // 设置相机位置 - 移到模型前方能看到模型的位置
            JFM_INFO("Setting up camera position...");
            auto& camera = m_CameraController.GetCamera();

            // 将相机放在模型前方5个单位，向上2个单位的位置
            camera.SetPosition(glm::vec3(0.0f, 2.0f, 5.0f));

            // 让相机看向原点（模型所在位置）
            camera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));

            JFM_INFO("Camera positioned at (0, 2, 5) looking at origin");
        }
        virtual void OnRender() override {
            // 检查着色器是否有效
            if (!m_Shader) {
                JFM_ERROR("Shader is null in OnRender!");
                return;
            }

            // 设置OpenGL渲染状态
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  // 深灰色背景
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // 首先绑定着色器 - 这是修复GL_INVALID_OPERATION错误的关键！
            m_Shader->Bind();

        // 创建一个简单立方体的顶点数据（如果还没创建）
        if (!m_TestCubeVA) {
            // 立方体顶点数据（位置 + 法线）
            float vertices[] = {
                // 前面 (位置x,y,z + 法线x,y,z)
                -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
                 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
                 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
                -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

                // 后面
                -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
                 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
                 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
                -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            };

            // 立方体索引
            unsigned int indices[] = {
                // ���面
                0, 1, 2, 2, 3, 0,
                // 后面
                4, 5, 6, 6, 7, 4,
                // 左面
                7, 3, 0, 0, 4, 7,
                // 右面
                1, 5, 6, 6, 2, 1,
                // 上面
                3, 2, 6, 6, 7, 3,
                // 下面
                0, 1, 5, 5, 4, 0
            };

            // 创建顶点数组和缓冲区
            m_TestCubeVA = VertexArray::Create();

            auto vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
            BufferLayout layout = {
                { ShaderDataType::Float3, "a_Position" },
                { ShaderDataType::Float3, "a_Normal" }
            };
            vertexBuffer->SetLayout(layout);

            auto indexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));

            m_TestCubeVA->AddVertexBuffer(vertexBuffer);
            m_TestCubeVA->SetIndexBuffer(indexBuffer);

            JFM_INFO("Test cube created successfully");
        }

        JFM_INFO("RenderTestCube: Starting cube render");
            auto camera= m_CameraController.GetCamera();
        // 设置立方体的变换矩阵
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f)); // 放在左边
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f)); // 小一点

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix();
        glm::mat4 viewProjection = projection * view;

        // 输出相机信息用于调试 - 修复格式化问题
        auto camPos = camera.GetPosition();
        JFM_INFO("Camera position: ({:.2f}, {:.2f}, {:.2f})", camPos.x, camPos.y, camPos.z);

        // 现在可以安全地设置uniform变量，因为着色器已经绑定
        m_Shader->SetMat4("u_ModelMatrix", model);
        m_Shader->SetMat4("u_ViewProjectionMatrix", viewProjection);
        m_Shader->SetMat3("u_NormalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

        // 设置明亮的红色材质，确保能看到
        MaterialProperties cubeMaterial = {
            glm::vec3(0.5f, 0.0f, 0.0f),  // 更亮的红色环境光
            glm::vec3(1.0f, 0.0f, 0.0f),  // 纯红色漫反射
            glm::vec3(1.0f, 1.0f, 1.0f),  // 白色镜面反射
            32.0f
        };

        auto& lightingManager = LightingManager::GetInstance();
        lightingManager.ApplyLighting(m_Shader, camera.GetPosition());
        lightingManager.ApplyMaterial(m_Shader, cubeMaterial);

        // 设置着色器uniform
        m_Shader->SetBool("u_HasDiffuseTexture", false);
        m_Shader->SetBool("u_HasSpecularTexture", false);
        m_Shader->SetBool("u_HasNormalTexture", false);
        m_Shader->SetBool("u_UseAnimation", false);

        JFM_INFO("Cube uniforms set, about to render");

        // 渲染立方体
        if (m_TestCubeVA) {
            m_TestCubeVA->Bind();
            JFM_INFO("Cube VAO bound, drawing {} triangles", 12);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

            // 检查OpenGL错误
            GLenum error = glGetError();
            if (error != GL_NO_ERROR) {
                JFM_ERROR("OpenGL error after drawing cube: {}", error);
            } else {
                JFM_INFO("Cube rendered successfully");
            }
        } else {
            JFM_ERROR("Test cube VAO is null!");
        }
        }
    private:
        // 渲染资源
        std::shared_ptr<Shader> m_Shader;

        // 摄像机控制器（包含相机对象）
        JFM::CameraController m_CameraController{800.0f/600.0f, true};

        // 立方体的VAO（仅用��测试）
        std::shared_ptr<VertexArray> m_TestCubeVA;
    };
    class IntegratedRenderingDemo : public Application {
    public:
        IntegratedRenderingDemo() {
            // 添加渲染层
            PushLayer(std::shared_ptr<Layer>(new RenderDemoLayer()));
        }
    };
    Application* CreateApplication() {
        return new IntegratedRenderingDemo();
    }
}