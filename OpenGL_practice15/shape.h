#pragma once


struct shape {
    float x{};
    float y{};
    float z{};

    float scale{1.0f};

    float angle_x{};
    float angle_y{};
    float angle_z{};

    Model model;

    glm::vec3 color;

    glm::mat4 trans;
    GLuint VAO, VBO, NBO;


    shape(float, float, float, const char *, glm::vec3);

    void update_position();

    void gen_buffer();
};