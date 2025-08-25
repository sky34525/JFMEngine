#include "JFMEngine/Renderer/Material.h"
#include "JFMEngine/Renderer/Shader.h"

namespace JFM {

    Material::Material(const MaterialProperties& properties)
        : m_Properties(properties) {
    }

    void Material::Bind(std::shared_ptr<Shader> shader) const {
        // 传统Phong光照属性
        shader->SetFloat3("u_Material.ambient", m_Properties.Ambient);
        shader->SetFloat3("u_Material.diffuse", m_Properties.Diffuse);
        shader->SetFloat3("u_Material.specular", m_Properties.Specular);
        shader->SetFloat("u_Material.shininess", m_Properties.Shininess);

        // PBR材质属性
        shader->SetFloat3("u_Material.albedo", m_Properties.Albedo);
        shader->SetFloat("u_Material.metallic", m_Properties.Metallic);
        shader->SetFloat("u_Material.roughness", m_Properties.Roughness);
        shader->SetFloat("u_Material.ao", m_Properties.AO);
    }

}
