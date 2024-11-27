#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
#include<gl/glm/ext.hpp>
#include<gl/glm/glm.hpp>
#include<gl/glm/gtc/matrix_transform.hpp>
#include"file_open.h"
#include"read_obj.h"
#include"robot.h"
#include"aabb.h"
#include<random>


robot::robot(float size) {
    robot_dx = 0;
    robot_dy = 0;
    robot_dz = 0;

    scale = size;

    x = 0;
    y = 0.1;
    z = 0;

    robot_speed = 0.02;

    jump_speed = 0.2;


    max_swing = 30;
    swing_speed = 1.5f;

    rotate_robot = 0;


    //¸ö
    scale_x[0] = 1.0f;
    scale_z[0] = 1.0f;
    scale_y[0] = 2.0f;
    first_y[0] = -1.0f;
    color[0] = glm::vec3(0.0f, 0.0f, 1.0f);


    //¸Ó¸®
    scale_x[1] = 0.7f;
    scale_y[1] = 0.7f;
    scale_z[1] = 0.7f;
    first_y[1] = -0.4f;
    color[1] = glm::vec3(0.0f, 0.0f, 1.0f);

    //¿ÞÆÈ
    scale_x[2] = 0.35f;
    scale_z[2] = 0.35f;
    scale_y[2] = 1.3f;
    first_x[2] = -0.3f;
    first_y[2] = -1.0f;
    swing_y[2] = -0.3;
    color[2] = glm::vec3(1.0f, 0.0f, 1.0f);

    //¿À¸¥ÆÈ
    scale_x[3] = 0.35f;
    scale_z[3] = 0.35f;
    scale_y[3] = 1.3f;
    first_x[3] = 0.3f;
    first_y[3] = -1.0f;
    swing_y[3] = -0.3;
    color[3] = glm::vec3(0.0f, 1.0f, 1.0f);

    //¿Þ´Ù¸®
    scale_x[4] = 0.35f;
    scale_z[4] = 0.35f;
    scale_y[4] = 1.5f;
    first_x[4] = -0.1f;
    first_y[4] = -1.75f;
    swing_y[4] = -0.4;
    color[4] = glm::vec3(1.0f, 1.0f, 0.0f);


    //¿À¸¥´Ù¸®
    scale_x[5] = 0.35f;
    scale_z[5] = 0.35f;
    scale_y[5] = 1.5f;
    first_x[5] = 0.1f;
    first_y[5] = -1.75f;
    swing_y[5] = -0.4;
    color[5] = glm::vec3(1.0f, 1.0f, 0.0f);

    //ÄÚ
    scale_x[6] = 0.1f;
    scale_y[6] = 0.1f;
    scale_z[6] = 0.3f;
    first_y[6] = -0.4f;
    first_z[6] = 0.2f;
    color[6] = glm::vec3(0.0f, 0.0f, 0.5f);
    
    for (Model& m : body) {
        read_obj_file("cube.obj", &m);
    }


    

    for (int i = 0; i < 7; ++i) {
        
        trans[i] = glm::mat4(1.0f);
        trans[i] = glm::translate(trans[i], glm::vec3(x, y + 2.0f, z));
        trans[i] = glm::rotate(trans[i], glm::radians(float(rotate_robot)), glm::vec3(0.0f, 1.0f, 0.0f));
        trans[i] = glm::translate(trans[i], glm::vec3(mini_x, mini_y + 2.0f, mini_z));
        trans[i] = glm::scale(trans[i], glm::vec3(scale, scale, scale));
        trans[i] = glm::translate(trans[i], glm::vec3(first_x[i], first_y[i], first_z[i]));
        trans[i] = glm::translate(trans[i], glm::vec3(0.0f, -swing_y[i], 0.0f));
        trans[i] = glm::rotate(trans[i], glm::radians(float(swing_angle[i])), glm::vec3(1.0f, 0.0f, 0.0f));
        trans[i] = glm::translate(trans[i], glm::vec3(0.0f, swing_y[i], 0.0f));
        trans[i] = glm::scale(trans[i], glm::vec3(scale_x[i], scale_y[i], scale_z[i]));

        
    }
}


void robot::gen_buffer() {
    for (int i = 0; i < 7; ++i) {
        glGenVertexArrays(1, &VAO[i]);
        glBindVertexArray(VAO[i]);

        glGenBuffers(1, &VBO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBufferData(GL_ARRAY_BUFFER, body[i].vertex_count * sizeof(Vertex), body[i].vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &EBO[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, body[i].face_count * sizeof(Face), body[i].faces, GL_STATIC_DRAW);
    }
}
void robot::update_position() {
    for (int i = 0; i < 7; ++i) {
        trans[i] = glm::mat4(1.0f);
        
        trans[i] = glm::translate(trans[i], glm::vec3(x, y + 2.0f, z));
        trans[i] = glm::rotate(trans[i], glm::radians(float(rotate_robot)), glm::vec3(0.0f, 1.0f, 0.0f));
        trans[i] = glm::scale(trans[i], glm::vec3(scale, scale, scale));
        trans[i] = glm::translate(trans[i], glm::vec3(0, mini_y, mini_z));
        trans[i] = glm::translate(trans[i], glm::vec3(first_x[i], first_y[i], first_z[i]));
        trans[i] = glm::translate(trans[i], glm::vec3(0.0f, -swing_y[i], 0.0f));
        trans[i] = glm::rotate(trans[i], glm::radians(float(swing_angle[i])), glm::vec3(1.0f, 0.0f, 0.0f));
        trans[i] = glm::translate(trans[i], glm::vec3(0.0f, swing_y[i], 0.0f));
        trans[i] = glm::scale(trans[i], glm::vec3(scale_x[i], scale_y[i], scale_z[i]));
    }
}

aabb robot::get_aabb(){
    aabb temp = {x-0.1, x+0.1, y, y+1.5, z-0.1, z+0.1};
    return temp;
}