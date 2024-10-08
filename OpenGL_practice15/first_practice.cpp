#include<iostream>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
#include<random>
#include<time.h>
GLvoid drawScene(GLvoid); 
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);


GLclampf r = 1.0f;
GLclampf g = 1.0f;
GLclampf b = 1.0f;

std::random_device rd;
std::mt19937 g_r(rd());

bool timer_flag = false;

void timer(int value) {
	if (timer_flag) {
		r = float(g_r() & 1000) / 1000.0f;
		g = float(g_r() & 1000) / 1000.0f;
		b = float(g_r() & 1000) / 1000.0f;

	}
	glutTimerFunc(1000, timer, 0);
	glutPostRedisplay();
}	
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
	glutTimerFunc(1000, timer, 0);
	glutMainLoop();

}


GLvoid drawScene(GLvoid) {
	
	glClearColor(r, g, b, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);

}
GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'c':
		r = 0.0f;
		g = 1.0f;
		b = 1.0f;
		break;

	case 'm':
		r = 1.0f;
		g = 0.0f;
		b = 1.0f;
		break;

	case 'y':
		r = 1.0f;
		g = 1.0f;
		b = 0.0f;
		break;

	case 'a':
		r = float(g_r() & 1000) / 1000.0f;
		g = float(g_r() & 1000) / 1000.0f;
		b = float(g_r() & 1000) / 1000.0f;
		break;

	case 'w':
		r = 1.0f;
		g = 1.0f;
		b = 1.0f;
		break;

	case 'k':
		r = 0.0f;
		g = 0.0f;
		b = 0.0f;
		break;

	case 't':
		timer_flag = true;
		break;

	case 's':
		timer_flag = false;
		break;

	case 'q':
		glutLeaveMainLoop();
		break;
	}
	
	glutPostRedisplay();
}