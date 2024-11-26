#pragma once

#include<string>


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
    float scale;
    float x;
    float y;
    float z;
    float dy;
    float gravity;
    
    std::string type;
    bool is_down;

    GLuint VAO, VBO, EBO;
    glm::mat4 trans;
    glm::vec3 color;

    my_floor() = default;
    my_floor(float, float);
    aabb get_aabb();
    void init();

    void gen_buffuer();
    void update_position();
}my_floor;