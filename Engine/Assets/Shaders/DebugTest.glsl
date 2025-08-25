#type vertex
#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;

uniform mat4 u_ViewProjectionMatrix;
uniform mat4 u_ModelMatrix;

out vec3 v_Color;

void main()
{
    // 直接使用法线作为颜色来调试
    v_Color = abs(a_Normal);  // 使用绝对值确保颜色为正
    
    gl_Position = u_ViewProjectionMatrix * u_ModelMatrix * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

in vec3 v_Color;
out vec4 FragColor;

void main()
{
    // 输出法线颜色，确保能看到东西
    FragColor = vec4(v_Color, 1.0);
}