//
// Model.cpp - 3D模型实现
//

#include "JFMEngine/Renderer/Model.h"
#include "JFMEngine/Utils/Log.h"
#include "JFMEngine/Renderer/Vertex.h"
#include "JFMEngine/Animation/Animation.h" // 添加动画头文件

// Assimp includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <fstream>
#include <algorithm>

namespace JFM {

    Model::Model(const std::string& path) : m_Directory(path) {
        LoadModel(path);
    }

    void Model::Draw(const std::shared_ptr<Shader>& shader) const {
        for (const auto& mesh : m_Meshes) {
            if (mesh) {
                mesh->Draw();
            }
        }
    }

    //负责从文件系统读取3D模型文件并将其转换为引擎可用的格式。
    void Model::LoadModel(const std::string& path) {
        // 检查文件是否存在
        std::ifstream file(path);
        if (!file.good()) {
            return;
        }
        file.close();

        // 创建Assimp导入器
        Assimp::Importer importer;

        // 设置后处理标志
        const aiScene* scene = importer.ReadFile(path,//aiScene 指针指向的是 Assimp 库解析后的完整 3D 场景数据结构，它是整个 3D 模型文件的内存表示。
            aiProcess_Triangulate |           // 确保所有面都是三角形
            aiProcess_FlipUVs |              // 翻转UV坐标
            aiProcess_CalcTangentSpace |     // 计算切线空间
            aiProcess_GenSmoothNormals |     // 生成光滑法线
            aiProcess_JoinIdenticalVertices | // 合并相同顶点
            aiProcess_ValidateDataStructure | // 验证数据结构
            aiProcess_ImproveCacheLocality   // 改善缓存局部性
        );

        // 检查导入是否成功
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            return;
        }

        // 提取目录路径
        size_t lastSlash = path.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            m_Directory = path.substr(0, lastSlash);
        } else {
            m_Directory = ".";
        }

        // 处理根节点
        ProcessNode(scene->mRootNode, scene);

        // 加载动画数据
        LoadAnimations(scene);
    }

    //遍历当前节点包含的所有网格（Mesh），并通过 ProcessMesh 处理后加入模型的网格列表。
    //递归调用自身，处理当前节点的所有子节点，直到遍历完整个场景树。
    void Model::ProcessNode(aiNode* node, const aiScene* scene) {
        // 处理节点的所有网格
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];//在Assimp加载的场景（aiScene）中，获取当前节点（aiNode）的第i个网格（mesh）的指针。
            auto processedMesh = ProcessMesh(mesh, scene);
            if (processedMesh) {
                m_Meshes.push_back(processedMesh);// 将处理后的网格添加到模型的网格列表中
            }
        }

        // 递归处理子节点
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            ProcessNode(node->mChildren[i], scene);
        }
    }

    //将 Assimp 库的 aiMesh 数据结构转换为引擎内部的 Mesh 对象
    std::shared_ptr<Mesh> Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // 处理顶点
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
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

                // 切线
                if (mesh->HasTangentsAndBitangents()) {
                    vertex.Tangent = glm::vec3(
                        mesh->mTangents[i].x,
                        mesh->mTangents[i].y,
                        mesh->mTangents[i].z
                    );

                    vertex.Bitangent = glm::vec3(
                        mesh->mBitangents[i].x,
                        mesh->mBitangents[i].y,
                        mesh->mBitangents[i].z
                    );
                }
            } else {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        // 处理索引
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        // 处理材质
        std::vector<std::shared_ptr<Texture>> textures;
        if (mesh->mMaterialIndex != UINT_MAX) {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            // 漫反射贴图
            auto diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            // 镜面反射贴图
            auto specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

            // 法线贴图 - 修正为正确的Assimp类型
            auto normalMaps = LoadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

            // 高度贴图 - 修正为正确的Assimp类型
            auto heightMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

            // 可选：添加更多PBR纹理类型
            // 金属度贴图
            auto metallicMaps = LoadMaterialTextures(material, aiTextureType_METALNESS, "texture_metallic");
            textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());

            // 粗糙度贴图
            auto roughnessMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness");
            textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());

            // 环境光遮蔽贴图
            auto aoMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT_OCCLUSION, "texture_ao");
            textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());
        }

        return std::make_shared<Mesh>(vertices, indices, textures);
    }

    std::vector<std::shared_ptr<Texture>> Model::LoadMaterialTextures(aiMaterial* mat, int type, const std::string& typeName) {
        std::vector<std::shared_ptr<Texture>> textures;

        // 将int类型转换为aiTextureType
        aiTextureType assimpType = static_cast<aiTextureType>(type);

        for (unsigned int i = 0; i < mat->GetTextureCount(assimpType); i++) {
            aiString str;
            mat->GetTexture(assimpType, i, &str);

            std::string texturePath = m_Directory + "/" + str.C_Str();

            // 使用Texture2D::Create来创建纹理，而不是直接构造
            auto texture = Texture2D::Create(texturePath);
            if (texture) {
                texture->SetType(typeName);
                textures.push_back(texture);
            }
        }

        return textures;
    }

    // 动画相关方法实现
    void Model::Update(float deltaTime) {
        if (m_Animator) {
            m_Animator->Update(deltaTime);
        }
    }

    void Model::LoadAnimations(const aiScene* scene) {
        // 加载所有动画剪辑
        for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
            const aiAnimation* assimpAnim = scene->mAnimations[i];
            auto animClip = AnimationClip::CreateFromAssimp(assimpAnim);
            if (animClip) {
                m_AnimationClips.push_back(animClip);
            }
        }

        // 如果有动画，创建默认动画器
        if (!m_AnimationClips.empty()) {
            m_Animator = std::make_shared<Animator>();
            m_Animator->SetAnimationClip(m_AnimationClips[0]); // 设置第一个动画为默认
        }
    }

    void Model::PlayAnimation(const std::string& animationName) {
        if (!m_Animator) return;

        // 查找指定名称的动画
        for (const auto& clip : m_AnimationClips) {
            if (clip->GetName() == animationName) {
                m_Animator->SetAnimationClip(clip);
                m_Animator->Play();
                return;
            }
        }
    }

    void Model::PlayAnimation(int animationIndex) {
        if (!m_Animator || animationIndex < 0 || animationIndex >= m_AnimationClips.size()) {
            return;
        }

        m_Animator->SetAnimationClip(m_AnimationClips[animationIndex]);
        m_Animator->Play();
    }

    void Model::StopAnimation() {
        if (m_Animator) {
            m_Animator->Stop();
        }
    }

    void Model::PauseAnimation() {
        if (m_Animator) {
            m_Animator->Pause();
        }
    }

    void Model::SetAnimationLoop(bool loop) {
        if (m_Animator) {
            m_Animator->SetLoop(loop);
        }
    }

    void Model::SetAnimationSpeed(float speed) {
        if (m_Animator) {
            m_Animator->SetSpeed(speed);
        }
    }

} // namespace JFM
