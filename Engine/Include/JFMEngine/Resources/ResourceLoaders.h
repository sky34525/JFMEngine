//
// ResourceLoaders.h - 各种资源加载器实现
//

#pragma once

#include "ResourceManager.h"
#include "JFMEngine/Renderer/Texture.h"
#include "JFMEngine/Renderer/Model.h"
#include "JFMEngine/Audio/AudioSystem.h"
// 暂时注释掉 Assimp 相关头文件，直到添加依赖
// #include <assimp/Importer.hpp>
// #include <assimp/scene.h>
// #include <assimp/postprocess.h>

namespace JFM {

    // 纹理资源包装类
    class JFM_API TextureResource : public Resource {
    public:
        TextureResource(const std::string& path)
            : Resource(path, ResourceType::TEXTURE) {}

        std::shared_ptr<Texture2D> GetTexture() const { return m_Texture; }

        virtual bool Load() override;
        virtual void Unload() override;
        virtual size_t GetMemoryUsage() const override;

        // 纹理特定设置
        void SetCompressionFormat(const std::string& format) { m_CompressionFormat = format; }
        void SetGenerateMipmaps(bool generate) { m_GenerateMipmaps = generate; }
        void SetWrapMode(const std::string& mode) { m_WrapMode = mode; }
        void SetFilterMode(const std::string& mode) { m_FilterMode = mode; }

    private:
        std::shared_ptr<Texture2D> m_Texture;
        std::string m_CompressionFormat = "DXT5";
        bool m_GenerateMipmaps = true;
        std::string m_WrapMode = "REPEAT";
        std::string m_FilterMode = "LINEAR";

        bool LoadFromFile();
        bool CompressTexture();
        void ApplyTextureSettings();
    };

    // 模型资源包装类
    class JFM_API ModelResource : public Resource {
    public:
        ModelResource(const std::string& path)
            : Resource(path, ResourceType::MODEL) {}

        std::shared_ptr<Model> GetModel() const { return m_Model; }

        virtual bool Load() override;
        virtual void Unload() override;
        virtual size_t GetMemoryUsage() const override;

        // 模型加载选项
        void SetImportFlags(uint32_t flags) { m_ImportFlags = flags; }
        void SetOptimizeMesh(bool optimize) { m_OptimizeMesh = optimize; }
        void SetCalculateTangents(bool calculate) { m_CalculateTangents = calculate; }

    private:
        std::shared_ptr<Model> m_Model;
        // 暂时注释掉 Assimp 相关代码
        // uint32_t m_ImportFlags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;
        uint32_t m_ImportFlags = 0;
        bool m_OptimizeMesh = true;
        bool m_CalculateTangents = true;

        // Assimp::Importer m_Importer;

        bool LoadWithAssimp();
        void ProcessNode(struct aiNode* node, const struct aiScene* scene);
        std::shared_ptr<Mesh> ProcessMesh(struct aiMesh* mesh, const struct aiScene* scene);
        std::vector<std::shared_ptr<Texture2D>> LoadMaterialTextures(struct aiMaterial* mat, int type);
    };

    // 音频资源包装类
    class JFM_API AudioResource : public Resource {
    public:
        AudioResource(const std::string& path)
            : Resource(path, ResourceType::AUDIO) {}

        std::shared_ptr<AudioClip> GetAudioClip() const { return m_AudioClip; }

        virtual bool Load() override;
        virtual void Unload() override;
        virtual size_t GetMemoryUsage() const override;

        // 音频加载选项
        void SetStreamingMode(bool streaming) { m_StreamingMode = streaming; }
        void SetCompressionQuality(float quality) { m_CompressionQuality = quality; }

    private:
        std::shared_ptr<AudioClip> m_AudioClip;
        bool m_StreamingMode = false;
        float m_CompressionQuality = 0.8f;

        bool LoadWAV();
        bool LoadOGG();
        bool LoadMP3();
    };

    // 纹理加载器
    class JFM_API TextureLoader : public ResourceLoader {
    public:
        virtual std::vector<std::string> GetSupportedExtensions() const override {
            return {".png", ".jpg", ".jpeg", ".bmp", ".tga", ".dds", ".hdr", ".exr"};
        }

        virtual std::shared_ptr<Resource> LoadResource(const std::string& path,
                                                      const std::unordered_map<std::string, std::string>& params = {}) override;

        virtual bool CanLoadAsync() const override { return true; }

    private:
        void ApplyLoadParameters(std::shared_ptr<TextureResource> resource,
                               const std::unordered_map<std::string, std::string>& params);
    };

    // 模型加载器
    class JFM_API ModelLoader : public ResourceLoader {
    public:
        virtual std::vector<std::string> GetSupportedExtensions() const override {
            return {".obj", ".fbx", ".gltf", ".glb", ".dae", ".3ds", ".blend", ".ply"};
        }

        virtual std::shared_ptr<Resource> LoadResource(const std::string& path,
                                                      const std::unordered_map<std::string, std::string>& params = {}) override;

        virtual bool CanLoadAsync() const override { return true; }

    private:
        void ApplyLoadParameters(std::shared_ptr<ModelResource> resource,
                               const std::unordered_map<std::string, std::string>& params);
    };

    // 音频加载器
    class JFM_API AudioLoader : public ResourceLoader {
    public:
        virtual std::vector<std::string> GetSupportedExtensions() const override {
            return {".wav", ".ogg", ".mp3", ".flac", ".aac"};
        }

        virtual std::shared_ptr<Resource> LoadResource(const std::string& path,
                                                      const std::unordered_map<std::string, std::string>& params = {}) override;

        virtual bool CanLoadAsync() const override { return true; }

    private:
        void ApplyLoadParameters(std::shared_ptr<AudioResource> resource,
                               const std::unordered_map<std::string, std::string>& params);
    };

    // 流式纹理加载器（用于大型纹理的分块加载）
    class JFM_API StreamingTextureLoader {
    public:
        struct TextureChunk {
            uint32_t MipLevel;
            uint32_t X, Y;
            uint32_t Width, Height;
            std::vector<uint8_t> Data;
        };

        StreamingTextureLoader(const std::string& path);
        ~StreamingTextureLoader();

        bool InitializeStreaming();
        TextureChunk LoadChunk(uint32_t mipLevel, uint32_t chunkX, uint32_t chunkY);
        void UnloadChunk(uint32_t mipLevel, uint32_t chunkX, uint32_t chunkY);

        uint32_t GetMipLevels() const { return m_MipLevels; }
        uint32_t GetChunksX(uint32_t mipLevel) const;
        uint32_t GetChunksY(uint32_t mipLevel) const;

    private:
        std::string m_Path;
        uint32_t m_Width, m_Height;
        uint32_t m_MipLevels;
        uint32_t m_ChunkSize = 512;

        std::unordered_map<uint64_t, TextureChunk> m_LoadedChunks;
        std::mutex m_ChunkMutex;

        uint64_t GetChunkKey(uint32_t mipLevel, uint32_t chunkX, uint32_t chunkY) const;
    };

    // 依赖解析器
    class JFM_API DependencyResolver {
    public:
        static std::vector<std::string> ExtractDependencies(const std::string& resourcePath, ResourceType type);
        static bool ValidateDependencies(const std::string& resourcePath);
        static std::vector<std::string> GetLoadOrder(const std::vector<std::string>& resources);

    private:
        static std::vector<std::string> ExtractModelDependencies(const std::string& modelPath);
        static std::vector<std::string> ExtractMaterialDependencies(const std::string& materialPath);
        static bool HasCircularDependency(const std::string& resource,
                                        const std::unordered_map<std::string, std::vector<std::string>>& deps,
                                        std::unordered_set<std::string>& visiting);
    };

}
