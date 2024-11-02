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

//------------------------------------------------------
//셰이더 용 선언
GLuint shader_program;
GLuint vertexShader;
GLuint fragmentShader;
GLuint VAO[11], VBO[11], EBO[11];

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

Model body[7];
Model stage;
Model obstacle[3];

float camera_angle{};

float open_x[2];

float camera_x;
float camera_y;
float camera_z;

bool flag_open;
bool flag_walk;
bool flag_jump;

float scale_x[7] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
float scale_y[7] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
float scale_z[7] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };

float first_x[7];
float first_y[7];
float first_z[7];

float robot_dx;
float robot_dy;
float robot_dz;

float robot_x;
float robot_y{ 0.1 };
float robot_z;

float robot_speed{0.02};

float jump_speed{0.06};

float swing_y[7];
int swing_angle[7]; 
float swing_da[7];
int max_swing{ 30 };
float swing_speed{ 1.0f };

int rotate_robot{ };

float obs_x[3];
float obs_z[3];

float plane{ 0.11 };


glm::vec3 body_color(0.0f, 0.0f, 1.0f);
glm::vec3 sky_color(0.0f, 0.5f, 1.0f);
glm::vec3 purple_color(1.0f, 0.0f, 1.0f);
glm::vec3 brown_color(0.5f, 0.3f, 0.0f);
glm::vec3 gray_color(0.5f, 0.5f, 0.5f);
glm::vec3 white_color(1.0f, 1.0f, 1.0f);
glm::vec3 red_color(1.0f, 0.0f, 0.0f);
glm::vec3 yellow_color(1.0f, 1.0f, 0.0f);

//------------------------------------------------------
//필요한 함수 선언
std::random_device(rd);
std::mt19937 g(rd());

void reset() {
    camera_angle = 0;

    open_x[0] = 0;
    open_x[1] = 0;

    camera_x = 0;
    camera_y = 0;
    camera_z = 0;

    flag_open = 0;
    flag_walk = 0;
    flag_jump = 0;

    for (int i = 0; i < 7;++i) {
        scale_x[i] = { 1.0f };
        scale_y[i] = { 1.0f };
        scale_z[i] = { 1.0f };

        first_x[i] = 0;
        first_y[i] = 0;
        first_z[i] = 0;
        swing_y[i] = 0;
        swing_angle[i] = 0;
        swing_da[i] = 0;
    }

    robot_dx = 0;
    robot_dy = 0;
    robot_dz = 0;

    robot_x = 0;
    robot_y = 0.1;
    robot_z = 0;

    robot_speed =  0.02;

    jump_speed = 0.06;

 
    max_swing = 30 ;
    swing_speed = 1.0f ;

    rotate_robot = 0;

    obs_x[3];
    obs_z[3];




    //몸
    scale_x[0] = 1.0f;
    scale_z[0] = 1.0f;
    scale_y[0] = 2.0f;
    first_y[0] = -1.0f;
    

    //머리
    scale_x[1] = 0.7f;
    scale_y[1] = 0.7f;
    scale_z[1] = 0.7f;
    first_y[1] = -0.4f;

    //왼팔
    scale_x[2] = 0.35f;
    scale_z[2] = 0.35f;
    scale_y[2] = 1.3f;
    first_x[2] = -0.3f;
    first_y[2] = -1.0f;
    swing_y[2] = -0.3;

    //오른팔
    scale_x[3] = 0.35f;
    scale_z[3] = 0.35f;
    scale_y[3] = 1.3f;
    first_x[3] = 0.3f;
    first_y[3] = -1.0f;
    swing_y[3] = -0.3;

    //왼다리
    scale_x[4] = 0.35f;
    scale_z[4] = 0.35f;
    scale_y[4] = 1.5f;
    first_x[4] = -0.1f;
    first_y[4] = -1.75f;
    swing_y[4] = -0.4;

    //오른다리
    scale_x[5] = 0.35f;
    scale_z[5] = 0.35f;
    scale_y[5] = 1.5f;
    first_x[5] = 0.1f;
    first_y[5] = -1.75f;
    swing_y[5] = -0.4;

    //코
    scale_x[6] = 0.1f;
    scale_y[6] = 0.1f;
    scale_z[6] = 0.3f;
    first_y[6] = -0.4f;
    first_z[6] = 0.2f;

    for (int i = 0; i < 3; ++i) {
        obs_x[i] = (float(g() % 6000) / 1000) - 3.0f;
        obs_z[i] = (float(g() % 6000) / 1000) - 3.0f;
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
    for (int i = 0; i < 7; ++i) {
        read_obj_file("cube.obj", &body[i]);
        if (i < 3) {
            read_obj_file("cube.obj", &obstacle[i]);
        }
    }
    read_obj_file("stage.obj", &stage);

    reset();
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
    temp = glm::translate(temp, glm::vec3(0.0f, -2.0f, 0.0f));

    glm::mat4 open(1.0f);
    open = glm::translate(open, glm::vec3(0.0f, -2.0f, 0.0f));

    glm::mat4 obs_[3] = { glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f) };
    for(int i=0;i<3;++i){
        obs_[i] = glm::translate(obs_[i], glm::vec3(obs_x[i], -1.5f, obs_z[i]));
        obs_[i] = glm::scale(obs_[i], glm::vec3(1.5f, 1.5f, 1.5f));
    }
    
    glm::mat4 robot[7] = {
        glm::mat4(1.0f), glm::mat4(1.0f),glm::mat4(1.0f),glm::mat4(1.0f),glm::mat4(1.0f),glm::mat4(1.0f),glm::mat4(1.0f),
    };
    
    for (int i = 0; i < 7; ++i) {
        robot[i] = glm::translate(robot[i], glm::vec3(robot_x, robot_y, robot_z));
        robot[i] = glm::rotate(robot[i], glm::radians(float(rotate_robot)), glm::vec3(0.0f, 1.0f, 0.0f));
        robot[i] = glm::translate(robot[i], glm::vec3(first_x[i], first_y[i], first_z[i]));
        robot[i] = glm::translate(robot[i], glm::vec3(0.0f, -swing_y[i], 0.0f));
        robot[i] = glm::rotate(robot[i], glm::radians(float(swing_angle[i])), glm::vec3(1.0f, 0.0f, 0.0f));
        robot[i] = glm::translate(robot[i], glm::vec3(0.0f, swing_y[i], 0.0f));
        robot[i] = glm::scale(robot[i], glm::vec3(scale_x[i], scale_y[i], scale_z[i]));
    }

    glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(temp));
    for (int i = 0; i < 11; ++i) {
        glBindVertexArray(VAO[i]);
        
        
        if (i < 6)
        {   
            if(i==0)
                glUniform3fv(color, 1, glm::value_ptr(body_color));
            if(i==1)
                glUniform3fv(color, 1, glm::value_ptr(body_color));
            if(i==2)
                glUniform3fv(color, 1, glm::value_ptr(purple_color));
            if(i==3)
                glUniform3fv(color, 1, glm::value_ptr(brown_color));
            if(i==4)
                glUniform3fv(color, 1, glm::value_ptr(yellow_color));
            glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(robot[i]));
            glDrawElements(GL_TRIANGLES, body[i].face_count * 3, GL_UNSIGNED_INT, 0);
        }
        if (i == 6)
        {
            glUniform3fv(color, 1, glm::value_ptr(sky_color));
            glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(robot[i]));
            glDrawElements(GL_TRIANGLES, body[i].face_count * 3, GL_UNSIGNED_INT, 0);
        }
        if(6<i && i<10)//장애물
        {
            glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(obs_[i-7]));
            glUniform3fv(color, 1, glm::value_ptr(red_color));
            glDrawElements(GL_TRIANGLES, obstacle[i-7].face_count * 3, GL_UNSIGNED_INT, 0);
        }
        if(i==10)
        {
            for (int j=0;j<stage.face_count/2;++j)
                {
                //0 왼쪽 1 왼앞 2 오앞 3 오른쪽 4뒤 5위 6아래
                
                if (j == 0) {
                    glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(temp));
                    glUniform3fv(color, 1, glm::value_ptr(purple_color));
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(float) * j * 6));
                }
                
                if(j == 1)
                {
                    open = glm::translate(open, glm::vec3(open_x[0], 0.0f, 0.0f));
                    glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(open));
                    glUniform3fv(color, 1, glm::value_ptr(sky_color));
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(float) * 1 * 6));
                }
                if (j == 2)
                {
                    open = glm::translate(open, glm::vec3(open_x[1], 0.0f, 0.0f));
                    glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(open));
                    glUniform3fv(color, 1, glm::value_ptr(sky_color));
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(float) * 2 * 6));
                }
                if (j == 3) {
                    glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(temp));
                    glUniform3fv(color, 1, glm::value_ptr(brown_color));
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(float) * j * 6));
                }
                if (j == 4) {
                    glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(temp));
                    glUniform3fv(color, 1, glm::value_ptr(gray_color));
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(float) * j * 6));
                }
                if (j == 5) {
                    glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(temp));
                    glUniform3fv(color, 1, glm::value_ptr(white_color));
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(float) * j * 6));
                }
                if (j == 6) {
                    glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(temp));
                    glUniform3fv(color, 1, glm::value_ptr(white_color));
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(float) * j * 6));
                }
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
        flag_walk = true;
        rotate_robot = 180;
        swing_da[2] = swing_speed;
        swing_da[3] = -swing_speed;
        swing_da[4] = swing_speed;
        swing_da[5] = -swing_speed;
        robot_dz = -robot_speed;
        robot_dx = 0;
        robot_dy = 0;

        break;

    case 's':
        flag_walk = true;
        rotate_robot = 0;
        swing_da[2] = swing_speed;
        swing_da[3] = -swing_speed;
        swing_da[4] = swing_speed;
        swing_da[5] = -swing_speed;
        robot_dz = robot_speed;
        robot_dx = 0;
        robot_dy = 0;

        break;
    
    case 'a':
        flag_walk = true;
        rotate_robot = -90;
        swing_da[2] = swing_speed;
        swing_da[3] = -swing_speed;
        swing_da[4] = swing_speed;
        swing_da[5] = -swing_speed;
        robot_dx = -robot_speed;
        robot_dy = 0;
        robot_dz = 0;

        break;

    case 'd':
        flag_walk = true;
        rotate_robot = 90;
        swing_da[2] = swing_speed;
        swing_da[3] = -swing_speed;
        swing_da[4] = swing_speed;
        swing_da[5] = -swing_speed;
        robot_dx = robot_speed;
        robot_dy = 0;
        robot_dz = 0;
        break;

    case'+':
        robot_speed += 0.01f;
        max_swing += 5.0f;
        swing_speed += 1.0f;
        break;

    case'-':
        if(robot_speed>0.02){
            robot_speed -= 0.01f;
        }
        if(robot_speed>35.0f){
            max_swing -= 5.0f;
        }
        if(swing_speed>2.0f){
            swing_speed -= 1.0f;
        }
        break;

    case'j':
        if (not flag_jump) {
            flag_jump = true;
            robot_dy = jump_speed;
            flag_walk = false;
        }
        break;
    case'o':
        flag_open = !flag_open;
        break;

    case 'z':
        camera_z += 0.5f;
        break;

    case 'Z':
        camera_z -= 0.5f;
        break;

    case 'y':
        camera_angle += 1.0;
        break;

    case 'Y':
        camera_angle -= 1.0;
        break;

    case 'x':
        camera_x += 0.5f;
        break;

    case 'X':
        camera_x -= 0.5f;
        break;

    case 'i':
        reset();
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
 
        glGenBuffers(1, &EBO[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, body[i].face_count * sizeof(Face), body[i].faces, GL_STATIC_DRAW);
    }
    for (int i = 7; i < 10; ++i) {
        glGenVertexArrays(1, &VAO[i]);
        glBindVertexArray(VAO[i]);

        glGenBuffers(1, &VBO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBufferData(GL_ARRAY_BUFFER, obstacle[i-7].vertex_count * sizeof(Vertex), obstacle[i-7].vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &EBO[i]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, obstacle[i-7].face_count * sizeof(Face), obstacle[i-7].faces, GL_STATIC_DRAW);
    }
    glGenVertexArrays(1, &VAO[10]);
    glBindVertexArray(VAO[10]);

    glGenBuffers(1, &VBO[10]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[10]);
    glBufferData(GL_ARRAY_BUFFER, stage.vertex_count * sizeof(Vertex), stage.vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &EBO[10]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[10]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, stage.face_count * sizeof(Face), stage.faces, GL_STATIC_DRAW);
    

}
GLvoid SpecialKeyboard(int key, int x, int y) {

    glutPostRedisplay();
}
GLvoid timer(int value) {
    
    
    

    if (flag_open) {
        if (open_x[0] > -3.0f) {
            open_x[0] = ((open_x[0] * 10) - 1) / 10;
        }
        if (open_x[1] < 6.0f) {
            open_x[1] = ((open_x[1]) * 10 + 1) / 10;
        }
    }
    else {
        if (open_x[0] < 0.0f) {
            open_x[0] = ((open_x[0] * 10) + 1) / 10;
        }
        if (open_x[1] > 0.0f) {
            open_x[1] = ((open_x[1]) * 10 - 1) / 10;
        }
    }
    if (flag_walk) {
        robot_x += robot_dx;
        
        robot_z += robot_dz;

        for (int i = 2; i < 6; ++i) {
            swing_angle[i] += swing_da[i];
        }
        if (swing_angle[2] > max_swing) {
            swing_da[2] = -swing_speed;
        }
        if (swing_angle[2] < -max_swing) {
            swing_da[2] = swing_speed;
        }

        if (swing_angle[3] < -max_swing) {
            swing_da[3] = swing_speed;
        }
        if (swing_angle[3] > max_swing) {
            swing_da[3] = -swing_speed;
        }

        if (swing_angle[4] > max_swing) {
            swing_da[4] = -swing_speed;
        }
        if (swing_angle[4] < -max_swing) {
            swing_da[4] = swing_speed;
        }

        if (swing_angle[5] < -max_swing) {
            swing_da[5] = swing_speed;
        }
        if (swing_angle[5] > max_swing) {
            swing_da[5] = -swing_speed;
        }
        if (robot_x < -2.8f) {
            robot_dx = robot_speed;
            rotate_robot += 180;
        }
        if (robot_x > 2.8f) {
            robot_dx = -robot_speed;
            rotate_robot -= 180;
        }
        if (robot_z < -2.8f) {
            robot_dz = robot_speed;
            rotate_robot += 180;
        }
        if (robot_z > 2.8f) {
            robot_dz = -robot_speed;
            rotate_robot -= 180;
        }
        if (robot_y > 0.11) {
            if ((obs_x[0] - 0.3 < robot_x && robot_x < obs_x[0] + 0.3 &&
                obs_z[0] - 0.3 < robot_z && robot_z < obs_z[0] + 0.3) ||
                (obs_x[1] - 0.3 < robot_x && robot_x < obs_x[1] + 0.3 &&
                    obs_z[1] - 0.3 < robot_z && robot_z < obs_z[1] + 0.3) ||
                (obs_x[2] - 0.3 < robot_x && robot_x < obs_x[2] + 0.3 &&
                    obs_z[2] - 0.3 < robot_z && robot_z < obs_z[2] + 0.3)
                ) {
                plane = 1.01;
            }
            else {
                plane = 0.11;
            }
        }
    }
    else {
        for (int i = 2; i < 6; ++i) {
            swing_angle[i] = 0;
        }
    }
    robot_y += robot_dy;
    if (flag_jump) {
        if ((obs_x[0] - 0.3 < robot_x && robot_x < obs_x[0] + 0.3 &&
            obs_z[0] - 0.3 < robot_z && robot_z < obs_z[0] + 0.3) ||
            (obs_x[1] - 0.3 < robot_x && robot_x < obs_x[1] + 0.3 &&
                obs_z[1] - 0.3 < robot_z && robot_z < obs_z[1] + 0.3) ||
            (obs_x[2] - 0.3 < robot_x && robot_x < obs_x[2] + 0.3 &&
                obs_z[2] - 0.3 < robot_z && robot_z < obs_z[2] + 0.3)
            ) {
            plane = 1.01;
        }
        else {
            plane = 0.11;
        }
        if (robot_y > plane || robot_dy>0.0f) {
            robot_dy -= 0.002;
            
        }
        else if(robot_y < plane && robot_dy < 0.0f) {
            robot_dy = 0;
            flag_jump = false;
            
        }
    }
    else {
        robot_y = plane - 0.01;
    }
    
    glutPostRedisplay();
    glutTimerFunc(10, timer, 0);
}
