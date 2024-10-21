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
#define vertex_shader_code "14_Vertex_shader.glsl"
#define fragment_shader_code "14_Fragment_shader.glsl"
std::random_device rd;
std::mt19937 g(rd());

//------------------------------------------------------
//콜백함수
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid SpecialKeyboard(int key, int x, int y);
GLvoid timer(int);

//------------------------------------------------------
//셰이더 용 선언
GLuint shader_program;
GLuint vertexShader;
GLuint fragmentShader;
GLuint VAO[2], VBO[2], CBO[2], EBO[2], a_axis, b_axis;

void make_vertex_shader();
void make_fragment_shader();
GLuint make_shader();
GLvoid init_buffer();
//------------------------------------------------------
//전역변수
GLclampf base_r = 0.0f;
GLclampf base_g = 0.0f;
GLclampf base_b = 0.0f;
GLint width{ 800 }, height{ 600 };

Model _cube_13;
Model _diamond_13;

bool draw_cube = false;
bool draw_pyramid = false;
bool is_wired = false;
bool is_cull = false;

glm::mat4 translate_mat(1.0f);

float dx = 0.0f;
float dy = 0.0f;

int draw_face1{ -1 };
int draw_face2{ -1 };

int spin_flag = -1;// 1=+x 2=-x 3=+y 4=-y

const float cube_color[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 1.0f,
};

const float pyramid_color[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 1.0f,
};

const float axis[] = {
    0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, -1.0f, 0.0, 1.0f, 1.0f, 1.0f,

    -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

};

float axis_dx{};
float axis_dy{};


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
    glutSpecialFunc(SpecialKeyboard);
    glutTimerFunc(60, timer, 0);
    read_obj_file("cube.obj", &_cube_13);
    read_obj_file("pyramid.obj", &_diamond_13);
    glEnable(GL_DEPTH_TEST);  // 깊이 테스트 활성화


    init_buffer();

    glutMainLoop();

}


GLUquadricObj* qobj;

GLvoid drawScene(GLvoid) {

    glClearColor(base_r, base_g, base_b, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader_program);

    glValidateProgram(shader_program);
    GLuint modelLoc2 = glGetUniformLocation(shader_program, "trans");


    glBindVertexArray(a_axis);

    glm::mat4 temp(1.0f);
    glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(temp));

    glDrawArrays(GL_LINES, 0, 12);

    if (is_cull) {
        glEnable(GL_CULL_FACE);
    }
    else {
        glDisable(GL_CULL_FACE);
    }
    glm::mat4 tr_mat = glm::mat4(1.0f);
    tr_mat = glm::translate(tr_mat, glm::vec3(dx, dy, 0.0f));
    tr_mat = glm::rotate(tr_mat, glm::radians(30.0f + axis_dx), glm::vec3(1.0, 0.0, 0.0));
    tr_mat = glm::rotate(tr_mat, glm::radians(30.0f + axis_dy), glm::vec3(0.0, 1.0, 0.0));
    GLuint modloc = glGetUniformLocation(shader_program, "trans");
    glUniformMatrix4fv(modloc, 1, GL_FALSE, glm::value_ptr(tr_mat));

    glBindVertexArray(VAO[0]);
    if(draw_cube){
        if(is_wired){
            for (int i = 0; i < (_cube_13.face_count * 3) - 1; ++i)
            {
                glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(i * sizeof(unsigned int)));
            }
        }
        else {
            glDrawElements(GL_TRIANGLES, _cube_13.face_count*3, GL_UNSIGNED_INT, 0);
        }
    }
    glUniformMatrix4fv(modloc, 1, GL_FALSE, glm::value_ptr(tr_mat));
    glBindVertexArray(VAO[1]);
    if(draw_pyramid){
        if(is_wired){
            for (int i = 0; i < (_diamond_13.face_count * 3); ++i) {
                glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (void*)(i * sizeof(unsigned int)));
            }
        }
        else {
            glDrawElements(GL_TRIANGLES, _diamond_13.face_count * 3, GL_UNSIGNED_INT, 0);
        }
    }

    glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);

}
GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'c':
        draw_cube = !draw_cube;
        break;

    case 'p':
        draw_pyramid = !draw_pyramid;

        break;
    case 'w':
        is_wired = true;
        break;

    case 'W':
        is_wired = false;
        break;

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
        dx = 0;
        dy = 0;
        break;

    case 'h':
        is_cull = !is_cull;

        break;
    
    case 'q':
        glutLeaveMainLoop();
        break;
    }


    if ('0' <= key && key <= '9') {
        draw_face1 = key - '0';
        std::cout << draw_face1;
        draw_face2 = -1;
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

    glGenBuffers(1, &VBO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, _cube_13.vertex_count * sizeof(Vertex), _cube_13.vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &EBO[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _cube_13.face_count * sizeof(Face), _cube_13.faces, GL_STATIC_DRAW);

    glGenBuffers(1, &CBO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, CBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_color), cube_color, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);
    glEnableVertexAttribArray(1);


    print_model_info(_cube_13);

    glGenVertexArrays(1, &VAO[1]);
    glBindVertexArray(VAO[1]);

    glGenBuffers(1, &VBO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, _diamond_13.vertex_count * sizeof(Vertex), _diamond_13.vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &CBO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, CBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_color), pyramid_color, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &EBO[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _diamond_13.face_count * sizeof(Face), _diamond_13.faces, GL_STATIC_DRAW);

    glGenVertexArrays(1, &a_axis);
    glBindVertexArray(a_axis);

    glGenBuffers(1, &b_axis);
    glBindBuffer(GL_ARRAY_BUFFER, b_axis);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis), axis, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    print_model_info(_diamond_13);
}
GLvoid SpecialKeyboard(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        dy += 0.1f;

        break;

    case GLUT_KEY_DOWN:
        dy -= 0.1f;

        break;
    case GLUT_KEY_LEFT:
        dx -= 0.1f;

        break;
    case GLUT_KEY_RIGHT:
        dx += 0.1f;

    default:

        break;
    }
    glutPostRedisplay();
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

    default:

        break;
    }

    

    glutPostRedisplay();
    glutTimerFunc(60, timer, 0);
}