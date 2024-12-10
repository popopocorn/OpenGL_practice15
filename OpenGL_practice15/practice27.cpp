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
#include"crane.h"



//�̸� �����Ұ�
#define vertex_shader_code "30_Vertex_shader.glsl"
#define fragment_shader_code "30_Fragment_shader.glsl"
std::random_device(rd);
std::mt19937 g(rd());
std::uniform_real_distribution<float> dist(-5.0f, 5.0f);
std::uniform_real_distribution<float> dist2(0.3f, 1.5f);
//------------------------------------------------------
//�ݹ��Լ�

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid SpecialKeyboard(int key, int x, int y);
GLvoid timer(int);

//------------------------------------------------------
//���̴� �� ����
GLuint shader_program;
GLuint vertexShader;
GLuint fragmentShader;
GLuint VAO[4], VBO[4], NBO[4];

void make_vertex_shader();
void make_fragment_shader();
GLuint make_shader();
GLvoid init_buffer();
//------------------------------------------------------
//��������
GLclampf base_r = 0.0f;
GLclampf base_g = 0.0f;
GLclampf base_b = 0.0f;
GLint width{ 800 }, height{ 600 };


glm::vec3 light(0.0f, 2.0f, -3.0);
bool light_on{ true };
glm::vec3 lc(0.7f, 0.7f, 0.7f);
glm::vec3 light_color(0.7f, 0.7f, 0.7f);

glm::vec3 camera_pos(0.0f, 0.0f, 0.0f);
glm::vec3 after_pos(0.0f, 0.0f, 0.0f);

glm::vec3 camera_target(camera_pos.x, camera_pos.y, -1.0f);

shape lo{ light.x, light.y + 0.2f, light.z, "sphere2.obj", white_color };
shape plane{ 0.0f, 0.0f, 0.0f, "cube1.obj", brown_color };

float cx;
float cy;
float cz;

float angle;

int p_level;

int light_loc;

bool flag_snow;

bool is_glass;

shape pyramid{0.0, -1.5f, 0.0, "pyramid2.obj", sky_color};

shape snow[] = {
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
    shape{dist(g), 5.0f, dist(g), "sphere2.obj", white_color},
 

};

shape tower[] = {
    shape(dist(g), -1.5f, dist(g), "cube1.obj", purple_color),
    shape(dist(g), -1.5f, dist(g), "cube1.obj", purple_color),
    shape(dist(g), -1.5f, dist(g), "cube1.obj", purple_color),
    shape(dist(g), -1.5f, dist(g), "cube1.obj", purple_color),
    shape(dist(g), -1.5f, dist(g), "cube1.obj", purple_color),

};

Model level[4];

//------------------------------------------------------
//�ʿ��� �Լ� ����

void sierpinskiTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, int level, Model& model);
void triangle(Model& model, int level);

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
    lo.gen_buffer();
    plane.gen_buffer();
    for (int i = 0; i < 4; ++i) {
        triangle(level[i], i + 1);
    }
    pyramid.gen_buffer();
    for (shape& s : snow) {
        s.gen_buffer();
        s.scale_x = 0.1f;
        s.scale_y = 0.1f;
        s.scale_z = 0.1f;
        s.dy = -(float(g() % 5 + 1) / 100.0);
    }

    for (shape& t : tower) {
        t.gen_buffer();
        t.scale_x = dist2(g);
        t.scale_y = dist2(g);
        t.scale_z = dist2(g);
    }

    glEnable(GL_DEPTH_TEST);  // ���� �׽�Ʈ Ȱ��ȭ
    init_buffer();

    glutMainLoop();

}

GLvoid drawScene(GLvoid) {

    glClearColor(base_r, base_g, base_b, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader_program);

    glValidateProgram(shader_program);


    //glEnable(GL_CULL_FACE);




    glm::mat4 temp(1.0f);

    glm::mat4 proj2 = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
    GLuint projection = glGetUniformLocation(shader_program, "projection");



    glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(proj2));


    glm::mat4 view(1.0f);

    glm::mat4 camera_trans(1.0f);

    glm::vec3 camera_up(0.0f, 1.0f, 0.0f);
    view = glm::lookAt(camera_pos, camera_target, camera_up);
    camera_trans = glm::translate(camera_trans, glm::vec3(cx, cy, cz - 10.0f));
    camera_trans = glm::rotate(camera_trans, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    view *= camera_trans;
    
    glm::mat4 ct(1.0f);
    ct = glm::translate(ct, glm::vec3(cx, cy, cz + 10.0f));
    ct = glm::rotate(ct, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    after_pos = glm::vec3(
        ct * glm::vec4(camera_pos, 1.0f)
    );
    GLuint view_mat = glGetUniformLocation(shader_program, "view");
    glUniformMatrix4fv(view_mat, 1, GL_FALSE, glm::value_ptr(view));


    unsigned int lighPosLocaiont = glGetUniformLocation(shader_program, "lightPos");
    glUniform3f(lighPosLocaiont, light.x, light.y, light.z);

    unsigned int lightColorLocation = glGetUniformLocation(shader_program, "lightColor");
    glUniform3f(lightColorLocation, light_color.x, light_color.y, light_color.z);

    unsigned int viewPosLocation = glGetUniformLocation(shader_program, "viewPos");
    glUniform3f(viewPosLocation, after_pos.x, after_pos.y, after_pos.z);


    GLuint trans_mat = glGetUniformLocation(shader_program, "model");
    GLuint trans_mat2 = glGetUniformLocation(shader_program, "model2");
    GLuint color = glGetUniformLocation(shader_program, "color");
    GLuint light_switch = glGetUniformLocation(shader_program, "light_on");
    GLuint gl = glGetUniformLocation(shader_program, "glass");
    glUniform1i(light_switch, light_on);
    is_glass = false;
    glUniform1i(gl, is_glass);
    /*glBindVertexArray(shape.VAO);
    shape.update_position();
    glUniform3fv(color, 1, glm::value_ptr(shape.color));
    glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(shape.trans));
    glDrawArrays(GL_TRIANGLES, 0, shape.model.vertices.size());*/
    glUniformMatrix4fv(trans_mat2, 1, GL_FALSE, glm::value_ptr(temp));
    glBindVertexArray(lo.VAO);
    lo.scale_x = 0.1f;
    lo.scale_y = 0.1f;
    lo.scale_z = 0.1f;
    lo.update_position();
    glUniform3fv(color, 1, glm::value_ptr(lo.color));
    glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(lo.trans));
    glDrawArrays(GL_TRIANGLES, 0, lo.model.vertices.size());

    glBindVertexArray(plane.VAO);
    plane.y = -2.3f;
    plane.scale_x = 10.0f;
    plane.scale_y = 0.2;
    plane.scale_z = 10.0f;
    plane.update_position();
    glUniform3fv(color, 1, glm::value_ptr(plane.color));
    glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(plane.trans));
    glDrawArrays(GL_TRIANGLES, 0, plane.model.vertices.size());

    for (int i = 0; i < 5; ++i) {
        if (p_level == 0) {
            glBindVertexArray(pyramid.VAO);
            pyramid.update_position();
            glUniform3fv(color, 1, glm::value_ptr(pyramid.color));
            glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(pyramid.trans));
            glDrawArrays(GL_TRIANGLES, 0, pyramid.model.vertices.size());
        }
        else if (i > 0 && p_level==i) {

            glBindVertexArray(VAO[i]);
            glUniform3fv(color, 1, glm::value_ptr(pyramid.color));
            glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(pyramid.trans));
            glDrawArrays(GL_TRIANGLES, 0, level[i-1].vertices.size());
        }
    }

    switch (p_level)
    {
    case 0:
        glBindVertexArray(pyramid.VAO);
        pyramid.update_position();
        glUniform3fv(color, 1, glm::value_ptr(pyramid.color));
        glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(pyramid.trans));
        glDrawArrays(GL_TRIANGLES, 0, pyramid.model.vertices.size());
        break;

    case 1:
        glBindVertexArray(VAO[p_level - 1]);
        glUniform3fv(color, 1, glm::value_ptr(pyramid.color));
        glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(pyramid.trans));
        glDrawArrays(GL_TRIANGLES, 0, level[p_level - 1].vertices.size());
        break;

    case 2:
        
        glBindVertexArray(VAO[p_level - 1]);
        glUniform3fv(color, 1, glm::value_ptr(pyramid.color));
        glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(pyramid.trans));
        glDrawArrays(GL_TRIANGLES, 0, level[p_level - 1].vertices.size());
        break;

    case 3:
        
        glBindVertexArray(VAO[p_level - 1]);
        glUniform3fv(color, 1, glm::value_ptr(pyramid.color));
        glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(pyramid.trans));
        glDrawArrays(GL_TRIANGLES, 0, level[p_level - 1].vertices.size());
        break;

    case 4:
        
        glBindVertexArray(VAO[p_level - 1]);
        glUniform3fv(color, 1, glm::value_ptr(pyramid.color));
        glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(pyramid.trans));
        glDrawArrays(GL_TRIANGLES, 0, level[p_level - 1].vertices.size());
        break;
    default:
        break;
    }

    if(flag_snow){
        for (shape& s : snow) {
            glBindVertexArray(s.VAO);
            s.update_position();
            glUniform3fv(color, 1, glm::value_ptr(s.color));
            glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(s.trans));
            glDrawArrays(GL_TRIANGLES, 0, s.model.vertices.size());
        }
    }

    //glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (shape& t : tower) {
        glBindVertexArray(t.VAO);
        is_glass = true;
        glUniform1i(gl, is_glass);
        t.update_position();
        glUniform3fv(color, 1, glm::value_ptr(t.color));
        glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(t.trans));
        glDrawArrays(GL_TRIANGLES, 0, t.model.vertices.size());
    }
    //glDepthMask(GL_TRUE);
    glutSwapBuffers();

}


GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);

}
GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case '0':
        p_level = 0;

        break;
    case '1':
        p_level = 1;

        break;
    case '2':
        p_level = 2;

        break;
    case '3':
        p_level = 3;

        break;
    case '4':
        p_level = 4;

        break;
    case 'r':
        light = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(3.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(light, 1.0f));
        break;

    case'R':

        light = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(-3.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(light, 1.0f));

        break;

    case'n':
    {
        glm::vec3 dir = glm::normalize(after_pos - light);
        light += dir * 0.1f;
    }
        //camera_pos.x += 0.5;
        //mother.x += 0.1f;
        break;

    case'f':
    {
        glm::vec3 dir = glm::normalize(after_pos - light);
        light -= dir * 0.1f;
    }
        //camera_pos.x -= 0.5;
        //mother.x -= 0.1f;
        break;

    case '+':
        light_color.x += 0.1f;
        light_color.y += 0.1f;
        light_color.z += 0.1f;
        break;
    case '-':
        light_color.x -= 0.1f;
        light_color.y -= 0.1f;
        light_color.z -= 0.1f;
        break;

    case 's':
        flag_snow = !flag_snow;

        break;

    case 'm':
        light_on = !light_on;
        break;

    case 'c':
        light_color = brown_color;

        break;

    case 'v':
        light_color = lc;

        break;

    case 'b':
        light_color = sky_color;

        break;
    case 'p':
        ++light_loc;
        if (light_loc > 3) {
            light_loc = 0;
        }
        switch (light_loc)
        {
        case 0:
            light = glm::vec3(1, 0, 0);
            break;

        case 1:
            light = glm::vec3(-1, 0, 0);
            break;

        case 2:
            light = glm::vec3(0, 0, 1);
            break;

        case 3:
            light = glm::vec3(0, 0, -1);
            break;


        default:
            break;
        }
        break;

    case 'y':
        angle += 1.0f;
        break;
    case 'Y':
        angle -= 1.0f;
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
        std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
        return;
    }
    else {
        std::cout << "������ ����\n";
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
        std::cerr << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
        return;
    }
    else {
        std::cout << "������ ����\n";
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
        std::cerr << "���̴� ���� ����\n" << errorLog << std::endl;

    }
    else {
        std::cout << "\n������ ����\n";

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

    }


    glUseProgram(shader01);

    return shader01;

}

GLvoid init_buffer() {
    for (int i = 0; i < 4; ++i) {
        glGenVertexArrays(1, &VAO[i]);
        glBindVertexArray(VAO[i]);

        glGenBuffers(1, &VBO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glBufferData(GL_ARRAY_BUFFER, level[i].vertices.size() * sizeof(glm::vec3), level[i].vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &NBO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, NBO[i]);
        glBufferData(GL_ARRAY_BUFFER, level[i].nvectors.size() * sizeof(glm::vec3), level[i].nvectors.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
    }

}
GLvoid SpecialKeyboard(int key, int x, int y) {

    glutPostRedisplay();
}

GLvoid timer(int value) {
    for (shape& s : snow) {
        s.y += s.dy;
        if (s.y < -2.0) {
            s.y = 5.0f;
        }
    }


    lo.x = light.x;
    lo.y = light.y;
    lo.z = light.z;

    glutPostRedisplay();
    glutTimerFunc(10, timer, 0);
}


void sierpinskiTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, int level, Model& model) {
    if (level == 0) {
        // �⺻ �ﰢ�� �߰�
        model.vertices.push_back(v1);
        model.vertices.push_back(v2);
        model.vertices.push_back(v3);
    //    std::cout << "(" << v1.x << ", " << v1.y << "), ("
    //<< v2.x << ", " << v2.y << "), ("
    //<< v3.x << ", " << v3.y << ")" << std::endl;

        // ��� ��� �� �߰�
        glm::vec3 edge1 = v2 - v1;
        glm::vec3 edge2 = v3 - v1;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
        for (int i = 0; i < 3; ++i) {
            model.nvectors.push_back(normal);
        }
    }
    else {
        // �߰��� ���
        glm::vec3 mid1 = (v1 + v2) * 0.5f;
        glm::vec3 mid2 = (v2 + v3) * 0.5f;
        glm::vec3 mid3 = (v3 + v1) * 0.5f;

        //std::cout << "(" << mid1.x << ", " << mid1.y << ", " << mid1.z << "), "
        //    << "(" << mid2.x << ", " << mid2.y << ", " << mid2.z << "), "
        //    << "(" << mid3.x << ", " << mid3.y << ", " << mid3.z << ")" << std::endl;



        // ���� �ﰢ�� ����
        sierpinskiTriangle(v1, mid1, mid3, level - 1, model);
        sierpinskiTriangle(mid1, v2, mid2, level - 1, model);
        sierpinskiTriangle(mid3, mid2, v3, level - 1, model);
    }
}


// �Ƕ�̵� ���� �Լ�
void triangle(Model& model, int level) {
    std::cout << level;
    // �ʱ� ���� ���� (�Ƕ�̵��� �������� �ٴ�)
    glm::vec3 top = { 0.0f, 0.5f, 0.0f };         // �Ƕ�̵� ������
    glm::vec3 base1 = { -0.5f, -0.5f, 0.5f };    // �ٴ� ���ϴ�
    glm::vec3 base2 = { 0.5f, -0.5f, 0.5f };     // �ٴ� ���ϴ�
    glm::vec3 base3 = { 0.5f, -0.5f, -0.5f };      // �ٴ� ����
    glm::vec3 base4 = { -0.5f, -0.5f, -0.5f };     // �ٴ� �»��
    // �þ��ɽ�Ű �ﰢ������ �� ���� ����
    sierpinskiTriangle(top, base1, base2, level, model); // �ո�
    sierpinskiTriangle(top, base2, base3, level, model); // ������
    sierpinskiTriangle(top, base3, base4, level, model); // �޸�
    sierpinskiTriangle(top, base4, base1, level, model); // ����

    model.vertices.push_back(base1);
    model.vertices.push_back(base2);
    model.vertices.push_back(base3);
    model.vertices.push_back(base1);
    model.vertices.push_back(base3);
    model.vertices.push_back(base4);
}



