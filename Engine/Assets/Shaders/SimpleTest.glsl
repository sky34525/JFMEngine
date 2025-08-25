#type vertex
#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;

uniform mat4 u_ViewProjectionMatrix;
uniform mat4 u_ModelMatrix;

void main()
{
    gl_Position = u_ViewProjectionMatrix * u_ModelMatrix * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

out vec4 FragColor;

void main()
{
    // 简单的红色输出，不依赖任何uniform
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}