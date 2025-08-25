//
// SceneFactory.cpp - 增强的场景工厂系统实现
//

#include "JFMEngine/Scene/SceneFactory.h"
#include "JFMEngine/Utils/Log.h"
#include "JFMEngine/Renderer/Renderer3D.h"
#include <glm/gtc/matrix_transform.hpp>

namespace JFM {

    // ========== PrimitiveLibrary 实现 ==========

    std::unordered_map<std::string, std::shared_ptr<Model>> PrimitiveLibrary::s_PrimitiveModels;
    std::unordered_map<std::string, std::shared_ptr<Material>> PrimitiveLibrary::s_DefaultMaterials;
    bool PrimitiveLibrary::s_Initialized = false;

    void PrimitiveLibrary::Initialize() {
        if (s_Initialized) return;

        CreatePrimitiveModels();
        CreateDefaultMaterials();

        s_Initialized = true;
    }

    void PrimitiveLibrary::Shutdown() {
        if (!s_Initialized) return;

        s_PrimitiveModels.clear();
        s_DefaultMaterials.clear();
        s_Initialized = false;
    }

    void PrimitiveLibrary::CreatePrimitiveModels() {
        // 创建立方体模型 - 暂时注释掉不存在的API调用
        auto cubeModel = std::make_shared<Model>("");  // 使用空路径创建
        // cubeModel->LoadPrimitive(Model::PrimitiveType::Cube);  // 此方法不存在
        // TODO: 使用 Primitives::CreateCube() 替代
        s_PrimitiveModels["Cube"] = cubeModel;

        // 创建球体模型
        auto sphereModel = std::make_shared<Model>("");
        // sphereModel->LoadPrimitive(Model::PrimitiveType::Sphere);  // 此方法不存在
        s_PrimitiveModels["Sphere"] = sphereModel;

        // 创建平面模型
        auto planeModel = std::make_shared<Model>("");
        // planeModel->LoadPrimitive(Model::PrimitiveType::Plane);  // 此方法不存在
        s_PrimitiveModels["Plane"] = planeModel;

        // 创建圆柱体模型
        auto cylinderModel = std::make_shared<Model>("");
        // cylinderModel->LoadPrimitive(Model::PrimitiveType::Cylinder);  // 此方法不存在
        s_PrimitiveModels["Cylinder"] = cylinderModel;

        // 创建圆锥体模型
        auto coneModel = std::make_shared<Model>("");
        // coneModel->LoadPrimitive(Model::PrimitiveType::Cone);  // 此方法不存在
        s_PrimitiveModels["Cone"] = coneModel;
    }

    void PrimitiveLibrary::CreateDefaultMaterials() {
        // 默认PBR材质
        auto defaultMaterial = std::make_shared<Material>();
        // defaultMaterial->SetAlbedo(glm::vec3(0.8f, 0.8f, 0.8f));  // 方法不存在，暂时注释
        // defaultMaterial->SetMetallic(0.0f);  // 方法不存在，暂时注释
        // defaultMaterial->SetRoughness(0.5f);  // 方法不存在，暂时注释
        s_DefaultMaterials["Default"] = defaultMaterial;

        // 无光照材质
        auto unlitMaterial = std::make_shared<Material>();
        // unlitMaterial->SetShader("Unlit");  // 方法不存在，暂时注释
        // unlitMaterial->SetAlbedo(glm::vec3(1.0f));  // 方法不存在，暂时注释
        s_DefaultMaterials["Unlit"] = unlitMaterial;

        // PBR材质
        auto pbrMaterial = std::make_shared<Material>();
        // pbrMaterial->SetShader("PBR");  // 方法不存在，暂时注释
        // pbrMaterial->SetAlbedo(glm::vec3(0.7f, 0.7f, 0.7f));  // 方法不存在，暂时注释
        // pbrMaterial->SetMetallic(0.1f);  // 方法不存在，暂时注释
        // pbrMaterial->SetRoughness(0.3f);  // 方法不存在，暂时注释
        s_DefaultMaterials["PBR"] = pbrMaterial;
    }

    std::shared_ptr<Model> PrimitiveLibrary::GetCubeModel() {
        return s_PrimitiveModels["Cube"];
    }

    std::shared_ptr<Model> PrimitiveLibrary::GetSphereModel() {
        return s_PrimitiveModels["Sphere"];
    }

    std::shared_ptr<Model> PrimitiveLibrary::GetPlaneModel() {
        return s_PrimitiveModels["Plane"];
    }

    std::shared_ptr<Model> PrimitiveLibrary::GetCylinderModel() {
        return s_PrimitiveModels["Cylinder"];
    }

    std::shared_ptr<Model> PrimitiveLibrary::GetConeModel() {
        return s_PrimitiveModels["Cone"];
    }

    std::shared_ptr<Material> PrimitiveLibrary::GetDefaultMaterial() {
        return s_DefaultMaterials["Default"];
    }

    std::shared_ptr<Material> PrimitiveLibrary::GetUnlitMaterial() {
        return s_DefaultMaterials["Unlit"];
    }

    std::shared_ptr<Material> PrimitiveLibrary::GetPBRMaterial() {
        return s_DefaultMaterials["PBR"];
    }

    // ========== SceneFactory 实现 ==========

    void SceneFactory::Initialize() {
        PrimitiveLibrary::Initialize();
    }

    void SceneFactory::Shutdown() {
        PrimitiveLibrary::Shutdown();
    }

    ECS::Entity SceneFactory::CreateRenderedCube(ECS::World* world, const std::string& name) {
        ECS::Entity entity = world->CreateEntity();

        // 添加基础组件
        world->AddComponent<ECS::TagComponent>(entity, name);
        world->AddComponent<ECS::TransformComponent>(entity);
        world->AddComponent<ECS::ActiveComponent>(entity, true);

        // 设置网格渲染器
        SetupMeshRenderer(world, entity, PrimitiveLibrary::GetCubeModel(),
                         PrimitiveLibrary::GetDefaultMaterial());

        return entity;
    }

    ECS::Entity SceneFactory::CreateRenderedSphere(ECS::World* world, const std::string& name) {
        ECS::Entity entity = world->CreateEntity();

        world->AddComponent<ECS::TagComponent>(entity, name);
        world->AddComponent<ECS::TransformComponent>(entity);
        world->AddComponent<ECS::ActiveComponent>(entity, true);

        SetupMeshRenderer(world, entity, PrimitiveLibrary::GetSphereModel(),
                         PrimitiveLibrary::GetDefaultMaterial());

        return entity;
    }

    ECS::Entity SceneFactory::CreateRenderedPlane(ECS::World* world, const std::string& name) {
        ECS::Entity entity = world->CreateEntity();

        world->AddComponent<ECS::TagComponent>(entity, name);
        world->AddComponent<ECS::TransformComponent>(entity);
        world->AddComponent<ECS::ActiveComponent>(entity, true);

        SetupMeshRenderer(world, entity, PrimitiveLibrary::GetPlaneModel(),
                         PrimitiveLibrary::GetDefaultMaterial());

        return entity;
    }

    ECS::Entity SceneFactory::CreateRenderedCylinder(ECS::World* world, const std::string& name) {
        ECS::Entity entity = world->CreateEntity();

        world->AddComponent<ECS::TagComponent>(entity, name);
        world->AddComponent<ECS::TransformComponent>(entity);
        world->AddComponent<ECS::ActiveComponent>(entity, true);

        SetupMeshRenderer(world, entity, PrimitiveLibrary::GetCylinderModel(),
                         PrimitiveLibrary::GetDefaultMaterial());

        return entity;
    }

    ECS::Entity SceneFactory::CreateFromModelFile(ECS::World* world, const std::string& modelPath,
                                                  const std::string& name) {
        ECS::Entity entity = world->CreateEntity();

        world->AddComponent<ECS::TagComponent>(entity, name);
        world->AddComponent<ECS::TransformComponent>(entity);
        world->AddComponent<ECS::ActiveComponent>(entity, true);

        // 加载模型文件
        auto model = std::make_shared<Model>(modelPath);  // 使用正确的构造函数
        // 移除不存在的 LoadFromFile 调用，Model 构造函数应该已经加载了文件
        SetupMeshRenderer(world, entity, model, PrimitiveLibrary::GetDefaultMaterial());

        return entity;
    }

    ECS::Entity SceneFactory::CreateVisualDirectionalLight(ECS::World* world, const glm::vec3& direction) {
        ECS::Entity entity = world->CreateEntity();

        world->AddComponent<ECS::TagComponent>(entity, "Directional Light");
        auto& transform = world->AddComponent<ECS::TransformComponent>(entity);
        world->AddComponent<ECS::ActiveComponent>(entity, true);

        // 设置光源组件
        auto& light = world->AddComponent<ECS::LightComponent>(entity);
        light.Type = LightType::Directional;
        light.Color = glm::vec3(1.0f, 1.0f, 0.9f);
        light.Intensity = 1.0f;

        // 设置变换（方向）
        glm::vec3 normalizedDir = glm::normalize(direction);
        transform.Rotation = glm::quatLookAt(normalizedDir, glm::vec3(0, 1, 0));

        // 添加可视化网格（小箭头或图标）
        auto arrowMaterial = CreateColoredMaterial(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
        SetupMeshRenderer(world, entity, PrimitiveLibrary::GetConeModel(), arrowMaterial);

        return entity;
    }

    ECS::Entity SceneFactory::CreateVisualPointLight(ECS::World* world, const glm::vec3& position) {
        ECS::Entity entity = world->CreateEntity();

        world->AddComponent<ECS::TagComponent>(entity, "Point Light");
        auto& transform = world->AddComponent<ECS::TransformComponent>(entity);
        world->AddComponent<ECS::ActiveComponent>(entity, true);

        // 设置光源组件
        auto& light = world->AddComponent<ECS::LightComponent>(entity);
        light.Type = LightType::Point;
        light.Color = glm::vec3(1.0f, 0.8f, 0.6f);
        light.Intensity = 1.0f;
        light.Range = 10.0f;

        // 设置位置
        transform.Position = position;
        transform.Scale = glm::vec3(0.1f); // 小球体作为可视化

        // 添加可视化网格（发光的小球）
        auto lightMaterial = CreateColoredMaterial(glm::vec4(1.0f, 1.0f, 0.5f, 1.0f));
        SetupMeshRenderer(world, entity, PrimitiveLibrary::GetSphereModel(), lightMaterial);

        return entity;
    }

    ECS::Entity SceneFactory::CreateDebugCamera(ECS::World* world, const std::string& name) {
        ECS::Entity entity = world->CreateEntity();

        world->AddComponent<ECS::TagComponent>(entity, name);
        world->AddComponent<ECS::TransformComponent>(entity);
        world->AddComponent<ECS::ActiveComponent>(entity, true);

        // 添加相机组件
        auto& camera = world->AddComponent<ECS::CameraComponent>(entity);
        camera.Type = ECS::CameraComponent::ProjectionType::Perspective;
        camera.FOV = 45.0f;
        camera.NearPlane = 0.1f;
        camera.FarPlane = 1000.0f;

        return entity;
    }

    void SceneFactory::ApplyMaterial(ECS::World* world, ECS::Entity entity, std::shared_ptr<Material> material) {
        if (world->HasComponent<ECS::MeshRendererComponent>(entity)) {
            auto& meshRenderer = world->GetComponent<ECS::MeshRendererComponent>(entity);
            meshRenderer.MaterialData = material;
        }
    }

    void SceneFactory::ApplyColor(ECS::World* world, ECS::Entity entity, const glm::vec4& color) {
        auto colorMaterial = CreateColoredMaterial(color);
        ApplyMaterial(world, entity, colorMaterial);
    }

    ECS::Entity SceneFactory::CreateTestScene(ECS::World* world) {
        // 创建根节点
        ECS::Entity sceneRoot = world->CreateEntity();
        world->AddComponent<ECS::TagComponent>(sceneRoot, "Test Scene Root");
        world->AddComponent<ECS::TransformComponent>(sceneRoot);
        world->AddComponent<ECS::ActiveComponent>(sceneRoot, true);
        world->AddComponent<ECS::ChildrenComponent>(sceneRoot);

        // 创建地面
        auto ground = CreateRenderedPlane(world, "Ground");
        auto& groundTransform = world->GetComponent<ECS::TransformComponent>(ground);
        groundTransform.Position = glm::vec3(0, 0, 0);
        groundTransform.Scale = glm::vec3(20, 1, 20);
        ApplyColor(world, ground, glm::vec4(0.3f, 0.7f, 0.3f, 1.0f)); // 绿色地面

        // 创建一些测试对象
        auto cube1 = CreateRenderedCube(world, "Test Cube 1");
        auto& cube1Transform = world->GetComponent<ECS::TransformComponent>(cube1);
        cube1Transform.Position = glm::vec3(-2, 1, 0);
        ApplyColor(world, cube1, glm::vec4(1.0f, 0.2f, 0.2f, 1.0f)); // 红色

        auto sphere1 = CreateRenderedSphere(world, "Test Sphere 1");
        auto& sphere1Transform = world->GetComponent<ECS::TransformComponent>(sphere1);
        sphere1Transform.Position = glm::vec3(2, 1, 0);
        ApplyColor(world, sphere1, glm::vec4(0.2f, 0.2f, 1.0f, 1.0f)); // 蓝色

        // 创建光源
        auto dirLight = CreateVisualDirectionalLight(world, glm::vec3(-0.3f, -1.0f, -0.3f));
        auto pointLight = CreateVisualPointLight(world, glm::vec3(0, 5, 3));

        return sceneRoot;
    }

    void SceneFactory::SetupMeshRenderer(ECS::World* world, ECS::Entity entity,
                                       std::shared_ptr<Model> model, std::shared_ptr<Material> material) {
        auto& meshRenderer = world->AddComponent<ECS::MeshRendererComponent>(entity);

        // 暂时注释掉不存在的 GetMesh 方法，使用空指针作为占位符
        // meshRenderer.MeshData = model->GetMesh(0); // Model 没有 GetMesh 方法
        meshRenderer.MeshData = nullptr; // 暂时使用空指针，待实现正确的 Mesh 提取逻辑
        meshRenderer.MaterialData = material;
        meshRenderer.CastShadows = true;
        meshRenderer.ReceiveShadows = true;
    }

    std::shared_ptr<Material> SceneFactory::CreateColoredMaterial(const glm::vec4& color) {
        auto material = std::make_shared<Material>();
        // material->SetAlbedo(glm::vec3(color));  // 方法不存在，暂时注释
        // material->SetMetallic(0.0f);  // 方法不存在，暂时注释
        // material->SetRoughness(0.8f);  // 方法不存在，暂时注释
        return material;
    }

    ECS::Entity SceneFactory::CreateVisualSpotLight(ECS::World* world, const glm::vec3& position) {
        auto entity = world->CreateEntity();

        // 添加Transform组件
        auto& transform = world->AddComponent<ECS::TransformComponent>(entity);
        transform.Position = position;
        transform.Rotation = glm::vec3(0.0f);
        transform.Scale = glm::vec3(1.0f);

        // 添加SpotLight组件
        auto& spotLight = world->AddComponent<ECS::SpotLightComponent>(entity);
        spotLight.Color = glm::vec3(1.0f, 1.0f, 0.8f);  // 暖白色光
        spotLight.Intensity = 1.0f;
        spotLight.Range = 20.0f;
        spotLight.InnerConeAngle = 30.0f;
        spotLight.OuterConeAngle = 45.0f;
        spotLight.Direction = glm::vec3(0.0f, -1.0f, 0.0f);  // 向下照射

        return entity;
    }

} // namespace JFM
