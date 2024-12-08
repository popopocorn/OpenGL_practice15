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
#define vertex_shader_code "26_Vertex_shader.glsl"
#define fragment_shader_code "26_Fragment_shader.glsl"

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
GLuint VAO[4], VBO[4], EBO[4];

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


glm::vec3 light(0.0f, 0.0f, 3.0);
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


shape pyramid{0.0, -2.0f, 0.0, "pyramid2.obj", sky_color};

Model level[4];

//------------------------------------------------------
//�ʿ��� �Լ� ����
std::random_device(rd);
std::mt19937 g(rd());
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


    glEnable(GL_DEPTH_TEST);  // ���� �׽�Ʈ Ȱ��ȭ
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

    glm::mat4 camera_trans(1.0f);

    glm::vec3 camera_up(0.0f, 1.0f, 0.0f);
    view = glm::lookAt(camera_pos, camera_target, camera_up);
    camera_trans = glm::translate(camera_trans, glm::vec3(cx, cy, cz - 10.0f));
    camera_trans = glm::rotate(camera_trans, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    view *= camera_trans;

    after_pos = glm::vec3(
        camera_trans * glm::vec4(camera_pos, 1.0f)
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
    glUniform1i(light_switch, light_on);
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

    glBindVertexArray(pyramid.VAO);
    pyramid.update_position();
    glUniform3fv(color, 1, glm::value_ptr(pyramid.color));
    glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(pyramid.trans));
    glDrawArrays(GL_TRIANGLES, 0, pyramid.model.vertices.size());

    glutSwapBuffers();

}


GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);

}
GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {

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


}
GLvoid SpecialKeyboard(int key, int x, int y) {

    glutPostRedisplay();
}

GLvoid timer(int value) {



    glutPostRedisplay();
    glutTimerFunc(10, timer, 0);
}


void sierpinskiTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, int level, Model& model) {
    if (level == 0) {
        // �ܰ谡 0�̸� �ﰢ�� �߰�
        model.vertices.push_back({ v1.x, v1.y, v1.z });
        model.vertices.push_back({ v2.x, v2.y, v2.z });
        model.vertices.push_back({ v3.x, v3.y, v3.z });

        // ��� ���
        glm::vec3 edge1 = v2 - v1;
        glm::vec3 edge2 = v3 - v1;
        glm::vec3 n = glm::normalize(glm::cross(edge1, edge2));

        // Normal Ÿ������ ��ȯ �� ����
        glm::vec3 normal = { n.x, n.y, n.z };
        model.nvectors.push_back(normal);
        model.nvectors.push_back(normal);
        model.nvectors.push_back(normal);
    }
    else {
        // �߰��� ���
        glm::vec3 mid1 = (v1 + v2) * 0.5f;
        glm::vec3 mid2 = (v2 + v3) * 0.5f;
        glm::vec3 mid3 = (v3 + v1) * 0.5f;

        // ���� �ﰢ�� ��� ȣ��
        sierpinskiTriangle(v1, mid1, mid3, level - 1, model);
        sierpinskiTriangle(mid1, v2, mid2, level - 1, model);
        sierpinskiTriangle(mid3, mid2, v3, level - 1, model);
        sierpinskiTriangle(mid1, mid2, mid3, level - 1, model);
    }
}

// �Ƕ�̵� ���� �Լ�
void triangle(Model& model, int level) {
    // �ʱ� ���� ���� (�Ƕ�̵��� �������� �ٴ�)
    glm::vec3 top = { 0.0f, 0.5f, 0.0f };         // �Ƕ�̵� ������
    glm::vec3 base1 = { -0.5f, 0.0f, -0.5f };    // �ٴ� ���ϴ�
    glm::vec3 base2 = { 0.5f, 0.0f, -0.5f };     // �ٴ� ���ϴ�
    glm::vec3 base3 = { 0.5f, 0.0f, 0.5f };      // �ٴ� ����
    glm::vec3 base4 = { -0.5f, 0.0f, 0.5f };     // �ٴ� �»��

    // �þ��ɽ�Ű �ﰢ������ �� ���� ����
    sierpinskiTriangle(top, base1, base2, level, model); // �ո�
    sierpinskiTriangle(top, base2, base3, level, model); // ������
    sierpinskiTriangle(top, base3, base4, level, model); // �޸�
    sierpinskiTriangle(top, base4, base1, level, model); // ����
    sierpinskiTriangle(base1, base2, base3, level, model); // �ٴ� 1
    sierpinskiTriangle(base1, base3, base4, level, model); // �ٴ� 2
}