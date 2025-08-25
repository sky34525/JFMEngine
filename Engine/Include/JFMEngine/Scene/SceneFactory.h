//
// SceneFactory.h - 增强的场景工厂系统
// 利用现有渲染资源创建标准化场景对象
//

#pragma once

#include "JFMEngine/Core/Core.h"
#include "JFMEngine/ECS/World.h"
#include "JFMEngine/ECS/Components.h"
#include "JFMEngine/Renderer/Model.h"
#include "JFMEngine/Renderer/Material.h"
#include "JFMEngine/Renderer/Texture.h"
#include "JFMEngine/Resources/ResourceManager.h"
#include <memory>
#include <unordered_map>

namespace JFM {

    // 标准几何体资源管理器
    class JFM_API PrimitiveLibrary {
    public:
        static void Initialize();
        static void Shutdown();

        // 获取预制几何体
        static std::shared_ptr<Model> GetCubeModel();
        static std::shared_ptr<Model> GetSphereModel();
        static std::shared_ptr<Model> GetPlaneModel();
        static std::shared_ptr<Model> GetCylinderModel();
        static std::shared_ptr<Model> GetConeModel();

        // 获取默认材质
        static std::shared_ptr<Material> GetDefaultMaterial();
        static std::shared_ptr<Material> GetUnlitMaterial();
        static std::shared_ptr<Material> GetPBRMaterial();

    private:
        static void CreatePrimitiveModels();
        static void CreateDefaultMaterials();

        static std::unordered_map<std::string, std::shared_ptr<Model>> s_PrimitiveModels;
        static std::unordered_map<std::string, std::shared_ptr<Material>> s_DefaultMaterials;
        static bool s_Initialized;
    };

    // 增强的场景工厂
    class JFM_API SceneFactory {
    public:
        // 初始化场景工厂
        static void Initialize();
        static void Shutdown();

        // 创建标准3D对象（带网格渲染）
        static ECS::Entity CreateRenderedCube(ECS::World* world, const std::string& name = "Cube");
        static ECS::Entity CreateRenderedSphere(ECS::World* world, const std::string& name = "Sphere");
        static ECS::Entity CreateRenderedPlane(ECS::World* world, const std::string& name = "Plane");
        static ECS::Entity CreateRenderedCylinder(ECS::World* world, const std::string& name = "Cylinder");

        // 从文件加载模型
        static ECS::Entity CreateFromModelFile(ECS::World* world, const std::string& modelPath,
                                             const std::string& name = "Model");

        // 创建光源（带可视化网格）
        static ECS::Entity CreateVisualDirectionalLight(ECS::World* world, const glm::vec3& direction);
        static ECS::Entity CreateVisualPointLight(ECS::World* world, const glm::vec3& position);
        static ECS::Entity CreateVisualSpotLight(ECS::World* world, const glm::vec3& position);

        // 创建相机（带调试可视化）
        static ECS::Entity CreateDebugCamera(ECS::World* world, const std::string& name = "Camera");

        // 创建天空盒
        static ECS::Entity CreateSkybox(ECS::World* world, const std::string& skyboxPath);

        // 应用材质和纹理
        static void ApplyMaterial(ECS::World* world, ECS::Entity entity, std::shared_ptr<Material> material);
        static void ApplyTexture(ECS::World* world, ECS::Entity entity, std::shared_ptr<Texture> texture);
        static void ApplyColor(ECS::World* world, ECS::Entity entity, const glm::vec4& color);

        // 创建复合对象
        static ECS::Entity CreateTestScene(ECS::World* world);
        static ECS::Entity CreateLightingTestScene(ECS::World* world);

    private:
        static std::shared_ptr<Material> CreateColoredMaterial(const glm::vec4& color);
        static void SetupMeshRenderer(ECS::World* world, ECS::Entity entity,
                                    std::shared_ptr<Model> model, std::shared_ptr<Material> material);
    };

} // namespace JFM
