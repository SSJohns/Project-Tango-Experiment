// GraphicsOGL.cpp


#define GL_GLEXT_PROTOTYPES

#include <unistd.h>
#include <chrono>
#include <cctype>
#include <iostream>
#include <string>
#include "../tango_data.h"
#include "Texture.h"
#include "Image.h"
//#include "Camera.h"
#include "GraphicsOGL.h"
//#include "Shader.h"
#include "Font.h"
#include <sstream>
//#include "../IO/InputController.h"
//#include "ShaderController.h"
//#include "../Primitives/Drawable.h"
//#include "../Primitives/Updateable.h"
#include "FontController.h"
//#include "../Functions/Math2D.h"
#include <ctime>
#include <cmath>
#include "../Functions.h"
//#include "../Environment/Heightmap.h"
//#include "../Characters/Player.h"
using namespace std;
using namespace std::chrono;


float ttime = 0;

float calcLenX(float len, float dir) {
	return len*cos(dir/180.*3.14159);
}

float calcLenY(float len, float dir) {
	return len*sin(dir/180.*3.14159);
}


static const char kVertexShader[] =
    "attribute vec4 vPosition;\n"
	"attribute vec2 vCoords;\n"
	"uniform mat4 mvp;\n"
	"uniform vec4 vColor;\n"
	"varying vec4 pColor;\n"
	"varying vec2 glTexCoord;\n"
	"mat4 projectionMatrix = mat4( 2.0/1200.0, 0.0, 0.0, -1.0, 0.0, 2.0/1920.0, 0.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0); \n"
    "void main() {\n"
    "  gl_PointSize = 20.0;\n"
	"  gl_Position = vPosition;\n"
	"  glTexCoord = vCoords;\n"
	"  pColor = vColor;\n"
    //"  gl_Position.x = gl_Position.x/1920. - .5;\n"
	//"  gl_Position.y = gl_Position.y/1200. - .5;\n"
    //"  gl_Position *= projectionMatrix;\n"
    "}\n";

static const char kFragmentShader[] =
	"uniform sampler2D tex;\n"
	"uniform int iTexOn;\n"
	"varying vec4 pColor;\n"
	"varying vec2 glTexCoord;\n"
    "void main() {\n"
	"  if(iTexOn)\n"
    "  		gl_FragColor = pColor*texture2D(tex, glTexCoord, 0.);\n"
	"  else\n"
	"		gl_FragColor = pColor;"
    "}\n";

static const glm::mat4 inverse_z_mat = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                                                 0.0f, -1.0f, 0.0f, 0.0f,
                                                 0.0f, 0.0f, -1.0f, 0.0f,
                                                 0.0f, 0.0f, 0.0f, 1.0f);




bool isDrawing = false;

//Player* p;
//Texture* GraphicsOGL::tst;


GraphicsOGL :: GraphicsOGL() {
	resolution = new float[2];

	SCREEN_WIDTH = resolution[0] = 1920;
	SCREEN_HEIGHT = resolution[1] = 1200;

	curProgram = 0;
	globalTime = 0;
	isTextureEnabled = 1;



	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	shader_program_ = GlUtil::CreateProgram(kVertexShader, kFragmentShader);

	uniform_mvp_mat_ = glGetUniformLocation(shader_program_, "mvp");
	uniform_color_ = glGetUniformLocation(shader_program_, "vColor");
	attrib_vertices_ = glGetAttribLocation(shader_program_, "vPosition");
	attrib_texcoords_ = glGetAttribLocation(shader_program_, "vCoords");
	glGenBuffers(1, &vertex_buffers_);

	initialize3D();
}

GraphicsOGL :: ~GraphicsOGL() {
	delete [] resolution;
}

void GraphicsOGL :: initialize3D() {
/*
	//Initialize OpenGL
	glutInit(&argc, argv);

	//Initialize OpenGL Window
	glutInitDisplayMode(GLUT_DEPTH|GLUT_SINGLE|GLUT_RGBA); 
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("OpenGL Game"); 	

	//Set Up View Port for Window
	glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
	glOrtho(0,SCREEN_WIDTH,SCREEN_HEIGHT,0, -3,1000);
 	 */
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_ALPHA_TEST);
	glEnable(GL_TEXTURE_2D);
	/*


		p = new Player(0,0,0);
		//curHeightmap = new Heightmap(1028,1028,1/32.);
		curHeightmap = new Heightmap(2048,2048,"Resources/Images/test.png");
		tst = new Texture("Resources/Images/test.png",false);
		//Load Resources, Create GraphicsOGL's Objects
		glCamera = new Camera();
		inputController = new InputController();
		shaderController = new ShaderController();

	//Set Up OpenGL Callbacks (Updating Functions...)
	glutIdleFunc(idleCallback);
	glutDisplayFunc(displayCallback);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//Start up Main Loop
	glutMainLoop();*/

	fontController = new FontController();
}

void GraphicsOGL :: update3D(glm::mat4 projection_mat, glm::mat4 view_mat) {
	mvp_mat = projection_mat * view_mat * inverse_z_mat; //projection_mat * view_mat * model_mat * inverse_z_mat;

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

//Get Screen Width
int GraphicsOGL :: getScreenWidth() {
	return SCREEN_WIDTH;
}

//Get Screen Height
int GraphicsOGL :: getScreenHeight() {
	return SCREEN_HEIGHT;
}


void GraphicsOGL :: display() {


	/*drawStart = getTime();

	


	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
 	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); 
	glAlphaFunc(GL_GREATER, 0);


	setFont("8bit");

	globalTime += 1;


	
	glColor3f(1,1,1);


	setPerspective();
		draw3DWall(-16,0,32,16,0,0,NULL);

		//enableShader("GalaxyTexture");	
		Drawable2 :: drawAll(this, 1);
	setOrtho();

			fillPolygon(getMouseX(),getMouseY(),30,3, globalTime);
			//fillRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
		//disableShaders();

		string fpsStr = "FPS ", dirStr = "Dir ";
			fpsStr = fpsStr + to_string(fps);
			dirStr = dirStr + to_string(getCamDir());

		drawStringScaled(0,0,.65,.65,fpsStr);
		drawStringScaled(0,20,.65,.65,dirStr);

	glFlush(); 
    	glutSwapBuffers();


	drawEnd = getTime();*/
}

//CAMERA FUNCTIONS
	/*float GraphicsOGL :: getCamDir() {
		return glCamera->getCamDir();
	}

	void GraphicsOGL :: setProjectionPrep(float cX, float cY, float cZ, float tX, float tY, float tZ) {
		glCamera->setProjectionPrep(cX,cY,cZ,tX,tY,tZ);
	}*/

//DRAWING FUNCTIONS
	void GraphicsOGL :: setColor(float R, float G, float B, float A) {
		drawColor = glm::vec4(R, G, B, A);
	}

	void GraphicsOGL :: clearScreen() {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	}

	/*void GraphicsOGL :: drawPoint(float x, float y) {
		float depth = 0;
	
		glBegin(GL_POINTS);
			glVertex3f(x, y, depth);
		glEnd();
	}

	void GraphicsOGL :: drawLine(float x1, float y1, float x2, float y2) {
		float depth = 0;
	
		glBegin(GL_LINES);
			glVertex3f(x1, y1, depth);
			glVertex3f(x2, y2, depth);
		glEnd();
	}

	void GraphicsOGL :: drawRect(float x1, float y1, float x2, float y2) {
		float depth = 0;
	
		glBegin(GL_LINE_LOOP);
			glTexCoord2f(0,0);
				glVertex3f(x1, y1, depth);
			glTexCoord2f(1,0);
				glVertex3f(x2, y1, depth);
			glTexCoord2f(1,1);
				glVertex3f(x2, y2, depth);
			glTexCoord2f(0,1);
				glVertex3f(x1, y2, depth);
		glEnd();
	}

	void GraphicsOGL :: fillRect(float x1, float y1, float x2, float y2) {
		float depth = 0;
	
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);
				glVertex3f(x1, y1, depth);
			glTexCoord2f(1,0);
				glVertex3f(x2, y1, depth);
			glTexCoord2f(1,1);
				glVertex3f(x2, y2, depth);
			glTexCoord2f(0,1);
				glVertex3f(x1, y2, depth);
		glEnd();
	}*/

	void GraphicsOGL :: drawCircle(float x, float y, float r) {

		//drawPolygon(x, y, r, 100, 0);
	}

	void GraphicsOGL :: fillCircle(float x, float y, float r) {

		fillPolygon(x, y, r, 100, 0);
	}

	/*void GraphicsOGL :: drawPolygon(float x, float y, float r, int vertNum, float angle) {
		float depth = 0, dir, xN, yN;
	
		glBegin(GL_LINE_LOOP);
			for(float i = 0; i < vertNum; i++) {
				dir = angle + i/vertNum*360;
				xN = calcLenX(1,dir);
				yN = -calcLenY(1,dir);
	
				glTexCoord2f(xN,yN);
					glVertex3f(x + r*xN, y + r*yN, depth);
			}
		glEnd();
	}*/

	void GraphicsOGL :: fillPolygon(float x, float y, float r, int vertNum, float angle) {
		y += 100;


		int numPts = vertNum+2;
		float dir, xN, yN, vX, vY;

		GLfloat vVertices[numPts*3];
		GLfloat tCoords[numPts*2];

		tCoords[0] = .5; tCoords[1] = .5;
		vVertices[0] = x; vVertices[1] = y; vVertices[2] = 0;
		for(int i = 0; i < vertNum+1; i++) {
			dir = angle + (i + 0.)/vertNum*360.;
			xN = calcLenX(1,dir);
			yN = -calcLenY(1,dir);

			tCoords[2 + 2*i] = xN;
			tCoords[2 + 2*i + 1] = yN;

			vVertices[3 + 3*i] = x + r*xN;
			vVertices[3 + 3*i + 1] = y + r*yN;
			vVertices[3 + 3*i + 2] = 0;
		}

		for(int i = 0; i < numPts*3; i += 3) {
			vVertices[i] = (vVertices[i]/1920.*2 - 1.);
			vVertices[i+1] = -(vVertices[i+1]/1200.*2 - 1.);
		}


		glUseProgram(shader_program_);

		glUniform1i(glGetUniformLocation(shader_program_,"iTexOn"),isTextureEnabled);

		// Set Drawing Color
		glUniform4fv(uniform_color_, 1, glm::value_ptr(drawColor));
		glUniformMatrix4fv(uniform_mvp_mat_, 1, GL_FALSE, glm::value_ptr(mvp_mat));

		// Bind vertex buffer.
		glVertexAttribPointer(attrib_vertices_, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
		glEnableVertexAttribArray(attrib_vertices_);
		glVertexAttribPointer(attrib_texcoords_, 2, GL_FLOAT, GL_FALSE, 0, tCoords);
		glEnableVertexAttribArray(attrib_texcoords_);


		glDrawArrays(GL_TRIANGLE_FAN, 0, numPts);

		glUseProgram(0);
	}

	void GraphicsOGL :: drawTexture(float x, float y, Texture* tex) {
		
		if(tex == NULL)
			return;

		drawTextureScaled(x, y, 1, 1, tex);
	}

	void GraphicsOGL :: drawTextureScaled(float x, float y, float xS, float yS, Texture* tex) {

		if(tex == NULL)
			return;

		glDisable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_2D);
		tex->bind(GL_TEXTURE0);
		isTextureEnabled = true;

		glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


		//glOrtho(0,640,480,0,-1000,1000);

		float x1, y1, x2, y2;
		float depth = 0, w, h;
		w = xS*tex->getWidth();
		h = yS*tex->getHeight();

		//y += 152;

		x1 = x;
		y1 = y;
		x2 = x + w;
		y2 = (y + h);

		x1 = (x1/1920.*2 - 1.);
		x2 = (x2/1920.*2 - 1.);
		y1 = -(y1/1200.*2 - 1.);
		y2 = -(y2/1200.*2 - 1.);

		/*GLfloat vVertices[] = {
				x,		y, 		depth,
				x,		y+h,	depth,
				x+w,	y,		depth
				//x+w,	y+h,	depth
		};*/

		GLfloat vVertices[] = {
				x1, y1, 0.f,
				x2, y1, 0.f,
				x2, y2, 0.f,

				x1, y1, 0.f,
				x2, y2, 0.f,
				x1, y2, 0.f
		};

		GLfloat tCoords[] = {
				0.f, 0.f,
				1.f, 0.f,
				1.f, 1.f,

				0.f, 0.f,
				1.f, 1.f,
				0.f, 1.f
		};




		glUseProgram(shader_program_);

		glUniform1i(glGetUniformLocation(shader_program_,"iTexOn"),isTextureEnabled);

		// Set Drawing Color
		glUniform4fv(uniform_color_, 1, glm::value_ptr(drawColor));
		glUniformMatrix4fv(uniform_mvp_mat_, 1, GL_FALSE, glm::value_ptr(mvp_mat));

		// Bind vertex buffer.
		glVertexAttribPointer(attrib_vertices_, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
		glEnableVertexAttribArray(attrib_vertices_);
		glVertexAttribPointer(attrib_texcoords_, 2, GL_FLOAT, GL_FALSE, 0, tCoords);
		glEnableVertexAttribArray(attrib_texcoords_);


		glDrawArrays(GL_TRIANGLES, 0, 6);

		glUseProgram(0);

		isTextureEnabled = false;
		glBindTexture(GL_TEXTURE0, 0);
		glDisable(GL_TEXTURE_2D);
	}

//3D DRAWING

	/*void GraphicsOGL :: draw3DWall(float x1, float y1, float z1, float x2, float y2, float z2, Texture* tex) {

			if(tex != NULL) {
				glEnable(GL_TEXTURE_2D);
				
				tex->bind();
			}
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
						
			
			glBegin(GL_QUADS);
				glTexCoord2d(0.0, 0.0); 	
					glVertex3d(x1, y1, z1);
				glTexCoord2d(1.0, 0.0);
					glVertex3d(x2, y2, z1);
				glTexCoord2d(1.0, 1.0);
					glVertex3d(x2, y2, z2);
				glTexCoord2d(0.0, 1.0);
					glVertex3d(x1, y1, z2);
			glEnd();
			
			//glLoadIdentity();
			
			
			//glColor4f(1f, 1f, 1f, 1f);

		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void GraphicsOGL :: draw3DFloor(float x1, float y1, float x2, float y2, float z, Texture* tex) {

			if(tex != NULL) {
				glEnable(GL_TEXTURE_2D);
				
				tex->bind();

				cout << "What" << endl;
			}
			
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
						
			
			glBegin(GL_QUADS);
				glTexCoord2d(0.0, 0.0);
					glVertex3d(x1, y1, z);
				glTexCoord2d(1.0, 0.0);
					glVertex3d(x2, y1, z);
				glTexCoord2d(1.0, 1.0);
					glVertex3d(x2, y2, z);
				glTexCoord2d(0.0, 1.0);
					glVertex3d(x1, y2, z);
			glEnd();
			
			
			//glLoadIdentity();
			
			
			//glColor4f(1f, 1f, 1f, 1f);

		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}*/

//SHADERS

	/*void GraphicsOGL :: disableShaders() {
		glUseProgram(0);
	}

	void GraphicsOGL :: enableShader(string name) {
		enableShader(shaderController->getProgram(name));
	}

	void GraphicsOGL :: enableShader(GLuint program) {
		glUseProgram(program);

		glUniform2fv(glGetUniformLocation(program, "iResolution"), 1, resolution);			
    		glUniform1f(glGetUniformLocation(program, "iGlobalTime"), globalTime/50.);      
		glUniform1f(glGetUniformLocation(program, "iRadius"), 3);        
	}*/

	void GraphicsOGL :: setCurrentTextureSize(int w, int h) {
		if(curProgram == 0)
			return;

		float si[2];
		si[0] = w;
		si[1] = h;

		glUniform2fv(glGetUniformLocation(curProgram, "iSize"), 1, si);				
	}

//TEXT DRAWING
	void GraphicsOGL :: setFont(string name) {
		curFont = fontController->getFont(name);
	}

	float GraphicsOGL :: drawChar(float x, float y, char c) {
		return drawCharScaled(x, y, 1, 1, c);
	}

	float GraphicsOGL :: drawCharScaled(float x, float y, float xS, float yS, char c) {
		Texture* t = curFont->getChar(c);

		if(t == NULL)
			return 0;

		if(c == 'g' || c == 'p' || c == 'q' || c == 'j' || c == 'y')
			y += yS*t->getHeight()*.25;

		drawTextureScaled(x, y, xS, yS, t);
		return t->getWidth()*xS;
	}

	void GraphicsOGL :: drawString(float x, float y, string str) {
		drawStringScaled(x,y,1,1,str);
	}

	void GraphicsOGL :: drawStringScaled(float x, float y, float xS, float yS, string str) {
		int len = str.length();
		char c;

		float s = 8, e = -1*xS;

		float dX = x, dY = y;

		for(int i = 0; i < len; i++) {
			c = str[i];

			if(c == '\n') { 
				dY += s*yS + 1;
				dX = x;
			}
			else if(c == ' ')
				dX += s*xS + e;
			else if(islower(c))
				dX += drawCharScaled(dX,dY + (s*yS*.25), xS,yS*.75, c) + e;
			else
				dX += drawCharScaled(dX,dY, xS, yS, c) + e;
		}
	}

//Graphics Mode
/*void GraphicsOGL :: setOrtho() {

	int SCREEN_WIDTH = 1920, SCREEN_HEIGHT = 1200;

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1000, 1000);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH);
}*/

/*void GraphicsOGL :: setPerspective() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glCamera->setProjection(this);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH);
}*/

unsigned long GraphicsOGL :: getTime() {

	nanoseconds ms = duration_cast< nanoseconds >(
 		high_resolution_clock::now().time_since_epoch()
	);

	return ms.count();
}
