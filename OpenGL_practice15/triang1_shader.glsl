#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 in_color;
out vec3 out_color;

uniform mat4 rotate_mat;


void main()
{
    gl_Position = rotate_mat * vec4(vPos.x, vPos.y, vPos.z, 1.0);
    out_color= in_color;
}
