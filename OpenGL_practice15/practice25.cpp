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
#include"shape.h"
#include"robot.h"
#include"aabb.h"
//미리 선언할거
#define vertex_shader_code "24_Vertex_shader.glsl"
#define fragment_shader_code "24_Fragment_shader.glsl"

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
GLuint VAO, VBO, EBO;

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

float angle{};

float camera_x;
float camera_y;
float camera_z;

glm::vec3 light(0.0f, 0.0f, 7.0f);
float radius{ 0.0f };



bool light_on{ true };

glm::vec3 body_color(0.0f, 0.0f, 1.0f);
glm::vec3 sky_color(0.0f, 0.5f, 1.0f);
glm::vec3 purple_color(1.0f, 0.0f, 1.0f);
glm::vec3 brown_color(0.5f, 0.3f, 0.0f);
glm::vec3 gray_color(0.5f, 0.5f, 0.5f);
glm::vec3 white_color(1.0f, 1.0f, 1.0f);
glm::vec3 red_color(1.0f, 0.0f, 0.0f);
glm::vec3 yellow_color(1.0f, 1.0f, 0.0f);
glm::vec3 light_color(1.0f, 1.0f, 1.0f);



shape mother{ 0.0f, 0.0f, 0.0f, "sphere2.obj", red_color };
shape child1{ -3.0f, 0.0f, 0.0f, "sphere2.obj", gray_color };
shape child2{ -5.0f, 0.0f, 0.0f, "sphere2.obj", sky_color };
//------------------------------------------------------
//필요한 함수 선언
std::random_device(rd);
std::mt19937 g(rd());

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

    mother.gen_buffer();
    child2.gen_buffer();
    child1.gen_buffer();

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




    glm::mat4 temp(1.0f);

    glm::mat4 proj2 = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
    GLuint projection = glGetUniformLocation(shader_program, "projection");

    glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(proj2));


    glm::mat4 view(1.0f);

    glm::vec3 camera_pos(0.0f, 0.0f, 0.0f);
    glm::vec3 camera_target(camera_pos.x, camera_pos.y, camera_pos.z - 1.0);
    glm::vec3 camera_up(0.0f, 1.0f, 0.0f);
    view = glm::lookAt(camera_pos, camera_target, camera_up);


    view = glm::translate(view, glm::vec3(camera_x, camera_y, camera_z - 15.0f));
    view = glm::rotate(view, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::rotate(view, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    GLuint view_mat = glGetUniformLocation(shader_program, "view");
    glUniformMatrix4fv(view_mat, 1, GL_FALSE, glm::value_ptr(view));


    unsigned int lighPosLocaiont = glGetUniformLocation(shader_program, "lightPos");
    glUniform3f(lighPosLocaiont, light.x, light.y, light.z);

    unsigned int lightColorLocation = glGetUniformLocation(shader_program, "lightColor");
    glUniform3f(lightColorLocation, light_color.x, light_color.y, light_color.z);

    unsigned int viewPosLocation = glGetUniformLocation(shader_program, "viewPos");
    glUniform3f(viewPosLocation, camera_pos.x, camera_pos.y, camera_pos.z);


    GLuint trans_mat = glGetUniformLocation(shader_program, "model");
    GLuint color = glGetUniformLocation(shader_program, "color");
    GLuint light_switch = glGetUniformLocation(shader_program, "light_on");
    glUniform1i(light_switch, light_on);
    /*glBindVertexArray(shape.VAO);
    shape.update_position();
    glUniform3fv(color, 1, glm::value_ptr(shape.color));
    glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(shape.trans));
    glDrawArrays(GL_TRIANGLES, 0, shape.model.vertices.size());*/

    glBindVertexArray(mother.VAO);
    mother.update_position();
    glUniform3fv(color, 1, glm::value_ptr(mother.color));
    glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(mother.trans));
    glDrawArrays(GL_TRIANGLES, 0, mother.model.vertices.size());

    glBindVertexArray(child1.VAO);
    child1.scale = 0.5f;
    child1.update_position();
    glUniform3fv(color, 1, glm::value_ptr(child1.color));
    glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(child1.trans));
    glDrawArrays(GL_TRIANGLES, 0, child1.model.vertices.size());

    glBindVertexArray(child2.VAO);
    child2.scale = 0.3f;
    child2.update_position();
    glUniform3fv(color, 1, glm::value_ptr(child2.color));
    glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(child2.trans));
    glDrawArrays(GL_TRIANGLES, 0, child2.model.vertices.size());

    glutSwapBuffers();

}


GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);

}
GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case'z':
        light = glm::vec3(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.1f)) * glm::vec4(light, 1.0f));
        radius += 0.1f;
        break;

    case'Z':
        light = glm::vec3(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.1f)) * glm::vec4(light, 1.0f));
        radius -= 0.1f;
        break;

    case'r':

        light = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(light, 1.0f));
        
        break;

    case'R':

        light = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(-3.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(light, 1.0f));
        
        break;

    case 'x':
        light_color = white_color;

        break;

    case 'c':
        light_color = yellow_color;

        break;

    case 'v':
        light_color = sky_color;

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


}
GLvoid SpecialKeyboard(int key, int x, int y) {

    glutPostRedisplay();
}
GLvoid timer(int value) {

    glutPostRedisplay();
    glutTimerFunc(10, timer, 0);
}

