#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
#include<gl/glm/ext.hpp>
#include<gl/glm/glm.hpp>
#include<gl/glm/gtc/matrix_transform.hpp>
#include"file_open.h"
#include"read_obj.h"
#include"floor.h"


aabb my_floor::get_aabb() {
    aabb temp = { x - 1.0f, x + 1.0f, y - 1.0f, y + 1.0f, z - 1.0f, z + 1.0f };
    return temp;
}
void my_floor::init() {
    read_obj_file("cube.obj", &model);
    x = 0;
    y = 0;
    dy = 0;
    trans = glm::mat4(1.0f);

    gen_buffuer();
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
    dy += gravity;
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(0.0f, dy, 0.0f));
    trans = glm::scale(trans, glm::vec3(2.0f, 2.0f, 2.0f));
}


my_floor::my_floor(int value) {

}