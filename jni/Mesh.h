// Mesh.h

#ifndef MESH_H
#define MESH_H

#include "tango-gl-renderer/axis.h"
#include "tango-gl-renderer/camera.h"
#include "tango-gl-renderer/frustum.h"
#include "tango-gl-renderer/gl_util.h"
#include "tango-gl-renderer/grid.h"
#include "tango-gl-renderer/transform.h"
#include "tango_data.h"
#include "Renderable.h"
#include "Graphics/GraphicsOGL.h"
#include <cstdio>

class TangoData;
class Mesh : public Renderable {

	public:
		Mesh(int);
		~Mesh();
		void Render(GraphicsOGL*, glm::mat4, glm::mat4);
		void clear();
		int getPointNumber();		// Get Number of Points in Mesh
		int getPointCloudNumber();
		int getMaxCloudPoints();
		bool isIOOpen();
		bool isSaving();
		bool isLoading();
		int saveToFile(GraphicsOGL*, string);
		int continueSaving(GraphicsOGL*);
		int loadFile(GraphicsOGL*, string);
		int continueLoading(GraphicsOGL*);
		void addPoints(float*, int, const glm::mat4);
		static int getCurCloud();

	private:
		void drawPoints(const glm::mat4& projection_mat, const glm::mat4& view_mat) const;

		static int curCloud;
		int numClouds;
		float** pointArray;
		int* pointArraySize;
		int maxPointNum;
		int resScale;
		int ioPos;
		FILE* curFile;
		int ioProcess;
};

#endif
