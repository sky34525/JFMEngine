//
// Simple3DDemo.cpp - 简单的3D渲染演示
// 展示基础3D几何体渲染：立方体、球体、平面
//

//TODO:检查渲染模块
#include "glad/glad.h"
#include "JFMEngine/JFMEngine.h"
#include <cmath>

namespace JFM {

    class Simple3DDemoLayer : public Layer {
    public:
        virtual void OnAttach() override {
            // 初始化着色器
            // 使用调试着色器来测试基本渲染
            std::string shaderPath = "/Users/kw/JFMEngine_New/Engine/Assets/Shaders/BasicCube.glsl";
            m_Shader = Shader::Create(shaderPath);

            if (!m_Shader) {
                return;
            }

            // 设置光照
            LightingManager::GetInstance().SetupDefaultLighting();

            // 设置相机位置
            auto& camera = m_CameraController.GetCamera();
            camera.SetPosition(glm::vec3(0.0f, 0.0f, 15.0f));
            m_CameraController.SetCameraPosition(glm::vec3(0.0f, 0.0f, 15.0f));

            // 创建几何体
            CreateCube();
            CreateSphere();

        }

        virtual void OnRender() override {
            if (!m_Shader) {
                return;
            }

            // 清理屏幕并设置OpenGL状态
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            
            // 禁用面剔除以确保所有面都显示
            //glDisable(GL_CULL_FACE);
            

            
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  // 青蓝色背景
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            // 确保视口设置正确
            glViewport(0, 0, 800, 600);

            // 绑定着色器
            m_Shader->Bind();

            // 使用相机控制器的矩阵
            glm::mat4 view = m_CameraController.GetCamera().GetViewMatrix();
            glm::mat4 projection = m_CameraController.GetCamera().GetProjectionMatrix();
            glm::mat4 viewProjection = projection * view;

            // 设置完整的光照uniform
            auto cameraPos = m_CameraController.GetCamera().GetPosition();
            
            // 设置相机位置（镜面反射需要）
            m_Shader->SetFloat3("u_ViewPos", cameraPos);
            
            // 设置方向光
            m_Shader->SetFloat3("u_DirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
            m_Shader->SetFloat3("u_DirLight.ambient", glm::vec3(0.3f, 0.3f, 0.3f));
            m_Shader->SetFloat3("u_DirLight.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
            m_Shader->SetFloat3("u_DirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            
            // 设置点光源数量（暂时设为0，只使用方向光）
            m_Shader->SetInt("u_NumPointLights", 0);

            // 渲染几何体
            RenderSphere(glm::vec3(0.0f,0.0f,0.5f),glm::vec3(1.0f, 1.0f, 1.0f), viewProjection);
            RenderCube(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(1.0f, 0.0f, 0.0f), viewProjection);



        }

        virtual void OnUpdate(float deltaTime) override {
            // 更新相机控制器 - 添加空指针检查
            if (m_Shader) {
                m_CameraController.OnUpdate(deltaTime);
            }
        }

        virtual void OnEvent(Event& event) override {
            // 处理相机事件 - 添加空指针检查
            if (m_Shader) {
                m_CameraController.OnEvent(event);
            }
        }

    private:

        void CreateCube() {
            // 使用引擎的几何体生成器创建立方体
            auto cubeGeometry = GeometryGenerator::CreateCube(1.0f);
            m_CubeVA = GeometryGenerator::CreateVertexArray(cubeGeometry);
            m_CubeIndexCount = cubeGeometry.indices.size();
            
        }

        void CreateSphere() {
            // 使用引擎的几何体生成器创建球体
            auto sphereGeometry = GeometryGenerator::CreateSphere(0.5f, 20, 20);
            m_SphereVA = GeometryGenerator::CreateVertexArray(sphereGeometry);
            m_SphereIndexCount = sphereGeometry.indices.size();
            
        }

        void CreatePlane() {
            // 使用引擎的几何体生成器创建平面
            auto planeGeometry = GeometryGenerator::CreatePlane(1.0f, 1.0f);
            m_PlaneVA = GeometryGenerator::CreateVertexArray(planeGeometry);
            m_PlaneIndexCount = planeGeometry.indices.size();
            
        }

        void RenderCube(const glm::vec3& position, const glm::vec3& color, const glm::mat4& viewProjection) {
            GeometryGenerator::RenderGeometry(m_CubeVA, m_CubeIndexCount, m_Shader,
                                            position, glm::vec3(1.0f), color, viewProjection);
        }

        void RenderSphere(const glm::vec3& position, const glm::vec3& color, const glm::mat4& viewProjection) {
            GeometryGenerator::RenderGeometry(m_SphereVA, m_SphereIndexCount, m_Shader,
                                            position, glm::vec3(0.8f), color, viewProjection);
        }

        void RenderPlane(const glm::vec3& position, const glm::vec3& color, const glm::mat4& viewProjection) {
            GeometryGenerator::RenderGeometry(m_PlaneVA, m_PlaneIndexCount, m_Shader,
                                            position, glm::vec3(1.5f, 1.0f, 1.5f), color, viewProjection);
        }
        

    private:
        // 渲染资源
        std::shared_ptr<Shader> m_Shader;

        // 相机控制器
        JFM::CameraController m_CameraController{800.0f/600.0f, true};

        // 几何体VAO
        std::shared_ptr<VertexArray> m_CubeVA;
        std::shared_ptr<VertexArray> m_SphereVA;
        std::shared_ptr<VertexArray> m_PlaneVA;

        // 索引数量
        size_t m_CubeIndexCount = 0;
        size_t m_SphereIndexCount = 0;
        size_t m_PlaneIndexCount = 0;
    };

    class Simple3DDemo : public Application {
    public:
        Simple3DDemo() {
            PushLayer(std::shared_ptr<Layer>(new Simple3DDemoLayer()));
        }
    };

    Application* CreateApplication() {
        return new Simple3DDemo();
    }
}
