#pragma once


struct shape {
    float x{};
    float y{};
    float z{};

    float dy{};

    float scale_x{1.0f};
    float scale_y{ 1.0f };
    float scale_z{ 1.0f };


    float angle_x{};
    float angle_y{};
    float angle_z{};

    Model model;

    glm::vec3 color;

    glm::mat4 trans;
    GLuint VAO, VBO, NBO;

    shape() = default;
    shape(float, float, float, const char *, glm::vec3);

    void update_position();

    void gen_buffer();
};