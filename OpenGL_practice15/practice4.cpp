#include<iostream>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
#include<random>

std::random_device rd;
std::mt19937 g(rd());


GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
void animation(int time);

GLclampf base_r = 0.12f;
GLclampf base_g = 0.12f;
GLclampf base_b = 0.12f;

typedef struct rects {
	GLclampf x1{};
	GLclampf y1{};
	GLclampf x2{};
	GLclampf y2{};

	GLclampf center[2]{};

	GLclampf r{};
	GLclampf g{};
	GLclampf b{};
};

rects rectangle_list[5];
int rect_index = 0;
int animation_flag = -1;

void reset_rect_position();


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
	glutTimerFunc(1, animation, 1);
	
	glutMainLoop();

}


GLvoid drawScene(GLvoid) {

	glClearColor(base_r, base_g, base_b, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	for (int i = 0; i < rect_index; ++i) {
		glColor3f(rectangle_list[i].r, rectangle_list[i].g, rectangle_list[i].b);
		glRectf(rectangle_list[i].x1, rectangle_list[i].y1, rectangle_list[i].x2, rectangle_list[i].y2);
	}
	glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);

}
GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (tolower(key)) {
	case '1':
		
		break;

	case '2':
		
		break;

	case '3':
		
		break;

	case '4':
		
		break;
	
	case 's':
		
		break;

	case 'm':
		reset_rect_position();
		break;

	case 'r':
		rect_index = 0;
		break;

	case 'q':
		glutLeaveMainLoop();
		break;
	}

	glutPostRedisplay();
}


GLvoid Mouse(int button, int state, int x, int y) {
	GLclampf mouse_x = (float)(x - (float)800 / 2.0) * (float)(1.0 / (float)(800 / 2.0));
	GLclampf mouse_y = -(float)(y - (float)600 / 2.0) * (float)(1.0 / (float)(600 / 2.0));
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && rect_index<5) {
		rectangle_list[rect_index] = {
			mouse_x - 0.05f,
			mouse_y + 0.05f,
			mouse_x + 0.05f,
			mouse_y - 0.05f,
			{mouse_x, mouse_y},
			(float(g() % 1000)) / 1000.0f,
			(float(g() % 1000)) / 1000.0f,
			(float(g() % 1000)) / 1000.0f
		};
		rect_index += 1;
	}
}


void animation(int time) {

}

void reset_rect_position() {
	for (int i = 0; i < rect_index; ++i) {
		rectangle_list[i].x1 = rectangle_list[i].center[0] - 0.05f;
		rectangle_list[i].y1 = rectangle_list[i].center[1] + 0.05f;
		rectangle_list[i].x2 = rectangle_list[i].center[0] + 0.05f;
		rectangle_list[i].y2 = rectangle_list[i].center[1] - 0.05f;
	}
}