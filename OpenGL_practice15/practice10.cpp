#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <random>
#include <vector>
#include <math.h>
#include "file_open.h"
// 미리 선언할거
#define vertex_shader_code "07_vertex_shader.glsl"
#define fragment_shader_code "07_fragment_shader.glsl"

const float M_PI = 3.14159265;

std::random_device rd;
std::mt19937 g(rd());

//------------------------------------------------------
// 콜백함수
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
//------------------------------------------------------
// 셰이더 용 선언
GLuint shader_program;
GLuint vertexShader;
GLuint fragmentShader;
GLuint VAO[6], VBO[6];

// 스파이럴 구조체
typedef struct spiral {
    float x, y, z; // 위치
    float r, g, b; // 색상
};

std::vector<spiral> spiralPointsCom; // 스파이럴 점들을 저장할 벡터

bool is_line = false;

void make_vertex_shader();
void make_fragment_shader();
GLuint make_shader();
GLvoid init_buffer();
void make_spiral(float centerX, float centerY, int turns, bool clockwise, int what);
void draw_spiral(int);

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
    if (is_line) {
        for (int i = 0; i < 6; ++i) {

            glBindVertexArray(VAO[i]);

            glDrawArrays(GL_LINE_STRIP, 0, spiralPointsCom.size());

        }
    }
    else {
        for (int i = 0; i < 6; ++i) {

            glBindVertexArray(VAO[i]);

            glDrawArrays(GL_POINTS, 0, spiralPointsCom.size());

        }
    }

    glutSwapBuffers();
}

GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y) {
    std::cout << key;
    switch (key) {
    case 'p':
        is_line = false;
        break;

    case 'l':
        is_line = true;
        break;

    case 'q':
        glutLeaveMainLoop();
        break;
    }
    float temp_x = float(g() % 2000)/1000.0f-1.0f;
    float temp_y = float(g() % 2000) / 1000.0f - 1.0f;

    if (1 <= key && key <= 5) {
        make_spiral(temp_x, temp_y, 3, false, key);
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
        std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
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
        std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
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
        std::cerr << "셰이더 생성 실패\n" << errorLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

GLvoid init_buffer() {
    for (int i =0;i<6;++i){
        glGenVertexArrays(1, &VAO[i]);
        glBindVertexArray(VAO[i]);

        glGenBuffers(1, &VBO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBufferData(GL_ARRAY_BUFFER, 600 * sizeof(spiral), nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(spiral), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(spiral), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
}

void make_spiral(float centerX, float centerY, int turns, bool clockwise, int what) {
    const int pointsPerTurn = 50;
    float angleIncrement = (2.0f * M_PI) / pointsPerTurn;
    float radius = 0.0f;
    if (what == 0)
        return;
    std::vector<spiral> spiralPoints; 


    for (int i = 0; i < turns * pointsPerTurn; ++i) {
        float angle = i * angleIncrement;
        if (clockwise) {
            angle = -angle; 
        }

        radius += 0.001f; 
        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);

   
        spiralPoints.push_back({ x, y, 0.0f, 1.0f, 0.0f, 0.0f });
    }

 
    spiral lastPoint = spiralPoints.back();  

 
    std::vector<spiral> symmetricSpiralPoints;

 
    for (const auto& point : spiralPoints) {
        float x_symmetric = lastPoint.x - (point.x - lastPoint.x);
        float y_symmetric = lastPoint.y - (point.y - lastPoint.y);


        symmetricSpiralPoints.push_back({ x_symmetric, y_symmetric, 0.0f, 1.0f, 0.0f, 0.0f });
    }

 
    spiralPointsCom = spiralPoints;
    spiralPointsCom.insert(spiralPointsCom.end(), symmetricSpiralPoints.begin(), symmetricSpiralPoints.end());
    if(what == 6){
        glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(spiral) * spiralPointsCom.size(), spiralPointsCom.data());
    }
    else {
        glBindBuffer(GL_ARRAY_BUFFER, VBO[what-1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(spiral) * spiralPointsCom.size(), spiralPointsCom.data());
        make_spiral(float(g() % 2000) / 1000.0f - 1.0f, float(g() % 2000) / 1000.0f - 1.0f, 3, true, what - 1);
    }
}

GLvoid Mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        GLclampf mouse_x = (float)(x - (float)800 / 2.0) * (float)(1.0 / (float)(800 / 2.0));
        GLclampf mouse_y = -(float)(y - (float)600 / 2.0) * (float)(1.0 / (float)(600 / 2.0));
        make_spiral(mouse_x, mouse_y, 3, true, 6);
        glutPostRedisplay();
    }
}

void draw_spiral(int) {
    int temp{};
    for (const auto& point : spiralPointsCom) {
        
    }
}