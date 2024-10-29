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
bool flag_barrel_rotate{};
bool flag_barrel_merge{};
bool flag_arm_rotate{};

float camera_x;
float camera_y;
float camera_z;



float camera_angle{};
float rotate_angle{};

float center_x;
float center_y;
float center_z;

float scale_x[6]{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, };
float scale_y[6]{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, };
float scale_z[6]{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, };

float trans_x[6];
float trans_y[6];
float trans_z[6];


float rotate_y[6];
float rotate_z[6];

float temp_a{};

float first_x[6];
float first_y[6];
float first_z[6];

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
void set_cube() {

    flag_move_x = 0;
    flag_rotate_y = 0;
    flag_barrel_rotate = false;
    flag_barrel_merge = false;
    flag_arm_rotate = false;

    camera_x = 0;
    camera_y = 0;
    camera_z = 0;

    camera_angle = 0;
    rotate_angle = 0;

    center_x = 0;
    center_y = 0;
    center_z = 0;

    for (int i = 0; i < 6;++i) {
        scale_x[i] = 1.0f;
        scale_y[i] = 1.0f;
        scale_z[i] = 1.0f;

        trans_x[i]=0;
        trans_y[i] = 0;
        trans_z[i] = 0;


        rotate_y[i] = 0;
        rotate_z[i] = 0;

        

        first_x[i] = 0;
        first_y[i] = 0;
        first_z[i] = 0;
    }

    temp_a = 0;


    //몸통
    scale_x[0] = 5.0f;
    scale_y[0] = 2.0f;
    scale_z[0] = 5.0f;


    //포신
    scale_x[1] = 0.5f;
    scale_y[1] = 0.5f;
    scale_z[1] = 2.5f;

    scale_x[2] = 0.5f;
    scale_y[2] = 0.5f;
    scale_z[2] = 2.5f;

    first_z[1] = 1.5f;
    first_z[2] = 1.5f;
    first_y[1] = -0.1f;
    first_y[2] = -0.1f;
    first_x[1] = 1.0f;
    first_x[2] = -1.0f;

    //중간몸통
    scale_x[3] = 2.5f;
    scale_y[3] = 1.5f;
    scale_z[3] = 2.5f;
    first_y[3] = 0.8f;

    //팔
    scale_x[4] = 0.5f;
    scale_y[4] = 2.5f;
    scale_z[4] = 0.5f;

    scale_x[5] = 0.5f;
    scale_y[5] = 2.5f;
    scale_z[5] = 0.5f;
    
    first_y[4] = 1.4f;
    first_y[5] = 1.4f;
    first_x[4] = 0.35;
    first_x[5] = -0.35;
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

    }

    glEnable(GL_DEPTH_TEST);  // 깊이 테스트 활성화
    set_cube();
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

    proj2 = glm::translate(proj2, glm::vec3(0.0f, 0.0f, -10.0f));

    glm::mat4 view(1.0f);

    glm::vec3 camera_pos(0.0f, 0.0f, 0.0f);
    glm::vec3 camera_target(0.0, 0.0, -1.0);
    glm::vec3 camera_up(0.0f, 1.0f, 0.0f);
    view = glm::lookAt(camera_pos, camera_target, camera_up);
    
    view = glm::rotate(view, glm::radians(30.0f+temp_a), glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::rotate(view, glm::radians(camera_angle), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::translate(view, glm::vec3(camera_x, camera_y, camera_z));
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, 10.0f));
    view = glm::rotate(view, glm::radians(rotate_angle), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));
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
        cube_trans[i] = glm::translate(cube_trans[i], glm::vec3(trans_x[0], trans_y[0], trans_z[0]));
        cube_trans[i] = glm::rotate(cube_trans[i], glm::radians(rotate_y[i]), glm::vec3(0.0f, 1.0f, 0.0f));
        cube_trans[i] = glm::rotate(cube_trans[i], glm::radians(rotate_z[i]), glm::vec3(0.0f, 0.0f, 1.0f));
        cube_trans[i] = glm::translate(cube_trans[i], glm::vec3(first_x[i], first_y[i], first_z[i]));
        cube_trans[i] = glm::scale(cube_trans[i], glm::vec3(scale_x[i], scale_y[i], scale_z[i]));
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
    case 'r':
        camera_angle += 1.0f;
        break;

    case 'R':
        camera_angle -= 1.0f;
        break;
    case 'y':
        rotate_angle += 1.0f;
        break;

    case 'Y':
        rotate_angle -= 1.0f;
        break;
    case 'u':
        temp_a += 1.0f;
        break;

    case 'U':
        temp_a -= 1.0f;
        break;

    case 'x':
        camera_x -= 0.1f;
        
        break;

    case 'X':
        camera_x += 0.1f;
        
        break;

    case 'z':
        camera_z += 0.1f;
        
        break;

    case 'Z':
        camera_z -= 0.1f;
        
        break;

    case 'b':
        if(flag_move_x !=1)
            flag_move_x = 1;
        else
            flag_move_x = 0;
        break;

    case 'B':
        if (flag_move_x != 2)
            flag_move_x = 2;
        else 
            flag_move_x = 0;
        break;

    case 'm':
        if (flag_rotate_y != 1)
            flag_rotate_y = 1;
        else
            flag_rotate_y = 0;
        break;

    case 'M':
        if (flag_rotate_y != 2)
            flag_rotate_y = 2;
        else
            flag_rotate_y = 0;
        break;

    case 'f':
        flag_barrel_rotate = !flag_barrel_rotate;
        break;

    case 'e':
        flag_barrel_merge = !flag_barrel_merge;

        break;

    case 't':
        flag_arm_rotate = !flag_arm_rotate;

        break;

    case'c':
        set_cube();
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
    /*
    int flag_move_x{};
    int flag_rotate_y{};
    int flag_barrel_rotate{};
    bool flag_barrel_merge{};
    int flag_arm_rotate{};
    */
    if (flag_move_x == 1) {
        trans_x[0] += 0.1f;
    }
    else if (flag_move_x == 2) {
        trans_x[0] -= 0.1f;
    }
    if (flag_rotate_y == 1) {
        rotate_y[3] += 1.0f;
        rotate_y[4] += 1.0f;
        rotate_y[5] += 1.0f;
    }
    else if (flag_rotate_y == 2) {
        rotate_y[3] -= 1.0f;
        rotate_y[4] -= 1.0f;
        rotate_y[5] -= 1.0f;
    }
    if (flag_barrel_rotate) {
        rotate_y[1] += 1.0f;

        rotate_y[2] -= 1.0f;
    }

    if (flag_barrel_merge) {
        if (int(rotate_y[1]) % 360 != 0) {
            rotate_y[1] += 1.0f;
        }
        if (int(rotate_y[2]) % 360 != 0) {
            rotate_y[2] -= 1.0f;
        }
        if (int(rotate_y[1]) % 360 == 0 && int(rotate_y[2]) % 360 == 0) {
            if (first_x[1] > 0) {
                first_x[1] = ((first_x[1] * 10) - 1) / 10;
            }
            if (first_x[2] < 0) {
                first_x[2] = ((first_x[2] * 10) + 1) / 10;
            }
        }
    }
    else {
        if (first_x[1] < 1.0f) {
            first_x[1] = ((first_x[1] * 10) + 1) / 10;
        }
        if (first_x[2] > -1.0f) {
            first_x[2] = ((first_x[2] * 10) - 1) / 10;
        }
    }
    if (flag_arm_rotate) {
        if (rotate_z[4] > -90) {
            rotate_z[4] -= 1.0f;
        }
        if (rotate_z[5] < 90) {
            rotate_z[5] += 1.0f;
        }
    }
    else {
        if (rotate_z[4] < 0) {
            rotate_z[4] += 1.0f;
        }
        if (rotate_z[5] > 0) {
            rotate_z[5] -= 1.0f;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(10, timer, 0);
}
