#include<iostream>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
#include<random>

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);


GLclampf base_r = 1.0f;
GLclampf base_g = 1.0f;
GLclampf base_b = 1.0f;

typedef struct random_rect {
	bool is_creat = false;
	GLclampf x1{};
	GLclampf y1{};
	GLclampf x2{};
	GLclampf y2{};
	GLclampf r{};
	GLclampf g{};
	GLclampf b{};
};

random_rect rectangle_list[40];




void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Example1");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "GLEW initialized\n";
	}
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	
	glutMainLoop();

}


GLvoid drawScene(GLvoid) {

	glClearColor(base_r, base_g, base_b, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

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
	}

	glutPostRedisplay();
}