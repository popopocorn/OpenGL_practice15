#include<iostream>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
#include"file_open.h"
#include<random>
#include<vector>
//미리 선언할거
#define vertex_shader_code "07_vertex_shader.glsl"
#define fragment_shader_code "07_fragment_shader.glsl"
std::random_device rd;
std::mt19937 g(rd());

//------------------------------------------------------
//콜백함수
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
//------------------------------------------------------
//셰이더 용 선언
GLuint shader_program;
GLuint vertexShader;
GLuint fragmentShader;
GLuint VAO[4], VBO[4], EBO;

void make_vertex_shader();
void make_fragment_shader();
GLuint make_shader();
GLvoid init_buffer();
//------------------------------------------------------
//전역변수
GLclampf base_r = 1.0f;
GLclampf base_g = 1.0f;
GLclampf base_b = 1.0f;
GLint width{ 800 }, height{ 600 };

int p_count;
int l_count;
int t_count;
int r_count;

typedef struct shapes {
    std::vector<GLclampf> vertices;
    GLclampf r, g, b;
    char shape_{};
    int shape_index{};
};

shapes shape_list[10];
int list_index{};
unsigned int vertex_index_list[10][6];


//------------------------------------------------------
//필요한 함수 선언
void create_point();
void create_line();
void create_tri();
void create_rect();
void update_vertex_buffer();
void clear_buffer();
void move(char);
//------------------------------------------------------
void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(width, height);
    glutCreateWindow("Example1");

    glewExperimental = GL_TRUE;
    glewInit();

    make_vertex_shader();
    make_fragment_shader();
    shader_program = make_shader();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);

    init_buffer();

    glutMainLoop();

}


GLvoid drawScene(GLvoid) {

    glClearColor(base_r, base_g, base_b, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);

    glValidateProgram(shader_program);


    glBindVertexArray(VAO[0]);
    glPointSize(10.0);
    glDrawArrays(GL_POINTS, 0, p_count);

    glBindVertexArray(VAO[1]);

    glDrawArrays(GL_LINES, 0, 2 * l_count);

    glBindVertexArray(VAO[2]);
    glDrawArrays(GL_TRIANGLES, 0, 3 * t_count);

    glBindVertexArray(VAO[3]);

    glDrawElements(GL_TRIANGLES, 6 * r_count, GL_UNSIGNED_INT, 0);






    glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);

}
GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'p':
        if (list_index < 10)
            create_point();

        ++p_count;
        break;

    case 'l':
        if (list_index < 10)
            create_line();

        ++l_count;
        break;

    case 't':
        if (list_index < 10)
            create_tri();

        ++t_count;
        break;

    case 'r':
        if (list_index < 10)
            create_rect();

        ++r_count;
        break;

    case 'w':
        move(key);
        break;

    case 'a':
        move(key);
        break;

    case 's':
        move(key);
        break;

    case 'd':
        move(key);
        break;

    case 'c':
        clear_buffer();
        break;

    case 'q':
        glutLeaveMainLoop();
        break;
    }

    glutPostRedisplay();
}

void make_vertex_shader() {
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLchar* vertexSource;
    vertexSource = file_to_buf(vertex_shader_code);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
        return;
    }
    else {
        std::cout << "컴파일 성공\n";
    }
}
void make_fragment_shader() {
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLchar* fragmentSource;
    fragmentSource = file_to_buf(fragment_shader_code);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
        return;
    }
    else {
        std::cout << "컴파일 성공\n";
    }
}
GLuint make_shader() {
    GLuint shader01;
    shader01 = glCreateProgram();
    glAttachShader(shader01, vertexShader);
    glAttachShader(shader01, fragmentShader);

    glLinkProgram(shader01);


    GLint result;
    GLchar errorLog[512];
    glGetProgramiv(shader01, GL_LINK_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(shader01, 512, NULL, errorLog);
        std::cerr << "셰이더 생성 실패\n" << errorLog << std::endl;

    }
    else {
        std::cout << "\n컴파일 성공\n";

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

    }


    glUseProgram(shader01);

    return shader01;

}

GLvoid init_buffer() {
    for (int i = 0; i < 4; ++i) {
        glGenVertexArrays(1, &VAO[i]);
        glBindVertexArray(VAO[i]);

        glGenBuffers(1, &VBO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 10 * (i + 1), nullptr, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6 * 10, nullptr, GL_STATIC_DRAW);

}

void create_point() {
    float x = float(g() % 2000) / 1000.0f - 1.0f;
    float y = float(g() % 2000) / 1000.0f - 1.0f;
    float r = float(g() % 1000) / 1000.0f;
    float _g = float(g() % 1000) / 1000.0f;
    float b = float(g() % 1000) / 1000.0f;
    shape_list[list_index] = { {x,y,0.0f}, r, _g, b, 'p', p_count };
    std::cout << shape_list[list_index].vertices[0];
    float vertex_data[6] = { shape_list[list_index].vertices[0],shape_list[list_index].vertices[1], shape_list[list_index].vertices[2], shape_list[list_index].r, shape_list[list_index].g, shape_list[list_index].b };
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex_data) * p_count, sizeof(vertex_data), vertex_data);

    ++list_index;
}

void create_line() {
    float x = float(g() % 2000) / 1000.0f - 1.0f;
    float y = float(g() % 2000) / 1000.0f - 1.0f;
    float r = float(g() % 1000) / 1000.0f;
    float _g = float(g() % 1000) / 1000.0f;
    float b = float(g() % 1000) / 1000.0f;
    shape_list[list_index] = { {x,y,0.0f}, r, _g, b, 'l', l_count };
    std::cout << shape_list[list_index].vertices[0];
    float line_data[12] = {
        shape_list[list_index].vertices[0], shape_list[list_index].vertices[1], shape_list[list_index].vertices[2], shape_list[list_index].r, shape_list[list_index].g, shape_list[list_index].b,
        shape_list[list_index].vertices[0] + 0.1f , shape_list[list_index].vertices[1] - 0.1f, shape_list[list_index].vertices[2], shape_list[list_index].r, shape_list[list_index].g, shape_list[list_index].b
    };
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(line_data) * l_count, sizeof(line_data), line_data);
    ++list_index;
}

void create_tri() {
    float x1 = float(g() % 2000) / 1000.0f - 1.0f;
    float y1 = float(g() % 2000) / 1000.0f - 1.0f;
    float r = float(g() % 1000) / 1000.0f;
    float _g = float(g() % 1000) / 1000.0f;
    float b = float(g() % 1000) / 1000.0f;
    shape_list[list_index] = { {x1,y1,0.0f}, r, _g, b, 't', t_count };
    std::cout << shape_list[list_index].vertices[0];

    float triangl_data[18] = {
            shape_list[list_index].vertices[0],shape_list[list_index].vertices[1], shape_list[list_index].vertices[2], shape_list[list_index].r, shape_list[list_index].g, shape_list[list_index].b,
            shape_list[list_index].vertices[0] - 0.1f,shape_list[list_index].vertices[1] - 0.2f, shape_list[list_index].vertices[2], shape_list[list_index].r, shape_list[list_index].g, shape_list[list_index].b,
            shape_list[list_index].vertices[0] + 0.1f,shape_list[list_index].vertices[1] - 0.2f, shape_list[list_index].vertices[2], shape_list[list_index].r, shape_list[list_index].g, shape_list[list_index].b

    };
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(triangl_data) * t_count, sizeof(triangl_data), triangl_data);
    ++list_index;
}

void create_rect() {
    float x1 = float(g() % 2000) / 1000.0f - 1.0f;
    float y1 = float(g() % 2000) / 1000.0f - 1.0f;
    float r = float(g() % 1000) / 1000.0f;
    float _g = float(g() % 1000) / 1000.0f;
    float b = float(g() % 1000) / 1000.0f;
    shape_list[list_index] = { {x1,y1,0.0f}, r, _g, b, 'r', r_count };
    std::cout << shape_list[list_index].vertices[0];

    float rectangle_data[24] = {
            shape_list[list_index].vertices[0],shape_list[list_index].vertices[1], shape_list[list_index].vertices[2], shape_list[list_index].r, shape_list[list_index].g, shape_list[list_index].b,
            shape_list[list_index].vertices[0] + 0.1f,shape_list[list_index].vertices[1], shape_list[list_index].vertices[2], shape_list[list_index].r, shape_list[list_index].g, shape_list[list_index].b,
            shape_list[list_index].vertices[0] + 0.1f,shape_list[list_index].vertices[1] - 0.1f, shape_list[list_index].vertices[2], shape_list[list_index].r, shape_list[list_index].g, shape_list[list_index].b,
            shape_list[list_index].vertices[0],shape_list[list_index].vertices[1] - 0.1f, shape_list[list_index].vertices[2], shape_list[list_index].r, shape_list[list_index].g, shape_list[list_index].b,
    };
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(rectangle_data) * r_count, sizeof(rectangle_data), rectangle_data);
    vertex_index_list[r_count][0] = 4 * r_count; //4 8 12 16..
    vertex_index_list[r_count][1] = 4 * r_count + 2; //5, 9 13 17
    vertex_index_list[r_count][2] = 4 * r_count + 1;
    vertex_index_list[r_count][3] = 4 * r_count;
    vertex_index_list[r_count][4] = 4 * r_count + 3;
    vertex_index_list[r_count][5] = 4 * r_count + 2;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_index_list[r_count]) * r_count, sizeof(vertex_index_list[r_count]), vertex_index_list[r_count]);
    ++list_index;
}

void update_vertex_buffer() {

}

void clear_buffer() {

    for (int i = 0; i < 4; ++i) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 10 * (i + 1), nullptr, GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6 * 10, nullptr, GL_STATIC_DRAW);


    p_count = 0;
    l_count = 0;
    t_count = 0;
    r_count = 0;
    list_index = 0;
}

void move(char key) {
    float dx{};
    float dy{};
    switch (key) {
    case'w':
        dy = 0.1f;
        break;

    case'a':
        dx = -0.1f;
        break;

    case's':
        dy = -0.1f;
        break;

    case'd':
        dx = 0.1f;
        break;

    }



    int target_ = g() % list_index;

    shape_list[target_].vertices[0] += dx;
    shape_list[target_].vertices[1] += dy;

    switch (shape_list[target_].shape_) {
    case 'p':
    {
        float vertex_data[6] = { shape_list[target_].vertices[0],shape_list[target_].vertices[1], shape_list[target_].vertices[2], shape_list[target_].r, shape_list[target_].g, shape_list[target_].b };
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex_data) * shape_list[target_].shape_index, sizeof(vertex_data), vertex_data);
        break;
    }

    case 'l':
    {
        float line_data[12] = {
            shape_list[target_].vertices[0], shape_list[target_].vertices[1], shape_list[target_].vertices[2], shape_list[target_].r, shape_list[target_].g, shape_list[target_].b,
            shape_list[target_].vertices[0] + 0.1f , shape_list[target_].vertices[1] - 0.1f, shape_list[target_].vertices[2], shape_list[target_].r, shape_list[target_].g, shape_list[target_].b
        };
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(line_data) * shape_list[target_].shape_index, sizeof(line_data), line_data);
    }

    break;

    case 't':
    {
        float triangl_data[18] = {
            shape_list[target_].vertices[0],shape_list[target_].vertices[1], shape_list[target_].vertices[2], shape_list[target_].r, shape_list[target_].g, shape_list[target_].b,
            shape_list[target_].vertices[0] - 0.1f,shape_list[target_].vertices[1] - 0.2f, shape_list[target_].vertices[2], shape_list[target_].r, shape_list[target_].g, shape_list[target_].b,
            shape_list[target_].vertices[0] + 0.1f,shape_list[target_].vertices[1] - 0.2f, shape_list[target_].vertices[2], shape_list[target_].r, shape_list[target_].g, shape_list[target_].b

        };
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(triangl_data) * shape_list[target_].shape_index, sizeof(triangl_data), triangl_data);
    }

    break;

    case 'r':
    {
        float rectangle_data[24] = {
        shape_list[target_].vertices[0],shape_list[target_].vertices[1], shape_list[target_].vertices[2], shape_list[target_].r, shape_list[target_].g, shape_list[target_].b,
        shape_list[target_].vertices[0] + 0.1f,shape_list[target_].vertices[1], shape_list[target_].vertices[2], shape_list[target_].r, shape_list[target_].g, shape_list[target_].b,
        shape_list[target_].vertices[0] + 0.1f,shape_list[target_].vertices[1] - 0.1f, shape_list[target_].vertices[2], shape_list[target_].r, shape_list[target_].g, shape_list[target_].b,
        shape_list[target_].vertices[0],shape_list[target_].vertices[1] - 0.1f, shape_list[target_].vertices[2], shape_list[target_].r, shape_list[target_].g, shape_list[target_].b,
        };
        glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(rectangle_data) * shape_list[target_].shape_index, sizeof(rectangle_data), rectangle_data);
    }

    break;

    }


}