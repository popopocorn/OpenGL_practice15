#include<iostream>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
#include<gl/glm/ext.hpp>
#include<gl/glm/glm.hpp>
#include<gl/glm/gtc/matrix_transform.hpp>
#include"read_obj.h"
#include"shape.h"


shape::shape(float in_x, float in_y, float in_z, const char * obj_name, glm::vec3 c) {
    x = in_x;
    y = in_y;
    z = in_z;
    trans = glm::translate(trans, glm::vec3(x, y, z));
    trans = glm::rotate(trans, glm::radians(angle_x), glm::vec3(1.0f, 0.0f, 0.0f));
    trans = glm::rotate(trans, glm::radians(angle_y), glm::vec3(0.0f, 1.0f, 0.0f));
    trans = glm::rotate(trans, glm::radians(angle_z), glm::vec3(0.0f, 0.0f, 1.0f));
    trans = glm::scale(trans, glm::vec3(scale_x, scale_y, scale_z));
    read_obj_file(obj_name, &model);
    color = c;

}

void shape::update_position() {
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(x, y, z));
    trans = glm::rotate(trans, glm::radians(angle_x), glm::vec3(1.0f, 0.0f, 0.0f));
    trans = glm::rotate(trans, glm::radians(angle_y), glm::vec3(0.0f, 1.0f, 0.0f));
    trans = glm::rotate(trans, glm::radians(angle_z), glm::vec3(0.0f, 0.0f, 1.0f));
    trans = glm::scale(trans, glm::vec3(scale_x, scale_y, scale_z));
}

void shape::gen_buffer() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(Vertex), model.vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &NBO);
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, model.nvectors.size() * sizeof(Normal), model.nvectors.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
}
