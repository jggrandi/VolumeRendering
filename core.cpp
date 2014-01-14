#include "core.h"


CGlutWindow *Core::c_pMainWindow=0;
unsigned int Core::fpsCurrent=0;
unsigned int Core::fpsCount=0;
Core::Core(int c_argc, char **c_argv)
{

	glutInit(&c_argc, c_argv);

	DATAINFO c_datasetInfo;
	PLANE_EQ c_planeInfo1;
	PLANE_EQ c_planeInfo2;

	int planesToRender=0;

	c_datasetInfo.inputFileName= c_argv[1];
	c_datasetInfo.resWidth	   = atoi(c_argv[2]);
	c_datasetInfo.resHeight	   = atoi(c_argv[3]);
	c_datasetInfo.initStack    = atoi(c_argv[4]);
	c_datasetInfo.endStack     = atoi(c_argv[5]);
	c_datasetInfo.resDepth     = c_datasetInfo.endStack - c_datasetInfo.initStack;
	
	if(c_argv[6] != NULL)
	{
		if(c_argc <= 9)
		{
			printf("Not enough parameters\n");
			planesToRender = -1;
		}
		else
			planesToRender++;
	}
	if(c_argv[10] != NULL)
	{
		if(c_argc <= 13)
		{
			printf("Not enough parameters\n");
			planesToRender = -1;
		}	
		else	
			planesToRender++;
	}

	if(planesToRender < 1)
	{
		c_pMainWindow = new CGlutWindow(c_datasetInfo);
		printf("Zero planes loaded...\n");
	}
	else if(planesToRender == 1)
	{
		c_planeInfo1.AX			   = atof(c_argv[6]);
		c_planeInfo1.BY			   = atof(c_argv[7]);
		c_planeInfo1.CZ			   = atof(c_argv[8]);	
		c_planeInfo1.D             = atof(c_argv[9]);
		c_pMainWindow = new CGlutWindow(c_datasetInfo, c_planeInfo1);
		printf("One plane loaded...\n");

	}
	else if(planesToRender == 2)
	{
		c_planeInfo1.AX			   = atof(c_argv[6]);
		c_planeInfo1.BY			   = atof(c_argv[7]);
		c_planeInfo1.CZ			   = atof(c_argv[8]);	
		c_planeInfo1.D             = atof(c_argv[9]);

		c_planeInfo2.AX			   = atof(c_argv[10]);
		c_planeInfo2.BY			   = atof(c_argv[11]);
		c_planeInfo2.CZ			   = atof(c_argv[12]);	
		c_planeInfo2.D             = atof(c_argv[13]);

		c_pMainWindow = new CGlutWindow(c_datasetInfo, c_planeInfo1, c_planeInfo2);
		printf("Two planes loaded...\n");
	}

}

Core::~Core(){}


void Core::run()
{
	glutDisplayFunc(display); 
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(move);
    glutTimerFunc(1000, countFPS, 1);
	glutIdleFunc(idle);
	glutMainLoop();
}


void Core::display(void)
{ 
	c_pMainWindow->renderFrame();
	fpsCurrent++;
}

void Core::idle(void)
{ 
	c_pMainWindow->idle();
}

void Core::reshape(int width, int height)
{ 
	c_pMainWindow->resize(width, height);
}

void Core::keyboard(unsigned char key, int x, int y)  
{ 
	c_pMainWindow->keyEvent(key,x,y);
}

void Core::mouse(int button, int state, int x, int y)
{ 
	c_pMainWindow->mouseButtonEvent(button, state,x,y);
}

void Core::move(int x, int y)                         
{ 
	c_pMainWindow->mouseMoveEvent(x,y);
}


void Core::countFPS(int value) {
	char title[120];
	fpsCount = fpsCurrent;
	fpsCurrent = 0;
	
	snprintf(title, 120, "%s. FPS: %d", "Image Match", fpsCount);
	glutSetWindowTitle(title);
	glutTimerFunc(1000, countFPS, 1);
}



