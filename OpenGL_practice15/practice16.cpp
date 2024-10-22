#include<iostream>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
#include<gl/glm/ext.hpp>
#include<gl/glm/glm.hpp>
#include<gl/glm/gtc/matrix_transform.hpp>
#include"file_open.h"
#include"read_obj.h"
#include<random>
#include<vector>
//미리 선언할거
#define vertex_shader_code "15_Vertex_shader.glsl"
#define fragment_shader_code "15_Fragment_shader.glsl"
std::random_device rd;
std::mt19937 g(rd());
/*
x/X x축 회전
y/Y y축 회전
r 공전
s멈춤
c바꾸기
e/E 제자리 scale
w/W 중심축 scale
iop 도형1이동
jkl 도형2이동
방향키 zx이동
1~5애니메이션
*/
//------------------------------------------------------
//콜백함수
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid DirKeyboard(int, int, int);
GLvoid timer(int);

//------------------------------------------------------
//셰이더 용 선언
GLuint shader_program;
GLuint vertexShader;
GLuint fragmentShader;
GLuint VAO[4], VBO[4], EBO[4], a_axis, b_axis;

void make_vertex_shader();
void make_fragment_shader();
GLuint make_shader();
GLvoid init_buffer();
//------------------------------------------------------
//전역변수
GLclampf base_r = 01.0f;
GLclampf base_g = 01.0f;
GLclampf base_b = 01.0f;
GLint width{ 800 }, height{ 600 };

Model cube_15;
Model cone_15;
Model sphere_15;
Model cylinder_15;

int spin_flag = -1;// 1=+x 2=-x 3=+y 4=-y

int what_spin = 3;

const float axis[] = {
    0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, -1.0f, 0.0, 1.0f, 1.0f, 1.0f,

    -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

    0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 1.0f,
};

float axis_dx{};
float axis_dy{};

int motion_flag = -1;

float dx1 = -0.5f;
float dy1{};
float dz1{};

float dx2 = 0.5f;
float dy2{};
float dz2{};

float d_scale1{ 1 };
float d_scale2{ 1 };
float axis_scale{ 1.0 };

float rotate_dx{};
float rotate_dy{};
float rotate_dz{};

bool is_c = false;

//------------------------------------------------------
//필요한 함수 선언

void print_model_info(const Model& model) {
    std::cout << "Total Vertices: " << model.vertex_count << std::endl;
    for (size_t i = 0; i < model.vertex_count; ++i) {
        std::cout << "Vertex " << i + 1 << ": ("
            << model.vertices[i].x << ", "
            << model.vertices[i].y << ", "
            << model.vertices[i].z << ")" << std::endl;
    }

    std::cout << "Total Faces: " << model.face_count << std::endl;
    for (size_t i = 0; i < model.face_count; ++i) {
        std::cout << "Face " << i + 1 << ": ("
            << model.faces[i].v1 + 1 << ", "
            << model.faces[i].v2 + 1 << ", "
            << model.faces[i].v3 + 1 << ")" << std::endl;
    }
    std::cout << "\n\n\n";
}

void spiral_move();
void cross_move();
void z_rotate();
void scale_rotate();
//------------------------------------------------------
void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
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

    glutTimerFunc(60, timer, 0);
    read_obj_file("cube.obj", &cube_15);
    read_obj_file("cone.obj", &cone_15);
    read_obj_file("sphere.obj", &sphere_15);
    read_obj_file("cylinder.obj", &cylinder_15);
    glEnable(GL_DEPTH_TEST); 


    init_buffer();

    glutMainLoop();

}


GLUquadricObj* qobj;

GLvoid drawScene(GLvoid) {

    glClearColor(base_r, base_g, base_b, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader_program);

    glValidateProgram(shader_program);
    GLuint modloc = glGetUniformLocation(shader_program, "trans");


    glBindVertexArray(a_axis);

    glm::mat4 temp(1.0f);

    temp = glm::rotate(temp, glm::radians(30.0f), glm::vec3(1.0, 0.0, 0.0));
    temp = glm::rotate(temp, glm::radians(-45.0f), glm::vec3(0.0, 1.0, 0.0));
    glUniformMatrix4fv(modloc, 1, GL_FALSE, glm::value_ptr(temp));

    glDrawArrays(GL_LINES, 0, 12);

    glm::mat4 tr_mat = glm::mat4(1.0f);
    tr_mat = glm::rotate(tr_mat, glm::radians(rotate_dz), glm::vec3(0.0, 0.0, 1.0));
    tr_mat = glm::rotate(tr_mat, glm::radians(30.0f + rotate_dx), glm::vec3(1.0, 0.0, 0.0));
    tr_mat = glm::rotate(tr_mat, glm::radians(-45.0f + rotate_dy), glm::vec3(0.0, 1.0, 0.0));
    tr_mat = glm::scale(tr_mat, glm::vec3(axis_scale, axis_scale, axis_scale));
    tr_mat = glm::translate(tr_mat, glm::vec3(dx1, dy1, dz1));
    tr_mat = glm::rotate(tr_mat, glm::radians(axis_dx), glm::vec3(1.0, 0.0, 0.0));
    tr_mat = glm::rotate(tr_mat, glm::radians(axis_dy), glm::vec3(0.0, 1.0, 0.0));
    tr_mat = glm::scale(tr_mat, glm::vec3(d_scale1, d_scale1, d_scale1));
    glUniformMatrix4fv(modloc, 1, GL_FALSE, glm::value_ptr(tr_mat));



    if (not is_c) {
        glBindVertexArray(VAO[0]);
        for (int i = 0; i < (cube_15.face_count * 3) - 1; ++i)
        {
            glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(i * sizeof(unsigned int)));
        }
    }
    else {
        glBindVertexArray(VAO[2]);
        for (int i = 0; i < (sphere_15.face_count * 3) - 1; ++i)
        {
            glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(i * sizeof(unsigned int)));
        }
    }

    glm::mat4 tr_mat2 = glm::mat4(1.0f);
    tr_mat2 = glm::rotate(tr_mat2, glm::radians(rotate_dz), glm::vec3(0.0, 0.0, 1.0));
    tr_mat2 = glm::rotate(tr_mat2, glm::radians(30.0f + rotate_dx), glm::vec3(1.0, 0.0, 0.0));
    tr_mat2 = glm::rotate(tr_mat2, glm::radians(-45.0f + rotate_dy), glm::vec3(0.0, 1.0, 0.0));
    tr_mat2 = glm::scale(tr_mat2, glm::vec3(axis_scale, axis_scale, axis_scale));
    tr_mat2 = glm::translate(tr_mat2, glm::vec3(dx2, dy2, dz2));
    tr_mat2 = glm::rotate(tr_mat2, glm::radians(axis_dx), glm::vec3(1.0, 0.0, 0.0));
    tr_mat2 = glm::rotate(tr_mat2, glm::radians(axis_dy), glm::vec3(0.0, 1.0, 0.0));
    tr_mat2 = glm::scale(tr_mat2, glm::vec3(d_scale2, d_scale2, d_scale2));
    glUniformMatrix4fv(modloc, 1, GL_FALSE, glm::value_ptr(tr_mat2));
    if (not is_c) {
        glBindVertexArray(VAO[1]);
        for (int i = 0; i < (cone_15.face_count * 3); ++i) {
            glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(i * sizeof(unsigned int)));
        }
    }
    else {
        glBindVertexArray(VAO[3]);
        for (int i = 0; i < (cylinder_15.face_count * 3) - 1; ++i)
        {
            glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(i * sizeof(unsigned int)));
        }
    }


    glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);

}
GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'x':
        spin_flag = 1;
        break;

    case 'X':
        spin_flag = 2;
        break;

    case 'y':
        spin_flag = 3;
        break;

    case 'Y':
        spin_flag = 4;
        break;

    case 's':
        spin_flag = -1;
        axis_dx = 0;
        axis_dy = 0;
        rotate_dy = 0;
        motion_flag = -1;
        break;

    case 'r':
        spin_flag = 5;
        break;

    case 'R':
        spin_flag = 6;
        break;
        
    case '3':
        spin_flag = 5;
        break;

    case 'c':
        is_c = !is_c;
        break;

    case 'e':
        d_scale1 += 0.1f;
        d_scale2 += 0.1f;
        break;

    case 'E':
        d_scale1 -= 0.1f;
        d_scale2 -= 0.1f;
        break;

    case 'w':
        axis_scale += 0.1f;
        break;

    case 'W':
        axis_scale -= 0.1f;
        break;

    case 'i':
        dx1 += 0.1f;
        break;

    case 'I':
        dx1 -= 0.1f;
        break;

    case 'o':
        dy1 += 0.1f;
        break;

    case 'O':
        dy1 -= 0.1f;
        break;

    case 'p':
        dz1 += 0.1f;
        break;

    case 'P':
        dz1 -= 0.1f;
        
        break;
    case 'j':
        dx2 += 0.1f;
        break;

    case 'J':
        dx2 -= 0.1f;
        break;

    case 'k':
        dy2 += 0.1f;
        break;

    case 'K':
        dy2 -= 0.1f;
        break;

    case 'l':
        dz2 += 0.1f;
        break;

    case 'L':
        dz2 -= 0.1f;


        break;
    case 'q':
        glutLeaveMainLoop();
        break;
    }

    if ('1' <= key && key <= '5') {
        motion_flag = key - '0';
    }

    std::cout << dz1 << std::endl;
    std::cout << dz2 << std::endl;
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

    glGenBuffers(1, &VBO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, cube_15.vertex_count * sizeof(Vertex), cube_15.vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &EBO[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube_15.face_count * sizeof(Face), cube_15.faces, GL_STATIC_DRAW);


    glGenVertexArrays(1, &VAO[1]);
    glBindVertexArray(VAO[1]);

    glGenBuffers(1, &VBO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, cone_15.vertex_count * sizeof(Vertex), cone_15.vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &EBO[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cone_15.face_count * sizeof(Face), cone_15.faces, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO[2]);
    glBindVertexArray(VAO[2]);

    glGenBuffers(1, &VBO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sphere_15.vertex_count * sizeof(Vertex), sphere_15.vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &EBO[2]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere_15.face_count * sizeof(Face), sphere_15.faces, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO[3]);
    glBindVertexArray(VAO[3]);

    glGenBuffers(1, &VBO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, cylinder_15.vertex_count * sizeof(Vertex), cylinder_15.vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &EBO[3]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylinder_15.face_count * sizeof(Face), cylinder_15.faces, GL_STATIC_DRAW);





    glGenVertexArrays(1, &a_axis);
    glBindVertexArray(a_axis);

    glGenBuffers(1, &b_axis);
    glBindBuffer(GL_ARRAY_BUFFER, b_axis);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis), axis, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

}

GLvoid timer(int value) {

    switch (spin_flag)
    {
    case 1:

        axis_dx += 1.0;

        break;

    case 2:

        axis_dx -= 1.0;

        break;

    case 3:

        axis_dy += 1.0;

        break;

    case 4:

        axis_dy -= 1.0;

        break;

    case 5:

        rotate_dy += 1.0;

        break;

    case 6:

        rotate_dy -= 1.0;

        break;

    default:

        break;
    }
    switch (motion_flag) {
    case 1:
        spiral_move();
        break;

    case 2:
        cross_move();
        break;

    case 4:
        z_rotate();
        break;

    case 5:
        scale_rotate();
        break;
    }


    glutPostRedisplay();
    glutTimerFunc(60, timer, 0);
}

GLvoid DirKeyboard(int key, int x, int y) {

}


void spiral_move() {
    rotate_dy += 1.0f;
    dx1 += 0.001f;
    dx2 -= 0.001f;
}
void cross_move() {
    if(dx1<0.5f)
        dx1 += 0.01f;
    if(dx2>-0.5f)
        dx2 -= 0.01f;
}
void z_rotate() {
    rotate_dz += 1.0f;
}
void scale_rotate() {
    rotate_dz += 1.0f;
    d_scale1 += 0.005f;
    d_scale2 -= 0.005f;
}