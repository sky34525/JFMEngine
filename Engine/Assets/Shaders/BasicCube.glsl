#type vertex
#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;

uniform mat4 u_ViewProjectionMatrix;
uniform mat4 u_ModelMatrix;
uniform mat3 u_NormalMatrix;

out vec3 v_FragPos;
out vec3 v_Normal;

void main()
{
    v_FragPos = vec3(u_ModelMatrix * vec4(a_Position, 1.0));
    v_Normal = normalize(u_NormalMatrix * a_Normal);

    gl_Position = u_ViewProjectionMatrix * vec4(v_FragPos, 1.0);
    
    // 调试：检查特定顶点的变换（约等于后左上角顶点）
    if (abs(a_Position.x + 0.5) < 0.01 && abs(a_Position.y - 0.5) < 0.01 && abs(a_Position.z + 0.5) < 0.01) {
        // 这是后左上角顶点(-0.5, 0.5, -0.5)，让它显示为红色进行调试
        // 这种调试方法可能不会完全工作，因为这是顶点着色器，但可以尝试
    }
}

#type fragment
#version 330 core

in vec3 v_FragPos;
in vec3 v_Normal;

// 材质属性 (匹配LightingManager的接口)
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float alpha;
};
uniform Material u_Material;

// 方向光 (匹配LightingManager的接口)
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight u_DirLight;

// 点光源 (匹配LightingManager的接口)
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

// 相机位置 (匹配LightingManager的接口)
uniform vec3 u_ViewPos;

out vec4 FragColor;

// 计算方向光
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    
    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0);
    
    // 镜面反射
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
    
    // 合成结果
    vec3 ambient = light.ambient * u_Material.ambient;
    vec3 diffuse = light.diffuse * diff * u_Material.diffuse;
    vec3 specular = light.specular * spec * u_Material.specular;
    
    return ambient + diffuse + specular;
}

// 计算点光源
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    // 漫反射
    float diff = max(dot(normal, lightDir), 0.0);
    
    // 镜面反射
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
    
    // 衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    // 合成结果
    vec3 ambient = light.ambient * u_Material.ambient;
    vec3 diffuse = light.diffuse * diff * u_Material.diffuse;
    vec3 specular = light.specular * spec * u_Material.specular;
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return ambient + diffuse + specular;
}

void main()
{
    vec3 norm = normalize(v_Normal);
    vec3 viewDir = normalize(u_ViewPos - v_FragPos);
    
    // 验证材质颜色，如果为零则使用默认值
    vec3 materialDiffuse = u_Material.diffuse;
    if (length(materialDiffuse) < 0.1) {
        materialDiffuse = vec3(0.8, 0.8, 0.8); // 默认灰色
    }
    
    // 使用完整的方向光计算
    vec3 result = CalcDirLight(u_DirLight, norm, viewDir);
    
    // 添加点光源计算（如果有的话）
    for(int i = 0; i < u_NumPointLights && i < MAX_POINT_LIGHTS; i++) {
        result += CalcPointLight(u_PointLights[i], norm, v_FragPos, viewDir);
    }
    
    // 确保最终颜色不会太暗（调试用）
    if (length(result) < 0.2) {
        result = mix(result, materialDiffuse * 0.6, 0.5);
    }
    
    // 限制颜色范围防止过曝
    result = clamp(result, 0.0, 1.0);
    
    FragColor = vec4(result, u_Material.alpha);
}
