//
// CubeRenderDemo.cpp - 专门的立方体渲染演示
// 展示立方体的详细顶点属性和渲染过程
//

#include "JFMEngine/JFMEngine.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

namespace JFM {

    class CubeRenderDemoLayer : public Layer {
    public:
        enum class RenderMode {
            FILLED,     // 填充模式（默认）
            WIREFRAME,  // 线框模式
            VERTICES    // 顶点模式
        };
        virtual void OnAttach() override {
            // 创建着色器
            std::string shaderPath = "/Users/kw/JFMEngine_New/Engine/Assets/Shaders/BasicCube.glsl";
            m_Shader = Shader::Create(shaderPath);

            if (!m_Shader) {
                return;
            }

            // 设置光照
            LightingManager::GetInstance().SetupDefaultLighting();

            // 设置相机
            auto& camera = m_CameraController.GetCamera();
            camera.SetPosition(glm::vec3(2.0f, 2.0f, 8.0f));
            m_CameraController.SetCameraPosition(glm::vec3(2.0f, 2.0f, 8.0f));

            // 创建立方体
            CreateCubes();
        }

        virtual void OnRender() override {
            if (!m_Shader) {
                return;
            }

            // 设置渲染状态
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            // 禁用面剔除测试，显示所有面
             //glEnable(GL_CULL_FACE);
             //glCullFace(GL_BACK);
            
            // 设置渲染模式
            SetRenderMode(m_CurrentRenderMode);
            
            glClearColor(0.1f, 0.15f, 0.2f, 1.0f);  // 深蓝灰背景
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // 绑定着色器
            m_Shader->Bind();

            // 获取视图投影矩阵
            glm::mat4 view = m_CameraController.GetCamera().GetViewMatrix();
            glm::mat4 projection = m_CameraController.GetCamera().GetProjectionMatrix();
            glm::mat4 viewProjection = projection * view;

            // 设置光照uniform
            SetupLighting();

            // 渲染多个立方体展示不同效果
            RenderMultipleCubes(viewProjection);
        }

        virtual void OnUpdate(float deltaTime) override {
            if (m_Shader) {
                m_CameraController.OnUpdate(deltaTime);
            }
        }

        virtual void OnEvent(Event& event) override {
            if (m_Shader) {
                m_CameraController.OnEvent(event);
                
                // 处理键盘事件切换渲染模式
                if (event.GetEventType() == EventType::KeyPressed) {
                    KeyPressedEvent& keyEvent = (KeyPressedEvent&)event;
                    
                    switch (keyEvent.GetKeyCode()) {
                        case JFM::Key::D1:
                            m_CurrentRenderMode = RenderMode::FILLED;
                            break;
                        case JFM::Key::D2:
                            m_CurrentRenderMode = RenderMode::WIREFRAME;
                            break;
                        case JFM::Key::D3:
                            m_CurrentRenderMode = RenderMode::VERTICES;
                            break;
                    }
                }
            }
        }

    private:
        void SetRenderMode(RenderMode mode) {
            switch (mode) {
                case RenderMode::FILLED:
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    glDisable(GL_PROGRAM_POINT_SIZE);
                    break;
                    
                case RenderMode::WIREFRAME:
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glDisable(GL_PROGRAM_POINT_SIZE);
                    break;
                    
                case RenderMode::VERTICES:
                    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                    glEnable(GL_PROGRAM_POINT_SIZE);
                    glPointSize(8.0f);
                    break;
            }
        }

        void CreateCubes() {
            auto cubeGeometry = GeometryGenerator::CreateCube(1.0f);
            m_CubeVA = GeometryGenerator::CreateVertexArray(cubeGeometry);
            m_CubeIndexCount = cubeGeometry.indices.size();
        }

        void SetupLighting() {
            auto cameraPos = m_CameraController.GetCamera().GetPosition();
            
            // 设置相机位置
            m_Shader->SetFloat3("u_ViewPos", cameraPos);
            
            // 设置方向光
            m_Shader->SetFloat3("u_DirLight.direction", glm::vec3(-0.3f, -1.0f, -0.5f));
            m_Shader->SetFloat3("u_DirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
            m_Shader->SetFloat3("u_DirLight.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
            m_Shader->SetFloat3("u_DirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            
            // 设置点光源数量
            m_Shader->SetInt("u_NumPointLights", 0);
        }

        void RenderMultipleCubes(const glm::mat4& viewProjection) {
            struct CubeInstance {
                glm::vec3 position;
                glm::vec3 scale;
                glm::vec3 color;
            };
            
            std::vector<CubeInstance> cubes = {
                {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
                {{3.0f, 0.0f, 0.0f}, {0.8f, 1.2f, 0.8f}, {0.0f, 1.0f, 0.0f}},
                {{-3.0f, 0.0f, 0.0f}, {1.5f, 0.5f, 1.5f}, {0.0f, 0.0f, 1.0f}},
                {{0.0f, 3.0f, 0.0f}, {0.6f, 0.6f, 0.6f}, {1.0f, 1.0f, 0.0f}},
                {{0.0f, -2.0f, 0.0f}, {2.0f, 0.3f, 2.0f}, {1.0f, 0.0f, 1.0f}}
            };
            
            for (const auto& cube : cubes) {
                GeometryGenerator::RenderGeometry(
                    m_CubeVA, m_CubeIndexCount, m_Shader,
                    cube.position, cube.scale, cube.color, viewProjection
                );
            }
        }


    private:
        // 渲染资源
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<VertexArray> m_CubeVA;
        size_t m_CubeIndexCount = 0;

        // 相机控制器
        JFM::CameraController m_CameraController{800.0f/600.0f, true};
        
        // 渲染模式
        RenderMode m_CurrentRenderMode = RenderMode::FILLED;
    };

    class CubeRenderDemo : public Application {
    public:
        CubeRenderDemo() {
            PushLayer(std::shared_ptr<Layer>(new CubeRenderDemoLayer()));
        }
    };

    Application* CreateApplication() {
        return new CubeRenderDemo();
    }
}