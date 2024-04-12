#include "function.h"

GLuint titleimage;
GLuint titleimage2;
GLuint planett;
GLuint map[6];
GLuint map2;
GLuint obs1[6];
GLuint obs2[6];
GLuint obs3[4];
GLuint obs4;
GLuint obs5;

CModel planemodel;

bool start = false; // 게임 화면으로 이동할 때의 변수
bool main_option = false;

float speed = 0.2f;

int countdown = 4;
int counts = 0;
bool gameStarted = false; // 카운트 다운 후 게임 시작할 때의 변수
bool gameover = false;
bool gameclear = false;

GLfloat plane[3] = { 0, 0, 490 }; // 비행기의 위치
GLfloat planeRot[3] = { 0,0,0 };

GLfloat cam[3] = { plane[0], plane[1] + 2, plane[2] + 5 };
GLfloat cam2[3] = { plane[0], plane[1] ,plane[2] };
GLfloat up[3] = { 0, 1, 0 };

GLfloat rot = 0.0;

GLfloat moveX = 0.0f;
GLfloat dirX = 0.2f;
GLfloat moveY = 0.0f;
GLfloat dirY = 0.1f;

const int obsCount = 100; // 장애물 오브젝트 수
GLfloat obsPosition[obsCount][3]; // 장애물 위치 저장

const int pointCount = 100; // 포인트 오브젝트 수
GLfloat pointPosition[pointCount][3]; // 포인트 위치 저장
int point = 0;

void mainsound(const char* path, bool play);
void drawText(float x, float y, const char* text);
void drawPointText(float x, float y, const char* text);
void drawGameText(float x, float y, const char* text);
void draw_axis(void);
void loadtitleimage(const char* imagePath);
void loadtitleimage2(const char* imagePath);
void planeTexture(const char* planePath);
void mapTexture(const char* mappath);
void mapTexture2(const char* mappath);
void obstexture1(const char* obspath);
void obstexture2(const char* obspath);
void obstexture3(const char* obspath);
void obstexture4(const char* obspath);
void obstexture5(const char* obspath);
void draw_plane();
void drawcuboid();
void cubeObstacle1();
void cubeObstacle2();
void cubeObstacle3();
void generateObsPositions();
void drawObstacle();
bool checkCollision();
void generatePointPositions();
void drawPoints();
bool checkPointCollision();
void reset();
void init(void);
void idle(void);
void draw();
void resize(int width, int height);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void main_menu_function(int option);

int main(int argc, char** argv) {
	ifstream fin("obj/part1.obj");
	planemodel.loadObj(fin);
	fin.close();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(500, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("PLANE GAME");
	init();
	glutCreateMenu(main_menu_function);
	glutAddMenuEntry("Exit", 999);
	glutAddMenuEntry("Main", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	/* Callback 함수 정의 */
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(draw);
	glutReshapeFunc(resize);
	glutMouseFunc(mouse);
	loadtitleimage("images/title.png");
	loadtitleimage2("images/title2.png");
	planeTexture("textures/plane.jpg");
	mapTexture("textures/map1.jpg");
	mapTexture2("textures/map2.jpg");
	obstexture1("textures/obs1.jpg");
	obstexture2("textures/obs2.jpg");
	obstexture3("textures/tnt1.jpg");
	obstexture4("textures/tnt2.jpg");
	obstexture5("textures/tnt3.jpg");
	/* Looping 시작 */
	glutMainLoop();
	return 0;
}

void mainsound(const char* path, bool play) {
	static bool p = false;

	if (play) {
		if (!p) {
			PlaySoundA(path, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
			p = true;
		}
	}
	else {
		if (p) {
			PlaySoundA(NULL, NULL, 0);
			p = false;
		}
	}
}

void drawText(float x, float y, const char* text) { // 카운트다운
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(0.002f, 0.002f, 0.002f);
	glColor3f(1.0f, 1.0f, 1.0f);
	int length = (int)strlen(text);
	for (int i = 0; i < length; i++) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN, text[i]);
	}
	glPopMatrix();
}

void drawPointText(float x, float y, const char* text) { // 포인트
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(0.0005f, 0.0005f, 0.0005f);
	glColor3f(1.0f, 1.0f, 1.0f);
	int length = (int)strlen(text);
	for (int i = 0; i < length; i++) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN, text[i]);
	}
	glPopMatrix();
}

void drawGameText(float x, float y, const char* text) { // 게임오버, 게임 클리어
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(0.001f, 0.001f, 0.001f);
	glColor3f(1.0f, 1.0f, 1.0f);
	int length = (int)strlen(text);
	for (int i = 0; i < length; i++) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN, text[i]);
	}
	glPopMatrix();
}

void draw_axis(void)
{
	glLineWidth(3);
	glBegin(GL_LINES);

	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(10, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 10, 0);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 10);
	glEnd();
	glLineWidth(1);
}

void loadtitleimage(const char* imagePath) {
	int imgWidth, imgHeight, channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(imagePath, &imgWidth, &imgHeight, &channels, 0);
	glGenTextures(1, &titleimage);
	glBindTexture(GL_TEXTURE_2D, titleimage);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void loadtitleimage2(const char* imagePath2) {
	int imgWidth, imgHeight, channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data2 = stbi_load(imagePath2, &imgWidth, &imgHeight, &channels, 0);
	glGenTextures(1, &titleimage2);
	glBindTexture(GL_TEXTURE_2D, titleimage2);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void planeTexture(const char* planePath) {
	int imgWidth, imgHeight, channels;
	unsigned char* img = stbi_load(planePath, &imgWidth, &imgHeight, &channels, 0);
	glGenTextures(1, &planett);
	glBindTexture(GL_TEXTURE_2D, planett);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void mapTexture(const char* mappath) {
	glGenTextures(6, map);
	int imgWidth, imgHeight, channels;
	for (int i = 0; i < 6; i++) {
		glBindTexture(GL_TEXTURE_2D, map[i]);
		unsigned char* img = stbi_load(mappath, &imgWidth, &imgHeight, &channels, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
}

void mapTexture2(const char* mappath) {
	glGenTextures(1, &map2);
	int imgWidth, imgHeight, channels;
	glBindTexture(GL_TEXTURE_2D, map2);
	unsigned char* img2 = stbi_load(mappath, &imgWidth, &imgHeight, &channels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void obstexture1(const char* obspath) {
	glGenTextures(6, obs1);
	int imgWidth, imgHeight, channels;
	for (int i = 0; i < 6; i++) {
		glBindTexture(GL_TEXTURE_2D, obs1[i]);
		unsigned char*  imgobs1 = stbi_load(obspath, &imgWidth, &imgHeight, &channels, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imgobs1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
}

void obstexture2(const char* obspath) {
	glGenTextures(6, obs2);
	int imgWidth, imgHeight, channels;
	for (int i = 0; i < 6; i++) {
		glBindTexture(GL_TEXTURE_2D, obs2[i]);
		unsigned char* imgobs2 = stbi_load(obspath, &imgWidth, &imgHeight, &channels, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imgobs2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
}

void obstexture3(const char* obspath) {
	glGenTextures(4, obs3);
	int imgWidth, imgHeight, channels;
	for (int i = 0; i < 4; i++) {
		glBindTexture(GL_TEXTURE_2D, obs3[i]);
		unsigned char* imgobs3 = stbi_load(obspath, &imgWidth, &imgHeight, &channels, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imgobs3);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
}

void obstexture4(const char* obspath) {
	glGenTextures(1, &obs4);
	int imgWidth, imgHeight, channels;
	glBindTexture(GL_TEXTURE_2D, obs4);
	unsigned char* imgobs4 = stbi_load(obspath, &imgWidth, &imgHeight, &channels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imgobs4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

}

void obstexture5(const char* obspath) {
	glGenTextures(1, &obs5);
	int imgWidth, imgHeight, channels;
	glBindTexture(GL_TEXTURE_2D, obs5);
	unsigned char* imgobs5 = stbi_load(obspath, &imgWidth, &imgHeight, &channels, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imgobs5);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void draw_plane() {
	glColor3f(1.0, 1.0, 1.0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, planett);
	GLfloat x, y, z, nx, ny, nz;
	int v_id, vt_id, vn_id;
	for (int o = 0; o < planemodel.objs.size(); o++) {
		int nFaces = planemodel.objs[o].f.size();
		for (int k = 0; k < nFaces; k++) {
			int nPoints = planemodel.objs[o].f[k].v_pairs.size();
			glBegin(GL_POLYGON);
			for (int i = 0; i < nPoints; i++) {
				v_id = planemodel.objs[o].f[k].v_pairs[i].d[0];
				vt_id = planemodel.objs[o].f[k].v_pairs[i].d[1];
				vn_id = planemodel.objs[o].f[k].v_pairs[i].d[2];

				x = planemodel.objs[o].v[v_id - 1].d[0];
				y = planemodel.objs[o].v[v_id - 1].d[1];
				z = planemodel.objs[o].v[v_id - 1].d[2];

				if (vn_id > 0 && vn_id <= planemodel.objs[o].vn.size()) {
					nx = planemodel.objs[o].vn[vn_id - 1].d[0];
					ny = planemodel.objs[o].vn[vn_id - 1].d[1];
					nz = planemodel.objs[o].vn[vn_id - 1].d[2];
					glNormal3f(nx, ny, nz);
				}
				if (vt_id > 0 && vt_id <= planemodel.objs[o].vt.size()) {
					float u = planemodel.objs[o].vt[vt_id - 1].d[0];
					float v = planemodel.objs[o].vt[vt_id - 1].d[1];
					glTexCoord2f(u * 10, v * 10);
				}
				glVertex3f(x, y, z);
			}
			glEnd();
		}
	}
	glDisable(GL_TEXTURE_2D);
}

void drawcuboid() {
	float length_z = 500;
	float length_xy = 10;
	float re = 1.0f;
	glColor3f(1.0, 1.0, 1.0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, map[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-length_xy, -length_xy, length_z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(length_xy, -length_xy, length_z);
	glTexCoord2f(re, re);
	glVertex3f(length_xy, length_xy, length_z);
	glTexCoord2f(0.0f, re);
	glVertex3f(-length_xy, length_xy, length_z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, map2);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-length_xy, -length_xy, -length_z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(length_xy, -length_xy, -length_z);
	glTexCoord2f(re, re);
	glVertex3f(length_xy, length_xy, -length_z);
	glTexCoord2f(0.0f, re);
	glVertex3f(-length_xy, length_xy, -length_z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, map[2]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-length_xy, -length_xy, -length_z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(-length_xy, -length_xy, length_z);
	glTexCoord2f(re, re);
	glVertex3f(-length_xy, length_xy, length_z);
	glTexCoord2f(0.0f, re);
	glVertex3f(-length_xy, length_xy, -length_z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, map[3]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(length_xy, -length_xy, -length_z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(length_xy, -length_xy, length_z);
	glTexCoord2f(re, re);
	glVertex3f(length_xy, length_xy, length_z);
	glTexCoord2f(0.0f, re);
	glVertex3f(length_xy, length_xy, -length_z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, map[4]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-length_xy, length_xy, -length_z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(length_xy, length_xy, -length_z);
	glTexCoord2f(re, re);
	glVertex3f(length_xy, length_xy, length_z);
	glTexCoord2f(0.0f, re);
	glVertex3f(-length_xy, length_xy, length_z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, map[5]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-length_xy, -length_xy, -length_z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(length_xy, -length_xy, -length_z);
	glTexCoord2f(re, re);
	glVertex3f(length_xy, -length_xy, length_z);
	glTexCoord2f(0.0f, re);
	glVertex3f(-length_xy, -length_xy, length_z);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void cubeObstacle1() {
	GLfloat x = 10.0f;
	GLfloat y = 1.0f;
	GLfloat z = 0.5f;
	GLfloat re = 4.0f;
	GLfloat re2 = 1.0f;

	glColor3f(1.0, 1.0, 1.0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, obs1[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-x, -y, -z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(x, -y, -z);
	glTexCoord2f(re, re2);
	glVertex3f(x, y, -z);
	glTexCoord2f(0.0f, re2);
	glVertex3f(-x, y, -z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, obs1[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-x, -y, z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(x, -y, z);
	glTexCoord2f(re, re2);
	glVertex3f(x, y, z);
	glTexCoord2f(0.0f, re2);
	glVertex3f(-x, y, z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, obs1[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-x, -y, -z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(-x, y, -z);
	glTexCoord2f(re, re2);
	glVertex3f(-x, y, z);
	glTexCoord2f(0.0f, re2);
	glVertex3f(-x, -y, z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, obs1[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(x, -y, -z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(x, y, -z);
	glTexCoord2f(re, re2);
	glVertex3f(x, y, z);
	glTexCoord2f(0.0f, re2);
	glVertex3f(x, -y, z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, obs1[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBegin(GL_QUADS);
	glTexCoord2f(re, 0.0f);
	glVertex3f(-x, -y, -z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(x, -y, -z);
	glTexCoord2f(re, re2);
	glVertex3f(x, -y, z);
	glTexCoord2f(0.0f, re2);
	glVertex3f(-x, -y, z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, obs1[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBegin(GL_QUADS);
	glTexCoord2f(re, 0.0f);
	glVertex3f(-x, y, -z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(x, y, -z);
	glTexCoord2f(re, re2);
	glVertex3f(x, y, z);
	glTexCoord2f(0.0f, re);
	glVertex3f(-x, y, z);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void cubeObstacle2() {
	GLfloat x = 1.0f;
	GLfloat y = 10.0f;
	GLfloat z = 0.5f;
	GLfloat re = 1.0f;
	GLfloat re2 = 4.0f;

	glColor3f(1.0, 1.0, 1.0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, obs2[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-x, -y, -z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(x, -y, -z);
	glTexCoord2f(re, re2);
	glVertex3f(x, y, -z);
	glTexCoord2f(0.0f, re2);
	glVertex3f(-x, y, -z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, obs2[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-x, -y, z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(x, -y, z);
	glTexCoord2f(re, re2);
	glVertex3f(x, y, z);
	glTexCoord2f(re, re2);
	glVertex3f(-x, y, z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, obs2[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-x, -y, -z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(-x, y, -z);
	glTexCoord2f(re, re2);
	glVertex3f(-x, y, z);
	glTexCoord2f(0.0f, re2);
	glVertex3f(-x, -y, z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, obs2[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(x, -y, -z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(x, y, -z);
	glTexCoord2f(re, re2);
	glVertex3f(x, y, z);
	glTexCoord2f(0.0f, re2);
	glVertex3f(x, -y, z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, obs2[4]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-x, -y, -z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(x, -y, -z);
	glTexCoord2f(re, re2);
	glVertex3f(x, -y, z);
	glTexCoord2f(0.0f, re2);
	glVertex3f(-x, -y, z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, obs2[5]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-x, y, -z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(x, y, -z);
	glTexCoord2f(re, re2);
	glVertex3f(x, y, z);
	glTexCoord2f(re, re2);
	glVertex3f(-x, y, z);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void cubeObstacle3() {
	GLfloat x = 1.0f;
	GLfloat y = 1.0f;
	GLfloat z = 1.0f;
	GLfloat re = 1.0f;
	GLfloat re2 = 1.0f;

	glColor3f(1.0, 1.0, 1.0);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, obs3[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-x, -y, -z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(x, -y, -z);
	glTexCoord2f(re, re2);
	glVertex3f(x, y, -z);
	glTexCoord2f(0.0f, re2);
	glVertex3f(-x, y, -z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, obs3[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-x, -y, z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(x, -y, z);
	glTexCoord2f(re, re2);
	glVertex3f(x, y, z);
	glTexCoord2f(0.0f, re2);
	glVertex3f(-x, y, z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, obs3[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-x, -y, -z);
	glTexCoord2f(0.0f, re);
	glVertex3f(-x, y, -z);
	glTexCoord2f(re, re2);
	glVertex3f(-x, y, z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(-x, -y, z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, obs3[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(x, -y, -z);
	glTexCoord2f(0.0f, re2);
	glVertex3f(x, y, -z);
	glTexCoord2f(re, re2);
	glVertex3f(x, y, z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(x, -y, z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, obs4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-x, -y, -z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(x, -y, -z);
	glTexCoord2f(re, re2);
	glVertex3f(x, -y, z);
	glTexCoord2f(0.0f, re2);
	glVertex3f(-x, -y, z);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, obs5);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-x, y, -z);
	glTexCoord2f(re, 0.0f);
	glVertex3f(x, y, -z);
	glTexCoord2f(re, re2);
	glVertex3f(x, y, z);
	glTexCoord2f(0.0f, re2);
	glVertex3f(-x, y, z);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void generateObsPositions() {
	for (int i = 0; i < obsCount; i++) {
		obsPosition[i][0] = (rand() % 17) - 8;
		obsPosition[i][1] = (rand() % 17) - 8;
		obsPosition[i][2] = (rand() % 961) - 480;
	}
}

void drawObstacle() {
	for (int i = -460; i <= 480; i = i + 150) {
		glPushMatrix();
		glTranslatef(0.0, moveY, i);
		cubeObstacle1();
		glPopMatrix();
	}
	for (int i = -400; i <= 480; i = i + 150) {
		glPushMatrix();
		glTranslatef(moveX, 0.0, i);
		cubeObstacle2();
		glPopMatrix();
	}

	for (int i = 0; i < obsCount; i++) {
		glPushMatrix();
		glTranslatef(obsPosition[i][0], obsPosition[i][1], obsPosition[i][2]);
		glRotatef(rot, 0.0, 0.0, 1.0);
		cubeObstacle3();
		glPopMatrix();
	}

	moveY += dirY;
	moveX += dirX;
	rot += 1.0f;

	if (moveY > 9.0f || moveY < -9.0f)
		dirY = -dirY;
	if (moveX > 9.0f || moveX < -9.0f)
		dirX = -dirX;
}

bool checkCollision() {
	for (int i = -460; i <= 480; i = i + 150) {
		GLfloat obsX = 0.0f, obsY = moveY, obsZ = i;
		GLfloat obsWidth = 10.0f, obsHeight = 1.0f, obsDepth = 0.5f;

		if (abs(plane[0] - obsX) * 2 < (1 + obsWidth) &&
			abs(plane[1] - obsY) * 2 < (1 + obsHeight) &&
			abs(plane[2] - obsZ) * 2 < (1 + obsDepth)) {
			gameStarted = false;
			PlaySoundA("sounds/game over.wav", NULL, SND_ASYNC | SND_ALIAS);
			return true;
		}
	}

	for (int i = -400; i <= 480; i = i + 150) {
		GLfloat obsX = moveX, obsY = 0.0f, obsZ = i;
		GLfloat obsWidth = 1.0f, obsHeight = 10.0f, obsDepth = 0.5f;

		if (abs(plane[0] - obsX) * 2 < (1 + obsWidth) &&
			abs(plane[1] - obsY) * 2 < (1+ obsHeight) &&
			abs(plane[2] - obsZ) * 2 < (1 + obsDepth)) {
			gameStarted = false;
			PlaySoundA("sounds/game over.wav", NULL, SND_ASYNC | SND_ALIAS);
			return true;
		}
	}
	for (int i = 0; i < obsCount; i++) {
		GLfloat obsX = obsPosition[i][0], obsY = obsPosition[i][1], obsZ = obsPosition[i][2];
		GLfloat obsWidth = 1.0f, obsHeight = 1.0f, obsDepth = 1.0f;

		if (abs(plane[0] - obsX) * 2 < (1 + obsWidth) &&
			abs(plane[1] - obsY) * 2 < (1 + obsHeight) &&
			abs(plane[2] - obsZ) * 2 < (1 + obsDepth)) {
			gameStarted = false;
			PlaySoundA("sounds/game over.wav", NULL, SND_ASYNC | SND_ALIAS);
			return true;
		}
	}
	return false;
}

void generatePointPositions() {
	for (int i = 0; i < pointCount; i++) {
		pointPosition[i][0] = (rand() % 17) - 7;
		pointPosition[i][1] = (rand() % 17) - 7;
		pointPosition[i][2] = (rand() % 961) - 480;
	}
}

void drawPoints() {
	for (int i = 0; i < pointCount; i++) {
		glPushMatrix();
		glTranslatef(pointPosition[i][0], pointPosition[i][1], pointPosition[i][2]);

		glColor3f(0.0f, 0.0f, 1.0f);
		glutSolidSphere(0.5f, 30, 30);

		glPopMatrix();
	}
}

bool checkPointCollision() {
	for (int i = 0; i < pointCount; i++) {
		float x = plane[0] - pointPosition[i][0];
		float y = plane[1] - pointPosition[i][1];
		float z = plane[2] - pointPosition[i][2];

		float dist = sqrt(x * x + y * y + z * z);
		if (dist <= 1.0f) {
			pointPosition[i][1] = -100.0f;
			PlaySoundA("sounds/point.wav", NULL, SND_ASYNC | SND_ALIAS);
			return true;
		}
	}
	return false;
}

void reset() {
	start = false;
	gameStarted = false;
	gameover = false;
	gameclear = false;

	point = 0;
	countdown = 4;
	plane[0] = 0;
	plane[1] = 0;
	plane[2] = 490;
	planeRot[0] = 0;
	planeRot[1] = 0;
	planeRot[2] = 0;
	cam[0] = plane[0];
	cam[1] = plane[1] + 2;
	cam[2] = plane[2] + 5;

	generateObsPositions();
	generatePointPositions();
	mainsound("sounds/game clear.wav", false);
}

void init(void) {
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	GLfloat ambientLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat specularLight[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	GLfloat ambientMaterial[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseMaterial[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat specularMaterial[] = { 0.9f, 0.9f, 0.9f, 1.0f };

	glMaterialfv(GL_FRONT, GL_AMBIENT, ambientMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specularMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, 60);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	generateObsPositions();
	generatePointPositions();
	printf("-----Game Guide-----\n");
	printf("Using the Game Start Button, you can start the game.\n");
	printf("Using the Option Button, you can change the speed of the plane.\n");
	printf("Using the Exit Button, you can exit from the game.\n\n");
	printf("[ In - Game ]\n");
	printf("W button > You can control the plane to the up.\n");
	printf("S button > You can control the plane to the down.\n");
	printf("A button > You can control the plane to the left.\n");
	printf("D button > You can control the plane to the right.\n\n");
	printf("* Goal of the Game *\n");
	printf("You must get 5 points and reach the Goal.\n");
	printf("If you reach the Goal without 5 points, Game Over\n\n");
	printf("Blue Obstacle - Point\n");
	printf("TNT, The moving pillar - Bomb (If you collide, Game Over)\n");
}

void idle(void) {
	if (!gameStarted && start) {
		if (countdown > 0) {
			int current = glutGet(GLUT_ELAPSED_TIME);
			if (current - counts >= 1000) {
				char countdownText[10];
				sprintf(countdownText, "%d", countdown);
				drawText(0.0f, 0.0f, countdownText);
				countdown--;
				counts = current;
			}
		}
		else {
			if (!gameover) {
				gameStarted = true;
			}
		}
	}
	else if (gameStarted && !gameover) {
		plane[2] -= speed;
		if (checkCollision()) {
			gameover = true;
		}
		if (checkPointCollision()) {
			point += 1;
		}
		if (plane[2] <= -490) {
			if (point > 4) {
				gameclear = true;
				mainsound("sounds/game clear.wav", true);
			}
			else {
				gameover = true;
				PlaySoundA("sounds/game over1.wav", NULL, SND_ASYNC | SND_ALIAS);
			}
			gameStarted = false;
		}
	}
	glutPostRedisplay();
}

void draw() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 800.0 / 800.0, 1, 500);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	cam[0] = plane[0];
	cam[1] = plane[1] + 2;
	cam[2] = plane[2] + 5;

	gluLookAt(cam[0], cam[1], cam[2], plane[0], plane[1], plane[2], up[0], up[1], up[2]);

	drawcuboid();
	drawObstacle();
	drawPoints();

	glPushMatrix();
	glTranslatef(plane[0], plane[1], plane[2]);
	glRotatef(180.0f, planeRot[0], planeRot[1] + 5.0f, planeRot[2]);
	draw_plane();
	//glDisable(GL_LIGHTING);
	//draw_axis();
	//glEnable(GL_LIGHTING);
	glPopMatrix();

	if (!start) {
		if (!main_option) {
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, titleimage);

			glColor3f(1.0f, 1.0f, 1.0f);
			glDisable(GL_LIGHTING);
			glDisable(GL_DEPTH_TEST);

			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			gluOrtho2D(-1, 1, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0); glVertex2f(-1.0, -1.0);
			glTexCoord2f(1.0, 0.0); glVertex2f(1.0, -1.0);
			glTexCoord2f(1.0, 1.0); glVertex2f(1.0, 1.0);
			glTexCoord2f(0.0, 1.0); glVertex2f(-1.0, 1.0);
			glEnd();

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
		}
		else {
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, titleimage2);

			glColor3f(1.0f, 1.0f, 1.0f);
			glDisable(GL_LIGHTING);
			glDisable(GL_DEPTH_TEST);

			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			gluOrtho2D(-1, 1, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0); glVertex2f(-1.0, -1.0);
			glTexCoord2f(1.0, 0.0); glVertex2f(1.0, -1.0);
			glTexCoord2f(1.0, 1.0); glVertex2f(1.0, 1.0);
			glTexCoord2f(0.0, 1.0); glVertex2f(-1.0, 1.0);
			glEnd();

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
		}
	}
	else {
		if (!gameStarted && countdown > 0) {
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			gluOrtho2D(-1, 1, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			glDisable(GL_LIGHTING);


			char countdownText[10];
			sprintf(countdownText, "%d", countdown);
			drawText(-0.05f, 0.0f, countdownText);

			glEnable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
		}
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(-1, 1, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_LIGHTING);

		char pointText[50];
		sprintf(pointText, "%d", point);
		drawPointText(-0.95f, 0.9f, pointText);

		glEnable(GL_LIGHTING);

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		if (plane[2] <= -490.0f) {
			if (gameclear) {
				gameStarted = false;
				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glLoadIdentity();
				gluOrtho2D(-1, 1, -1, 1);
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();

				glDisable(GL_LIGHTING);

				char gameovertext[11] = "GAME CLEAR";
				drawGameText(-0.4f, 0.0f, gameovertext);

				glEnable(GL_LIGHTING);

				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
			}
			else {
				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glLoadIdentity();
				gluOrtho2D(-1, 1, -1, 1);
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();

				glDisable(GL_LIGHTING);

				char gameovertext[10] = "GAME OVER";
				drawGameText(-0.4f, 0.0f, gameovertext);

				glEnable(GL_LIGHTING);

				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
			}
		}
		if (gameover) {
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			gluOrtho2D(-1, 1, -1, 1);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			glDisable(GL_LIGHTING);

			char gameovertext[10] = "GAME OVER";
			drawGameText(-0.4f, 0.0f, gameovertext);

			glEnable(GL_LIGHTING);

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
		}
	}
	glutSwapBuffers();
}

void resize(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (!start) {
		gluOrtho2D(-1, 1, -1, 1);
	}
	else {
		gluPerspective(45, (float)width / (float)height, 1, 500);
	}

	glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
	if (gameStarted && !gameover && !gameclear) {
		switch (key) {
		case 'w':
			if (plane[1] + 1.5f <= 8)
				plane[1] += 0.5f;
			planeRot[0] = 0.0f;
			break;
		case 's':
			if (plane[1] - 1.5f >= -8)
				plane[1] -= 0.5f;
			planeRot[0] = 0.0f;
			break;
		case 'a':
			if (plane[0] - 1.5f >= -8)
				plane[0] -= 0.5f;
			planeRot[0] = -1.0f;
			break;
		case 'd':
			if (plane[0] + 1.5f <= 8)
				plane[0] += 0.5f;
			planeRot[0] = 1.0f;
			break;
		}
	}
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		float mainX = (float)x / glutGet(GLUT_WINDOW_WIDTH) * 2 - 1;
		float mainY = 1 - (float)y / glutGet(GLUT_WINDOW_HEIGHT) * 2;

		if (!start) {
			if (!main_option) {
				if (mainX >= 0.7 && mainX <= 1.0 && mainY <= -0.4 && mainY >= -0.6) {
					printf("Game started!\n");
					PlaySoundA("sounds/click.wav", NULL, SND_ASYNC | SND_ALIAS);
					start = true;
					glutReshapeWindow(800, 800);
					glutPositionWindow(500, 100);
				}
				if (mainX >= 0.7 && mainX <= 1.0 && mainY <= -0.6 && mainY >= -0.8) {
					printf("Option\n");
					PlaySoundA("sounds/click.wav", NULL, SND_ASYNC | SND_ALIAS);
					main_option = true;
				}
				if (mainX >= 0.7 && mainX <= 1.0 && mainY <= -0.8 && mainY >= -1.0) {
					printf("Exit\n");
					glutLeaveMainLoop();
				}
			}
			else {
				if (mainX >= 0.7 && mainX <= 1.0 && mainY <= 0.5 && mainY >= 0.4) {
					printf("level 3\n");
					PlaySoundA("sounds/click.wav", NULL, SND_ASYNC | SND_ALIAS);
					speed = 0.6f;
					main_option = false;
				}
				if (mainX >= 0.7 && mainX <= 1.0 && mainY <= 0.15 && mainY >= 0.05) {
					printf("level 2\n");
					PlaySoundA("sounds/click.wav", NULL, SND_ASYNC | SND_ALIAS);
					speed = 0.4f;
					main_option = false;
				}
				if (mainX >= 0.7 && mainX <= 1.0 && mainY <= -0.2 && mainY >= -0.3) {
					printf("level 1\n");
					PlaySoundA("sounds/click.wav", NULL, SND_ASYNC | SND_ALIAS);
					speed = 0.2f;
					main_option = false;
				}
				if (mainX >= 0.7 && mainX <= 1.0 && mainY <= -0.8 && mainY >= -1.0) {
					printf("Back to title\n");
					PlaySoundA("sounds/click.wav", NULL, SND_ASYNC | SND_ALIAS);
					main_option = false;
				}
			}
		}
	}
}

void main_menu_function(int option)
{
	if (option == 999) {
		exit(0);
	}
	else if (option == 1) {
		reset();
		PlaySoundA("sounds/click.wav", NULL, SND_ASYNC | SND_ALIAS);
		main_option = false;
		printf("Move to Title\n");
	}
}