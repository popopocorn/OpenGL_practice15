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
#define vertex_shader_code "middle_vertex_shader.glsl"
#define fragment_shader_code "middle_fragment_shader.glsl"


std::random_device rd;
std::mt19937 g(rd());

//------------------------------------------------------
//콜백함수
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int, int, int, int);
GLvoid MoveMouse(int, int);
GLvoid timer(int);
//------------------------------------------------------
//셰이더 용 선언
GLuint shader_program;
GLuint vertexShader;
GLuint fragmentShader;

GLuint LAO, LBO;

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

typedef struct shapes {
    GLuint VAO, VBO, EBO;
    std::vector<glm::vec3> vertices;
    glm::vec3 body_color = glm::vec3(0.0f, 0.0f, 0.0f);
    int num_of_point{};
    int num_of_face{};
    float dx{};
    float dy{};
    float x{};
    float y{};
    float first_x{ 1.0 };
    float first_y{};
    float angle{};
    bool split = false;
    std::vector<unsigned int>index_list;
    glm::mat4 mod_trans = glm::mat4(1.0f);
    shapes* other;

    void init();
    void init_mat();
    void gen_buffuer();
    void update_buffer();
};
void shapes::gen_buffuer() {
    // 버퍼 생성
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size() * 3, vertices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_of_face * 3 * sizeof(unsigned int), index_list.data(), GL_STATIC_DRAW);

}
void shapes::init_mat() {
    mod_trans = glm::translate(mod_trans, glm::vec3(first_x + x, first_y + y, 0.0f));
    mod_trans = glm::rotate(mod_trans, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));



    body_color = glm::vec3(float(g() % 1000) / 1000.0f, float(g() % 1000) / 1000.0f, float(g() % 1000) / 1000.0f);
}
void shapes::init() {
    init_mat();
    
    num_of_point = g() % 4 + 3;

    switch (num_of_point) {
    case 3: // 삼각형
        vertices = {
            glm::vec3(0.0f,  0.15f, 0.0f),  // 위
           glm::vec3(-0.15f, -0.15f, 0.0f),  // 왼쪽
            glm::vec3(0.15f, -0.15f, 0.0f)   // 오른쪽
        };
        index_list = { 0, 1, 2 };  // 삼각형의 인덱스
        num_of_face = 1;
        break;

    case 4: // 사각형
        vertices = {
            glm::vec3(-0.15f,  0.15f, 0.0f),  // 왼쪽 위
             glm::vec3(0.15f,  0.15f, 0.0f),   // 오른쪽 위
             glm::vec3(0.15f, -0.15f, 0.0f),   // 오른쪽 아래
            glm::vec3(-0.15f, -0.15f, 0.0f)   // 왼쪽 아래
        };
        index_list = { 0, 1, 2, 0, 2, 3 };
        num_of_face = 2;
        break;

    case 5: // 오각형
        vertices = {
            glm::vec3(0.0f,  0.15f, 0.0f),   // 상단
            glm::vec3(0.15f,  0.05f, 0.0f),  // 오른쪽 위
            glm::vec3(0.09f, -0.2f, 0.0f),   // 오른쪽 아래
           glm::vec3(-0.09f, -0.2f, 0.0f),   // 왼쪽 아래
           glm::vec3(-0.15f,  0.05f, 0.0f)   // 왼쪽 위
        };
        index_list = { 0, 1, 2, 0, 2, 3, 0, 3, 4 };
        num_of_face = 3;
        break;

    case 6: // 육각형
        vertices = {
            glm::vec3(0.0f,  0.15f, 0.0f),   // 상단
            glm::vec3(0.15f,  0.075f, 0.0f), // 오른쪽 위
            glm::vec3(0.15f, -0.075f, 0.0f), // 오른쪽 아래
            glm::vec3(0.0f, -0.15f, 0.0f),   // 하단
           glm::vec3(-0.15f, -0.075f, 0.0f), // 왼쪽 아래
           glm::vec3(-0.15f,  0.075f, 0.0f)  // 왼쪽 위
        };
        index_list = { 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5 };
        num_of_face = 4;
        break;

    default:

        break;
    }
    gen_buffuer();
}
void shapes::update_buffer() {
    num_of_point = vertices.size();
    num_of_face = num_of_point - 2;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size() * 3, vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_of_face * 3 * sizeof(unsigned int), index_list.data(), GL_STATIC_DRAW);
}

bool flag_drag;
bool flag_wired;

float mouse_first_x;
float mouse_first_y;
float mouse_end_x;
float mouse_end_y;

bool flag_split;

float mouse_line[6] = { };

glm::vec3 red_color(1.0f, 0.0f, 0.0f);

//------------------------------------------------------
//필요한 함수 선언

void is_hit(int);

int ccw(glm::vec3, glm::vec3, glm::vec3);
bool get_intersection_point(const glm::vec3& p1, const glm::vec3& p2,
    const glm::vec3& q1, const glm::vec3& q2,
    glm::vec3& intersection);

void spilt_shape(int, std::vector<glm::vec3>);

void create_shape();


std::vector<shapes> s;
//------------------------------------------------------
void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
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
    glutMouseFunc(Mouse);
    glutMotionFunc(MoveMouse);
    glutTimerFunc(60, timer, 0);
    shapes first;
    first.init();
    s.push_back(first);
    

    init_buffer();


    glutMainLoop();

}


GLvoid drawScene(GLvoid) {

    glClearColor(base_r, base_g, base_b, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);

    glValidateProgram(shader_program);


    GLuint color = glGetUniformLocation(shader_program, "in_color");
    GLuint trans_mat = glGetUniformLocation(shader_program, "trans");

    glm::mat4 temp(1.0f);

    if(not flag_wired){
        for (int i = 0; i < s.size(); ++i) {
            glUniform3fv(color, 1, glm::value_ptr(s[i].body_color));
            glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(s[i].mod_trans));
            glBindVertexArray(s[i].VAO);
            glDrawElements(GL_TRIANGLES, s[i].num_of_face * 3, GL_UNSIGNED_INT, 0);
        }
    }
    else {
        for (int i = 0; i < s.size(); ++i) {
            glUniform3fv(color, 1, glm::value_ptr(s[i].body_color));
            glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(s[i].mod_trans));
            glBindVertexArray(s[i].VAO);
            //glDrawElements(GL_LINE_STRIP, s[i].num_of_face * 3 + 1, GL_UNSIGNED_INT, 0);
            glDrawArrays(GL_POINTS, 0, s[i].vertices.size());

        }
    }




    if (flag_drag) {
        glLineWidth(3.0f);
        glUniform3fv(color, 1, glm::value_ptr(red_color));
        glUniformMatrix4fv(trans_mat, 1, GL_FALSE, glm::value_ptr(temp));
        glBindVertexArray(LAO);
        glDrawArrays(GL_LINES, 0, 2);

    }
    glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);

}
GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'l':
        flag_wired = !flag_wired;
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


    glGenVertexArrays(1, &LAO);
    glBindVertexArray(LAO);

    glGenBuffers(1, &LBO);
    glBindBuffer(GL_ARRAY_BUFFER, LBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mouse_line), mouse_line, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);



}

GLvoid Mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        mouse_first_x = (float)(x - (float)width / 2.0) * (float)(1.0 / (float)(width / 2.0));
        mouse_first_y = -(float)(y - (float)height / 2.0) * (float)(1.0 / (float)(height / 2.0));
        flag_drag = true;
        flag_split = true;

        mouse_line[0] = mouse_first_x;
        mouse_line[1] = mouse_first_y;
        mouse_line[3] = mouse_first_x;
        mouse_line[4] = mouse_first_y;

        glBindBuffer(GL_ARRAY_BUFFER, LBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(mouse_line), mouse_line);
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        for (int i = 0; i < s.size(); ++i) {
            if(flag_split)
                is_hit(i);
        }
        flag_drag = false;
        std::cout << s.size();
    }
    glutPostRedisplay();
}
GLvoid MoveMouse(int x, int y) {
    if (flag_drag) {
        mouse_end_x = (float)(x - (float)width / 2.0) * (float)(1.0 / (float)(width / 2.0));
        mouse_end_y = -(float)(y - (float)height / 2.0) * (float)(1.0 / (float)(height / 2.0));

        
        mouse_line[3] = mouse_end_x;
        mouse_line[4] = mouse_end_y;


        glBindBuffer(GL_ARRAY_BUFFER, LBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(mouse_line), mouse_line);

  
    }
    glutPostRedisplay();
}
GLvoid timer(int value) {
    for (int i = 0; i < s.size(); ++i) {
        for (int j = 0; j < s[i].vertices.size(); ++j) {
            
        }
    }
    
    glutTimerFunc(60, timer, 0);
    glutPostRedisplay();
}

int ccw(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    float s = p1.x * p2.y + p2.x * p3.y + p3.x * p1.y;
    s -= (p1.y * p2.x + p2.y * p3.x + p3.y * p1.x);

    if (s > 0)return 1;
    else if (s == 0) return 0;
    else return -1;
}

void is_hit(int index) {
    shapes& current_shape = s[index];
    glm::vec3 p1(mouse_first_x, mouse_first_y, 0.0f);
    glm::vec3 p2(mouse_end_x, mouse_end_y, 0.0f);
    glm::vec3 intersection;
    std::vector<glm::vec3> intersections;
    bool is_duplicate = false;
    bool is_split = false;
    for (int i = 0; i < current_shape.index_list.size(); ++i) {
        glm::vec3 q1 = current_shape.vertices[current_shape.index_list[i]];
        glm::vec3 q2 = current_shape.vertices[current_shape.index_list[(i + 1) % current_shape.index_list.size()]];

        int ccw1 = ccw(p1, p2, q1) * ccw(p1, p2, q2);
        int ccw2 = ccw(q1, q2, p1) * ccw(q1, q2, p2);

        if (ccw1 <= 0 && ccw2 <= 0) {
            if (get_intersection_point(p1, p2, q1, q2, intersection)) {
                // 중복된 교차점 여부 확인
                is_duplicate = false;
                for (const auto& existing_intersection : intersections) {
                    if (glm::length(existing_intersection - intersection) < 0.001f) {
                        is_duplicate = true;
                        break;
                    }
                }

                if (not is_duplicate) {
                    intersections.push_back(intersection);
                    is_split = true;
                    /*std::cout << "도형 " << index << "이/가 선에 의해 맞았습니다! 교차점: ("
                        << intersection.x << ", " << intersection.y << ")" << std::endl;*/
                }
            }
        }
    }
    if(is_split){
        //for (const auto& inter : intersections) {
        //    s[index].vertices.push_back(inter);  // vertices에 교차점 추가
        //    ++s[index].num_of_point;
        //    ++s[index].num_of_face;
        //   
        //}
        spilt_shape(index, intersections);

    }
}

bool get_intersection_point(const glm::vec3& p1, const glm::vec3& p2,
    const glm::vec3& q1, const glm::vec3& q2,
    glm::vec3& intersection) {
    glm::vec3 r = p2 - p1;
    glm::vec3 s = q2 - q1;

    float rxs = r.x * s.y - r.y * s.x;
    float qpxr = (q1.x - p1.x) * r.y - (q1.y - p1.y) * r.x;

    // rxs가 0이면 선분이 평행하거나 일치
    if (rxs == 0 && qpxr == 0) {
        return false; // 평행하거나 일치하는 경우 교차점이 없음
    }

    float t = ((q1.x - p1.x) * s.y - (q1.y - p1.y) * s.x) / rxs;
    float u = qpxr / rxs;

    // 0 <= t <= 1 및 0 <= u <= 1이면 두 선분이 교차
    if (rxs != 0 && t >= 0 && t <= 1 && u >= 0 && u <= 1) {
        intersection = p1 + t * r;
        return true;
    }

    return false; // 교차하지 않는 경우
}

float is_under(float x) {

    float m = (mouse_end_y - mouse_first_y) / (mouse_end_x - mouse_first_x);
    float y = m * x + (mouse_first_y - (m * mouse_first_x));
    return y;
}
void spilt_shape(int index , std::vector<glm::vec3> intersections) {
    std::vector<glm::vec3> temp1;           //top의 정점
    std::vector<glm::vec3> temp2;           //bottom의 정점
    std::vector<unsigned int> temp_list1;   //top의 인덱스리스트
    std::vector<unsigned int> temp_list2;   //bottom의 인덱스 리스트

    shapes top;
    shapes bottom;
    if (mouse_end_x == mouse_first_x) {
        for (int i = 0; i < s[index].vertices.size(); ++i) {
            if (s[index].vertices[i].x <= mouse_end_x) {

                temp1.push_back(s[index].vertices[i]);
            }
            if (s[index].vertices[i].x >= mouse_end_x) {
                temp2.push_back(s[index].vertices[i]);
            }
        }
    }
    else {
        for (int i = 0; i < s[index].vertices.size(); ++i) {
            if (s[index].vertices[i].y >= is_under(s[index].vertices[i].x)) {
                temp1.push_back(s[index].vertices[i]);
            }
            if (s[index].vertices[i].y <= is_under(s[index].vertices[i].x)) {
                temp2.push_back(s[index].vertices[i]);
            }
        }
    }
    /*for (glm::vec3 temp : intersections) {
        temp1.push_back(temp);
        temp2.push_back(temp);
    }*/

    for (int i = intersections.size() - 1; i > -1; --i) {
        temp1.push_back(intersections[i]);
        temp2.push_back(intersections[i]);
    }
    top.vertices = temp1;
    top.num_of_point = top.vertices.size();
    top.num_of_face = top.num_of_point - 2;
    for (int i = 0; i < top.num_of_face; ++i) {
        temp_list1.push_back(0);
        temp_list1.push_back(i + 1);
        temp_list1.push_back(i + 2);
    }
    top.index_list = temp_list1;
    top.gen_buffuer();
    top.init_mat();

    bottom.vertices = temp2;
    bottom.num_of_point = bottom.vertices.size();
    bottom.num_of_face = bottom.num_of_point - 2;
    for (int i = 0; i < bottom.num_of_face; ++i) {
        temp_list2.push_back(0);
        temp_list2.push_back(i + 1);
        temp_list2.push_back(i + 2);
    }
    bottom.index_list = temp_list2;
    bottom.gen_buffuer();
    bottom.init_mat();

    s.erase(s.begin() + index);
    s.push_back(top);
    s.push_back(bottom);

    flag_split = false;
}

void create_shape() {
    shapes temp;
    temp.init();
    s.push_back(temp);
}