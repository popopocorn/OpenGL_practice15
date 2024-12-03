#pragma once
#include"aabb.h"
#include"shape.h"

#define green_color glm::vec3(0.0f, 1.0f, 0.0f)
#define blue_color glm::vec3(0.0f, 0.0f, 1.0f)
#define sky_color glm::vec3(0.0f, 0.5f, 1.0f)
#define purple_color glm::vec3(1.0f, 0.0f, 1.0f)
#define brown_color glm::vec3(0.5f, 0.3f, 0.0f)
#define gray_color glm::vec3(0.5f, 0.5f, 0.5f)
#define white_color glm::vec3(1.0f, 1.0f, 1.0f)
#define red_color glm::vec3(1.0f, 0.0f, 0.0f)
#define yellow_color glm::vec3(1.0f, 1.0f, 0.0f)


struct crane {
    float x;
    float y{ -2.0f };
    float z;
    float scale;
    float dx{ 0.1f };
    shape c[6];

    glm::mat4 trans;

    crane(float);
    void set_crane();
    void gen_buffer();
    void update_position();

    
};