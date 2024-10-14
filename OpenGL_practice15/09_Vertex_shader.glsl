#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 in_color;
out vec3 out_color;

uniform float time;

uniform float dx;
uniform float dy;


void main()
{
    vec3 moved_pos=vPos;

    moved_pos.x += dx*time;
    moved_pos.y += dy*time;

    gl_Position = vec4(moved_pos.x, moved_pos.y, moved_pos.z, 1.0);

    out_color= in_color;
}
