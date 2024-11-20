#pragma once
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
#include<gl/glm/ext.hpp>
#include<gl/glm/glm.hpp>
#include<gl/glm/gtc/matrix_transform.hpp>


struct aabb
{
    float x1;
    float x2;
    float y1;
    float y2;
    float z1;
    float z2;
};

typedef struct my_floor
{
    Model model;
    float x;
    float y;
    float z;
    float dy;
    float gravity;
    GLuint VAO, VBO, EBO;
    glm::mat4 trans;

    my_floor(int);
    aabb get_aabb();
    void init();

    void gen_buffuer();
    void update_position();
}my_floor;