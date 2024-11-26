#pragma once


struct robot {
    float x;
    float y;
    float z;
    float scale;

    float scale_x[7];
    float scale_y[7];
    float scale_z[7];
    
    float first_x[7];
    float first_y[7];
    float first_z[7];

    float robot_dx;
    float robot_dy;
    float robot_dz;
    
    float robot_speed{ 0.04 };
    float jump_speed{ 0.1f };

    float swing_y[7];
    int swing_angle[7];
    float swing_da[7];
    int max_swing{ 30 };
    float swing_speed{ 1.5f };

    int rotate_robot{ };

    GLuint VAO[7], VBO[7], EBO[7];
    Model body[7];

    glm::mat4 trans[7];
    glm::vec3 color[7];

    robot(float);

    void gen_buffer();
    void update_position();

    //aabb get_aabb();
};