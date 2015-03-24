// Camera.h


#ifndef CAMERA_H
#define CAMERA_H

#include "GraphicsOGL.h"
 	
class GraphicsOGL;
class Camera {
	public:
		Camera();
		float getCamDir();		
		void setProjection(GraphicsOGL*);
		void setProjectionPrep(float, float, float, float, float, float); 	

	private:

		float camX;
		float camY;
		float camZ;
		float focusX;
		float focusY;
		float focusZ;
		float newCamX;
		float newCamY;
		float newCamZ;
		float toX;
		float toY;
		float toZ;
		float camDir;
};

#endif
