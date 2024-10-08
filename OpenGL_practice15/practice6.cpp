#include<iostream>
#include<gl/glew.h>
#include<gl/freeglut.h>
#include<gl/freeglut_ext.h>
#include<random>


GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
void animation(int value);

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
	int motion_flag{ -1 };
};

typedef struct devided {
	GLclampf rect1_xy[2];
	GLclampf rect2_xy[2];
	GLclampf rect3_xy[2];
	GLclampf rect4_xy[2];
	GLclampf width = 0.1;
	
	int what_ani = -1;
	int index{};
};

std::random_device rd;
std::mt19937 g(rd());

random_rect rectangle_list[10];
devided devide_rect[20];

void reset();
bool check_rect(int, GLclampf, GLclampf);

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
	glutTimerFunc(50, animation, 1);
	reset();
	glutMainLoop();

}


GLvoid drawScene(GLvoid) {

	glClearColor(base_r, base_g, base_b, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	for (random_rect temp : rectangle_list) {
		if(temp.is_creat){
			glColor3f(temp.r, temp.g, temp.b);
			glRectf(temp.x1, temp.y1, temp.x2, temp.y2);
		}
	}
	for (int i = 0; i < 20;i++) {
		if(devide_rect[i].what_ani != -1) {
			if(i<10){
				glColor3f(rectangle_list[i].r, rectangle_list[i].g, rectangle_list[i].b);
			}
			else {
				glColor3f(rectangle_list[i-10].r, rectangle_list[i-10].g, rectangle_list[i-10].b);
			}
			glRectf(devide_rect[i].rect1_xy[0], devide_rect[i].rect1_xy[1], devide_rect[i].rect1_xy[0] + devide_rect[i].width, devide_rect[i].rect1_xy[1] - devide_rect[i].width);
			glRectf(devide_rect[i].rect2_xy[0], devide_rect[i].rect2_xy[1], devide_rect[i].rect2_xy[0] + devide_rect[i].width, devide_rect[i].rect2_xy[1] - devide_rect[i].width);
			glRectf(devide_rect[i].rect3_xy[0], devide_rect[i].rect3_xy[1], devide_rect[i].rect3_xy[0] + devide_rect[i].width, devide_rect[i].rect3_xy[1] - devide_rect[i].width);
			glRectf(devide_rect[i].rect4_xy[0], devide_rect[i].rect4_xy[1], devide_rect[i].rect4_xy[0] + devide_rect[i].width, devide_rect[i].rect4_xy[1] - devide_rect[i].width);

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
		reset();
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}

	glutPostRedisplay();
}

void reset() {
	for (int i = 0; i < 10; ++i) {
		GLclampf temp_x = float(g() % 2000) / 1000.0f - 1;
		GLclampf temp_y = float(g() % 2000) / 1000.0f - 1;
		if (i < 5) {
		
			rectangle_list[i] = { true,temp_x,temp_y, temp_x+0.2f, temp_y-0.2f, float(g() % 1000) / 1000.0f, float(g() % 1000) / 1000.0f , float(g() % 1000) / 1000.0f };

		}
		else {
			if (g() % 2 == 0) {
				rectangle_list[i] = { true,temp_x,temp_y, temp_x + 0.2f, temp_y - 0.2f, float(g() % 1000) / 1000.0f, float(g() % 1000) / 1000.0f , float(g() % 1000) / 1000.0f };
			}
		}
	}
}
int dir;
GLvoid Mouse(int button, int state, int x, int y) {
	GLclampf mouse_x = (float)(x - (float)800 / 2.0) * (float)(1.0 / (float)(800 / 2.0));
	GLclampf mouse_y = -(float)(y - (float)600 / 2.0) * (float)(1.0 / (float)(600 / 2.0));
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		for (int i = 0; i < 10; i++) {
			if (rectangle_list[i].is_creat && check_rect(i, mouse_x, mouse_y)) {
				devide_rect[i] = {
					{rectangle_list[i].x1, rectangle_list[i].y1},
					{rectangle_list[i].x1 + 0.1f, rectangle_list[i].y1},
					{rectangle_list[i].x1, rectangle_list[i].y1 - 0.1f},
					{rectangle_list[i].x1 + 0.1f, rectangle_list[i].y1 - 0.1f},
					0.1f,
					int(g() % 4) + 1
					, i
				};
				devide_rect[i + 10] = devide_rect[i];
				std::cout << devide_rect[i].what_ani << std::endl;
				rectangle_list[i].is_creat = false;
				dir = int(g() % 4);
			}
		}

	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
	
	}
	glutPostRedisplay();
}

bool check_rect(int index, GLclampf mouse_x, GLclampf mouse_y) {
	if (rectangle_list[index].x1 < mouse_x && mouse_x < rectangle_list[index].x2
		&& rectangle_list[index].y2 < mouse_y && mouse_y < rectangle_list[index].y1){
		std::cout << index << std::endl;
		return true;
	}
	return false;
}

void animation(int value) {
	int i;
	for (devided &temp : devide_rect) {
		switch (temp.what_ani){
		case 1:
			temp.rect1_xy[1] += 0.1f;
			temp.rect2_xy[0] += 0.1f;
			temp.rect3_xy[0] -= 0.1f;
			temp.rect4_xy[1] -= 0.1f;
			if(temp.width>0.01f){
				temp.width -= 0.01f;
			}
			else {
				temp.what_ani = -1;
			}
			break;

		case 2:
			temp.rect1_xy[1] += 0.1f;
			temp.rect1_xy[0] += 0.1f;
			temp.rect2_xy[1] += 0.1f;
			temp.rect2_xy[0] -= 0.1f;
			temp.rect3_xy[1] -= 0.1f;
			temp.rect3_xy[0] -= 0.1f;
			temp.rect4_xy[1] -= 0.1f;
			temp.rect4_xy[0] += 0.1f;
			if (temp.width > 0.01f) {
				temp.width -= 0.01f;
			}
			else {
				temp.what_ani = -1;
			}
			break;


		case 3:
		
			if(dir == 0){
				temp.rect1_xy[1] += 0.1f;
				temp.rect2_xy[1] += 0.1f;
				temp.rect3_xy[1] += 0.1f;
				temp.rect4_xy[1] += 0.1f;
				if (temp.width > 0.01f) {
					temp.width -= 0.01f;
				}
				else {
					temp.what_ani = -1;
				}
			}
			else if (dir == 1) {
				temp.rect1_xy[0] += 0.1f;
				temp.rect2_xy[0] += 0.1f;
				temp.rect3_xy[0] += 0.1f;
				temp.rect4_xy[0] += 0.1f;
				if (temp.width > 0.01f) {
					temp.width -= 0.01f;
				}
				else {
					temp.what_ani = -1;
				}
			}
			else if (dir == 2) {
				temp.rect1_xy[1] -= 0.1f;
				temp.rect2_xy[1] -= 0.1f;
				temp.rect3_xy[1] -= 0.1f;
				temp.rect4_xy[1] -= 0.1f;
				if (temp.width > 0.01f) {
					temp.width -= 0.01f;
				}
				else {
					temp.what_ani = -1;
				}
			}
			else {
				temp.rect1_xy[0] -= 0.1f;
				temp.rect2_xy[0] -= 0.1f;
				temp.rect3_xy[0] -= 0.1f;
				temp.rect4_xy[0] -= 0.1f;
				if (temp.width > 0.01f) {
					temp.width -= 0.01f;
				}
				else {
					temp.what_ani = -1;
				}
			}
			break;


		case 4:
			i = temp.index;
			temp.rect1_xy[1] += 0.1f;
			temp.rect2_xy[0] += 0.1f;
			temp.rect3_xy[0] -= 0.1f;
			temp.rect4_xy[1] -= 0.1f;
			devide_rect[i + 10].what_ani = 4;
			devide_rect[i + 10].rect1_xy[1] += 0.1f;
			devide_rect[i + 10].rect1_xy[0] += 0.1f;
			devide_rect[i + 10].rect2_xy[1] += 0.1f;
			devide_rect[i + 10].rect2_xy[0] -= 0.1f;
			devide_rect[i + 10].rect3_xy[1] -= 0.1f;
			devide_rect[i + 10].rect3_xy[0] -= 0.1f;
			devide_rect[i + 10].rect4_xy[1] -= 0.1f;
			devide_rect[i + 10].rect4_xy[0] += 0.1f;
			
			if (temp.width > 0.01f) {
				temp.width -= 0.01f;
			}
			else {
				temp.what_ani = -1;
			}
			
			if (devide_rect[i + 10].width > 0.01f) {
				devide_rect[i + 10].width -= 0.01f;
			}
			else {
				devide_rect[i + 10].what_ani = -1;
			}
			break;



		default:
			break;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(50, animation, 1);
}