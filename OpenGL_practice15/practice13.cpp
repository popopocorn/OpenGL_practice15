#include<iostream>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
#include<gl/glm/ext.hpp>
#include<gl/glm/glm.hpp>
#include<gl/glm/gtc/matrix_transform.hpp>
#include"file_open.h"
#include"read_obj.h"
#include<random>
#include<vector>
//미리 선언할거
#define vertex_shader_code "13_vertex_shader.glsl"
#define fragment_shader_code "13_fragment_shader.glsl"
std::random_device rd;
std::mt19937 g(rd());

//------------------------------------------------------
//콜백함수
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
//------------------------------------------------------
//셰이더 용 선언
GLuint shader_program;
GLuint vertexShader;
GLuint fragmentShader;
GLuint VAO[2], VBO[2], EBO[2];

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

Model _cube_13;
Model _diamond_13;

int draw_face1{-1};
int draw_face2{-1};


//------------------------------------------------------
//필요한 함수 선언

void print_model_info(const Model& model) {
    std::cout << "Total Vertices: " << model.vertex_count << std::endl;
    for (size_t i = 0; i < model.vertex_count; ++i) {
        std::cout << "Vertex " << i + 1 << ": ("
            << model.vertices[i].x << ", "
            << model.vertices[i].y << ", "
            << model.vertices[i].z << ")" << std::endl;
    }

    std::cout << "Total Faces: " << model.face_count << std::endl;
    for (size_t i = 0; i < model.face_count; ++i) {
        std::cout << "Face " << i + 1 << ": ("
            << model.faces[i].v1 + 1 << ", " 
            << model.faces[i].v2 + 1 << ", "
            << model.faces[i].v3 + 1 << ")" << std::endl;
    }
    std::cout << "\n\n\n";
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
    read_obj_file("cube.obj", &_cube_13);
    read_obj_file("diamond.obj", &_diamond_13);
    

    init_buffer();

    glutMainLoop();

}


GLUquadricObj* qobj;

GLvoid drawScene(GLvoid) {

    glClearColor(base_r, base_g, base_b, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);

    glValidateProgram(shader_program);


    glm::mat4 rotate_mat1 = glm::rotate(glm::mat4(1.0f), glm::radians(50.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotate_mat2 = glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(.0f, 0.0f, 1.0f));
    glm::mat4 rotate_mat = rotate_mat1 * rotate_mat2;
    GLuint modelLoc = glGetUniformLocation(shader_program, "rotate");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(rotate_mat));
    
    glBindVertexArray(VAO[0]);

    for (int i = 0; i < (_cube_13.face_count / 2); ++i) {
        if(i==draw_face1 - 1 || i == draw_face2 - 1)
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(i*sizeof(Face)*2));

    }
    rotate_mat1 = glm::rotate(glm::mat4(1.0f), glm::radians(50.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotate_mat2 = glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(.0f, 0.0f, 1.0f));
    rotate_mat = rotate_mat1 * rotate_mat2;
    modelLoc = glGetUniformLocation(shader_program, "rotate");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(rotate_mat));

    glBindVertexArray(VAO[1]);
    for (int i = 0; i < _diamond_13.face_count;++i) {
        if(i == draw_face1 - 7 || i == draw_face2 - 7 )
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(i*sizeof(Face)));

    }
    glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
    glViewport(0, 0, w, h);

}
GLvoid Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'c':
        draw_face1 = g() % (_cube_13.face_count / 2) + 1;
        draw_face2 = g() % (_cube_13.face_count / 2) + 1;
        while (draw_face1 == draw_face2) {
            draw_face2 = g() % ((_cube_13.face_count / 2) - 1) + 1;
        }
        std::cout << draw_face1 << std::endl;;
        std::cout << draw_face2 << std::endl;;

        break;
        
    case 't':
        draw_face1 = (g() % _diamond_13.face_count) + 7;
        draw_face2 = (g() % _diamond_13.face_count) + 7;
        while (draw_face1 == draw_face2) {
            draw_face2 = (g() % _diamond_13.face_count) + 7;
        }

        break;

    case 'q':
        glutLeaveMainLoop();
        break;
    }


    if ('1' <= key && key <= '9') {
        draw_face1 = key - '0';
        std::cout << draw_face1;
        draw_face2 = -1;
    }
    if (key == '0') {
        draw_face1 = 10;
        std::cout << draw_face1;
        draw_face2 = -1;
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
    glBufferData(GL_ARRAY_BUFFER, _cube_13.vertex_count*sizeof(Vertex), _cube_13.vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &EBO[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _cube_13.face_count*sizeof(Face), _cube_13.faces, GL_STATIC_DRAW);


    print_model_info(_cube_13);

    glGenVertexArrays(1, &VAO[1]);
    glBindVertexArray(VAO[1]);

    glGenBuffers(1, &VBO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, _diamond_13.vertex_count * sizeof(Vertex), _diamond_13.vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &EBO[1]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _diamond_13.face_count * sizeof(Face), _diamond_13.faces, GL_STATIC_DRAW);


    print_model_info(_diamond_13);
}
