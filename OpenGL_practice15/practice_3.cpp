#include<iostream>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
#include<random>
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);


std::random_device rd;
std::mt19937 g(rd());


GLclampf base_r = 1.0f;
GLclampf base_g = 1.0f;
GLclampf base_b = 1.0f;

void generate_rect();

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

random_rect rectangle_list[10];



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
	glutMouseFunc(Mouse);
	glutMainLoop();

}


GLvoid drawScene(GLvoid) {

	glClearColor(base_r, base_g, base_b, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	for (random_rect temp : rectangle_list) {
		if (temp.is_creat) {
			glColor3f(temp.r, temp.g, temp.b);
			glRectf(temp.x1, temp.y1, temp.x2, temp.y2);

		}
	}


	glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);

}
GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'a':
		generate_rect();
		
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}

	glutPostRedisplay();
}



void generate_rect() {
	for (random_rect& temp : rectangle_list) {
		if (g() % 10 > 3) {
			temp.is_creat = true;
			temp.x1 = ((float)(g() % 2000) - 1000.0) / 1000.0;
			temp.y1 = ((float)(g() % 2000) - 1000.0) / 1000.0;
			temp.x2 = temp.x1 + 0.3;
			temp.y2 = temp.y1 - 0.3;
			temp.r = (float(g() % 1000)) / 1000.0;
			temp.g = (float(g() % 1000)) / 1000.0;
			temp.b = (float(g() % 1000)) / 1000.0;
		}

	}
}

GLvoid Mouse(int button, int state, int x, int y) {
	GLclampf mouse_x = 0.0f;
	GLclampf mouse_y = 0.0f;
	mouse_x = (float)(x - (float)800 / 2.0) * (float)(1.0 / (float)(800 / 2.0));
	mouse_y = -(float)(y - (float)600 / 2.0) * (float)(1.0 / (float)(600 / 2.0));
	
}
