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
#include<chrono>
//미리 선언할거
#define vertex_shader_code "21_Vertex_shader.glsl"
#define fragment_shader_code "21_Fragment_shader.glsl"


//------------------------------------------------------
//콜백함수

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid SpecialKeyboard(int key, int x, int y);
GLvoid timer(int);
GLvoid Mouse(int x, int y);


//------------------------------------------------------
//셰이더 용 선언
GLuint shader_program;
GLuint vertexShader;
GLuint fragmentShader;
GLuint VAO[9], VBO[9], EBO[9];

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

Model stage;
Model box[3];
Model ball[5];

float camera_angle{};

float camera_x;
float camera_y;
float camera_z;

float rotate_by_mouse;

float mouse_x;

float post_mouse_x;

int dir = 0;

bool flag_rotate = false;

bool flag_open;

bool flag_x_down;
bool flag_y_down;
bool flag_x_up;
bool flag_y_up;

float open_angle;

auto lastMoustTime = std::chrono::high_resolution_clock::now();

float ball_x[5];
float ball_y[5];
float ball_dx[5]{0.1f, -0.1f, 0.1f, -0.1f, 0.1f, };
float ball_dy[5]{ -0.1f, 0.1f, -0.1f, 0.1f, -0.1f, };
float ball_count{};

float cube_scale[3]{1.1f, 0.7f, 0.5f};
float cube_first_y[3]{-2.75, -2.78, -2.85};
float cube_first_z[3]{0.0, 0.6, 1.0};
float cube_x[3];
float cube_y[3];

glm::vec3 body_color(0.0f, 0.0f, 1.0f);
glm::vec3 sky_color(0.0f, 0.5f, 1.0f);
glm::vec3 purple_color(1.0f, 0.0f, 1.0f);
glm::vec3 brown_color(0.5f, 0.3f, 0.0f);
glm::vec3 gray_color(0.5f, 0.5f, 0.5f);
glm::vec3 white_color(1.0f, 1.0f, 1.0f);
glm::vec3 red_color[3] = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.7f, 0.0f, 0.0f), glm::vec3(0.5f, 0.0f, 0.0f) };
glm::vec3 yellow_color(1.0f, 1.0f, 0.0f);

//------------------------------------------------------
//필요한 함수 선언
std::random_device(rd);
std::mt19937 g(rd());
void gen_ball() {
    for (int i = 0; i < 5; ++i) {
        ball_x[i] = float(g() % 500) / 1000;
        ball_y[i] = (float(g() % 500) / 1000) + 1.5f;
    }
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
    glutPassiveMotionFunc(Mouse);
    for (int i = 0; i < 5; ++i) {
        if (i < 3) {
            read_obj_file("cube.obj", &box[i]);
        }
        read_obj_file("sphere.obj", &ball[i]);
    }
    read_obj_file("stage.obj", &stage);
    glEnable(GL_DEPTH_TEST);  // 깊이 테스트 활성화
    init_buffer();
    gen_ball();
    glutMainLoop();

}

GLvoid drawScene(GLvoid) {

    glClearColor(base_r, base_g, base_b, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader_program);

    glValidateProgram(shader_program);


    glEnable(GL_CULL_FACE);

    glm::mat4 proj2 = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
    GLuint projection = glGetUniformLocation(shader_program, "projection");
    proj2 = glm::translate(proj2, glm::vec3(0.0f, 0.0f, -15.0f));
    glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(proj2));


    glm::mat4 view(1.0f);

    glm::vec3 camera_pos(0.0f, 0.0f, 0.0f);
    glm::vec3 camera_target(0.0, 0.0, -1.0);
    glm::vec3 camera_up(0.0f, 1.0f, 0.0f);
    view = glm::lookAt(camera_pos, camera_target, camera_up);

    view = glm::rotate(view, glm::radians(camera_angle), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::translate(view, glm::vec3(camera_x, camera_y, camera_z));
    GLuint view_mat = glGetUniformLocation(shader_program, "view");
    glUniformMatrix4fv(view_mat, 1, GL_FALSE, glm::value_ptr(view));


    GLuint trans_mat = glGetUniformLocation(shader_program, "trans");
    GLuint color = glGetUniformLocation(shader_program, "in_color");

    glm::mat4 temp(1.0f);
    temp = glm::rotate(temp, glm::radians(rotate_by_mouse), glm::vec3(0.0f, 0.0f, 1.0f));
    temp = glm::translate(temp, glm::vec3(0.0f, -3.0f, 0.0f));

    glm::mat4 ball_transt[5] = {glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f)};
    for (int i = 0; i < 5; ++i) {
        ball_transt[i] = glm::rotate(ball_transt[i], glm::radians(rotate_by_mouse), glm::vec3(0.0f, 0.0f, 1.0f));
        ball_transt[i] = glm::translate(ball_transt[i], glm::vec3(ball_x[i], ball_y[i], 0.0f));
        ball_transt[i] = glm::scale(ball_transt[i], glm::vec3(0.5f, 0.5f, 0.5f));
    }
    glm::mat4 cube_trans[3] = { glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f) };
    for (int i = 0; i < 3; ++i) {
        cube_trans[i] = glm::rotate(cube_trans[i], glm::radians(rotate_by_mouse), glm::vec3(0.0f, 0.0f, 1.0f));
        cube_trans[i] = glm::translate(cube_trans[i], glm::vec3(0.0f, cube_y[i], 0.0f));
        cube_trans[i] = glm::translate(cube_trans[i], glm::vec3(cube_x[i], cube_first_y[i], cube_first_z[i]));
        cube_trans[i] = glm::scale(cube_trans[i], glm::vec3(cube_scale[i], cube_scale[i], cube_scale[i]));
    }
    glm::mat4 open(1.0f);
    open = glm::rotate(open, glm::radians(rotate_by_mouse), glm::vec3(0.0f, 0.0f, 1.0f));
    open = glm::translate(open, glm::vec3(-3.0f, -3.0f, 0.0f));
    open = glm::rotate(open, glm::radians(open_angle), glm::vec3(0.0f, 0.0f, 1.0f));
    open = glm::translate(open, glm::vec3(3.0f, 0.0f, 0.0f));

    glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(temp));
    for (int i = 0; i < 9; ++i) {
        
        if (i == 0)
        {
            glBindVertexArray(VAO[i]);
            for (int j = 0; j < stage.face_count / 2; ++j)
            {
                //0 왼쪽 1 왼앞 2 오앞 3 오른쪽 4뒤 5위 6아래

                if (j == 0) {
                    
                    glUniform3fv(color, 1, glm::value_ptr(purple_color));
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(float) * j * 6));
                }

               
                
                if (j == 3) {

                    glUniform3fv(color, 1, glm::value_ptr(brown_color));
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(float) * j * 6));
                }
                if (j == 4) {

                    glUniform3fv(color, 1, glm::value_ptr(gray_color));
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(float) * j * 6));
                }
                if (j == 5) {

                    glUniform3fv(color, 1, glm::value_ptr(white_color));
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(float) * j * 6));
                }
                if (j == 6) {

                    glUniform3fv(color, 1, glm::value_ptr(white_color));
                    glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(open));
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(float) * j * 6));
                }
            }
        }
        if (0 < i && i < ball_count + 1) {
            glBindVertexArray(VAO[i]);
            glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(ball_transt[i-1]));
            glUniform3fv(color, 1, glm::value_ptr(red_color[0]));
            glDrawElements(GL_TRIANGLES, ball[i-1].face_count * 3, GL_UNSIGNED_INT, 0);
        }
        if(5 < i && i < 9) {
            glBindVertexArray(VAO[i]);
            glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(cube_trans[i - 6]));
            glUniform3fv(color, 1, glm::value_ptr(red_color[i-6]));
            glDrawElements(GL_TRIANGLES, box[i - 6].face_count * 3, GL_UNSIGNED_INT, 0);
        }
    }




    glutSwapBuffers();

}


GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);

}
GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'z':
        camera_z += 0.1f;
        break;

    case 'Z':
        camera_z -= 0.1f;
        break;

    case 'y':
        camera_angle += 1.0f;
        break;

    case 'Y':
        camera_angle -= 1.0f;
        break;

    case 'b':

        if (ball_count < 5) {
            ++ball_count;
        }
        break;

    case 'o':
        flag_open = true;

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

    glGenBuffers(1, &VBO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, stage.vertex_count * sizeof(Vertex), stage.vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &EBO[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, stage.face_count * sizeof(Face), stage.faces, GL_STATIC_DRAW);

    for (int i = 1; i < 6; ++i) {
        glGenVertexArrays(1, &VAO[i]);
        glBindVertexArray(VAO[i]);

        glGenBuffers(1, &VBO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBufferData(GL_ARRAY_BUFFER, ball[i-1].vertex_count * sizeof(Vertex), ball[i-1].vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &EBO[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, ball[i-1].face_count * sizeof(Face), ball[i-1].faces, GL_STATIC_DRAW);

    }

    for (int i = 6; i < 9; ++i) {
        glGenVertexArrays(1, &VAO[i]);
        glBindVertexArray(VAO[i]);

        glGenBuffers(1, &VBO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBufferData(GL_ARRAY_BUFFER, box[i - 6].vertex_count * sizeof(Vertex), box[i - 6].vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &EBO[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, box[i - 6].face_count * sizeof(Face), box[i - 6].faces, GL_STATIC_DRAW);

    }
}
GLvoid SpecialKeyboard(int key, int x, int y) {

    glutPostRedisplay();
}
GLvoid timer(int value) {


    if (dir < 0) {
        rotate_by_mouse += 1.0f;
    }
    else if (dir > 0) {
        rotate_by_mouse -= 1.0f;
    }
    if (rotate_by_mouse > 360 || rotate_by_mouse < -360) {
        rotate_by_mouse = 0;
    }
    /*auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> pass_time = currentTime - lastMoustTime;
    if (pass_time.count() > 0.1f) {
        flag_rotate = false;
    }


    if(flag_rotate){
        if (dir < 0) {
            rotate_by_mouse += 1.0f;
        }
        else if (dir > 0) {
            rotate_by_mouse -= 1.0f;
        }
    }*/
    if (rotate_by_mouse == 0) {
        flag_x_down = false;
        flag_x_up = false;
        flag_y_down = true;
        flag_y_up = false;
    }
    if ((0 < rotate_by_mouse && rotate_by_mouse < 90)||
        (-360 < rotate_by_mouse && rotate_by_mouse < -270)) {
        flag_x_down = true;
        flag_x_up = false;
        flag_y_down = true;
        flag_y_up = false;
    }
    if ((90 < rotate_by_mouse && rotate_by_mouse < 180)||
        (-270 < rotate_by_mouse && rotate_by_mouse < -180)) {
        flag_x_down = true;
        flag_x_up = false;
        flag_y_down = false;
        flag_y_up = true;
    }
    if ((-90 < rotate_by_mouse && rotate_by_mouse < 0) ) {
        flag_x_down = false;
        flag_x_up = true;
        flag_y_down = true;
        flag_y_up = false;
    }
    if ((270 < rotate_by_mouse && rotate_by_mouse < 360)) {
        flag_x_down = false;
        flag_x_up = true;
        flag_y_down = true;
        flag_y_up = false;
    }
    if ((-180 < rotate_by_mouse && rotate_by_mouse < -90) ||
        (180 < rotate_by_mouse && rotate_by_mouse < 270)) {
        flag_x_down = false;
        flag_x_up = true;
        flag_y_down = false;
        flag_y_up = true;
    }


    if (flag_x_down) {
        if (cube_x[0] > -2.75)
            cube_x[0] -= 0.05f;
        if (cube_x[1] > -2.78)
            cube_x[1] -= 0.05f;
        if (cube_x[2] > -2.85)
            cube_x[2] -= 0.05f;

    }
    if (flag_x_up){
        if (cube_x[0] < 2.75)
            cube_x[0] += 0.05f;
        if (cube_x[1] < 2.78)
            cube_x[1] += 0.05f;
        if (cube_x[2] < 2.85)
            cube_x[2] += 0.05f;
    }
    if (flag_y_down) {
        if(not flag_open){
            if (cube_y[0] > 0)
                cube_y[0] -= 0.05f;
            if (cube_y[1] > -0)
                cube_y[1] -= 0.05f;
            if (cube_y[2] > -0)
                cube_y[2] -= 0.05f;
        }
        else {
            cube_y[0] -= 0.05f;
            
            cube_y[1] -= 0.05f;
            
            cube_y[2] -= 0.05f;
        }

    }
    if (flag_y_up) {
        if (cube_y[0] < 2.75 - cube_first_y[0])
            cube_y[0] += 0.05f;
        if (cube_y[1] < 2.78 - cube_first_y[1])
            cube_y[1] += 0.05f;
        if (cube_y[2] < 2.85 - cube_first_y[2])
            cube_y[2] += 0.05f;
    }

    for (int i = 0; i < ball_count + 1; ++i) {
        if(not flag_open){
            ball_x[i] += ball_dx[i];
            ball_y[i] += ball_dy[i];
            if (ball_x[i] > 2.75) {
                ball_dx[i] = -0.1f;
            }
            if (ball_y[i] > 2.75) {
                ball_dy[i] = -0.1f;
            }
            if (ball_x[i] < -2.75) {
                ball_dx[i] = 0.1f;
            }
            if (ball_y[i] < -2.75) {
                ball_dy[i] = 0.1f;
            }
        }
        else {
            ball_x[i] += ball_dx[i];
            ball_y[i] += ball_dy[i];
            if(ball_y[i]>-2.75){
                if (ball_x[i] > 2.75) {
                    ball_dx[i] = -0.1f;
                }
                if (ball_y[i] > 2.75) {
                    ball_dy[i] = -0.1f;
                }
            }
            if (ball_x[i] < -2.75) {
                ball_dx[i] = 0.1f;
            }
        }
    }

    if (flag_open) {
        if(open_angle>-90)
            open_angle -= 1.0f;
    }
    glutPostRedisplay();
    glutTimerFunc(10, timer, 0);
}

GLvoid Mouse(int x, int y) {
    mouse_x = (float)(x - (float)width / 2.0) * (float)(1.0 / (float)(width / 2.0));

    if (mouse_x < -0.3) {
        dir = -1;
    }
    else if (mouse_x > 0.3) {
        dir = 1;
    }
    else {
        dir = 0;
    }

    /*post_mouse_x = (float)(x - (float)width / 2.0) * (float)(1.0 / (float)(width / 2.0));



    if (mouse_x - post_mouse_x > 0) {
        dir = -1;
        flag_rotate = true;
    }
    else if (mouse_x - post_mouse_x < 0) {
        dir = 1;
        flag_rotate = true;
    }
    else {
        dir = 0;
    }
    mouse_x = post_mouse_x;
    lastMoustTime = std::chrono::high_resolution_clock::now();
    */

}
