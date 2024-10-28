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
#define vertex_shader_code "18_Vertex_shader.glsl"
#define fragment_shader_code "18_Fragment_shader.glsl"

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
GLuint VAO[7], VBO[7], EBO[7];
GLuint OAO[6], OBO[6];

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

Model sphere[7];

float center_x;
float center_y;
float center_z;

float orbit_x[7];
float orbit_y[7];
float orbit_z[7];

glm::vec4 mother1(1.0f, 0.0, 0.0f, 1.0f);
glm::mat4 temp = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
glm::vec4 mother2 = temp * mother1;
glm::mat4 temp2 = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
glm::vec4 mother3 = temp2 * mother1;

float rotate_z[7];
float rotate_y[7];
float rotate_ys[7];
float rotate_xy[7];
float rotate_yx[7];


float scale[7] = { 0.8f, 0.4f, 0.4f, 0.4f, 0.1f, 0.1f, 0.1f };

bool projection_flag = false;
bool is_wired = false;
bool ccw = false;
int rot_z = 0;

glm::vec3 sphere_color[7] = {
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 1.0f),
    glm::vec3(0.0f, 1.0f, 0.5f),
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(1.0f, 0.5f, 0.0f),
    glm::vec3(1.0f, 0.1f, 0.0f),
};

float orbit[6][360][3];
//------------------------------------------------------
//필요한 함수 선언
void set_sphere() {
    orbit_x[1] = mother1.x;
    orbit_y[1] = mother1.y;
    orbit_z[1] = mother1.z;

    orbit_x[2] = mother2.x;
    orbit_y[2] = mother2.y;
    orbit_z[2] = mother2.z;

    orbit_x[3] = mother3.x;
    orbit_y[3] = mother3.y;
    orbit_z[3] = mother3.z;



}

void make_orbit() {
    glm::mat4 trans(1.0f);
    for (int i = 0; i < 360; ++i) {
        
        trans = glm::rotate(trans, glm::radians(float(i)), glm::vec3(0.0, 1.0, 0.0));
        //trans = glm::rotate(trans, glm::radians(float(i)), glm::vec3(sqrt(2.0), sqrt(2.0), 0.0));
        //trans = glm::rotate(trans, glm::radians(float(i)), glm::vec3(-sqrt(2.0), sqrt(2.0), 0.0));
        orbit[0][i][0] = (trans * mother1).x;
        orbit[0][i][1] = (trans * mother1).y;
        orbit[0][i][2] = (trans * mother1).z;
    }
    glBindBuffer(GL_VERTEX_ARRAY, OBO[0]);
    glBufferSubData(GL_VERTEX_ARRAY, 0, sizeof(orbit[0]), orbit[0]);

    trans = glm::mat4(1.0f);
    for (int i = 0; i < 360; ++i) {

        //trans = glm::rotate(trans, glm::radians(float(i)), glm::vec3(0.0, 1.0, 0.0));
        //trans = glm::rotate(trans, glm::radians(float(i)), glm::vec3(sqrt(2.0), sqrt(2.0), 0.0));
        trans = glm::rotate(trans, glm::radians(float(i)), glm::vec3(-sqrt(2.0), sqrt(2.0), 0.0));
        orbit[1][i][0] = (trans * mother2).x;
        orbit[1][i][1] = (trans * mother2).y;
        orbit[1][i][2] = (trans * mother2).z;
    }
    glBindBuffer(GL_VERTEX_ARRAY, OBO[1]);
    glBufferSubData(GL_VERTEX_ARRAY, 0, sizeof(orbit[1]), orbit[1]);

    trans = glm::mat4(1.0f);
    for (int i = 0; i < 360; ++i) {

        //trans = glm::rotate(trans, glm::radians(float(i)), glm::vec3(0.0, 1.0, 0.0));
        trans = glm::rotate(trans, glm::radians(float(i)), glm::vec3(sqrt(2.0), sqrt(2.0), 0.0));
        //trans = glm::rotate(trans, glm::radians(float(i)), glm::vec3(-sqrt(2.0), sqrt(2.0), 0.0));
        orbit[2][i][0] = (trans * mother3).x;
        orbit[2][i][1] = (trans * mother3).y;
        orbit[2][i][2] = (trans * mother3).z;
    }
    glBindBuffer(GL_VERTEX_ARRAY, OBO[2]);
    glBufferSubData(GL_VERTEX_ARRAY, 0, sizeof(orbit[2]), orbit[2]);
   
    trans = glm::mat4(1.0f);
    glm::vec4 temp(0.5f, 0.0f, 0.0, 0.0);
    for (int i = 0; i < 360; ++i) {

        trans = glm::rotate(trans, glm::radians(float(i)), glm::vec3(0.0, 1.0, 0.0));
        //trans = glm::rotate(trans, glm::radians(float(i)), glm::vec3(sqrt(2.0), sqrt(2.0), 0.0));
        //trans = glm::rotate(trans, glm::radians(float(i)), glm::vec3(-sqrt(2.0), sqrt(2.0), 0.0));
        orbit[3][i][0] = (trans * temp).x;
        orbit[3][i][1] = (trans * temp).y;
        orbit[3][i][2] = (trans * temp).z;
    }
    glBindBuffer(GL_VERTEX_ARRAY, OBO[3]);
    glBufferSubData(GL_VERTEX_ARRAY, 0, sizeof(orbit[3]), orbit[3]);

    trans = glm::mat4(1.0f);
    temp = glm::vec4(0.5f, 0.0, 0.0, 0.0);
    for (int i = 0; i < 360; ++i) {

        trans = glm::rotate(trans, glm::radians(float(i)), glm::vec3(0.0, 1.0, 0.0));
        //trans = glm::rotate(trans, glm::radians(float(i)), glm::vec3(sqrt(2.0), sqrt(2.0), 0.0));
        //trans = glm::rotate(trans, glm::radians(float(i)), glm::vec3(-sqrt(2.0), sqrt(2.0), 0.0));
        orbit[4][i][0] = (trans * temp).x;
        orbit[4][i][1] = (trans * temp).y;
        orbit[4][i][2] = (trans * temp).z;
    }
    glBindBuffer(GL_VERTEX_ARRAY, OBO[3]);
    glBufferSubData(GL_VERTEX_ARRAY, 0, sizeof(orbit[4]), orbit[4]);
    trans = glm::mat4(1.0f);
    temp = glm::vec4(0.5f, 0.0, 0.0, 0.0);
    for (int i = 0; i < 360; ++i) {

        trans = glm::rotate(trans, glm::radians(float(i)), glm::vec3(0.0, 1.0, 0.0));
        //trans = glm::rotate(trans, glm::radians(float(i)), glm::vec3(sqrt(2.0), sqrt(2.0), 0.0));
        //trans = glm::rotate(trans, glm::radians(float(i)), glm::vec3(-sqrt(2.0), sqrt(2.0), 0.0));
        orbit[5][i][0] = (trans * temp).x;
        orbit[5][i][1] = (trans * temp).y;
        orbit[5][i][2] = (trans * temp).z;
    }
    glBindBuffer(GL_VERTEX_ARRAY, OBO[3]);
    glBufferSubData(GL_VERTEX_ARRAY, 0, sizeof(orbit[5]), orbit[5]);
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
    glutTimerFunc(10, timer, 0);
    for (int i = 0; i < 7; ++i) {
        read_obj_file("sphere.obj", &sphere[i]);
    }
    glEnable(GL_DEPTH_TEST);  // 깊이 테스트 활성화

    make_orbit();
    set_sphere();
    init_buffer();

    glutMainLoop();

}

GLvoid drawScene(GLvoid) {

    glClearColor(base_r, base_g, base_b, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader_program);

    glValidateProgram(shader_program);

    glEnable(GL_CULL_FACE);


    glm::mat4 proj1 = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
    proj1 = glm::rotate(proj1, glm::radians(30.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    glm::mat4 proj2 = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
    proj2 = glm::translate(proj2, glm::vec3(0.0f, 0.0f, -5.0f));
    proj2 = glm::rotate(proj2, glm::radians(30.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    
    GLuint projection = glGetUniformLocation(shader_program, "projection");
    if (projection_flag) {
        glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(proj2));
    }
    else {
        glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(proj1));
    }

    glm::mat4 trans[7] = { glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f) };
    for (int i = 0; i < 7; ++i) {
        if (i >= 4)
        {

            
            trans[i] = glm::translate(trans[i], glm::vec3(orbit_x[i], orbit_y[i], orbit_z[i]));
            trans[i] = glm::rotate(trans[i], glm::radians(rotate_z[i]), glm::vec3(0.0, 0.0, 1.0));
            trans[i] = glm::rotate(trans[i], glm::radians(rotate_ys[i]), glm::vec3(0.0, 1.0, 0.0));
            trans[i] = glm::rotate(trans[i], glm::radians(rotate_y[i]), glm::vec3(0.0, 1.0, 0.0));
            trans[i] = glm::rotate(trans[i], glm::radians(rotate_xy[i]), glm::vec3(sqrt(2.0), sqrt(2.0), 0.0));
            trans[i] = glm::rotate(trans[i], glm::radians(rotate_yx[i]), glm::vec3(-sqrt(2.0), sqrt(2.0), 0.0));
            trans[i] = glm::translate(trans[i], glm::vec3(0.5f, 0.0f, 0.0f));
            trans[i] = glm::scale(trans[i], glm::vec3(scale[i], scale[i], scale[i]));
        }
        else {
            trans[i] = glm::translate(trans[i], glm::vec3(center_x, center_y, center_z));
            trans[i] = glm::rotate(trans[i], glm::radians(rotate_z[i]), glm::vec3(0.0, 0.0, 1.0));
            trans[i] = glm::rotate(trans[i], glm::radians(rotate_ys[i]), glm::vec3(0.0, 1.0, 0.0));
            trans[i] = glm::rotate(trans[i], glm::radians(rotate_y[i]), glm::vec3(0.0, 1.0, 0.0));
            trans[i] = glm::rotate(trans[i], glm::radians(rotate_xy[i]), glm::vec3(sqrt(2.0), sqrt(2.0), 0.0));
            trans[i] = glm::rotate(trans[i], glm::radians(rotate_yx[i]), glm::vec3(-sqrt(2.0), sqrt(2.0), 0.0));
            trans[i] = glm::translate(trans[i], glm::vec3(orbit_x[i], orbit_y[i], orbit_z[i]));
            trans[i] = glm::scale(trans[i], glm::vec3(scale[i], scale[i], scale[i]));
        }
    }

    GLuint color_loc = glGetUniformLocation(shader_program, "in_color");
    GLuint mod_trans = glGetUniformLocation(shader_program, "trans");
    if (not is_wired) {
        for (int ii = 0; ii < 7; ++ii) {
            glBindVertexArray(VAO[ii]);
            glUniformMatrix4fv(mod_trans, 1, GL_FALSE, glm::value_ptr(trans[ii]));
            glUniform3fv(color_loc, 1, glm::value_ptr(sphere_color[ii]));
            glDrawElements(GL_TRIANGLES, sphere[ii].face_count * 3, GL_UNSIGNED_INT, 0);

        }
    }
    else {
        for (int ii = 0; ii < 7; ++ii) {
            glBindVertexArray(VAO[ii]);
            glUniformMatrix4fv(mod_trans, 1, GL_FALSE, glm::value_ptr(trans[ii]));
            glUniform3fv(color_loc, 1, glm::value_ptr(sphere_color[ii]));
            glDrawElements(GL_LINE_STRIP, sphere[ii].face_count * 3, GL_UNSIGNED_INT, 0);

        }
    }
    glm::mat4 temp[6] = {
        glm::mat4(1.0f),glm::mat4(1.0f),glm::mat4(1.0f),glm::mat4(1.0f),glm::mat4(1.0f),glm::mat4(1.0f),
    };
    
    for (int i = 0; i < 3; ++i) {
        temp[i] = glm::translate(temp[i], glm::vec3(center_x, center_y, center_z));
        temp[i] = glm::rotate(temp[i], glm::radians(rotate_z[i]), glm::vec3(0.0, 0.0, 1.0));
    }
    for (int i = 3; i < 6; ++i) {
        
        temp[i] = glm::translate(temp[i], glm::vec3(orbit_x[i+1], orbit_y[i+1], orbit_z[i+1]));
        temp[i] = glm::rotate(temp[i], glm::radians(rotate_z[i]), glm::vec3(0.0, 0.0, 1.0));
        
    }

    glm::vec3 tempc(0.0, 0.0, 0.0);
    
    glUniform3fv(color_loc, 1, glm::value_ptr(tempc));
    for (int i = 0; i < 6; ++i) {
        if(i<3){
            for(int ii=0;ii<359;++ii){
                glBindVertexArray(OAO[i]);
                glUniformMatrix4fv(mod_trans, 1, GL_FALSE, glm::value_ptr(temp[1]));
                glDrawArrays(GL_LINES, ii, ii+1);
            }
        }
        else {
            for(int ii=0;ii<359;++ii){
                glBindVertexArray(OAO[i]);
                glUniformMatrix4fv(mod_trans, 1, GL_FALSE, glm::value_ptr(temp[i]));
                glDrawArrays(GL_LINES, ii, ii+1);
            }
        }
    }

    glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);

}
GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w':
        center_y += 0.1f;
        break;

    case 'a':
        center_x -= 0.1f;
        break;

    case 's':
        center_y -= 0.1f;
        break;

    case 'd':
        center_x += 0.1f;
        break;

    case 'p':
        projection_flag = !projection_flag;
        break;

    case 'm':
        is_wired = !is_wired;
        break;

    case 'u':
        center_z += 0.1f;
        break;

    case 'i':
        center_z -= 0.1f;
        break;

    case 'y':
        ccw = !ccw;
        break;

    case 'z':
        if (rot_z < 2) {
            ++rot_z;
        }
        else {
            rot_z = 0;
        }
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
    for (int i = 0; i < 7;++i) {
        glGenVertexArrays(1, &VAO[i]);
        glBindVertexArray(VAO[i]);

        glGenBuffers(1, &VBO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBufferData(GL_ARRAY_BUFFER, sphere[i].vertex_count * sizeof(Vertex), sphere[i].vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &EBO[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere[i].face_count * sizeof(Face), sphere[i].faces, GL_STATIC_DRAW);

    }
    for (int i = 0; i < 6; ++i) {
        glGenVertexArrays(1, &OAO[i]);
        glBindVertexArray(OAO[i]);

        glGenBuffers(1, &OBO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, OBO[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 360, orbit[i], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

    }
}
GLvoid SpecialKeyboard(int key, int x, int y) {

    glutPostRedisplay();
}
GLvoid timer(int value) {

    glm::mat4 trans[7] = { glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f) };
    for (int i = 0; i < 7; ++i) {
        
        trans[i] = glm::translate(trans[i], glm::vec3(center_x, center_y, center_z));
        trans[i] = glm::rotate(trans[i], glm::radians(rotate_z[i]), glm::vec3(0.0, 0.0, 1.0));
        trans[i] = glm::rotate(trans[i], glm::radians(rotate_y[i]), glm::vec3(0.0, 1.0, 0.0));
        trans[i] = glm::rotate(trans[i], glm::radians(rotate_xy[i]), glm::vec3(sqrt(2.0), sqrt(2.0), 0.0));
        trans[i] = glm::rotate(trans[i], glm::radians(rotate_yx[i]), glm::vec3(-sqrt(2.0), sqrt(2.0), 0.0));
    }
    
    if(not ccw){

        rotate_y[1] += 1.0f;
        rotate_yx[2] += 1.5f;
        rotate_xy[3] += 1.3f;

        orbit_x[4] = (trans[1] * mother1).x;
        orbit_y[4] = (trans[1] * mother1).y;
        orbit_z[4] = (trans[1] * mother1).z;
        rotate_y[4] += 4.0f;


        orbit_x[5] = (trans[2] * mother2).x;
        orbit_y[5] = (trans[2] * mother2).y;
        orbit_z[5] = (trans[2] * mother2).z;
        rotate_y[5] += 5.0f;



        orbit_x[6] = (trans[3] * mother3).x;
        orbit_y[6] = (trans[3] * mother3).y;
        orbit_z[6] = (trans[3] * mother3).z;
        rotate_y[6] += 6.0f;
    }
    else {
        rotate_y[1] -= 1.0f;
        rotate_yx[2] -= 1.5f;
        rotate_xy[3] -= 1.3f;


        orbit_x[4] = (trans[1] * mother1).x;
        orbit_y[4] = (trans[1] * mother1).y;
        orbit_z[4] = (trans[1] * mother1).z;
        rotate_y[4] -= 4.0f;


        orbit_x[5] = (trans[2] * mother2).x;
        orbit_y[5] = (trans[2] * mother2).y;
        orbit_z[5] = (trans[2] * mother2).z;
        rotate_y[5] -= 5.0f;



        orbit_x[6] = (trans[3] * mother3).x;
        orbit_y[6] = (trans[3] * mother3).y;
        orbit_z[6] = (trans[3] * mother3).z;
        rotate_y[6] -= 6.0f;
    }
    if (rot_z == 1) {
        for(int i=1;i<7;++i){
            rotate_z[i] += 1.0f;
        }
    }
    else if(rot_z==2){
        for (int i = 1; i < 7; ++i) {
            rotate_z[i] -= 1.0f;
        }
    }
    else {


    }

    glutPostRedisplay();
    glutTimerFunc(10, timer, 0);
}