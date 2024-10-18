#include<iostream>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
#include"file_open.h"
#include<random>
#include<vector>
#include<cmath>
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
GLvoid Mouse(int button, int state, int x, int y);
void update(int);
//------------------------------------------------------
//셰이더 용 선언
GLuint shader_program;
GLuint vertexShader;
GLuint fragmentShader;
GLuint VAO, VBO;

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

typedef struct spiral_vertex {
    std::vector<GLclampf> vertices;
    GLclampf r, g, b;
    float dx{ 0.01f }, dy{ -0.01f };
};

int sp_point = 0; // 현재 점의 수
bool drawing = false; // 나선을 그리기 시작할지 여부
float mouse_x = 0.0f; // 클릭한 x 좌표
float mouse_y = 0.0f; // 클릭한 y 좌표
int spiral_count;
int repeat;

std::vector<std::vector<float>> tempspiral_point;
//------------------------------------------------------
//필요한 함수 선언
void draw_spiral(float m_x, float m_y);


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

    init_buffer();

    glutTimerFunc(1000 / 60, update, 0); // 타이머 시작

    glutMainLoop();
}


GLvoid drawScene(GLvoid) {
    glClearColor(base_r, base_g, base_b, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);
    glBindVertexArray(VAO);

    if (sp_point > 0) {
        glPointSize(5.0);
        glDrawArrays(GL_POINTS, 0, (spiral_count * 251) + sp_point); // 생성된 점을 그린다.
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
    if ('1' <= key && key <= '5') {
        repeat = key - '0';
        while(repeat>0){

            if (sp_point == 0) {
                mouse_x = float(g() % 2000) / 1000.0f - 1.0f;
                mouse_y = float(g() % 2000) / 1000.0f - 1.0f;
                tempspiral_point.clear();
                drawing = true; // 그리기 시작
                --repeat;
            }
        }
    }
    glutPostRedisplay();
}

GLvoid Mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        mouse_x = (float)(x - (float)width / 2.0) * (float)(1.0 / (float)(width / 2.0));
        mouse_y = -(float)(y - (float)height / 2.0) * (float)(1.0 / (float)(height / 2.0));
        
        drawing = true; // 그리기 시작
    }
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
    else {
        std::cout << "컴파일 성공\n";
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
    else {
        std::cout << "컴파일 성공\n";
    }
}

GLuint make_shader() {
    GLuint shader01 = glCreateProgram();
    glAttachShader(shader01, vertexShader);
    glAttachShader(shader01, fragmentShader);
    glLinkProgram(shader01);

    GLint result;
    GLchar errorLog[512];
    glGetProgramiv(shader01, GL_LINK_STATUS, &result);
    if (!result) {
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
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 1000, nullptr, GL_DYNAMIC_DRAW); // 동적 버퍼 설정

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

// 타이머 업데이트 함수
void update(int) {
    if (drawing) {
        draw_spiral(mouse_x, mouse_y); // 나선 그리기
    }

    glutPostRedisplay();
    glutTimerFunc(1000 / 60, update, 0);
}
float last_x;
float last_y;
void draw_spiral(float m_x, float m_y) {
    if(sp_point < 126){
        float theta = sp_point * 0.1f;
        float r = 0.1f * exp(0.1f * theta);
        float x = r * cos(theta) + m_x;
        float y = r * sin(theta) + m_y;

        // VBO에 점 추가
        float sp_vertex[2] = { x, y };
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * 2 * ((spiral_count * 251) + sp_point), sizeof(sp_vertex), sp_vertex);

        sp_point++; // 점 수 증가
        last_x = x;
        last_y = y;
        tempspiral_point.push_back({ x, y });
    }
    else if(sp_point<252){
        float temp_x = tempspiral_point.back()[0];
        float temp_y = tempspiral_point.back()[1];
        float x = last_x - (temp_x - last_x);
        float y = last_y - (temp_y - last_y);;
        tempspiral_point.erase(tempspiral_point.end() - 1);
        // VBO에 점 추가
        float sp_vertex[2] = { x, y };
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * 2 * ((spiral_count * 251) + sp_point), sizeof(sp_vertex), sp_vertex);

        sp_point++; // 점 수 증가

    }
    
    if (sp_point > 251) {
        ++spiral_count;
        sp_point = 0;
        tempspiral_point.clear();
    }
}