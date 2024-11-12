#include<iostream>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
#include<gl/glm/ext.hpp>
#include<gl/glm/glm.hpp>
#include<gl/glm/gtc/matrix_transform.hpp>
#include"file_open.h"
#include"read_obj.h"
#include<vector>
#include<random>
//�̸� �����Ұ�
#define vertex_shader_code "middle_vertex_shader.glsl"
#define fragment_shader_code "middle_fragment_shader.glsl"
std::random_device rd;
std::mt19937 g(rd());

//------------------------------------------------------
//�ݹ��Լ�
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int, int, int, int);
GLvoid MoveMouse(int, int);
GLvoid timer(int);
//------------------------------------------------------
//���̴� �� ����
GLuint shader_program;
GLuint vertexShader;
GLuint fragmentShader;

GLuint LAO, LBO;

void make_vertex_shader();
void make_fragment_shader();
GLuint make_shader();
GLvoid init_buffer();
//------------------------------------------------------
//��������
GLclampf base_r = 0.0f;
GLclampf base_g = 0.0f;
GLclampf base_b = 0.0f;
GLint width{ 800 }, height{ 1100 };

typedef struct shapes {
    GLuint VAO, VBO, EBO;
    std::vector<float> vertices;
    glm::vec3 body_color = glm::vec3(0.0f, 0.0f, 1.0f);
    int num_of_point{};
    int num_of_face{};
    std::vector<unsigned int>index_list;


    void init() {
        num_of_point = rand() % 4 + 3;  

        switch (num_of_point) {
        case 3: // �ﰢ��
            vertices = {
                0.0f,  0.15f, 0.0f,  // ��
               -0.15f, -0.15f, 0.0f,  // ����
                0.15f, -0.15f, 0.0f   // ������
            };
            index_list = { 0, 1, 2 };  // �ﰢ���� �ε���
            num_of_face = 1;
            break;

        case 4: // �簢��
            vertices = {
                -0.15f,  0.15f, 0.0f,  // ���� ��
                 0.15f,  0.15f, 0.0f,   // ������ ��
                 0.15f, -0.15f, 0.0f,   // ������ �Ʒ�
                -0.15f, -0.15f, 0.0f   // ���� �Ʒ�
            };
            index_list = { 0, 1, 2, 0, 2, 3 };  
            num_of_face = 2;
            break;

        case 5: // ������
            vertices = {
                0.0f,  0.15f, 0.0f,   // ���
                0.15f,  0.05f, 0.0f,  // ������ ��
                0.09f, -0.2f, 0.0f,   // ������ �Ʒ�
               -0.09f, -0.2f, 0.0f,   // ���� �Ʒ�
               -0.15f,  0.05f, 0.0f   // ���� ��
            };
            index_list = { 0, 1, 2, 0, 2, 3, 0, 3, 4 };  
            num_of_face = 3;
            break;

        case 6: // ������
            vertices = {
                0.0f,  0.15f, 0.0f,   // ���
                0.15f,  0.075f, 0.0f, // ������ ��
                0.15f, -0.075f, 0.0f, // ������ �Ʒ�
                0.0f, -0.15f, 0.0f,   // �ϴ�
               -0.15f, -0.075f, 0.0f, // ���� �Ʒ�
               -0.15f,  0.075f, 0.0f  // ���� ��
            };
            index_list = { 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5 };  
            num_of_face = 4;
            break;

        default:

            break;
        }

        // ���� ����
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_of_face * 3 * sizeof(unsigned int), index_list.data(), GL_STATIC_DRAW);
    }

};

bool flag_drag;

float mouse_first_x;
float mouse_first_y;
float mouse_end_x;
float mouse_end_y;

float mouse_line[6] = { };

glm::vec3 red_color(1.0f, 0.0f, 0.0f);

//------------------------------------------------------
//�ʿ��� �Լ� ����


shapes s[10];
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
    glutMouseFunc(Mouse);
    glutMotionFunc(MoveMouse);
    glutTimerFunc(60, timer, 0);

    init_buffer();

    for (int i = 0; i < 10; ++i) {
        s[i].init();
    }

    glutMainLoop();

}


GLvoid drawScene(GLvoid) {

    glClearColor(base_r, base_g, base_b, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);

    glValidateProgram(shader_program);


    GLuint color = glGetUniformLocation(shader_program, "in_color");
    GLuint trans_mat = glGetUniformLocation(shader_program, "trans");

    glm::mat4 temp(1.0f);
    if(flag_drag){
        glLineWidth(3.0f);
        glUniform3fv(color, 1, glm::value_ptr(red_color));
        glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(temp));
        glBindVertexArray(LAO);
        glDrawArrays(GL_LINES, 0, 2);

    }
    for (int i = 0; i < 10; ++i) {
        glUniform3fv(color, 1, glm::value_ptr(red_color));
        glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(temp));
        glBindVertexArray(s[i].VAO);
        glDrawElements(GL_TRIANGLES, s[i].num_of_face * 3, GL_UNSIGNED_INT, 0);
    }


    glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);

}
GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {


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
        std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
        return;
    }
    else {
        std::cout << "������ ����\n";
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
        std::cerr << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
        return;
    }
    else {
        std::cout << "������ ����\n";
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
        std::cerr << "���̴� ���� ����\n" << errorLog << std::endl;

    }
    else {
        std::cout << "\n������ ����\n";

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

    }


    glUseProgram(shader01);

    return shader01;

}

GLvoid init_buffer() {


    glGenVertexArrays(1, &LAO);
    glBindVertexArray(LAO);

    glGenBuffers(1, &LBO);
    glBindBuffer(GL_ARRAY_BUFFER, LBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mouse_line), mouse_line, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);



}

GLvoid Mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        mouse_first_x = (float)(x - (float)width / 2.0) * (float)(1.0 / (float)(width / 2.0));
        mouse_first_y = -(float)(y - (float)height / 2.0) * (float)(1.0 / (float)(height / 2.0));
        flag_drag = true;


        mouse_line[0] = mouse_first_x;
        mouse_line[1] = mouse_first_y;
        mouse_line[3] = mouse_first_x;
        mouse_line[4] = mouse_first_y;

        glBindBuffer(GL_ARRAY_BUFFER, LBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(mouse_line), mouse_line);
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        flag_drag = false;
    }
    glutPostRedisplay();
}
GLvoid MoveMouse(int x, int y) {
    if (flag_drag) {
        mouse_end_x = (float)(x - (float)width / 2.0) * (float)(1.0 / (float)(width / 2.0));
        mouse_end_y = -(float)(y - (float)height / 2.0) * (float)(1.0 / (float)(height / 2.0));

        
        mouse_line[3] = mouse_end_x;
        mouse_line[4] = mouse_end_y;


        glBindBuffer(GL_ARRAY_BUFFER, LBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(mouse_line), mouse_line);

  
    }
    glutPostRedisplay();
}
GLvoid timer(int value) {
    
    glutTimerFunc(60, timer, 0);
    glutPostRedisplay();
}