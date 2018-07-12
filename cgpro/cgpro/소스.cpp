//***************S e j o n g U n i v e r s i t y**************************//
//********************D i g i t a l C o n t e n t s***********************//
//************************ 1 3 0 1 1 1 8 6 *******************************//
//******************P R O V I D E D   B Y  L e e D o n g h o o n *********//
//************************* 2 0 1 8 / 0 6 / 1 8 * * * ********************//
//******************* C o m p u t e r G r a p h i c s ********************//
//********************** P R O F E S S O R . L E E  G U K  H W A *********//
//****************************F i n a l   P r o j e c t * * **************//
#include <glut.h>
#include <gl/GLU.h>
#include <gl/GL.h>
#include <iostream>
#include <Windows.h>
#include<vector>
#include <fstream>
#include <stdio.h>
#include <cmath>
#include <mmsystem.h>
#include <string.h>

#pragma warning(disable:4996)
#pragma comment(lib, "glaux.lib")
using namespace std;

static double time = 0; // ������ ���ÿ� ���ְ��� ��Ʈ���ϴ� �� ����ϴ� time ��������
static double time2 = 0; //���ְ� �ֺ��� ���� �༺���� �ð� time2 ��������
static bool blending = false;

int angle = 0;
int MenuChk;

GLint width = 800, height = 600;
GLfloat eye = 0.0; //���� �Բ� �ü��� �̵��ϵ��� ���ִ� ��������
unsigned int speed = 5; //���� �ӵ��� �����ϴ� ��������
float b_lightPosition[] = { 0.0f, 4.0f, 16.0f }; //���� �� ���� ��ǥ X, Y, Z
bool jump = false, over = false, go = false; //���� ��������, ���ӿ��� ����, ����Ⱑ ������ ���� �������� �����̴�.

GLfloat g_light[] = { 0.0, 0.0, 1.5, 1.0 };
GLfloat light0_ambient[] = { 0.5, 0.4, 0.3, 1.0 };
GLfloat light0_diffuse[] = { 0.8, 0.7, 0.6, 1.0 };
GLfloat light0_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light1_ambient[] = { 1.0,0.0,0.0,1.0 };
GLfloat mat1_ambient[] = { 0.5,0.4,0.3,1.0 };
GLuint MyTextureObject; 
GLuint MyBall;

const char * filename = "bluesea.bmp";   //��� 1 �ؽ�ó

const char * filename3 = "cosmic.bmp"; //ground�� ���� ����� �ؽ�ó
typedef union PixelInfo
{
	std::uint32_t Colour;
	struct
	{
		std::uint8_t B, G, R, A;
	};
} *PPixelInfo;
//***********************************************************Example Code Start*****************************************************************

class BMP
{
private:
	std::uint32_t width, height;
	std::uint16_t BitsPerPixel;
	std::vector<std::uint8_t> Pixels;

public:
	BMP(const char* FilePath);
	std::vector<std::uint8_t> GetPixels() const { return this->Pixels; }
	std::uint32_t GetWidth() const { return this->width; }
	std::uint32_t GetHeight() const { return this->height; }
	bool HasAlphaChannel() { return BitsPerPixel == 32; }
};

BMP::BMP(const char* FilePath)
{
	std::fstream hFile(FilePath, std::ios::in | std::ios::binary);
	if (!hFile.is_open()) throw std::invalid_argument("Error: File Not Found.");

	hFile.seekg(0, std::ios::end);
	std::size_t Length = hFile.tellg();
	hFile.seekg(0, std::ios::beg);
	std::vector<std::uint8_t> FileInfo(Length);
	hFile.read(reinterpret_cast<char*>(FileInfo.data()), 54);

	if (FileInfo[0] != 'B' && FileInfo[1] != 'M')
	{
		hFile.close();
		throw std::invalid_argument("Error: Invalid File Format. Bitmap Required.");
	}

	if (FileInfo[28] != 24 && FileInfo[28] != 32)
	{
		hFile.close();
		throw std::invalid_argument("Error: Invalid File Format. 24 or 32 bit Image Required.");
	}

	BitsPerPixel = FileInfo[28];
	width = FileInfo[18] + (FileInfo[19] << 8);
	height = FileInfo[22] + (FileInfo[23] << 8);
	std::uint32_t PixelsOffset = FileInfo[10] + (FileInfo[11] << 8);
	std::uint32_t size = ((width * BitsPerPixel + 31) / 32) * 4 * height;
	Pixels.resize(size);

	hFile.seekg(PixelsOffset, std::ios::beg);
	hFile.read(reinterpret_cast<char*>(Pixels.data()), size);
	hFile.close();
}

/*Load BITMAP file to make texture */
void LoadGLTextures(const char * filename) {
	glClearColor(0.0, 0.0, 0.0, 0.5);

	BMP info = BMP(filename);
	glGenTextures(1, &MyTextureObject);
	glBindTexture(GL_TEXTURE_2D, MyTextureObject);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, info.GetWidth(), info.GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, info.GetPixels().data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glEnable(GL_TEXTURE_2D);


}

//***********************************************************Example Code Endline*****************************************************************

void MyInit(void) {  //��ü , �������� defalut ����
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
}

void PolarView(GLfloat radius, GLfloat elevation, GLfloat azimuth, GLfloat twist) {
	glTranslatef(0.0, 0.0, -radius); //�� ��ǥ�� ����
	glRotatef(-elevation, 1.0, 0.0, 0.0);
	glRotatef(azimuth, 0.0, 0.0, 1.0);
	glRotatef(-twist, 0.0, 0.0, 1.0);
}

void Ball() { //�� ���
	glFrontFace(GL_CW);
	GLUquadricObj *obj;
	GLUquadricObj *obj2;
	obj = gluNewQuadric();
	obj2 = gluNewQuadric();

	gluQuadricDrawStyle(obj, GLU_FILL); //���ΰ� ���ְ�
	gluQuadricDrawStyle(obj2, GLU_FILL); //�� �ֺ��� ���� �༺��
		glPushMatrix();
		glRotatef(angle, -1, 0, 0);
		glLightfv(GL_LIGHT0, GL_AMBIENT, light1_ambient);
		glEnable(GL_TEXTURE_2D);
		gluQuadricTexture(obj, true);
		gluSphere(obj, 0.5, 30.0, 10.0);
		if (blending == true) {
			glMaterialfv(GL_FRONT, GL_AMBIENT, mat1_ambient);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);
		}
		else
			glDisable(GL_BLEND);
		glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
		glColor3f(1.0, 1.0, 1.0);
			glPushMatrix();
			glTranslatef(0, 0.3, 0);
			glTranslated(sin(time2+30), 0, 0);
			gluSphere(obj2, 0.1, 30.0, 10.0);
			glPopMatrix();

			glPushMatrix();
			glTranslatef(0.35, 0.5, 0);
			glTranslated(cos(time2 + 30), 0, 0);
			gluSphere(obj2, 0.1, 30.0, 10.0);
			glPopMatrix();
		glPopMatrix();
}
void ground(float w, float h) { // (w = ���� �� , h = ���� ����)
	glColor3f(0.5f, 0.5f, 0.5f);
	glDisable(GL_LIGHTING);
	glPushMatrix();
	
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBegin(GL_QUADS);
	glBindTexture(GL_TEXTURE_2D, MyBall);
	glTexCoord2f(0.0, 0.0); glVertex3f(-w, w, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(w, w, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(w, -h, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-w, -h, 0.0);
	glEnd();
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glColor3f(1.0, 1.0, 1.0);
}
void map() { //���� �����ϴ� �޼ҵ�

	glTranslatef(0.0f, 3.5f, 16.0f);
	
	glColor3f(1.0, 0.0, 0.0);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	ground(10.0, 1.0); //1 

	glTranslatef(2.0f, -5.0f, 0.0f);
	glutSolidCone(1.0, -3.5, 30, 10);
	ground(1.0, 7.0); //2

	glTranslatef(2.0f, -5.0f, 0.0f);
	glutSolidCone(1.0,-3.5,30,10);
	ground(1.0, 7.0); //3
	
	glTranslatef(-4.0f, -10.0f, 0.0f);
	glutSolidOctahedron();
	ground(1.0, 7.0); //4

	glTranslatef(-4.0f, -10.0f, 0.0f);
	glutSolidOctahedron();
	ground(1.0, 7.0); //5

	glTranslatef(2.0f, -5.0f, 0.0f);
	glutSolidOctahedron();
	ground(1.0, 7.0); //6

	glTranslatef(2.0f, -5.0f, 0.0f);
	glutSolidOctahedron();
	ground(1.0, 7.0); //7

	glTranslatef(4.0f, -10.0f, 0.0f);
	glutSolidOctahedron();
	ground(1.0, 6.0); //8

	glTranslatef(-8.0f, -2.0f, 0.0f);
	glutSolidOctahedron();
	ground(1.0, 20.0); //9 - 1

	glTranslatef(3.0f, -8.0f, 0.0f);
	glutSolidOctahedron();
	ground(2.0, 12.0); //9 - 2

	glColor3f(0.5f, 0.5f, 0.5f);
	
	glTranslatef(0.0f, -9.0f, 1.0f);
	ground(1.0, 12.0); //10
}

void MyDisplay() {
	GLfloat LightPosition[] = { 0.0, 0.0, 1.5, 1.0 };
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	LoadGLTextures(filename);  //�ؽ�ó ���� �ε�
	float r = 999.0f;
	glPushMatrix();
	
	glBindTexture(GL_TEXTURE_2D, MyTextureObject);   //sky ����
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.f); glVertex3f(-r, -r / 2, -r);
	glTexCoord2f(1.f, 0.f); glVertex3f(r, -r / 2, -r);
	glTexCoord2f(1.f, 1.f); glVertex3f(r, r, -r);
	glTexCoord2f(0.f, 1.f); glVertex3f(-r, r, -r);
	glEnd();

	
	glBindTexture(GL_TEXTURE_2D, MyTextureObject);
	glBegin(GL_QUADS);
	glTexCoord2f(0.f, 0.f); glVertex3f(r, -r / 2, r);
	glTexCoord2f(1.f, 0.f); glVertex3f(-r, -r / 2, r);
	glTexCoord2f(1.f, 1.f); glVertex3f(-r, r, r);
	glTexCoord2f(0.f, 1.f); glVertex3f(r, r, r);
	glEnd();

	
	glBindTexture(GL_TEXTURE_2D, MyTextureObject);
	glBegin(GL_QUADS);
	glTexCoord2f(0.f, 0.f); glVertex3f(r, -r / 2, -r);
	glTexCoord2f(1.f, 0.f); glVertex3f(r, -r / 2, r);
	glTexCoord2f(1.f, 1.f); glVertex3f(r, r, r);
	glTexCoord2f(0.f, 1.f); glVertex3f(r, r, -r);
	glEnd();

	
	glBindTexture(GL_TEXTURE_2D, MyTextureObject);
	glBegin(GL_QUADS);
	glTexCoord2f(0.f, 0.f); glVertex3f(-r, -r / 2, r);
	glTexCoord2f(1.f, 0.f); glVertex3f(-r, -r / 2, -r);
	glTexCoord2f(1.f, 1.f); glVertex3f(-r, r, -r);
	glTexCoord2f(0.f, 1.f); glVertex3f(-r, r, r);
	glEnd();

	
	glBindTexture(GL_TEXTURE_2D, MyTextureObject);
	glBegin(GL_QUADS);
	glTexCoord2f(0.f, 0.f); glVertex3f(-r, r, -r);
	glTexCoord2f(1.f, 0.f); glVertex3f(r, r, -r);
	glTexCoord2f(1.f, 1.f); glVertex3f(r, r, r);
	glTexCoord2f(0.f, 1.f); glVertex3f(-r, r, r);
	glEnd();

	
	glBindTexture(GL_TEXTURE_2D, MyTextureObject);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3f(-r, -r / 2, r);
	glTexCoord2f(1, 0); glVertex3f(r, -r / 2, r);
	glTexCoord2f(1, 1); glVertex3f(r, -r / 2, -r);
	glTexCoord2f(0, 1); glVertex3f(-r, -r / 2, -r);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);

	glDisable(GL_TEXTURE_2D);

	PolarView(27.0, -20.0, 0.0, 0.0); //�� ��ǥ�� ����
	glTranslatef(0.0, 0.0, eye); //������ ���� �Բ� �����̵��� ���� 
	glFrustum(-1.0, 1.0, -1.0, 1.0, 0.0, 5.0); //���� ����
	glPushMatrix(); //�������� ����
	glTranslatef(0.0, 15.0, eye);
	glLightfv(GL_LIGHT0, GL_POSITION, g_light);
	glDisable(GL_LIGHTING);
	glColor3f(0.9, 0.9, 0.9);
	glutWireSphere(0.06, 10, 10);
	glEnable(GL_LIGHTING);
	glPopMatrix();

	LoadGLTextures(filename3);    //ground , ball �ؽ�ó �ε�
	glPushMatrix();
	glTranslatef(b_lightPosition[0], b_lightPosition[1], b_lightPosition[2]); //���� ��ġ ����
	Ball(); // �� ����
	glPopMatrix();

	
	glDisable(GL_LIGHTING);
	map(); //�� ����
	glEnable(GL_LIGHTING);

	glEnable(GL_TEXTURE_2D);

	glutSwapBuffers();
}

void MyReshape(int NewWidth, int NewHeight) {
	glViewport(0, 0, NewWidth, NewHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, (GLfloat)NewWidth / (GLfloat)NewHeight, 0.5, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void reset() { //�޴����� reset�׸��� �����ϰų� r ��ư�� ���� �� ����Ǵ� �Լ�
	cout << "������ ���� �����մϴ�." << endl;
	b_lightPosition[0] = 0.0f;             //���۽� ���ְ� ����.
	b_lightPosition[1] = 4.0f;
	b_lightPosition[2] = 16.0f;
	eye = 0.0f;
	over = false;
	go = false;
	speed = 5;
}


void myjump(int Value) { //���� ������Ű�� �޼ҵ�
	if (jump) {
		b_lightPosition[1] = 4.0f+abs((sin(time) * 45.0)); //sin�Լ��� ����Ͽ� ���� ����.
		if (time = 50)  //�ð��� ���� ������ false��Ŵ
			jump = false;
	}
	else {
		b_lightPosition[1] = 4.0f;
	}
	glutTimerFunc(500, myjump, 1);
	glutPostRedisplay();
}
void background()
{
	if (filename == "bluesea.bmp")  //��� 1
		filename = "cosmo.bmp";   //��� 2
	else
		filename = "bluesea.bmp";
	cout << "BACKGROUND COLOR FRESH" << endl;


	glutPostRedisplay();
}
void blend()
{
	if (blending == true)
		blending = false;
	else
		blending = true;
}
void MyKeyboard(unsigned char KeyPressed, int X, int Y) { //Ű���� �̺�Ʈ
	switch (KeyPressed) {
	case 'r':
		reset(); //���� �޼ҵ�
		break;
	case 'b': // ��� �ؽ�ó���� ����
		background();
		break;
	case 'l':
		blend();
		break;
	case 32: //�����̽���(return 32) �Է�
		jump = true; //���� myjump�Լ����� ������ �� �� �ֵ��� �ϴ� Ʈ����
		break;
	case 27:  //esc��ư �Է½� ���α׷� ���� 
		cout << "PROGRAM EXIT" << endl;
		exit(0);
		break;
	default:
		break; 
	}
	glutPostRedisplay();
}

void MySpecial(int key, int x, int y) {
	if (key == GLUT_KEY_UP) {
		go = true; //���� ������ �����̰� �Ѵٴ� Ʈ���� ����
		if (speed >= 140) { //���ǵ��� ���� 140�� ���� ��� ���̻� �������� ����
			cout << "Can't be accelerated anymore" << endl;
			speed = 140;
		}
		else {
			speed += 5; //���ǵ� ����
		}
	}
	if (key == GLUT_KEY_DOWN) {
		if (speed <= 5) { //���ǵ��� ���� 5���� ���� ��� ���̻� ���ҵ��� �ʰ� ���� ���ߴ� Ʈ���� ����
			cout << "Can't be slowed anymore" << endl;
			speed = 5;
			go = false;
		}
		else {
			speed -= 5; //���ǵ� ����
		}
	}
	if (key == GLUT_KEY_LEFT) {
		b_lightPosition[0] -= 0.1f; //���� �������� �̵���Ŵ
	}
	else if (key == GLUT_KEY_RIGHT) {
		b_lightPosition[0] += 0.1f; //���� ���������� �̵���Ŵ
	}

	glutPostRedisplay();
}

void MyTimer(int Value) {
	time2 = time2 + 0.05;

	if (go) { //go �� ���� ��쿡�� ����
		if (b_lightPosition[2] > -74.7) {
			b_lightPosition[2] -= 0.1f; //���� Z�� �������� �̵���Ŵ
			eye += 0.1; //������ ���� �Բ� �̵�
			angle = (angle + 8)%360;
			if (jump != false)
			{
				time = 0;
				time = time + 0.05;
			}
		}
		else {
			b_lightPosition[2] = -74.7; //���� ������ �����ϸ� ���ڸ����� ����
			cout << ">>> End <<<" << endl;
			over = true;
		}
	}
	glutTimerFunc(150 - speed, MyTimer, 1);
	glutPostRedisplay();
}

void Guide() { //�ܼ�â�� ����ϴ� ���� ����
	cout << "--------------G A M E G U I D E--------------" << endl;
	cout << "----------------------------------------------" << endl;
	cout << " �� & �� : Move the ball left and right." << endl;
	cout << " �� & �� : Ball's speed control." << endl;
	cout << " space	: Jumping the ball." << endl;
	cout << " r	: Reset Game." << endl;
	cout << " b	: change the background." << endl;
	cout << " l	: Blend MODE." << endl;
	cout << "----------------------------------------------" << endl;
	cout << " esc	: End game." << endl;
	cout << "----------------------------------------------" << endl;
	cout << "---------Computer Graphics Final Project------" << endl;
	cout << "----------------------------------------------" << endl;
	cout << "----------D i g i t a l C o n t e n t s-------" << endl;
	cout << "----------------------------------------------" << endl;
	cout << "---------13011186 L E E D O N G H O O N-------" << endl;
	cout << "----------------------------------------------" << endl;
}

void MenuPoint(int entryID) {
	MenuChk = entryID;

	
	if (entryID == 1) {
		reset(); //����
	}
	if (entryID = 2) {
		Guide(); //���� ���� �ܼ�â�� �Է�
	}
	if (entryID = 3) {
		background(); // ��� ����
	}

	
	glutPostRedisplay();
}

void MenuFunc() {
	
	GLint mainMenu = glutCreateMenu(MenuPoint); //���� �޴�
	glutAddMenuEntry("Reset", 1);
	glutAddMenuEntry("Game Guide", 2);
	glutAddMenuEntry("Change BackGround", 3);
	

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}




int main(int argc, char** argv) {
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("TRAVEL OF SPACE BALL");

	
	MyInit();

	glutDisplayFunc(MyDisplay);
	glutReshapeFunc(MyReshape);

	glutKeyboardFunc(MyKeyboard);
	glutSpecialFunc(MySpecial);
	glutTimerFunc(150 - speed, MyTimer, 1);
	glutTimerFunc(500, myjump, 1);
	MenuFunc();

	glutMainLoop();


	return 0;
}