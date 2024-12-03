#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
#include<gl/glm/ext.hpp>
#include<gl/glm/glm.hpp>
#include<gl/glm/gtc/matrix_transform.hpp>
#include"file_open.h"
#include"read_obj.h"
#include"crane.h"
#include<random>




crane::crane(float size) {
    scale = size;
    c[0] = shape{ 0.0f, 0.0f, 0.0f, "cube1.obj", red_color };
    c[1] = shape{ 0.0f, 0.0f, 0.0f, "cube1.obj", green_color };
    c[2] = shape{ 0.0f, 0.0f, 0.0f, "cube1.obj", yellow_color };
    c[3] = shape{ 0.0f, 0.0f, 0.0f, "cube1.obj", yellow_color };
    c[4] = shape{ 0.0f, 0.0f, 0.0f, "cube1.obj", blue_color };
    c[5] = shape{ 0.0f, 0.0f, 0.0f, "cube1.obj", blue_color };

    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(x, y, z));
    trans = glm::scale(trans, glm::vec3(scale, scale, scale));

    set_crane();
    
}

void crane::set_crane(){
    c[0].scale_x = 5.0f;
    c[0].scale_y = 2.0f;
    c[0].scale_z = 5.0f;

    //Æ÷½Å
    c[1].scale_x = 0.5f;
    c[1].scale_y = 0.5f;
    c[1].scale_z = 2.5f;

    c[2].scale_x = 0.5f;
    c[2].scale_y = 0.5f;
    c[2].scale_z = 2.5f;

    c[1].z = 1.5f;
    c[2].z = 1.5f;
    c[1].y = -0.1f;
    c[2].y = -0.1f;
    c[1].x = 1.0f;
    c[2].x = -1.0f;

    //Áß°£¸öÅë
    c[3].scale_x = 2.5f;
    c[3].scale_y = 1.5f;
    c[3].scale_z = 2.5f;
    c[3].y = 0.8f;

    //ÆÈ
    c[4].scale_x = 0.5f;
    c[4].scale_y = 2.5f;
    c[4].scale_z = 0.5f;

    c[5].scale_x = 0.5f;
    c[5].scale_y = 2.5f;
    c[5].scale_z = 0.5f;

    c[4].y = 1.4f;
    c[5].y = 1.4f;
    c[4].x = 0.35;
    c[5].x = -0.35;

}

void crane::gen_buffer() {
    for (shape& s : c) {
        s.gen_buffer();
    }
}
void crane::update_position() {
    trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(x, y, z));
    trans = glm::scale(trans, glm::vec3(scale, scale, scale));
}
