#type vertex
#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;

uniform mat4 u_ViewProjectionMatrix;
uniform mat4 u_ModelMatrix;

out vec3 v_WorldPos;
out vec3 v_LocalPos;

void main()
{
    v_LocalPos = a_Position;  // 原始顶点位置
    v_WorldPos = vec3(u_ModelMatrix * vec4(a_Position, 1.0));  // 世界坐标
    gl_Position = u_ViewProjectionMatrix * vec4(v_WorldPos, 1.0);
}

#type fragment
#version 330 core

in vec3 v_WorldPos;
in vec3 v_LocalPos;

out vec4 FragColor;

void main()
{
    // 根据本地坐标位置着色，便于识别顶点
    vec3 color = vec3(0.5) + v_LocalPos * 0.5;  // 将[-1,1]映射到[0,1]
    
    // 特别标记后左上角顶点(-0.5, 0.5, -0.5)
    if (abs(v_LocalPos.x + 0.5) < 0.1 && abs(v_LocalPos.y - 0.5) < 0.1 && abs(v_LocalPos.z + 0.5) < 0.1) {
        color = vec3(1.0, 0.0, 1.0);  // 品红色，便于识别
    }
    
    FragColor = vec4(color, 1.0);
}