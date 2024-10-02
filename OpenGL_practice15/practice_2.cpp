#include<iostream>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
#include<random>
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);

GLclampf base_r = 0.5f;
GLclampf base_g = 0.5f;
GLclampf base_b = 0.5f;


GLclampf rect1_r = 1.0f;
GLclampf rect1_g = 1.0f;
GLclampf rect1_b = 0.0f;

GLclampf rect1_x1 = -1.0f;
GLclampf rect1_y1 = 1.0f;
GLclampf rect1_x2 = 0.0f;
GLclampf rect1_y2 = 0.0f;


GLclampf rect2_r = 0.0f;
GLclampf rect2_g = 1.0f;
GLclampf rect2_b = 0.0f;

GLclampf rect2_x1 = 0.0f;
GLclampf rect2_y1 = 1.0f;
GLclampf rect2_x2 = 1.0f;
GLclampf rect2_y2 = 0.0f;


GLclampf rect3_r = 1.0f;
GLclampf rect3_g = 0.0f;
GLclampf rect3_b = 1.0f;

GLclampf rect3_x1 = -1.0f;
GLclampf rect3_y1 = 0.0f;
GLclampf rect3_x2 = 0.0f;
GLclampf rect3_y2 = -1.0f;


GLclampf rect4_r = 0.0f;
GLclampf rect4_g = 1.0f;
GLclampf rect4_b = 1.0f;

GLclampf rect4_x1 = 0.0f;
GLclampf rect4_y1 = 0.0f;
GLclampf rect4_x2 = 1.0f;
GLclampf rect4_y2 = -1.0f;


int click_area(GLclampf x, GLclampf y);


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
	glClearColor(base_r, base_g, base_b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glColor3f(rect1_r, rect1_g, rect1_b);
	glRectf(rect1_x1, rect1_y1, rect1_x2, rect1_y2);
	
	glColor3f(rect2_r, rect2_g, rect2_b);
	glRectf(rect2_x1, rect2_y1, rect2_x2, rect2_y2);

	glColor3f(rect3_r, rect3_g, rect3_b);
	glRectf(rect3_x1, rect3_y1, rect3_x2, rect3_y2);

	glColor3f(rect4_r, rect4_g, rect4_b);
	glRectf(rect4_x1, rect4_y1, rect4_x2, rect4_y2);

	glutSwapBuffers();
}
GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);

}

GLvoid Keyboard(unsigned char key, int x, int y) {
	if (key == 'q') {
		glutLeaveMainLoop();
	}
	glutPostRedisplay();
}
GLvoid Mouse(int button, int state, int x, int y) {
	GLclampf mouse_x = 0.0f;
	GLclampf mouse_y = 0.0f;
	mouse_x = (float)(x - (float)800 / 2.0) * (float)(1.0 / (float)(800 / 2.0));
	mouse_y = -(float)(y - (float)600 / 2.0)* (float)(1.0 / (float)(600 / 2.0));
	std::random_device rd;
	std::mt19937 g(rd());
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		std::cout << mouse_x << mouse_y << std::endl;
		switch (click_area(mouse_x, mouse_y)) {
		case 1:
			
			rect1_r = float((g() & 1000)) / 1000.0f;
			rect1_g = float((g() & 1000)) / 1000.0f;
			rect1_b = float((g() & 1000)) / 1000.0f;
			std::cout << "1\n";
			break;

		case 2:
			rect2_r = float((g() & 1000)) / 1000.0f;
			rect2_g = float((g() & 1000)) / 1000.0f;
			rect2_b = float((g() & 1000)) / 1000.0f;
			std::cout << "2\n";
			break;


		case 3:
			rect3_r = float((g() & 1000)) / 1000.0f;
			rect3_g = float((g() & 1000)) / 1000.0f;
			rect3_b = float((g() & 1000)) / 1000.0f;
			std::cout << "3\n";
			break;


		case 4:
			rect4_r = float((g() & 1000)) / 1000.0f;
			rect4_g = float((g() & 1000)) / 1000.0f;
			rect4_b = float((g() & 1000)) / 1000.0f;
			std::cout << "4\n";
			break;


		default:
			base_r = float((g() & 1000)) / 1000.0f;
			base_g = float((g() & 1000)) / 1000.0f;
			base_b = float((g() & 1000)) / 1000.0f;
			std::cout << "0\n";
			break;

		}
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		switch (click_area(mouse_x, mouse_y)) {
		case 1:
			if (rect1_x2 - rect1_x1 > 0.2 && rect1_y1 - rect1_y2 > 0.2) {
				rect1_x1 += 0.05;
				rect1_y1 -= 0.05;
				rect1_x2 -= 0.05;
				rect1_y2 += 0.05;
			}
			break;

		case 2:
			if (rect2_x2 - rect2_x1 > 0.2 && rect2_y1 - rect2_y2 > 0.2) {
				rect2_x1 += 0.05;
				rect2_y1 -= 0.05;
				rect2_x2 -= 0.05;
				rect2_y2 += 0.05;
			}
			break;


		case 3:
			if (rect3_x2 - rect3_x1 > 0.2 && rect3_y1 - rect3_y2 > 0.2) {
				rect3_x1 += 0.05;
				rect3_y1 -= 0.05;
				rect3_x2 -= 0.05;
				rect3_y2 += 0.05;
			}
			break;


		case 4:
			if (rect4_x2 - rect4_x1 > 0.2 && rect4_y1 - rect4_y2 > 0.2) {
				rect4_x1 += 0.05;
				rect4_y1 -= 0.05;
				rect4_x2 -= 0.05;
				rect4_y2 += 0.05;
			}
			break;
			
		case 10:
			rect1_x1 -= 0.05;
			rect1_y1 += 0.05;
			rect1_x2 += 0.05;
			rect1_y2 -= 0.05;
			break;

		case 20:
			rect2_x1 -= 0.05;
			rect2_y1 += 0.05;
			rect2_x2 += 0.05;
			rect2_y2 -= 0.05;
			break;

		case 30:
			rect3_x1 -= 0.05;
			rect3_y1 += 0.05;
			rect3_x2 += 0.05;
			rect3_y2 -= 0.05;
			break;

		case 40:
			rect4_x1 -= 0.05;
			rect4_y1 += 0.05;
			rect4_x2 += 0.05;
			rect4_y2 -= 0.05;
			break;

		}
	}
	glutPostRedisplay();
}

int click_area(GLclampf x, GLclampf y) {
	if (rect1_x1<x && rect1_y1>y && rect1_x2 > x && rect1_y2 < y) {

		return 1;
	}else if (rect2_x1<x && rect2_y1>y && rect2_x2 > x && rect2_y2 < y) {

		return 2;
	}
	else if(rect3_x1<x && rect3_y1>y && rect3_x2 > x && rect3_y2 < y) {

		return 3;
	}
	else if(rect4_x1<x&& rect4_y1>y&& rect4_x2 > x&& rect4_y2 < y) {
	
		return 4;
	}
	else if (-1<x && x<0 && 0<y && y<1) {
		return 10;
	}
	else if (0 < x && x < 1 && 0 < y && y < 1) {
		return 20;
	}
	else if (-1 < x && x < 0 && -1 < y && y < 0) {
		return 30;
	}
	else if (0 < x && x < 1 && -1 < y && y < 0) {
		return 40;
	}
	return 0;
}