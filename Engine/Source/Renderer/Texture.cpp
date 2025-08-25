#include "JFMEngine/Renderer/Texture.h"
#include <filesystem>
#include <iostream>

namespace JFM {

    void TextureLibrary::Add(const std::string& name, const std::shared_ptr<Texture2D>& texture) {
        if (Exists(name)) {
            std::cout << "Warning: Texture '" << name << "' already exists in library, overriding" << std::endl;
        }
        m_Textures[name] = texture;
    }

    void TextureLibrary::Add(const std::shared_ptr<Texture2D>& texture) {
        auto name = ExtractName(texture->GetPath());
        Add(name, texture);
    }

    std::shared_ptr<Texture2D> TextureLibrary::Load(const std::string& path) {
        auto name = ExtractName(path);
        return Load(name, path);
    }

    std::shared_ptr<Texture2D> TextureLibrary::Load(const std::string& name, const std::string& path) {
        if (Exists(name)) {
            return Get(name);
        }

        auto texture = Texture2D::Create(path);
        if (texture) {
            Add(name, texture);
            std::cout << "Info: Loaded texture '" << name << "' from '" << path << "'" << std::endl;
            return texture;
        }

        std::cerr << "Error: Failed to load texture '" << name << "' from '" << path << "'" << std::endl;
        return nullptr;
    }

    std::shared_ptr<Texture2D> TextureLibrary::Get(const std::string& name) {
        if (!Exists(name)) {
            std::cerr << "Error: Texture '" << name << "' not found in library" << std::endl;
            return nullptr;
        }
        return m_Textures[name];
    }

    bool TextureLibrary::Exists(const std::string& name) const {
        return m_Textures.find(name) != m_Textures.end();
    }

    void TextureLibrary::Clear() {
        m_Textures.clear();
        std::cout << "Info: Texture library cleared" << std::endl;
    }

    std::string TextureLibrary::ExtractName(const std::string& filepath) {
        return std::filesystem::path(filepath).stem().string();
    }

}
