#pragma once

#include <fstream>

#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include "TransferFunction.h"
#include "mathutil/CVector.h"
#include "mathutil/CPosition.h"

#include <cstdlib>
#include <cstdio>
#include "utils.h"
#include <assert.h>

class CArcBall;

class CGlutWindow
{
public:
	CGlutWindow();
	CGlutWindow(DATAINFO dInfo);
	CGlutWindow(DATAINFO dInfo, PLANE_EQ dPlane);
	CGlutWindow(DATAINFO dInfo, PLANE_EQ dPlane1, PLANE_EQ dPlane2);
	~CGlutWindow(void);

	enum {
		MOVE_CAMERA,
		MOVE_LIGHT,
	};

	enum {
		MBUTTON_LEFT   = 0x01, 
		MBUTTON_MIDDLE = 0x02,
		MBUTTON_RIGHT  = 0x04,
		KBUTTON_CTRL   = 0x08,
		KBUTTON_SHIFT  = 0x10
	};

	virtual void cgRenderGeometry();

	void renderFrame();
	void idle();
	void resize(int width, int height);
	void keyEvent(unsigned char key,int x,int y);
	void mouseButtonEvent(int button, int state,int x,int y);
	void mouseMoveEvent(int x,int y);
	void renderGeometry();
	bool handleButtonEvent(int button, int state, int x, int y);
	bool handleMoveEvent(int x, int y);

protected:
    //
    DATAINFO m_datasetInfo;
    PLANE_EQ m_planeInfo1;
    PLANE_EQ m_planeInfo2;

	int      m_nMode;
	bool	 m_showGrid; 
	bool	 m_showAxis;
	bool     m_bDisplayTF;
	bool	 m_drawPlane1;
	bool	 m_drawPlane2;
	bool	 m_frame;
	bool     m_plane;
	float 	 m_blend;
	double	 m_changeVolumeSide;
	GLuint   m_pTextureIds[3];
	int      m_nNumSlices;
	CVector  m_pVertices[8];
	Edge     m_pEdges[12];

	CGprogram *m_pVertexPrograms;
	CGprogram *m_pFragmentPrograms;
	//

	int m_nWidth, m_nHeight;
	double m_dFieldOfView;
	double m_dCenter, m_dRadius, m_dZoom;
	
	CArcBall* m_pCameraArcball;
	CArcBall* m_pLightArcball;

	CPosition m_vecMouseDown;
	bool m_bDragging;
	bool m_bZooming;
	bool m_bRotating;
	bool m_bExternalMouseHandling;
	
	CVector m_vecCameraPosition;
	double m_dTranslateX, m_dTranslateY;

	int m_nMouseState;
	int m_nInteractionMode;

	void initializeAll();
	void initializeAppParameters();
	void initializeGL();
	bool loadTextures();
	bool createPrograms();

	void defaultGrid(float gridSize, float gridQuadSpacing);
	void defaultAxis(float axisSize, float axisLineWidth);
	
	CTransferFunction *m_pTransferFunction;

	// clipping planes
	GLdouble eqn1[4];

private:
 	CGcontext s_CgContext;
	CGprofile s_vertexProfile, s_fragmentProfile;
	
	
	void chooseProfiles();
	

	virtual void initializeCg();	

};



