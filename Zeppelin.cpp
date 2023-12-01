#define _CRT_SECURE_NO_WARNINGS
#define TEXCOORD0_ATTRIBUTE 8
#define BUFFER_OFFSET(offset) ((void*)(offset))
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <GL/glew.h>
#include <gl/glut.h>
#include <GL/freeglut.h>
#include <math.h>
#include <sstream>
#include <vector>
#include "surfaceModeller.h"
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <SOIL/SOIL.h> 


const int vWidth = 1080;    // Viewport width in pixels
const int vHeight = 720;    // Viewport height in pixels

// Light properties
GLfloat light_position0[] = { -4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_position1[] = { 4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };

GLfloat groundMat_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
GLfloat groundMat_specular[] = { 0.01, 0.01, 0.01, 1.0 };
GLfloat groundMat_diffuse[] = { 0.4, 0.4, 0.7, 1.0 };
GLfloat groundMat_shininess[] = { 1.0 };


GLfloat light_diffuse0[] = { 1.0, 1.0, 1.0, 1.0 };


GLfloat light_diffuse1[] = { 1.0, 1.0, 1.0, 1.0 };


GLfloat model_ambient[] = { 0.5, 0.5, 0.5, 1.0 };

GLfloat quadMat_ambient[] = { 0.1, 0.2, 0.4, 1.0 };
GLfloat quadMat_specular[] = { 0.45, 0.55, 0.45, 1.0 };
GLfloat quadMat_diffuse[] = { 0.2, 0.3, 0.5, 1.0 };
GLfloat quadMat_shininess[] = { 40.0 };

//way the ship is pointing
float xz_ang = 0.0;
float xy_ang = 0.0;
float zy_ang = 0.0;

float xz_ang_e = 0.0;
float xy_ang_e = 0.0;
float zy_ang_e = 0.0;

float proAng = 0.0;

float bodyWidth = 5.0;
float bodyheight = 2.5;
float bodyDepth = 2.5;

float x_dir = 0.0;
float y_dir = 0.0;
float z_dir = 0.0;

float x_dir_e = 0.0;
float y_dir_e = 0.0;
float z_dir_e = 0.0;

float loc[] = {0.0,0.0,0.0};
float loc_e[] = { 0.0,0.0,50.0 };

float speed = 0.0;
float speed_e = 0.5;

int lastMouseX;
int lastMouseY;

GLdouble eyeX = 0.0, eyeY = 3.0, eyeZ = 20.0;
GLdouble radius = eyeZ;

double theta = asin(eyeX / radius);
double thi = asin(eyeY / radius);

double zscale = 1.0;

GLdouble fov = 80.0;
GLdouble zNear = 0.1, zFar = 200.0;
GLdouble aspect = (GLdouble)vWidth / vHeight;

bool fpv = false;

double clx = 0.0;
double cly = 3.0;
double clz = 0.0;

double fpvx = loc[0];
double fpvy = loc[1]+3;
double fpvz = loc[2];

double nfpx, nfpy, nfpz;

float locFP[] = {-11.0,0.0,0.0};

unsigned int numTris = 0;
unsigned int numVertices = 0;
unsigned int numIndices = 0;

//for body mesh

int fc = 0; // face count
int vc = 0; // vertex count
int nc = 0; // normal count

//hit box for player zep f_ is for front b_ is back
double f_tr[] = {10.0,2.5,10.0};
 
double f_bl[] = {-10.0,-2.5,-10.0};


//hit box for the enemy zep
double f_tr_e[] = {8.0, 2, 60.0};
double f_bl_e[] = {-8.0, -2, 40.0 };


double temp[] = {-20.0,0.0,0.0};

//missle pos in space
double mx, my, mz;

//missle dir vec
double mxp, myp, mzp;

//missle dir vec enepy
double mxpe, mype, mzpe, mxe, mye, mze;


bool hit_e = false;

bool fired = false;
bool firede = false;
bool inrange = false;

GLuint list;



void initOpenGL(int w, int h)
{
	// Set up and enable lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);   // This second light is currently off

	// Other OpenGL setup
	glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
	glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
	glClearColor(0.4F, 0.4F, 0.4F, 0.0F);  // Color and depth for glClear
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}
void init3DSurfaceWindow()
{

	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glClearColor(0.4F, 0.4F, 0.4F, 0.0F);  // Color and depth for glClear

	glViewport(0, 0, (GLsizei)vWidth, (GLsizei)vHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspect, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0, 3.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	// REPLACE call to myLookAt with myLookAt(0.0, 3.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void drawBody() {
	glPushMatrix();
	glScalef(2*bodyWidth,1.0*bodyheight,1.0*bodyDepth);
	glutSolidSphere(1, 25, 25);
	glPopMatrix();
}

void drawFin() {
	glPushMatrix();

	glTranslatef(1.65*bodyWidth,0,0.0);
	glScalef(3,1.5*bodyheight,0.15);
	glutSolidCube(1);

	glPopMatrix();

	glPushMatrix();

	glTranslatef(1.65 * bodyWidth, 0, 0.0);
	glScalef(3, 0.15, 1.5 * bodyDepth);
	glutSolidCube(1);

	glPopMatrix();
}



void drawCabin() {
	glPushMatrix();

	glTranslatef(0,-1*bodyheight, 0);
	glScalef(1*bodyWidth,1,0.5*bodyDepth);

	glutSolidCube(1);

	glPopMatrix();

	glPushMatrix();

	glTranslatef(-0.5*bodyWidth,-0.9*bodyheight,0);
	glScalef(1,1,0.5*bodyDepth);
	glRotatef(45,0,0,1);


	glutSolidCube(1);

	glPopMatrix();

	glPushMatrix();

	glTranslatef(0.5 * bodyWidth, -0.9 * bodyheight, 0);
	glScalef(1, 1, 0.5 * bodyDepth);
	glRotatef(45, 0, 0, 1);


	glutSolidCube(1);

	glPopMatrix();
}

void drawMiddle() {
	glPushMatrix();

	glScalef(1,1,1.2*bodyDepth);
	gluCylinder(gluNewQuadric(), 1.0, 1.0, 1.0, 25, 25);

	glPopMatrix();

	glPushMatrix();

	glTranslatef(0,0,1.2*bodyDepth);
	gluDisk(gluNewQuadric(),0,1.0,25,25);

	glPopMatrix();

	glPushMatrix();

	glScalef(1, 1, -1.2 * bodyDepth);
	gluCylinder(gluNewQuadric(), 1.0, 1.0, 1.0, 25, 25);

	glPopMatrix();

	glPushMatrix();

	glTranslatef(0, 0, -1.2 * bodyDepth);
	glRotatef(180,0,1,0);
	gluDisk(gluNewQuadric(), 0, 1.0, 25, 25);

	glPopMatrix();
}

void drawPro() {
	glPushMatrix();

	glTranslatef(1,0,1.1*bodyDepth);
	glScalef(0.35, 0.05, 0.05);
	glRotatef(90, 0, 1, 0);
	gluCylinder(gluNewQuadric(), 1.0, 1.0, 1.0, 25, 25);


	glPopMatrix();

	glPushMatrix();

	glTranslatef(1, 0, -1.1 * bodyDepth);
	glScalef(0.35, 0.05, 0.05);
	glRotatef(90, 0, 1, 0);
	gluCylinder(gluNewQuadric(), 1.0, 1.0, 1.0, 25, 25);


	glPopMatrix();

	glPushMatrix();

	glTranslatef(0.525*bodyWidth, -1.12 * bodyheight,0);
	glScalef(0.7,0.1,0.1);
	glRotatef(90,0,1,0);
	gluCylinder(gluNewQuadric(),1.0,1.0,1.0,25,25);
	glPopMatrix();

	glPushMatrix();

	glTranslatef(1.35, 0, -1.1 * bodyDepth);
	glScalef(0.35, 0.05, 0.05);
	glRotatef(90, 0, 1, 0);
	gluDisk(gluNewQuadric(), 0, 1.0, 25, 25);

	glPopMatrix();

	glPushMatrix();

	glTranslatef(1.35, 0, 1.1 * bodyDepth);
	glScalef(0.35, 0.05, 0.05);
	glRotatef(90, 0, 1, 0);
	gluDisk(gluNewQuadric(), 0, 1.0, 25, 25);

	glPopMatrix();

	glPushMatrix();

	glTranslatef(0.525 * bodyWidth +0.7, -1.12 * bodyheight, 0);
	glScalef(0.7, 0.1, 0.1);
	glRotatef(90, 0, 1, 0);
	gluDisk(gluNewQuadric(), 0, 1.0, 25, 25);

	glPopMatrix();

}

void drawProFin() {
	glPushMatrix();

	glTranslatef(1.29, 0, 1.1 * bodyDepth);
	glRotatef(proAng,1.0,0.0,0.0 );
	glScalef(0.05, 0.075, 0.45);

	glutSolidCube(1);

	glPopMatrix();

	glPushMatrix();

	glTranslatef(1.29, 0, 1.1 * bodyDepth);
	glRotatef(proAng, 1.0, 0.0, 0.0);
	glScalef(0.05, 0.45, 0.075);

	glutSolidCube(1);

	glPopMatrix();

	glPushMatrix();

	glTranslatef(1.29, 0, -1.1 * bodyDepth);
	glRotatef(proAng, 1.0, 0.0, 0.0);
	glScalef(0.05, 0.075, 0.45);

	glutSolidCube(1);

	glPopMatrix();

	glPushMatrix();

	glTranslatef(1.29, 0, -1.1 * bodyDepth);
	glRotatef(proAng, 1.0, 0.0, 0.0);
	glScalef(0.05, 0.45, 0.075);

	glutSolidCube(1);

	glPopMatrix();

	glPushMatrix();

	glTranslatef(0.525 * bodyWidth + 0.65, -1.12 * bodyheight, 0.0);
	glRotatef(proAng, 1.0, 0.0, 0.0);
	glScalef(0.05, 0.78, 0.075);

	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();

	glTranslatef(0.525 * bodyWidth + 0.65, -1.12 * bodyheight, 0.0);
	glRotatef(proAng, 1.0, 0.0, 0.0);
	glScalef(0.05, 0.075, 0.78);

	glutSolidCube(1);

	glPopMatrix();
}

void miss() {
	glPushMatrix();
	glScalef(0.5,0.5,0.5);
	glutSolidCube(1);

	glPopMatrix();
}



void reshape(int w, int h)
{
	// Set up viewport, projection, then change to modelview matrix mode - 
	// display function will then set up camera and do modeling transforms.
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, (GLdouble)w / h, 0.2, zFar);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	gluLookAt(0.0, 6.0, 22.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}
bool stop = false;

void arrows(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_UP:
		loc[1] += 3;
		printf("Direction: <%f,%f,%f>\n", x_dir, y_dir, z_dir);
		printf("location: <%f,%f,%f>\n", loc[0], loc[1], loc[2]);
		break;
	case GLUT_KEY_LEFT:
		xz_ang += 5;
		printf("Direction: <%f,%f,%f>\n", x_dir, y_dir, z_dir);
		printf("location: <%f,%f,%f>\n", loc[0], loc[1], loc[2]);
		break;
	case GLUT_KEY_RIGHT:
		xz_ang -= 5;
		printf("Direction: <%f,%f,%f>\n", x_dir, y_dir, z_dir);
		printf("location: <%f,%f,%f>\n", loc[0], loc[1], loc[2]);
		break;
	case GLUT_KEY_DOWN:
		loc[1] -= 3;
		printf("Direction: <%f,%f,%f>\n", x_dir, y_dir, z_dir);
		printf("location: <%f,%f,%f>\n", loc[0], loc[1], loc[2]);
		break;
	}
	
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		speed = 1.25;
		break;
	case 'W':
		speed = 2;
		break;

	case 'd':
		xz_ang -= 5;
		break;

	case 'a':
		xz_ang += 5;
		break;

	case 's':
		speed =-1.25;
		break;

	case 'e':
		fpv = true;
		break;

	case 'E':
		fpv = false;
		break;

	case ' ':
		loc[1] += 1.25;
		locFP[1] += 1.25;
		f_tr[1] += 1.25;
		f_bl[1] += 1.25;
		break;

	case 'b':
		loc[1] -= 1.25;
		locFP[1] -= 1.25;
		f_tr[1] -= 1.25;
		f_bl[1] -= 1.25;
		break;
	
	case 'f':
		if(fired == false){
			printf("FIRED\n");
			if (fpv == false) {
				mx = loc[0];
				my = loc[1];
				mz = loc[2];
			}
			else {
				mx = fpvx;
				my = fpvy;
				mz = fpvz;
			}
			fired = true;
		}

	}

	glutPostRedisplay();
}
int currentButton;
void mouseMotionHandler3D(int x, int y)
{
	int dx = x - lastMouseX;
	int dy = y - lastMouseY;

	// REQUIREMENT 3 code here
	if (currentButton == GLUT_LEFT_BUTTON)
	{

		if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)
		{


		}
		else

		{
			double camX, camY, camZ, t_theta, t_thi;
			t_theta = theta - dx * 0.01;
			t_thi = thi + dy * 0.01;

			if (fpv == false) {
				camZ = radius * cos(t_thi) * cos(t_theta);
				camX = radius * cos(t_thi) * sin(t_theta);
				camY = radius * sin(t_thi);

			}
			else {
				camZ = cos(t_thi) * cos(t_theta)*0.5;
				camX = cos(t_thi) * sin(t_theta)*0.5;
				camY = -sin(t_thi)*0.5;
			}

			if (t_thi * 180 / 3.1415 < 89 && t_thi * 180 / 3.1415 > -89 && fpv == false) {

				eyeY = camY;
				eyeX = camX;
				eyeZ = camZ;
				theta = t_theta;
				thi = t_thi;

			}
			else if(t_thi * 180 / 3.1415 > -89 && t_thi * 180/3.1415 <=44 && fpv == true) {
				float lengthF = sqrt(pow(camX, 2) + pow(camY, 2) + pow(camZ, 2));
				nfpx = (camX / lengthF);
				nfpy = camY / lengthF;
				nfpz = (camZ / lengthF);
				eyeY = camY;
				eyeX = camX;
				eyeZ = camZ;
				theta = t_theta;
				thi = t_thi;
			}
			else if ((t_thi * 180 / 3.1415 > 89 || t_thi * 180 / 3.1415 < -89) && fpv == false) {
				camY = sin(thi);
				eyeY = camY;
				eyeX = camX;
				eyeZ = camZ;

			}
			else if ((t_thi * 180 / 3.1415 < -89 || t_thi * 180 / 3.1415 > 43) && fpv == true) {
				camY = -sin(thi) * 0.5;
				float lengthF = sqrt(pow(camX, 2) + pow(camY, 2) + pow(camZ, 2));
				nfpx = (camX / lengthF);
				nfpy = camY / lengthF;
				nfpz = (camZ / lengthF);
				eyeY = camY;
				eyeX = camX;
				eyeZ = camZ;
			}

		}
	}
	lastMouseX = x;
	lastMouseY = y;
	glutPostRedisplay();
}

void mouseButtonHandler3D(int button, int state, int x, int y)
{
	currentButton = button;
	lastMouseX = x;
	lastMouseY = y;
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{


		}
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void animationHandler(int param)
{
	if (!stop)
	{
		proAng += 0.25;
		if (proAng > 90) {
			proAng = 0;
		}
		glutPostRedisplay();
		glutTimerFunc(10, animationHandler, 0);
	}
}

std::vector<GLfloat> vertices;  // vertex positions - used for VBO draw
std::vector<GLfloat> normals;    // normal vectors for each vertex - used for VBO draw
std::vector<GLuint> indices;
std::vector<GLfloat> textureCords;

std::vector<GLfloat> vertices2;  // vertex positions - used for VBO draw
std::vector<GLfloat> normals2;    // normal vectors for each vertex - used for VBO draw
std::vector<GLuint> indices2;
std::vector<GLfloat> textureCords2;

std::vector<GLfloat> vertices3;  // vertex positions - used for VBO draw
std::vector<GLfloat> normals3;    // normal vectors for each vertex - used for VBO draw
std::vector<GLuint> indices3;
std::vector<GLfloat> textureCords3;



void readOBJ()
{
	std::ifstream infile("body.obj");
	std::string line;

	while (std::getline(infile, line)) {
		std::istringstream iss(line);
		std::string prefix;
		iss >> prefix;

		if (prefix == "v" || prefix == "vn") {
			GLfloat x, y, z;
			if (!(iss >> x >> y >> z)) {
				break;  // error in reading line
			}

			if (prefix == "v") {
				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(z);


				//generate the texture cords for the mesh
				GLfloat r = sqrt(x * x + y * y + z * z);
				GLfloat p = acos(z / r);  // theta in the spherical coordinates
				GLfloat t = atan2(y, x);  // phi in the spherical coordinates


				GLfloat u = p / 3.14159265;
				GLfloat v = (t + 3.14159265) / (2 * 3.14159265);

				//store them
				textureCords.push_back(u);
				textureCords.push_back(v);


			}
			else if (prefix == "vn") {
				normals.push_back(x);
				normals.push_back(y);
				normals.push_back(z);
			}
		}
		else if (prefix == "f") {
			std::string vertex1, vertex2, vertex3;
			if (!(iss >> vertex1 >> vertex2 >> vertex3)) {
				break;  // error in reading line
			}

			std::istringstream iss1(vertex1), iss2(vertex2), iss3(vertex3);
			GLuint idx[3];
			char ch;  // to skip the '/' character

			if ((iss1 >> idx[0] >> ch) && (iss2 >> idx[1] >> ch) && (iss3 >> idx[2] >> ch)) {
				indices.push_back(idx[0] );
				indices.push_back(idx[1] );
				indices.push_back(idx[2] );
			}
		}
	}

	std::ifstream infile2("ground.obj");
	std::string line2;

	while (std::getline(infile2, line2)) {
		std::istringstream iss(line2);
		std::string prefix;
		iss >> prefix;

		if (prefix == "v" || prefix == "vn") {
			GLfloat x, y, z;
			if (!(iss >> x >> y >> z)) {
				break;  // error in reading line
			}

			if (prefix == "v") {
				vertices2.push_back(x);
				vertices2.push_back(y);
				vertices2.push_back(z);


				//generate the texture cords for the mesh
				GLfloat r = sqrt(x * x + y * y + z * z);
				GLfloat p = acos(z / r);  // theta in the spherical coordinates
				GLfloat t = atan2(y, x);  // phi in the spherical coordinates


				GLfloat u = p / 3.14159265;
				GLfloat v = (t + 3.14159265) / (2 * 3.14159265);

				//store them
				textureCords2.push_back(u);
				textureCords2.push_back(v);


			}
			else if (prefix == "vn") {
				normals2.push_back(x);
				normals2.push_back(y);
				normals2.push_back(z);
			}
		}
		else if (prefix == "f") {
			std::string vertex1, vertex2, vertex3;
			if (!(iss >> vertex1 >> vertex2 >> vertex3)) {
				break;  // error in reading line
			}

			std::istringstream iss1(vertex1), iss2(vertex2), iss3(vertex3);
			GLuint idx[3];
			char ch;  // to skip the '/' character

			if ((iss1 >> idx[0] >> ch) && (iss2 >> idx[1] >> ch) && (iss3 >> idx[2] >> ch)) {
				indices2.push_back(idx[0]);
				indices2.push_back(idx[1]);
				indices2.push_back(idx[2]);
			}
		}
	}


	std::ifstream infile3("ground_mesh.obj");
	std::string line3;

	while (std::getline(infile3, line3)) {
		std::istringstream iss(line3);
		std::string prefix;
		iss >> prefix;

		if (prefix == "v" || prefix == "vn") {
			GLfloat x, y, z;
			if (!(iss >> x >> y >> z)) {
				break;  // error in reading line
			}

			if (prefix == "v") {
				vertices3.push_back(x);
				vertices3.push_back(y);
				vertices3.push_back(z);


				//generate the texture cords for the mesh
				GLfloat r = sqrt(x * x + y * y + z * z);
				GLfloat p = acos(z / r);  // theta in the spherical coordinates
				GLfloat t = atan2(y, x);  // phi in the spherical coordinates


				GLfloat u = p / 3.14159265;
				GLfloat v = (t + 3.14159265) / (2 * 3.14159265);

				//store them
				textureCords3.push_back(u);
				textureCords3.push_back(v);


			}
			else if (prefix == "vn") {
				normals3.push_back(x);
				normals3.push_back(y);
				normals3.push_back(z);
			}
		}
		else if (prefix == "f") {
			std::string vertex1, vertex2, vertex3;
			if (!(iss >> vertex1 >> vertex2 >> vertex3)) {
				break;  // error in reading line
			}

			std::istringstream iss1(vertex1), iss2(vertex2), iss3(vertex3);
			GLuint idx[3];
			char ch;  // to skip the '/' character

			if ((iss1 >> idx[0] >> ch) && (iss2 >> idx[1] >> ch) && (iss3 >> idx[2] >> ch)) {
				indices3.push_back(idx[0]);
				indices3.push_back(idx[1]);
				indices3.push_back(idx[2]);
			}
		}
	}

}




GLuint texture = 0;
GLuint texture2 = 0;
GLuint texture3 = 0;
GLuint texture4 = 0;

void loadTexture()
{

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
		printf("Error\n");

	// Load the texture file using SOIL
	int width, height;
	GLuint image = SOIL_load_OGL_texture("texture.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	std::cout << "Loaded first texture image" << std::endl;
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	texture = image;
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Unbind the texture and free image data

	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint image2 = SOIL_load_OGL_texture("Foliage_Lilypads_001_BaseColor.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	std::cout << "Loaded second texture image" << std::endl;

	texture2 = image2;
	glBindTexture(GL_TEXTURE_2D, texture2);
	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Unbind the texture and free image data

	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint image3 = SOIL_load_OGL_texture("Fabric_036_basecolor.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	std::cout << "Loaded third texture image" << std::endl;
	texture3 = image3;
	glBindTexture(GL_TEXTURE_2D, texture3);
	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Unbind the texture and free image data

	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint image4 = SOIL_load_OGL_texture("Stylized_Stone_Floor_003_basecolor.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	std::cout << "Loaded fourth texture image" << std::endl;
	texture4 = image4;
	glBindTexture(GL_TEXTURE_2D, texture4);
	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Unbind the texture and free image data

	glBindTexture(GL_TEXTURE_2D, 0);

	
}


unsigned int VBOv, VBOn, VBOi, VBOt;

void drawBodyObj(GLuint tex) {


	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
		printf("Error\n");
	

	
	glPushMatrix();
	glScalef(0.5*bodyWidth, 0.85 * bodyheight, 0.6 * bodyDepth);
	glTranslatef(0,0.1,0);
	glGenBuffers(1, &VBOv);

	glGenBuffers(1, &VBOn);

	glGenBuffers(1, &VBOi);
	
	glGenBuffers(1, &VBOt);


	//populate VBOv
	glBindBuffer(GL_ARRAY_BUFFER, VBOv);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_DYNAMIC_DRAW);
	//populate VBOn
	glBindBuffer(GL_ARRAY_BUFFER, VBOn);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), normals.data(), GL_DYNAMIC_DRAW);

	//populate VBOi
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOi);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_DYNAMIC_DRAW);

	//texture cords VBOt
	glBindBuffer(GL_ARRAY_BUFFER, VBOt);
	glBufferData(GL_ARRAY_BUFFER, textureCords.size() * sizeof(GLfloat), textureCords.data(), GL_DYNAMIC_DRAW);

	// Enable and set texture coordinate array
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, VBOt);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
	glEnable(GL_TEXTURE_2D);

	//drawing the object    
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, VBOv);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBOn);
	glNormalPointer(GL_FLOAT, 0, 0);

	
	glBindTexture(GL_TEXTURE_2D, tex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOi);
	glDrawElements(GL_TRIANGLES, indices.size()*sizeof(GLuint), GL_UNSIGNED_INT, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableClientState(GL_VERTEX_ARRAY);

	glDisableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glPopMatrix();
}

unsigned int VBOvG, VBOnG, VBOiG, VBOtG;
void drawGroundThings(GLuint tex) {
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
		printf("Error\n");



	glPushMatrix();

	glGenBuffers(1, &VBOvG);

	glGenBuffers(1, &VBOnG);

	glGenBuffers(1, &VBOiG);

	glGenBuffers(1, &VBOtG);


	//populate VBOv
	glBindBuffer(GL_ARRAY_BUFFER, VBOvG);
	glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(GLfloat), vertices2.data(), GL_DYNAMIC_DRAW);
	//populate VBOn
	glBindBuffer(GL_ARRAY_BUFFER, VBOnG);
	glBufferData(GL_ARRAY_BUFFER, normals2.size() * sizeof(GLfloat), normals2.data(), GL_DYNAMIC_DRAW);

	//populate VBOi
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOiG);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices2.size() * sizeof(GLuint), indices2.data(), GL_DYNAMIC_DRAW);

	//texture cords VBOt
	glBindBuffer(GL_ARRAY_BUFFER, VBOtG);
	glBufferData(GL_ARRAY_BUFFER, textureCords2.size() * sizeof(GLfloat), textureCords2.data(), GL_DYNAMIC_DRAW);

	// Enable and set texture coordinate array
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, VBOtG);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
	glEnable(GL_TEXTURE_2D);

	//drawing the object    
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, VBOvG);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBOnG);
	glNormalPointer(GL_FLOAT, 0, 0);


	glBindTexture(GL_TEXTURE_2D, tex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOiG);
	glDrawElements(GL_TRIANGLES, indices2.size() * sizeof(GLuint), GL_UNSIGNED_INT, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableClientState(GL_VERTEX_ARRAY);

	glDisableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glPopMatrix();
}

unsigned int VBOvGM, VBOnGM, VBOiGM, VBOtGM;
void drawGroundMesh(GLuint tex) {
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
		printf("Error\n");



	glPushMatrix();
	glScalef(150,1,150);
	glTranslatef(0,-4,0);
	glGenBuffers(1, &VBOvGM);

	glGenBuffers(1, &VBOnGM);

	glGenBuffers(1, &VBOiGM);

	glGenBuffers(1, &VBOtGM);


	//populate VBOv
	glBindBuffer(GL_ARRAY_BUFFER, VBOvGM);
	glBufferData(GL_ARRAY_BUFFER, vertices3.size() * sizeof(GLfloat), vertices3.data(), GL_DYNAMIC_DRAW);
	//populate VBOn
	glBindBuffer(GL_ARRAY_BUFFER, VBOnGM);
	glBufferData(GL_ARRAY_BUFFER, normals2.size() * sizeof(GLfloat), normals2.data(), GL_DYNAMIC_DRAW);

	//populate VBOi
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOiGM);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices3.size() * sizeof(GLuint), indices3.data(), GL_DYNAMIC_DRAW);

	//texture cords VBOt
	glBindBuffer(GL_ARRAY_BUFFER, VBOtGM);
	glBufferData(GL_ARRAY_BUFFER, textureCords3.size() * sizeof(GLfloat), textureCords3.data(), GL_DYNAMIC_DRAW);

	// Enable and set texture coordinate array
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, VBOtGM);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
	glEnable(GL_TEXTURE_2D);

	//drawing the object    
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, VBOvGM);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBOnGM);
	glNormalPointer(GL_FLOAT, 0, 0);


	glBindTexture(GL_TEXTURE_2D, tex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOiGM);
	glDrawElements(GL_TRIANGLES, indices3.size() * sizeof(GLuint), GL_UNSIGNED_INT, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableClientState(GL_VERTEX_ARRAY);

	glDisableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glPopMatrix();
}



void drawZep() {
	
	glPushMatrix();
	glTranslatef(loc[0] + speed * x_dir, loc[1] + speed * y_dir, loc[2] + speed * z_dir);
	
	glRotatef(xz_ang, 0.0, 1.0, 0.0);

	//update new center of mass for player
	loc[0] += speed * x_dir;
	loc[1] += speed * y_dir;
	loc[2] += speed * z_dir;

	//update hitbox loc
	f_bl[0] += speed * x_dir;
	f_bl[1] += speed * y_dir;
	f_bl[2] += speed * z_dir;
	
	f_tr[0] += speed * x_dir;
	f_tr[1] += speed * y_dir;
	f_tr[2] += speed * z_dir;

	x_dir = -1 * cos(xz_ang * 3.1415 / 180);
	z_dir = sin(xz_ang * 3.1415 / 180);

	speed = 0;
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, quadMat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, quadMat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, quadMat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, quadMat_shininess);
	//drawBody();
	drawBodyObj(texture);
	drawFin();
	drawCabin();
	drawMiddle();
	drawPro();
	drawProFin();
	
	glPopMatrix();
	y_dir = 0;
}


void drawZepE() {

	glPushMatrix();
	
	glTranslatef(loc_e[0] + speed_e * x_dir_e, loc_e[1] + speed_e * y_dir_e, loc_e[2] + speed_e * z_dir_e);
	
	double tempx = loc[0] - loc_e[0];
	double tempy = loc[1] - loc_e[1];
	double tempz = loc[2] - loc_e[2];

	double mag1 = sqrt(x_dir_e * x_dir_e + y_dir_e * y_dir_e + z_dir_e * z_dir_e);
	double mag2 = sqrt(tempx * tempx + tempy * tempy + tempz * tempz);


	tempx = tempx / mag2;
	tempy = tempy / mag2;
	tempz = tempz / mag2;
	double dot = x_dir_e * tempx + y_dir_e * tempy + z_dir_e * tempz;
	if (abs(loc[0] - loc_e[0]) <= 40 && abs(loc[1] - loc_e[1]) <= 30 && abs(loc[2] - loc_e[2]) <= 40) {
		xz_ang_e = acos(dot);
		if (loc[2] < loc_e[2]) {
			xz_ang_e = -xz_ang_e;

		}
	}
	glRotatef(xz_ang_e * (180.0f / 3.1415f), 0.0, 1.0, 0.0);


	//update new center of mass for enemy
	loc_e[0] += speed_e * x_dir_e;
	loc_e[1] += speed_e * y_dir_e;
	loc_e[2] += speed_e * z_dir_e;

	//update hitbox loc
	f_bl_e[0] += speed_e * x_dir_e;
	f_bl_e[1] += speed_e * y_dir_e;
	f_bl_e[2] += speed_e * z_dir_e;

	f_tr_e[0] += speed_e * x_dir_e;
	f_tr_e[1] += speed_e * y_dir_e;
	f_tr_e[2] += speed_e * z_dir_e;

	x_dir_e = -1 * cos(0 * 3.1415 / 180);
	z_dir_e = sin(0 * 3.1415 / 180);


		if (loc_e[0] <= -60) {
			speed_e = -0.5;
		}
		if (loc_e[0] >= 60) {
			speed_e = 0.5;
		}
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, quadMat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, quadMat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, quadMat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, quadMat_shininess);
	//drawBody();
	drawBodyObj(texture3);
	drawFin();
	drawCabin();
	drawMiddle();
	drawPro();
	drawProFin();

	glPopMatrix();
	y_dir_e = 0.0;
	z_dir_e = 0.0;
}

unsigned int vbo;


void drawGround()
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, groundMat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, groundMat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, groundMat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, groundMat_shininess);

	drawGroundMesh(texture4);

	glPushMatrix();
	glTranslatef(36.0,-4.0,40.0);
	drawGroundThings(texture2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-30.0, -3.5, -90.0);
	drawGroundThings(texture2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(90.0, -4.0, 30.0);
	glScalef(1, 1, 2);
	drawGroundThings(texture2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-90.0, -3.7, 10.0);
	drawGroundThings(texture2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-10.0, -4.2, -10.0);
	drawGroundThings(texture2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(70.0, -4.0, -30.0);
	drawGroundThings(texture2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-20.0, -4.0, 40.0);
	glScalef(2,1,2);
	drawGroundThings(texture2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-48.0, -4.0, -54.0);
	drawGroundThings(texture2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(20.0, -3.5, -45.0);
	glScalef(2, 1, 5);
	drawGroundThings(texture2);
	glPopMatrix();

}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	// Create Viewing Matrix V
	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	if (fpv == true) {
		clx = eyeX + loc[0];
		cly = eyeY + loc[1]+4;
		clz = eyeZ + loc[2];

		fpvx = clx-nfpx;
		fpvy = cly+nfpy;
		fpvz = clz-nfpz;
		
		gluLookAt(clx, cly, clz, fpvx, fpvy, fpvz, 0.0, 1.0, 0.0);
	}
	else {
		clx = eyeX + loc[0];
		cly = eyeY + loc[1];
		clz = eyeZ + loc[2];

		fpvx = loc[0];
		fpvy = loc[1]+3;
		fpvz = loc[2];

		gluLookAt(clx, cly, clz, fpvx, fpvy, fpvz, 0.0, 1.0, 0.0);
	}
	
	
	if (fired == false) {
		mxp = fpvx - clx;
		myp = fpvy - cly;
		mzp = fpvz - clz;

		double mag = sqrt(mxp * mxp + myp * myp + mzp * mzp);


		mxp = (fpvx - clx) / mag;
		myp = (fpvy - cly) / mag;
		mzp = (fpvz - clz) / mag;
	}
	
 

	if (abs(loc[0] - loc_e[0])<=40 && abs(loc[1]-loc_e[1])<=30 && abs(loc[2] - loc_e[2])<=40) {
		if (firede == false) {
			inrange == true;
			mxpe = loc[0]-loc_e[0];
			mype = loc[1]-loc_e[1];
			mzpe = loc[2]-loc_e[2];
			
			double mag = sqrt(mxpe * mxpe + mype * mype + mzpe * mzpe);
			printf("INCOMING FIRE\n");
			mxpe = mxpe / mag;
			mype = mype / mag;
			mzpe = mzpe / mag;


			firede = true;
			mxe = loc_e[0];
			mye = loc_e[1];
			mze = loc_e[2];

		}
	}
	drawGround();

	// Apply modelling transformations M to move robot
	// Current transformation matrix is set to IV, where I is identity matrix
	// CTM = IV
	
	drawZep();

	if (fpv == true) {
		glPushMatrix();
		glTranslatef(fpvx, fpvy, fpvz);
		glutSolidSphere(0.005, 10, 10);
		glPopMatrix();
	}



	drawZepE();
	
	if (firede == true &&abs(loc_e[0] - mxe) < 150 && abs(loc_e[1] - mye) < 100 && abs(loc_e[1] - mze) < 150) {
		glPushMatrix();
		glTranslatef(mxe+2*mxpe,mye+2*mype, mze+2*mzpe);
		miss();
		glPopMatrix();

		mxe += 2 * mxpe;
		mye += 2 * mype;
		mze += 2 * mzpe;

	}

	if (fired == true && abs(loc[0] - mx)<150 && abs(loc[1]-my)<100 && abs(loc[1]-mz)<150) {
		glPushMatrix();
		glTranslatef(mx+2*mxp,my+2*myp,mz+2*mzp);
		miss();

		glPopMatrix();
		mx += 2 * mxp;
		my += 2 * myp;
		mz += 2 * mzp;
	}


	//checking for hit on player
	if (firede == true && (mxe < f_tr[0] && mxe > f_bl[0]) && (mye<f_tr[1] && mye>f_bl[1]) && (mze< f_tr[2] && mze>f_bl[2])) {
		firede == false;
		mxe = 500;
		mye = 500;
		mze = 500;

		printf("YOU HAVE BEEN HIT\n");

		x_dir = 0 - loc[0];
		z_dir = -100;
		y_dir = 5 - loc[1];
		speed = 1;
	}


	//checking for hit on enemy
	if (fired == true &&(mx < f_tr_e[0] && mx > f_bl_e[0]) && (my<f_tr_e[1] && my>f_bl_e[1]) && (mz< f_tr_e[2] && mz>f_bl_e[2])) {
		fired == false;
		mx = 500;
		my = 500;
		mz = 500;
		printf("HIT HIT HIT\n");

		int temp1 = rand() % 30 + 5;

		int temp2 = rand() % 50 ;
		if (loc_e[1] + temp1 -15< 60 && loc_e[1]+temp1-15>5) {
			y_dir_e = temp1;
		}
		if (loc_e[2] + temp2-25<80&& loc_e[2]+temp2-25>-80) {
			z_dir_e = temp2;
		}

		if (loc_e[2]+temp2-25>80) {
			z_dir_e = -70;
		}

		if (loc_e[2] + temp2 - 25 < -80) {
			z_dir_e = +70;
		}

		if (loc_e[1] + temp1 - 15 > 60) {
			y_dir_e = -40;
		}

		if (loc_e[1] + temp1 - 15 < 5) {
			y_dir_e = 10;
		}
		speed_e = 1;

	}

	//add check for when the enemy missle misses the player ship.

	if ((abs(loc[0] - mx) > 150 || abs(loc[1] - my) > 100 || abs(loc[1] - mz) > 150)) {
		fired = false;
		mx = loc[0];
		my = loc[1];
		mz = loc[2];
		
		printf("RELOAD\n");
	}

	if ((abs(loc_e[0] - mxe) > 150 || abs(loc_e[1] - mye) > 100 || abs(loc_e[1] - mze) > 150)) {
		firede = false;
		mxe = loc_e[0];
		mye = loc_e[1];
		mze = loc_e[2];

		printf("ENEMY RELOADED\n");
	}


	glutTimerFunc(10, animationHandler, 0);


	glPopMatrix();
	glutPostRedisplay();
	glutSwapBuffers();   // Double buffering, swap buffers
}

int main(int argc, char** argv)
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition(300, 60);
	glutCreateWindow("Zepplin");
	readOBJ();
	loadTexture();


	// Initialize GL
	initOpenGL(vWidth, vHeight);
	
	// Register callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(arrows);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseButtonHandler3D);
	glutMotionFunc(mouseMotionHandler3D);

	// Start event loop, never returns

	init3DSurfaceWindow();
	glutMainLoop();

	return 0;
}
