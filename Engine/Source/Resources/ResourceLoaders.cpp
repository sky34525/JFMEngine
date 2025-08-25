//
// ResourceLoaders.cpp - 各种资源加载器实现
//

#include "JFMEngine/Resources/ResourceLoaders.h"
#include "JFMEngine/Utils//Log.h"
#include <glad/glad.h>  // 添加 OpenGL 头文件
#include <stb_image.h>
#include <filesystem>
#include <fstream>

// Assimp 前向声明
struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;

namespace JFM {

    // TextureResource 实现
    bool TextureResource::Load() {
        if (m_State == ResourceState::LOADED) return true;

        m_State = ResourceState::LOADING;

        if (!LoadFromFile()) {
            m_State = ResourceState::ERROR;
            return false;
        }

        CompressTexture();

        ApplyTextureSettings();
        m_State = ResourceState::LOADED;
        return true;
    }

    void TextureResource::Unload() {
        if (m_Texture) {
            m_Texture.reset();
        }
        m_State = ResourceState::UNLOADED;
    }

    size_t TextureResource::GetMemoryUsage() const {
        if (!m_Texture) return 0;

        // 估算纹理内存使用量（宽度 * 高度 * 通道数 * 字节大小）
        return m_Texture->GetWidth() * m_Texture->GetHeight() * 4; // RGBA
    }

    bool TextureResource::LoadFromFile() {
        int width, height, channels;

        // 使用stb_image加载图像数据
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(m_Path.c_str(), &width, &height, &channels, 0);

        if (!data) {
            return false;
        }

        // 创建纹理对象 - 使用 Texture2D::Create 而不是直接实例化
        m_Texture = Texture2D::Create(width, height);

        // 设置纹理数据
        uint32_t dataSize = width * height * channels;
        m_Texture->SetData(data, dataSize);

        stbi_image_free(data);
        return true;
    }

    bool TextureResource::CompressTexture() {
        // 这里可以实现纹理压缩逻辑
        // 根据m_CompressionFormat进行相应的压缩
        // 暂时返回true表示跳过压缩
        return true;
    }

    void TextureResource::ApplyTextureSettings() {
        if (!m_Texture) return;

        // 应用纹理设置
        m_Texture->Bind();

        // 设置包装模式
        GLenum wrapMode = GL_REPEAT;
        if (m_WrapMode == "CLAMP") wrapMode = GL_CLAMP_TO_EDGE;
        else if (m_WrapMode == "MIRROR") wrapMode = GL_MIRRORED_REPEAT;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

        // 设置过滤模式
        GLenum filterMode = GL_LINEAR;
        if (m_FilterMode == "NEAREST") filterMode = GL_NEAREST;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);

        // 生成Mipmap
        if (m_GenerateMipmaps) {
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }

        m_Texture->Unbind();
    }

    // ModelResource 实现
    bool ModelResource::Load() {
        if (m_State == ResourceState::LOADED) return true;

        m_State = ResourceState::LOADING;

        if (!LoadWithAssimp()) {
            m_State = ResourceState::ERROR;
            return false;
        }

        m_State = ResourceState::LOADED;
        return true;
    }

    void ModelResource::Unload() {
        if (m_Model) {
            m_Model.reset();
        }
        m_State = ResourceState::UNLOADED;
    }

    size_t ModelResource::GetMemoryUsage() const {
        if (!m_Model) return 0;

        // 估算模型内存使用量
        size_t totalSize = 0;
        for (const auto& mesh : m_Model->GetMeshes()) {
            // 使用正确的方法名
            totalSize += mesh->Vertices.size() * sizeof(Vertex);
            totalSize += mesh->Indices.size() * sizeof(uint32_t);
        }
        return totalSize;
    }

    bool ModelResource::LoadWithAssimp() {
        // 暂时注释掉 Assimp 相关代码，直到添加 Assimp 依赖
        /*
        // 设置导入标志
        if (m_CalculateTangents) {
            m_ImportFlags |= aiProcess_CalcTangentSpace;
        }
        if (m_OptimizeMesh) {
            m_ImportFlags |= aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph;
        }

        // 使用Assimp加载场景
        const aiScene* scene = m_Importer.ReadFile(m_Path, m_ImportFlags);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            JFM_CORE_ERROR("Assimp error: {}", m_Importer.GetErrorString());
            return false;
        }

        // 创建模型对象
        m_Model = std::make_shared<Model>();

        // 处理场景节点
        ProcessNode(scene->mRootNode, scene);

        JFM_CORE_INFO("Successfully loaded model with {} meshes", m_Model->GetMeshes().size());
        */

        // 临时实现：创建空模型
        m_Model = std::make_shared<Model>(m_Path);
        return true;
    }

    void ModelResource::ProcessNode(aiNode* node, const aiScene* scene) {
        // 暂时注释掉 Assimp 相关代码
        /*
        // 处理当前节点的所有网格
        for (uint32_t i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            auto processedMesh = ProcessMesh(mesh, scene);
            if (processedMesh) {
                m_Model->AddMesh(processedMesh);
            }
        }

        // 递归处理子节点
        for (uint32_t i = 0; i < node->mNumChildren; i++) {
            ProcessNode(node->mChildren[i], scene);
        }
        */
    }

    std::shared_ptr<Mesh> ModelResource::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
        // 暂时注释掉 Assimp 相关代码
        /*
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        // 处理顶点数据
        for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;

            // 位置
            vertex.Position = glm::vec3(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z
            );

            // 法线
            if (mesh->HasNormals()) {
                vertex.Normal = glm::vec3(
                    mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z
                );
            }

            // 纹理坐标
            if (mesh->mTextureCoords[0]) {
                vertex.TexCoords = glm::vec2(
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y
                );
            } else {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            // 切线
            if (mesh->HasTangentsAndBitangents()) {
                vertex.Tangent = glm::vec3(
                    mesh->mTangents[i].x,
                    mesh->mTangents[i].y,
                    mesh->mTangents[i].z
                );
            }

            vertices.push_back(vertex);
        }

        // 处理索引
        for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (uint32_t j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        // 处理材质
        std::vector<std::shared_ptr<Texture>> textures;
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            // 漫反射贴图
            auto diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            // 镜面反射贴图
            auto specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR);
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

            // 法线贴图
            auto normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT);
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        }

        return std::make_shared<Mesh>(vertices, indices, textures);
        */

        // 返回空 Mesh 作为占位符
        return nullptr;
    }

    std::vector<std::shared_ptr<Texture2D>> ModelResource::LoadMaterialTextures(
        aiMaterial* mat, int type) {
        // 暂时注释掉 Assimp 相关代码
        /*
        std::vector<std::shared_ptr<Texture>> textures;

        for (uint32_t i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);

            // 构建纹理路径
            std::filesystem::path modelPath(m_Path);
            std::filesystem::path texturePath = modelPath.parent_path() / str.C_Str();

            // 使用ResourceManager加载纹理
            if (std::filesystem::exists(texturePath)) {
                auto textureResource = ResourceManager::GetInstance()
                    .LoadResource<TextureResource>(texturePath.string());

                if (textureResource.IsValid()) {
                    textures.push_back(textureResource->GetTexture());
                }
            } else {
                JFM_CORE_WARN("Texture not found: {}", texturePath.string());
            }
        }

        return textures;
        */

        // 返回空纹理列表
        return {};
    }

    // AudioResource 实现
    bool AudioResource::Load() {
        if (m_State == ResourceState::LOADED) return true;

        m_State = ResourceState::LOADING;

        std::filesystem::path filePath(m_Path);
        std::string extension = filePath.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

        bool success = false;
        if (extension == ".wav") {
            success = LoadWAV();
        } else if (extension == ".ogg") {
            success = LoadOGG();
        } else if (extension == ".mp3") {
            success = LoadMP3();
        } else {
            m_State = ResourceState::ERROR;
            return false;
        }

        if (success) {
            m_State = ResourceState::LOADED;
        } else {
            m_State = ResourceState::ERROR;
        }

        return success;
    }

    void AudioResource::Unload() {
        if (m_AudioClip) {
            m_AudioClip.reset();
        }
        m_State = ResourceState::UNLOADED;
    }

    size_t AudioResource::GetMemoryUsage() const {
        if (!m_AudioClip) return 0;

        // 使用正确的方法名
        // AudioClip 只有构造函数，没有 GetSampleCount，需要估算
        return 1024 * 1024; // 临时返回1MB作为估算值
    }

    bool AudioResource::LoadWAV() {
        // 使用正确的构造函数
        m_AudioClip = std::make_shared<AudioClip>(m_Path);

        // TODO: 实现实际的WAV加载逻辑
        return true;
    }

    bool AudioResource::LoadOGG() {
        // 使用正确的构造函数
        m_AudioClip = std::make_shared<AudioClip>(m_Path);

        // TODO: 实现实际的OGG加载逻辑
        return true;
    }

    bool AudioResource::LoadMP3() {
        // 使用正确的构造函数
        m_AudioClip = std::make_shared<AudioClip>(m_Path);

        // TODO: 实现实际的MP3加载逻辑
        return true;
    }

    // TextureLoader 实现
    std::shared_ptr<Resource> TextureLoader::LoadResource(const std::string& path,
                                                         const std::unordered_map<std::string, std::string>& params) {
        auto resource = std::make_shared<TextureResource>(path);
        ApplyLoadParameters(resource, params);

        if (!resource->Load()) {
            return nullptr;
        }

        return resource;
    }

    void TextureLoader::ApplyLoadParameters(std::shared_ptr<TextureResource> resource,
                                          const std::unordered_map<std::string, std::string>& params) {
        auto it = params.find("compression");
        if (it != params.end()) {
            resource->SetCompressionFormat(it->second);
        }

        it = params.find("mipmaps");
        if (it != params.end()) {
            resource->SetGenerateMipmaps(it->second == "true");
        }

        it = params.find("wrap");
        if (it != params.end()) {
            resource->SetWrapMode(it->second);
        }

        it = params.find("filter");
        if (it != params.end()) {
            resource->SetFilterMode(it->second);
        }
    }

    // ModelLoader 实现
    std::shared_ptr<Resource> ModelLoader::LoadResource(const std::string& path,
                                                       const std::unordered_map<std::string, std::string>& params) {
        auto resource = std::make_shared<ModelResource>(path);
        ApplyLoadParameters(resource, params);

        if (!resource->Load()) {
            return nullptr;
        }

        return resource;
    }

    void ModelLoader::ApplyLoadParameters(std::shared_ptr<ModelResource> resource,
                                        const std::unordered_map<std::string, std::string>& params) {
        auto it = params.find("optimize");
        if (it != params.end()) {
            resource->SetOptimizeMesh(it->second == "true");
        }

        it = params.find("tangents");
        if (it != params.end()) {
            resource->SetCalculateTangents(it->second == "true");
        }

        it = params.find("import_flags");
        if (it != params.end()) {
            resource->SetImportFlags(std::stoul(it->second));
        }
    }

    // AudioLoader 实现
    std::shared_ptr<Resource> AudioLoader::LoadResource(const std::string& path,
                                                       const std::unordered_map<std::string, std::string>& params) {
        auto resource = std::make_shared<AudioResource>(path);
        ApplyLoadParameters(resource, params);

        if (!resource->Load()) {
            return nullptr;
        }

        return resource;
    }

    void AudioLoader::ApplyLoadParameters(std::shared_ptr<AudioResource> resource,
                                        const std::unordered_map<std::string, std::string>& params) {
        auto it = params.find("streaming");
        if (it != params.end()) {
            resource->SetStreamingMode(it->second == "true");
        }

        it = params.find("quality");
        if (it != params.end()) {
            resource->SetCompressionQuality(std::stof(it->second));
        }
    }

}
