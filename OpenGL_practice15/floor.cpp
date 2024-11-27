#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
#include<gl/glm/ext.hpp>
#include<gl/glm/glm.hpp>
#include<gl/glm/gtc/matrix_transform.hpp>
#include"file_open.h"
#include"read_obj.h"
#include"floor.h"
#include"aabb.h"
#include<random>

std::random_device(frd);
std::mt19937 fg(frd());

/*
    Model model;
    float x;
    float y;
    float z;
    float dy;
    float gravity;
    GLuint VAO, VBO, EBO;
    glm::mat4 trans;
*/

aabb my_floor::get_aabb() {
    aabb temp = { x - 1.0f, x + 1.0f, y - 1.0f, y + 1.0f, z - 1.0f, z + 1.0f };
    return temp;
}
void my_floor::init() {
    read_obj_file("cube.obj", &model);
    x = 0;
    y = -0.8;
    z = 0.0;
    dy = 0;
    scale = 3.0f;
    gravity = 0.098;
    is_down = fg() % 10;
    type = "floor";

    trans = glm::mat4(1.0f);
    color = glm::vec3(1.0f, 0.0f, 1.0f);
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(x, y, z));
    trans = glm::scale(trans, glm::vec3(scale, scale, scale));

}
void my_floor::gen_buffuer() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, model.vertex_count * sizeof(Vertex), model.vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.face_count * sizeof(Face), model.faces, GL_STATIC_DRAW);
}
void my_floor::update_position() {
    y += dy;
    
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(x, y, z));
    trans = glm::scale(trans, glm::vec3(scale, scale, scale));
}


my_floor::my_floor(float inx, float inz) {
    init();
    x = inx;
    z = inz;
    gen_buffuer();
}