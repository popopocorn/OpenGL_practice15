#version 330 core

layout(location = 0) in vec3 position;
uniform vec3 in_color;
out vec3 out_color;

uniform mat4 trans;
uniform mat4 view;
uniform mat4 projection;

void main() {

    gl_Position = projection * trans * vec4(position, 1.0);
    out_color=in_color;
}
