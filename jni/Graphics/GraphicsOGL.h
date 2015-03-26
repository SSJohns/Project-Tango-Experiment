// GraphicsOGL.h

#ifndef GRAPHICSOGL_H
#define GRAPHICSOGL_H

#include "../tango_data.h"
#include "Texture.h"
#include "Font.h"
//#include "../IO/InputController.h"
//#include "ShaderController.h"
#include "FontController.h"
//#include "Camera.h"
//#include "../Environment/Heightmap.h"


//class Camera;
//class Heightmap;
class GraphicsOGL {
	public:
		GraphicsOGL();
		~GraphicsOGL();

		void idle();
		void display();

		void update3D(glm::mat4, glm::mat4);

		//ACCESSOR
			int getScreenWidth();
			int getScreenHeight();
			int getMouseX();
			int getMouseY();
			bool getShift();
			float getWASDDir();

		//CAMERA
			float getCamDir();
			void setProjectionPrep(float,float,float,float,float,float);
		
		//DRAWING
			void setColor(float,float,float,float);
			void clearScreen();
			void drawPoint(float,float);
			void drawLine(float,float,float,float);
			void drawRect(float,float,float,float);
			void fillRect(float,float,float,float);
			void drawCircle(float,float,float,int);
			void fillCircle(float,float,float,int);
			void drawPolygon(float,float,float,int,float);
			void fillPolygon(float,float,float,int,float);

		//3D DRAWING

			void draw3DWall(float,float,float,float,float,float, Texture*);
			void draw3DFloor(float,float,float,float,float, Texture*);

			void drawTexture(float, float, Texture*);
			void drawTextureScaled(float, float, float, float, Texture*);
			float drawChar(float, float, char);
			float drawCharScaled(float, float, float, float, char);
			void drawString(float, float, string);
			void drawStringScaled(float, float, float, float, string);

		//SHADERS
			void disableShaders();
			void enableShader(GLuint);
			void enableShader(string);
			void setCurrentTextureSize(int,int);

		//FONT
			void setFont(string);

		//HEIGHTMAP
			//Heightmap* getHeightmap();

		//PROJECTION
			void setOrtho();
			void setPerspective();

		//TIME
			unsigned long getTime();

	private:
		GLuint vertex_buffers_;
		GLuint shader_program_;
		GLuint attrib_vertices_;
		GLuint attrib_texcoords_;
		GLuint uniform_mvp_mat_;
		GLuint uniform_color_;

		void initialize3D();

		glm::mat4 mvp_mat;
		glm::vec4 drawColor;

		int SCREEN_WIDTH;
		int SCREEN_HEIGHT;

		//FPS Variables
			unsigned long fpsStart, fpsEnd;
			unsigned long drawStart, drawEnd;
			int fps;

		//Heightmap Variables
			//Heightmap* curHeightmap;

		//Input Variables
			//InputController* inputController;

		//Shader Variables
			//ShaderController* shaderController;
			GLuint curProgram;
			float* resolution;
			float globalTime;

		//Font Variables
			FontController* fontController;
			Font2D* curFont;

		//Camera* glCamera;

		static Texture* tst;
};


#endif
