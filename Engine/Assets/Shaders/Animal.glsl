#type vertex
#version 330 core

// 顶点属性
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in vec4 a_BoneWeights;  // 骨骼权重
layout (location = 4) in ivec4 a_BoneIndices; // 骨骼索引

// 变换矩阵
uniform mat4 u_ViewProjectionMatrix;
uniform mat4 u_ModelMatrix;
uniform mat3 u_NormalMatrix;

// 骨骼动画矩阵 (最多支持100个骨骼)
uniform mat4 u_BoneMatrices[100];
uniform bool u_UseAnimation;

// 输出到片段着色器
out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoord;

void main()
{
    vec4 totalPosition = vec4(0.0);
    vec3 totalNormal = vec3(0.0);

    if (u_UseAnimation) {
        // 骨骼动画计算
        for(int i = 0; i < 4; i++) {
            if(a_BoneIndices[i] >= 0) {
                mat4 boneTransform = u_BoneMatrices[a_BoneIndices[i]];
                vec4 posePosition = boneTransform * vec4(a_Position, 1.0);
                totalPosition += posePosition * a_BoneWeights[i];

                vec3 poseNormal = mat3(boneTransform) * a_Normal;
                totalNormal += poseNormal * a_BoneWeights[i];
            }
        }
    } else {
        // 静态模型
        totalPosition = vec4(a_Position, 1.0);
        totalNormal = a_Normal;
    }

    // 世界空间变换
    v_FragPos = vec3(u_ModelMatrix * totalPosition);
    v_Normal = normalize(u_NormalMatrix * totalNormal);
    v_TexCoord = a_TexCoord;

    gl_Position = u_ViewProjectionMatrix * vec4(v_FragPos, 1.0);
}

#type fragment
#version 330 core

out vec4 FragColor;

// 从顶点着色器传入
in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

// 材质纹理
uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_SpecularTexture;
uniform sampler2D u_NormalTexture;
uniform bool u_HasDiffuseTexture;
uniform bool u_HasSpecularTexture;
uniform bool u_HasNormalTexture;

// 材质属性
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float alpha;
};
uniform Material u_Material;

// 方向光（太阳光）
struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirectionalLight u_DirLight;

// 点光源
struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
#define MAX_POINT_LIGHTS 4
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];
uniform int u_NumPointLights;

// 摄像机位置
uniform vec3 u_ViewPos;

// 计算方向光
vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 diffuseColor, vec3 specularColor)
{
    vec3 lightDir = normalize(-light.direction);

    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0);

    // 镜面反射
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);

    // 合并结果
    vec3 ambient = light.ambient * diffuseColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularColor;

    return (ambient + diffuse + specular);
}

// 计算点光源
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, vec3 specularColor)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0);

    // 镜面反射
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);

    // 衰减计算
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // 合并结果
    vec3 ambient = light.ambient * diffuseColor;
    vec3 diffuse = light.diffuse * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularColor;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

void main()
{
    // 法线处理
    vec3 norm = normalize(v_Normal);
    vec3 viewDir = normalize(u_ViewPos - v_FragPos);

    // 获取材质颜色
    vec3 diffuseColor;
    vec3 specularColor;

    if (u_HasDiffuseTexture) {
        diffuseColor = texture(u_DiffuseTexture, v_TexCoord).rgb;
    } else {
        diffuseColor = u_Material.diffuse;
    }

    if (u_HasSpecularTexture) {
        specularColor = texture(u_SpecularTexture, v_TexCoord).rgb;
    } else {
        specularColor = u_Material.specular;
    }

    // 法线贴图处理（如果有的话）
    if (u_HasNormalTexture) {
        // 简化的法线贴图实现
        vec3 normalMap = texture(u_NormalTexture, v_TexCoord).rgb * 2.0 - 1.0;
        // 这里应该使用TBN矩阵，但为了简化暂时直接使用
        norm = normalize(norm + normalMap * 0.1);
    }

    // 计算光照
    vec3 result = vec3(0.0);

    // 方向光
    result += CalcDirLight(u_DirLight, norm, viewDir, diffuseColor, specularColor);

    // 点光源
    for (int i = 0; i < u_NumPointLights && i < MAX_POINT_LIGHTS; i++) {
        result += CalcPointLight(u_PointLights[i], norm, v_FragPos, viewDir, diffuseColor, specularColor);
    }

    // 添加环境光
    result += u_Material.ambient * diffuseColor;

    // 输出最终颜色
    FragColor = vec4(result, u_Material.alpha);
}
