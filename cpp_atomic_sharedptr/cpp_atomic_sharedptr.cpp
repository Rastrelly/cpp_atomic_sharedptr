#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>
#include <GL/freeglut.h>
#include <mutex>
#include "mathMgr.h"

std::chrono::steady_clock::time_point lastUpdate = std::chrono::steady_clock::now();

//atomic variables exist unchanged for each system tick
std::atomic<float> deltaTime = 0;
std::atomic<float> timestamp = 0;
std::atomic<bool> exitFlag = false;
std::atomic<bool> pauseInput = false;

int cwx = 800; int cwy = 600;

//mutexes block access to data we don't want to be accesses when in works
std::mutex locker1;
std::mutex locker2;

//shared pointers are protected by system
//and won't be deleted until last user is released
std::shared_ptr<mathMgr> mMgr = std::make_shared<mathMgr>();

//forward decs
void funcThr1();
void funcThr2(int argc, char **argv);
float getDeltaTime();

void cbIdle();
void cbReshape(int wx, int wy);
void cbDisplay();

//MAIN FUNC
int main(int argc, char **argv)
{

	std::thread thr1(funcThr1);
	std::thread thr2(funcThr2,argc,argv);

	while (true)
	{
		locker1.lock();
		if (!pauseInput)
		{
			mMgr->pushForw(timestamp); //appending current values
			deltaTime = getDeltaTime();
			timestamp = timestamp + deltaTime;
			if (timestamp > 1000000) timestamp = 0.0f;
		}
		if (exitFlag) break;
		locker1.unlock();
	}

	thr1.join();
	thr2.join();

}

void funcThr1()
{
	while (!exitFlag)
	{
		printf("Select action:\n0 - Inp refr interval\n1 - Inp A, F;\n2 - Exit\n");
		int choice;
		std::cin >> choice;
		switch (choice)
		{
		case 0:
		{
			locker1.lock();
			pauseInput = true;
			float xi = 0;;
			printf("Input XI\n");
			std::cin >> xi;
			mMgr->setXI(xi);
			pauseInput = false;
			locker1.unlock();
			break;
		}
		case 1:
		{
			locker1.lock();
			pauseInput = true;
			float a = 0; float f = 0;
			printf("Input A F\n");
			std::cin >> a >> f;
			mMgr->setA(a);
			mMgr->setF(f);
			pauseInput = false;
			locker1.unlock();
			break;
		}
		case 2:
		{
			exitFlag = true;
			break;
		}
		}
	}
}

void funcThr2(int argc, char **argv)
{
	//GLUT system init block
	glutInit(&argc, argv); //initializing GLUT

	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	glutInitWindowSize(cwx, cwy);

	glutCreateWindow("Chart Display");

	//OpenGL setup block
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//GLUT launch block
	glutIdleFunc(cbIdle);
	glutReshapeFunc(cbReshape);
	glutDisplayFunc(cbDisplay);

	glutMainLoop();
}

float getDeltaTime()
{
	auto cm = std::chrono::steady_clock::now();
	auto pm = lastUpdate;
	std::chrono::duration<double, std::micro> fp_ms = cm - pm;
	float dt = fp_ms.count() / 1000000.0f;
	lastUpdate = cm;
	return dt;
}

void cbIdle()
{
	cbDisplay();
	if (exitFlag)
	{
		glutLeaveMainLoop();
	}
}

void cbReshape(int wx, int wy)
{
	glViewport(0, 0, wx, wy);
	cwx = wx; cwy = wy;
}
void cbDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//renew window size
	float orthoLeft = mMgr->getXMin();
	float orthoRight = mMgr->getXMax();
	float orthoBottom = mMgr->getYMin();
	float orthoTop = mMgr->getYMax();

	gluOrtho2D(orthoLeft,orthoRight,orthoTop,orthoBottom);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//draw stuff

	int l = mMgr->getMaxVSize();

	glBegin(GL_LINE_STRIP);
	glColor3f(1.0f,0.0f,0.0f);
	for (int i = 0; i < l; i++)
	{
		locker2.lock();
		glVertex2f(mMgr->getChartX(i), mMgr->getChartY(i));
		locker2.unlock();
	}
	glEnd();

	glutSwapBuffers();	
}