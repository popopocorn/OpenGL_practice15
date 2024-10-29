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
//미리 선언할거
#define vertex_shader_code "19_Vertex_shader.glsl"
#define fragment_shader_code "19_Fragment_shader.glsl"

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
GLuint VAO[7], VBO[7], EBO[7], CBO[7];

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

Model body[7];


int flag_move_x{};
int flag_rotate_y{};
int flag_barrel_rotate{};
bool flag_barrel_merge{};
int flag_arm_rotate{};

float camera_x;
float camera_y;
float camera_z{5.0f};
float dir_x;
float dir_y;
float dir_z{ -1.0f };

float camera_angle{};

float center_x;
float center_y;
float center_z;

float scale_x[6];
float scale_y[6];
float scale_z[6];

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

//------------------------------------------------------
//필요한 함수 선언



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
    glutTimerFunc(10, timer, 0);
    for (int i = 0; i < 7; ++i) {
        read_obj_file("cube.obj", &body[i]);

    }

    glEnable(GL_DEPTH_TEST);  // 깊이 테스트 활성화

    init_buffer();

    glutMainLoop();

}

GLvoid drawScene(GLvoid) {

    glClearColor(base_r, base_g, base_b, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader_program);

    glValidateProgram(shader_program);

    glEnable(GL_CULL_FACE);

    glm::mat4 proj2 = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);

    proj2 = glm::translate(proj2, glm::vec3(0.0f, 0.0f, -5.0f));

    glm::mat4 view(1.0f);

    glm::vec3 camera_pos(0.0f, 0.0f, 5.0f);
    glm::vec3 camera_dir(dir_x, dir_y, dir_z);
    glm::vec3 camera_up(0.0f, 1.0f, 0.0f);

    glm::vec4 rotate_target(camera_dir.x, camera_dir.y, camera_dir.z, 1.0f);
    glm::mat4 camera_rotate(1.0f);
    view = glm::lookAt(camera_pos, camera_dir, camera_up);
    view = glm::translate(view, glm::vec3(camera_x, camera_y, camera_z));
    view = glm::rotate(view, glm::radians(camera_angle), glm::vec3(0.0f, 1.0f, 0.0f));
    

    GLuint view_mat = glGetUniformLocation(shader_program, "view");
    GLuint projection = glGetUniformLocation(shader_program, "projection");
    GLuint trans_mat = glGetUniformLocation(shader_program, "trans");

    glUniformMatrix4fv(view_mat, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(proj2));

    glm::mat4 temp(1.0f);
    glm::mat4 plane_trans(1.0f);
    plane_trans = glm::translate(plane_trans, glm::vec3(0.0f, -0.5f, 0.0f));
    plane_trans = glm::scale(plane_trans, glm::vec3(25.0f, 1.0f, 25.0f));
    glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(plane_trans));

    glm::mat4 cube_trans[6]={glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f)};
    for (int i = 0; i < 6; ++i) {
        //cube_trans[i] = glm::scale(cube_trans[i], glm::vec3());
    }


    for(int i=0;i<7;++i){
        glBindVertexArray(VAO[i]);
        if(i==0){
            glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(plane_trans));
            glDrawElements(GL_TRIANGLES, body[i].face_count * 3, GL_UNSIGNED_INT, 0);
        }
        else {
            glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(cube_trans[i-1]));
            glDrawElements(GL_TRIANGLES, body[i].face_count * 3, GL_UNSIGNED_INT, 0);
        }
    }

    





    glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);

}
GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'y':
        camera_angle += 1.0f;
        break;

    case 'x':
        camera_x -= 0.1f;
        dir_x -= 0.1f;
        break;

    case 'X':
        camera_x += 0.1f;
        dir_x += 0.1f;
        break;

    case 'z':
        camera_z += 0.1f;
        
        break;

    case 'Z':
        camera_z -= 0.1f;
        
        break;


    case 'q':
        glutLeaveMainLoop();
        break;

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
    for (int i = 0; i < 7; ++i) {
        glGenVertexArrays(1, &VAO[i]);
        glBindVertexArray(VAO[i]);

        glGenBuffers(1, &VBO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBufferData(GL_ARRAY_BUFFER, body[i].vertex_count * sizeof(Vertex), body[i].vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &CBO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, CBO[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_color), cube_color, GL_STATIC_DRAW);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &EBO[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, body[i].face_count * sizeof(Face), body[i].faces, GL_STATIC_DRAW);
    }


}
GLvoid SpecialKeyboard(int key, int x, int y) {

    glutPostRedisplay();
}
GLvoid timer(int value) {


    glutPostRedisplay();
    glutTimerFunc(10, timer, 0);
}