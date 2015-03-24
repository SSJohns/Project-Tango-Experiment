// Camera.cpp


#include <GL/freeglut.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include "../Functions/Math2D.h"
#include "GraphicsOGL.h"
#include "Camera.h"



Camera :: Camera() {
	camX = camY = camZ = toX = toY = toZ = newCamX = newCamY = newCamZ = camDir = 0;
}
		
void Camera :: setProjectionPrep(float cX, float cY, float cZ, float tX, float tY, float tZ) {
	newCamX = cX;
	newCamY = cY;
	newCamZ = cZ;
	toX = tX;
	toY = tY;
	toZ = tZ;
	
	camDir = calcPtDir(newCamX, newCamY, toX, toY);
}
		
float Camera :: getCamDir() {
	return camDir;
}
		
void Camera :: setProjection(GraphicsOGL* gl) {
	float cX = newCamX, cY = newCamY, cZ = newCamZ;

	float camSpeed = 5;
	float oldCZ, oldToZ, cZD, toZD;
	oldCZ = camZ;	oldToZ = toZ;
	cZD = (cZ - camZ)/(2*camSpeed);
	toZD = (toZ - focusZ)/(2*camSpeed);
	camZ += cZD;
	focusZ += toZD;
	
	//Update Listener Source
	/*float n, nX, nY, nZ;
	n = (float) (Math.sqrt(Math2D.sqr(toX-cX) + Math2D.sqr(toY-cY) + Math2D.sqr(focusZ-camZ)));
	nX = (toX-cX)/n;
	nY = (toY-cY)/n;
	nZ = (focusZ-camZ)/n;*/
	
	//SoundController.updateListener(cX,cY,camZ, cX-camX,cY-camY,cZD, nX,nY,nZ, 0,0,1);
		
	
	// Change to projection matrix.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Perspective.
	float widthHeightRatio = (float) gl->getScreenWidth() / (float) gl->getScreenHeight();
	gluPerspective(45, widthHeightRatio, 1, 1000);
	gluLookAt(cX, cY, camZ, toX, toY, focusZ, 0, 0, 1);

	// Move Background!
	/*float add, d;
	add = Math2D.calcProjDis(cX-camX, cY-camY, Math2D.lengthdirX(1,camDir+90),Math2D.lengthdirY(1,camDir+90));
	d = (Math2D.angleDifference(Math2D.pointDirection(camX, camY, cX, cY), camDir) > 0) ? 1 : -1;
	bgX += .25*d*add;*/
		camX = cX;	focusX = camX;
		camY = cY;	focusY = camY;
		camZ = cZ;

	camDir = calcPtDir(camX,camY,toX,toY);

	// Change back to model view matrix.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();	
}
