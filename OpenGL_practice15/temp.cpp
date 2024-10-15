#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <random>
#include <vector>
#include <math.h>
#include "file_open.h"
// �̸� �����Ұ�
#define vertex_shader_code "07_vertex_shader.glsl"
#define fragment_shader_code "07_fragment_shader.glsl"

const float M_PI = 3.14159265;

std::random_device rd;
std::mt19937 g(rd());

//------------------------------------------------------
// �ݹ��Լ�
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
//------------------------------------------------------
// ���̴� �� ����
GLuint shader_program;
GLuint vertexShader;
GLuint fragmentShader;
GLuint VAO[5], VBO[5];

// �����̷� ����ü
typedef struct spiral {
    float x, y, z; // ��ġ
    float r, g, b; // ����
};

std::vector<spiral> spiralPointsCom; // �����̷� ������ ������ ����

void make_vertex_shader();
void make_fragment_shader();
GLuint make_shader();
GLvoid init_buffer();
void make_spiral(float centerX, float centerY, int turns, bool clockwise);

//------------------------------------------------------
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Spiral Animation");

    glewExperimental = GL_TRUE;
    glewInit();

    make_vertex_shader();
    make_fragment_shader();
    shader_program = make_shader();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);

    init_buffer();

    glutMainLoop();
}

//------------------------------------------------------
GLvoid drawScene(GLvoid) {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);
    glBindVertexArray(VAO[0]);

    // ������ ����
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(spiral) * spiralPointsCom.size(), spiralPointsCom.data());
    glDrawArrays(GL_POINTS, 0, spiralPointsCom.size());

    glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y) {
    if (key == 'q') {
        glutLeaveMainLoop();
    }
    glutPostRedisplay();
}

void make_vertex_shader() {
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLchar* vertexSource = file_to_buf(vertex_shader_code);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
        return;
    }
}

void make_fragment_shader() {
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLchar* fragmentSource = file_to_buf(fragment_shader_code);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
        return;
    }
}

GLuint make_shader() {
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint result;
    GLchar errorLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, errorLog);
        std::cerr << "���̴� ���� ����\n" << errorLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

GLvoid init_buffer() {
    glGenVertexArrays(1, &VAO[0]);
    glBindVertexArray(VAO[0]);

    glGenBuffers(1, &VBO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, 600 * sizeof(spiral), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(spiral), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(spiral), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void make_spiral(float centerX, float centerY, int turns, bool clockwise) {
    const int pointsPerTurn = 100;
    float angleIncrement = (2.0f * M_PI) / pointsPerTurn;
    float radius = 0.0f;

    std::vector<spiral> spiralPoints;  // �����̷� ������ ������ ����

    // ù ��° �����̷�: �ȿ��� ������
    for (int i = 0; i < turns * pointsPerTurn; ++i) {
        float angle = i * angleIncrement;
        if (clockwise) {
            angle = -angle;  // �ð� ����
        }

        radius += 0.001f;  // ������ ���� ��
        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);

        // ���� ���Ϳ� ����
        spiralPoints.push_back({ x, y, 0.0f, 1.0f, 0.0f, 0.0f }); // z, r, g, b�� �����Ͽ� ����
    }

    // ù ��° �����̷��� ������ ���� �������� ��Ī �׸���
    spiral lastPoint = spiralPoints.back();  // ������ ��

    // ��Ī�� ���� ������ ����
    std::vector<spiral> symmetricSpiralPoints;

    // �� ��° �����̷�: ������ ���� �������� ��Ī
    for (const auto& point : spiralPoints) {
        float x_symmetric = lastPoint.x - (point.x - lastPoint.x);  // X ��Ī
        float y_symmetric = lastPoint.y - (point.y - lastPoint.y);  // Y ��Ī

        // ��Ī�� ���� �� ���Ϳ� ����
        symmetricSpiralPoints.push_back({ x_symmetric, y_symmetric, 0.0f, 1.0f, 0.0f, 0.0f }); // ��Ī�� �� ����
    }

    // ���� �����̷� ���� ��Ī�� ���� ����
    spiralPointsCom = spiralPoints;
    spiralPointsCom.insert(spiralPointsCom.end(), symmetricSpiralPoints.begin(), symmetricSpiralPoints.end());
}

GLvoid Mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        GLclampf mouse_x = (float)(x - (float)800 / 2.0) * (float)(1.0 / (float)(800 / 2.0));
        GLclampf mouse_y = -(float)(y - (float)600 / 2.0) * (float)(1.0 / (float)(600 / 2.0));
        make_spiral(mouse_x, mouse_y, 3, true);
        glutPostRedisplay();
    }
}
