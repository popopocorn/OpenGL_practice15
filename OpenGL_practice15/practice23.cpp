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
#include"floor.h"
#include"robot.h"
#include"aabb.h"
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

Model body[7];
Model stage;

float camera_angle{};

float open_x[2];

float camera_x;
float camera_y;
float camera_z;


bool flag_open;
bool flag_walk;
bool flag_jump;


glm::vec3 body_color(0.0f, 0.0f, 1.0f);
glm::vec3 sky_color(0.0f, 0.5f, 1.0f);
glm::vec3 purple_color(1.0f, 0.0f, 1.0f);
glm::vec3 brown_color(0.5f, 0.3f, 0.0f);
glm::vec3 gray_color(0.5f, 0.5f, 0.5f);
glm::vec3 white_color(1.0f, 1.0f, 1.0f);
glm::vec3 red_color(1.0f, 0.0f, 0.0f);
glm::vec3 yellow_color(1.0f, 1.0f, 0.0f);



robot robots{1.0f};

my_floor floors[64];
my_floor obs[14];
//------------------------------------------------------
//필요한 함수 선언
std::random_device(rd);
std::mt19937 g(rd());

void reset();
bool collide(int);
bool collide(aabb, aabb);

void initialize_floors() {
    int index = 0;
    float cube_size = 1.5f; // 육면체 한 변의 길이
    float start_x = -6.0f + cube_size / 2; // x 좌표 시작점
    float start_z = -6.0f + cube_size / 2; // z 좌표 시작점

    int random_indices[3]; // 랜덤 인덱스를 저장할 배열
    bool is_chosen[64] = { false }; // 인덱스가 선택되었는지 여부를 체크하는 배열

    // 랜덤으로 3개의 인덱스 선택
    for (int i = 0; i < 3; ++i) {
        int random_index;
        do {
            random_index = g() % 64; // 0~63 범위의 랜덤 값
        } while (is_chosen[random_index]); // 이미 선택된 경우 반복
        random_indices[i] = random_index;
        is_chosen[random_index] = true;
    }

    // 바닥 블록과 장애물 초기화
    for (int i = 0; i < 8; ++i) { // 행
        for (int j = 0; j < 8; ++j) { // 열
            float x = start_x + i * cube_size;
            float z = start_z + j * cube_size;

            // 바닥 블록 생성
            floors[index] = my_floor(x, z);

            // 흰색과 검정색 번갈아가며 배치
            floors[index].color = ((i + j) % 2 == 0)
                ? glm::vec3(1.0f, 1.0f, 1.0f)  // 흰색
                : glm::vec3(0.0f, 0.0f, 0.0f); // 검정색

            // 장애물 큐브 배치
            for (int k = 0; k < 3; ++k) {
                if (random_indices[k] == index) {
                
                    obs[k] = my_floor(x, z); // obs 큐브 위치 설정
                    obs[k].y = 0.4;
                    obs[k].type = "obs";
                    obs[k].color = glm::vec3(1.0f, 0.0f, 0.0f); // 빨간색
                    
                }
            }

            ++index;
        }
    }
    for (int i = 3; i < 14; ++i) {
        obs[i].init();
    }
    for (int i = 3; i < 10; ++i) {
        read_obj_file("cube.obj", &obs[i].model);
        obs[i].type = "pill";
    }
    obs[3].y = 1.0f;
    obs[3].x = 2.0f;
    obs[4].y = 2.5f;
    obs[4].x = 2.0f;
    obs[5].y = 4.0f;
    obs[5].x = 2.0f;
    obs[6].y = 4.0f;
    obs[6].x = 3.5f;
    obs[7].y = 5.5f;
    obs[7].x = 5.0f;
    obs[7].y = 4.0f;
    obs[7].x = 5.0f;
    obs[8].y = 2.5f;
    obs[8].x = 5.0f;
    obs[9].y = 1.0f;
    obs[9].x = 5.0f;


}

/*
무대 좌표: 좌-6 우 6 하 -5
*/

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
    read_obj_file("stage.obj", &stage);

    reset();
    robots.gen_buffer();

    initialize_floors();
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
    proj2 = glm::translate(proj2, glm::vec3(0.0f, -5.0f, -0.0f));
    glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(proj2));


    glm::mat4 view(1.0f);

    glm::vec3 camera_pos(0.0f, 0.0f, 0.0f);
    glm::vec3 camera_target(0.0, 0.0, -1.0);
    glm::vec3 camera_up(0.0f, 1.0f, 0.0f);
    view = glm::lookAt(camera_pos, camera_target, camera_up);

    
    view = glm::translate(view, glm::vec3(camera_x, camera_y, camera_z - 25.0f));
    view = glm::rotate(view, glm::radians(camera_angle), glm::vec3(0.0f, 1.0f, 0.0f));
    GLuint view_mat = glGetUniformLocation(shader_program, "view");
    glUniformMatrix4fv(view_mat, 1, GL_FALSE, glm::value_ptr(view));


    GLuint trans_mat = glGetUniformLocation(shader_program, "trans");
    GLuint color = glGetUniformLocation(shader_program, "in_color");

    glm::mat4 temp(1.0f);
    //temp = glm::translate(temp, glm::vec3(0.0f, -5.0f, -10.0f));
    temp = glm::scale(temp, glm::vec3(2.0f, 2.0f, 2.0f));

    glm::mat4 open(1.0f);
    open = glm::translate(open, glm::vec3(0.0f, -5.0f, -10.0f));
    open = glm::scale(open, glm::vec3(2.0f, 2.0f, 2.0f));

    glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(temp));
    for (int i = 0; i < 11; ++i) {
        if (i < 7) {
            glBindVertexArray(robots.VAO[i]);
            robots.update_position();
            glUniform3fv(color, 1, glm::value_ptr(robots.color[i]));
            glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(robots.trans[i]));
            glDrawElements(GL_TRIANGLES, robots.body[i].face_count * 3, GL_UNSIGNED_INT, 0);
        }

    }
    for (int j = 0; j < stage.face_count / 2; ++j)
    {
        //0 왼쪽 1 왼앞 2 오앞 3 오른쪽 4뒤 5위 6아래
        glBindVertexArray(VAO);
        if (j == 0) {
            glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(temp));
            glUniform3fv(color, 1, glm::value_ptr(purple_color));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(float) * j * 6));
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
    }

    for (int i = 0; i < 64; ++i) {
        glBindVertexArray(floors[i].VAO);
        floors[i].update_position();
        glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(floors[i].trans));
        glUniform3fv(color, 1, glm::value_ptr(floors[i].color));
        glDrawElements(GL_TRIANGLES, floors[i].model.face_count*3, GL_UNSIGNED_INT, 0);
    }
    for (int i = 0; i < 14; ++i) {
        glBindVertexArray(obs[i].VAO);
        obs[i].update_position();
        glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(obs[i].trans));
        glUniform3fv(color, 1, glm::value_ptr(obs[i].color));
        glDrawElements(GL_TRIANGLES, obs[i].model.face_count * 3, GL_UNSIGNED_INT, 0);
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
        robots.rotate_robot = 180;
        robots.swing_da[2] = robots.swing_speed;
        robots.swing_da[3] = -robots.swing_speed;
        robots.swing_da[4] = robots.swing_speed;
        robots.swing_da[5] = -robots.swing_speed;
        robots.robot_dz = -robots.robot_speed;
        robots.robot_dx = 0;
        robots.robot_dy = 0;

        break;

    case 's':
        flag_walk = true;
        robots.rotate_robot = 0;
        robots.swing_da[2] = robots.swing_speed;
        robots.swing_da[3] = -robots.swing_speed;
        robots.swing_da[4] = robots.swing_speed;
        robots.swing_da[5] = -robots.swing_speed;
        robots.robot_dz = robots.robot_speed;
        robots.robot_dx = 0;
        robots.robot_dy = 0;

        break;

    case 'a':
        flag_walk = true;
        robots.rotate_robot = -90;
        robots.swing_da[2] = robots.swing_speed;
        robots.swing_da[3] = -robots.swing_speed;
        robots.swing_da[4] = robots.swing_speed;
        robots.swing_da[5] = -robots.swing_speed;
        robots.robot_dx = -robots.robot_speed;
        robots.robot_dy = 0;
        robots.robot_dz = 0;

        break;

    case 'd':
        flag_walk = true;
        robots.rotate_robot = 90;
        robots.swing_da[2] = robots.swing_speed;
        robots.swing_da[3] = -robots.swing_speed;
        robots.swing_da[4] = robots.swing_speed;
        robots.swing_da[5] = -robots.swing_speed;
        robots.robot_dx = robots.robot_speed;
        robots.robot_dy = 0;
        robots.robot_dz = 0;
        break;

    case'+':
        robots.robot_speed += 0.01f;
        robots.max_swing += 5.0f;
        robots.swing_speed += 1.0f;
        break;

    case'-':
        if (robots.robot_speed > 0.02) {
            robots.robot_speed -= 0.01f;
        }
        if (robots.robot_speed > 35.0f) {
            robots.max_swing -= 5.0f;
        }
        if (robots.swing_speed > 2.0f) {
            robots.swing_speed -= 1.0f;
        }
        break;

    case'j':
        if (not flag_jump) {
            std::cout << "jump";
            flag_jump = true;
            robots.robot_dy = robots.jump_speed;
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
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, stage.vertex_count * sizeof(Vertex), stage.vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
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
        robots.x += robots.robot_dx;

        robots.z += robots.robot_dz;

        for (int i = 2; i < 6; ++i) {
            robots.swing_angle[i] += robots.swing_da[i];
        }
        if (robots.swing_angle[2] > robots.max_swing) {
            robots.swing_da[2] = -robots.swing_speed;
        }
        if (robots.swing_angle[2] < -robots.max_swing) {
            robots.swing_da[2] = robots.swing_speed;
        }

        if (robots.swing_angle[3] < -robots.max_swing) {
            robots.swing_da[3] = robots.swing_speed;
        }
        if (robots.swing_angle[3] > robots.max_swing) {
            robots.swing_da[3] = -robots.swing_speed;
        }

        if (robots.swing_angle[4] > robots.max_swing) {
            robots.swing_da[4] = -robots.swing_speed;
        }
        if (robots.swing_angle[4] < -robots.max_swing) {
            robots.swing_da[4] = robots.swing_speed;
        }

        if (robots.swing_angle[5] < -robots.max_swing) {
            robots.swing_da[5] = robots.swing_speed;
        }
        if (robots.swing_angle[5] > robots.max_swing) {
            robots.swing_da[5] = -robots.swing_speed;
        }
        if (robots.x < -5.8f) {
            robots.robot_dx = robots.robot_speed;
            robots.rotate_robot += 180;
        }
        if (robots.x > 5.8f) {
            robots.robot_dx = -robots.robot_speed;
            robots.rotate_robot -= 180;
        }
        if (robots.z < -5.8f) {
            robots.robot_dz = robots.robot_speed;
            robots.rotate_robot += 180;
        }
        if (robots.z > 5.8f) {
            robots.robot_dz = -robots.robot_speed;
            robots.rotate_robot -= 180;
        }
    }
    else {
        for (int i = 2; i < 6; ++i) {
            robots.swing_angle[i] = 0;
        }
    }
    for (int i = 0; i < 64; ++i) {
        if (not collide(i)) {
            
            flag_jump = true;
        }
        if (i < 14) {
            if(obs[i].type=="obs"){
                if (collide(robots.get_aabb(), obs[i].get_aabb()) && robots.y >= obs[i].get_aabb().y2 - 0.5) {
                    obs[i].dy = -0.02f;
                }
                else if (not collide(robots.get_aabb(), obs[i].get_aabb())) {
                    if (obs[i].y < 0.4f)
                        obs[i].dy = 0.005f;
                    else
                        obs[i].dy = 0;
                }
            }
            if(obs[i].type=="pill"){

                if (collide(robots.get_aabb(), obs[i].get_aabb()) && robots.y < obs[i].get_aabb().y2 - 0.5) {
                    if(robots.x < obs[i].get_aabb().x1 || robots.x > obs[i].get_aabb().x2)
                        robots.robot_dx = 0;
                    if (robots.z < obs[i].get_aabb().z1 || robots.z > obs[i].get_aabb().z2)
                        robots.robot_dz = 0;
                }

            }

        }

    }

    robots.y += robots.robot_dy;
    if (flag_jump) {

        robots.robot_dy -= 0.002;
        if (robots.robot_dy <=0 ){
            for (int i = 0; i < 64; ++i) {
                if (collide(i) && robots.y >= floors[i].get_aabb().y2 - 0.5) {
                    robots.robot_dy = 0;
                    flag_jump = false;
                }
            }
        }

    }


    glutPostRedisplay();
    glutTimerFunc(10, timer, 0);
}

void reset() {
    camera_angle = 0;

    open_x[0] = 0;
    open_x[1] = 0;

    camera_x = 0;
    camera_y = 0;
    camera_z = 0;

    flag_open = 0;
    flag_walk = 0;
    flag_jump = true;
}

bool collide(int idx) {
    if (idx < 3) {
        if (robots.x + 0.05 < obs[idx].get_aabb().x1) return false;
        if (robots.x - 0.05 > obs[idx].get_aabb().x2) return false;
        if (robots.y < obs[idx].get_aabb().y1) return false;
        if (robots.y > obs[idx].get_aabb().y2) return false;
        if (robots.z + 0.05 < obs[idx].get_aabb().z1) return false;
        if (robots.z - 0.05 > obs[idx].get_aabb().z2) return false;
        return true;
    }
    if (robots.x + 0.05 < floors[idx].get_aabb().x1) return false;
    if (robots.x - 0.05 > floors[idx].get_aabb().x2) return false;
    if (robots.y < floors[idx].get_aabb().y1) return false;
    if (robots.y > floors[idx].get_aabb().y2) return false;
    if (robots.z + 0.05 < floors[idx].get_aabb().z1) return false;
    if (robots.z - 0.05 > floors[idx].get_aabb().z2) return false;
    return true;

}

bool collide(aabb first, aabb second) {
    if (first.x2< second.x1) return false;
    if (first.x1 > second.x2) return false;
    if (first.y2 < second.y1) return false;
    if (first.y1 > second.y2) return false;
    if (first.z2 < second.z1) return false;
    if (first.z1 > second.z2) return false;
    return true;
}