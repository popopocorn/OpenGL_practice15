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
GLvoid Mouse(int button, int state, int x, int y);
//------------------------------------------------------
//셰이더 용 선언
GLuint shader_program;
GLuint vertexShader;
GLuint fragmentShader;
GLuint VAO[2], VBO, EBO, line_buffer;

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

unsigned int index_buffer[] = {
    0, 1,
    1, 2,
    2, 0,

    3, 4,
    4, 5,
    5, 3,

    6, 7,
    7, 8,
    8, 6,

    9, 10,
    10, 11,
    11, 9
};

bool is_face = true;

float line[] = {
    0.0, 1.0, 0.0,
    0.0, -1.0, 0.0,

    -1.0, 0.0, 0.0, 
    1.0, 0.0, 0.0
};

//------------------------------------------------------
//필요한 함수 선언
int click_area(GLclampf x, GLclampf y);
void create_tri(int, GLclampf, GLclampf);
void init_tri();

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
    init_tri();

    glutMainLoop();

}


GLvoid drawScene(GLvoid) {

    glClearColor(base_r, base_g, base_b, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);

    glValidateProgram(shader_program);

    glBindVertexArray(VAO[0]);

    if(is_face){
        
        glDrawArrays(GL_TRIANGLES, 0, 12);
    }
    else {
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(VAO[1]);
    glDrawArrays(GL_LINES, 0, 12);

    glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);

}
GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'a':
        is_face = true;

        break;

    case 'b':
        is_face = false;
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
    glGenVertexArrays(1, &VAO[0]);
    glBindVertexArray(VAO[0]);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 3 * 4, nullptr, GL_STATIC_DRAW);

    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 2 * 3 * 4, index_buffer, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &VAO[1]);
    glBindVertexArray(VAO[1]);
    glGenBuffers(1, &line_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, line_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


}

GLvoid Mouse(int button, int state, int x, int y) {
    GLclampf mouse_x = (float)(x - (float)width / 2.0) * (float)(1.0 / (float)(width / 2.0));
    GLclampf mouse_y = -(float)(y - (float)height / 2.0) * (float)(1.0 / (float)(height / 2.0));
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int clicked = click_area(mouse_x, mouse_y);
        std::cout << click_area(mouse_x, mouse_y);
        create_tri(clicked, mouse_x, mouse_y);
    }
    
}

int click_area(GLclampf x, GLclampf y) {
  
    if (-1 < x && x < 0 && 0 < y && y < 1) {
        return 0;
    }
    else if (0 < x && x < 1 && 0 < y && y < 1) {
        return 1;
    }
    else if (-1 < x && x < 0 && -1 < y && y < 0) {
        return 2;
    }
    else if (0 < x && x < 1 && -1 < y && y < 0) {
        return 3;
    }

}

void create_tri(int clicked_area, GLclampf x, GLclampf y) {
    GLclampf tri_r = float(g() % 100) / 100.0f;
    GLclampf tri_g = float(g() % 100) / 100.0f;
    GLclampf tri_b = float(g() % 100) / 100.0f;
    {

        float tri_vertex1[18] = {
            x, y + 0.2f, 0.0f, tri_r, tri_g, tri_b,
            x - 0.1f, y - 0.1f, 0.0f, tri_r, tri_g, tri_b,
            x + 0.1f, y - 0.1f, 0.0f, tri_r, tri_g, tri_b,
        };
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(tri_vertex1) * clicked_area, sizeof(tri_vertex1), tri_vertex1);

    }
}

void init_tri() {
    GLclampf tri_r;
    GLclampf tri_g;
    GLclampf tri_b;
    
    GLclampf x[4];
    GLclampf y[4];




    for (int i = 0; i < 4; ++i) {
        switch(i){
        case 0:
            x[i] = float(g() % 800) / 1000.0f - 0.9f;
            y[i] = float(g() % 800) / 1000.0f + 0.2f;
            break;

        case 1:
            x[i] = float(g() % 800) / 1000.0f + 0.1f;
            y[i] = float(g() % 800) / 1000.0f + 0.2f;
            
            break;

        case 2:
            x[i] = float(g() % 800) / 1000.0f - 0.9f;
            y[i] = float(g() % 800) / 1000.0f - 0.8f;
            break;

        case 3:
            x[i] = float(g() % 800) / 1000.0f + 0.1f;
            y[i] = float(g() % 800) / 1000.0f - 0.8f;
            break;
        }
        tri_r = float(g() % 100) / 100.0f;
        tri_g = float(g() % 100) / 100.0f;
        tri_b = float(g() % 100) / 100.0f;
        float tri_vertex1[18] = {
            x[i], y[i] + 0.1f, 0.0f, tri_r, tri_g, tri_b,
            x[i] - 0.1f, y[i] - 0.2f, 0.0f, tri_r, tri_g, tri_b,
            x[i] + 0.1f, y[i] - 0.2f, 0.0f, tri_r, tri_g, tri_b,
        };
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(tri_vertex1)*i, sizeof(tri_vertex1), tri_vertex1);

    }


}