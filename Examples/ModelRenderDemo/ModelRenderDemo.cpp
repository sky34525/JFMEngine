//
// ModelRenderDemo.cpp - 模型渲染演示
// 加载并渲染黑猫模型
//

#include "JFMEngine/JFMEngine.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <fstream>

namespace JFM {

    class ModelRenderDemoLayer : public Layer {
    public:
        virtual void OnAttach() override {
            // 创建着色器
            std::string shaderPath = "/Users/kw/JFMEngine_New/Engine/Assets/Shaders/BasicCube.glsl";
            m_Shader = Shader::Create(shaderPath);

            if (!m_Shader) {
                return;
            }

            // 设置光照
            LightingManager::GetInstance().SetupDefaultLighting();

            // 设置相机位置 - 适合观察模型
            auto& camera = m_CameraController.GetCamera();
            camera.SetPosition(glm::vec3(0.0f, 2.0f, 10.0f));
            m_CameraController.SetCameraPosition(glm::vec3(0.0f, 2.0f, 10.0f));

            // 加载黑猫模型
            try {
                m_Model = std::make_shared<Model>("/Users/kw/JFMEngine_New/res/model/blackcat.fbx");
                JFM_INFO("黑猫模型加载完成，网格数量: {}", m_Model ? m_Model->GetMeshCount() : 0);
            } catch (const std::exception& e) {
                JFM_ERROR("模型加载异常: {}", e.what());
                m_Model = nullptr;
            }
            
            // 检查文件是否存在
            std::string modelPath = "/Users/kw/JFMEngine_New/res/model/blackcat.fbx";
            std::ifstream file(modelPath);
            if (!file.good()) {
                JFM_ERROR("模型文件不存在: {}", modelPath);
            }
            
            // 创建备用立方体用于测试
            CreateFallbackCube();
        }

        virtual void OnRender() override {
            if (!m_Shader) {
                return;
            }

            // 设置渲染状态
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            
            glClearColor(0.2f, 0.3f, 0.4f, 1.0f);  // 天蓝色背景
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // 绑定着色器
            m_Shader->Bind();

            // 获取视图投影矩阵
            glm::mat4 view = m_CameraController.GetCamera().GetViewMatrix();
            glm::mat4 projection = m_CameraController.GetCamera().GetProjectionMatrix();
            glm::mat4 viewProjection = projection * view;

            // 设置光照uniform
            SetupLighting();

            // 渲染模型
            RenderModel(viewProjection);
        }

        virtual void OnUpdate(float deltaTime) override {
            if (m_Shader) {
                m_CameraController.OnUpdate(deltaTime);
                
                // 模型自动旋转
                m_ModelRotation += deltaTime * 30.0f; // 每秒30度
                if (m_ModelRotation >= 360.0f) {
                    m_ModelRotation -= 360.0f;
                }
            }
        }

        virtual void OnEvent(Event& event) override {
            if (m_Shader) {
                m_CameraController.OnEvent(event);
            }
        }

    private:


        void SetupLighting() {
            auto cameraPos = m_CameraController.GetCamera().GetPosition();
            
            // 设置相机位置
            m_Shader->SetFloat3("u_ViewPos", cameraPos);
            
            // 设置方向光 - 从上方照射
            m_Shader->SetFloat3("u_DirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
            m_Shader->SetFloat3("u_DirLight.ambient", glm::vec3(0.4f, 0.4f, 0.4f));
            m_Shader->SetFloat3("u_DirLight.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
            m_Shader->SetFloat3("u_DirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
            
            // 设置点光源数量
            m_Shader->SetInt("u_NumPointLights", 0);
        }

        void RenderModel(const glm::mat4& viewProjection) {
            // 创建模型变换矩阵
            glm::mat4 model = glm::mat4(1.0f);
            
            // 应用旋转
            model = glm::rotate(model, glm::radians(m_ModelRotation), glm::vec3(0.0f, 1.0f, 0.0f));
            
            // 适当缩放模型（FBX模型可能很大）
            model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
            
            // 设置矩阵uniform
            m_Shader->SetMat4("u_ModelMatrix", model);
            m_Shader->SetMat4("u_ViewProjectionMatrix", viewProjection);
            m_Shader->SetMat3("u_NormalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
            
            if (m_Model) {
                // 渲染加载的模型
                m_Model->Draw(m_Shader);
            } else if (m_CubeVA) {
                // 渲染替代立方体
                GeometryGenerator::RenderGeometry(
                    m_CubeVA, m_CubeIndexCount, m_Shader,
                    glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.8f, 0.6f, 0.2f), viewProjection
                );
            }
        }

        void CreateFallbackCube() {
            // 使用GeometryGenerator创建简单立方体
            if (!m_CubeVA) {
                auto cubeGeometry = GeometryGenerator::CreateCube(1.0f);
                m_CubeVA = GeometryGenerator::CreateVertexArray(cubeGeometry);
                m_CubeIndexCount = cubeGeometry.indices.size();
                JFM_INFO("创建备用立方体几何体，索引数量: {}", m_CubeIndexCount);
            }
        }

        // 渲染资源



        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<Model> m_Model;
        
        // 备用立方体（如果模型加载失败）
        std::shared_ptr<VertexArray> m_CubeVA;
        size_t m_CubeIndexCount = 0;

        // 相机控制器
        JFM::CameraController m_CameraController{800.0f/600.0f, true};
        
        // 动画参数
        float m_ModelRotation = 0.0f;
    };
    

    class ModelRenderDemo : public Application {
    public:
        ModelRenderDemo() {
            PushLayer(std::shared_ptr<Layer>(new ModelRenderDemoLayer()));
        }
    };

    Application* CreateApplication() {
        return new ModelRenderDemo();
    }
}