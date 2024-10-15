#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 in_color;
out vec3 out_color;

uniform float time;
uniform int motion_type;

float dx;
float dy;

void main()
{
    switch(motion_type){
    case 1:
        
        break;

    case 2:
        
        break;

    case 3: 
        
        break;

    case 4:
        
        break;
    
    
    }

    out_color = in_color;
}
