/*
 * Copyright 2014 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Functions.h"
#include "Mesh.h"
#include "Renderable.h"
#include "Graphics/GraphicsOGL.h"
#include <stdio.h>

#define IO_NULL -1
#define IO_SAVE 0
#define IO_LOAD 1


static const glm::mat4 inverse_z_mat = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                                                 0.0f, -1.0f, 0.0f, 0.0f,
                                                 0.0f, 0.0f, -1.0f, 0.0f,
                                                 0.0f, 0.0f, 0.0f, 1.0f);

static bool isDrawing = false, isAdding = false;

int Mesh :: curCloud = 0;

Mesh :: Mesh(int maxPoints) : Renderable() {
	maxPointNum = maxPoints;


	curCloud = 0;
	numClouds = 1000;
	resScale = 5;//00;

	ioPos = -1;
	ioProcess = IO_NULL;


	//Initialize Point Array
	pointArray = new float*[numClouds];
	pointArraySize = new int[numClouds];

	// Initialize Point Cloud Arrays and Point Cloud Array Size Array
	for(int i = 0; i < numClouds; i++) {
		pointArray[i] = new float[0];
		pointArraySize[i] = 0;
	}
}

Mesh :: ~Mesh() {
	delete [] pointArray;
}

void Mesh :: clear() {

	if(isIOOpen())
		return;

	curCloud = 0;
	for(int i = 0; i < numClouds; i++) {
		delete [] pointArray[i];

		// Find Size of Current Point Cloud
		pointArraySize[i] = 0;

		// Create New Array for Current Cloud
		pointArray[i] = new float[0];
	}
}

void Mesh :: setResolution(int newRes) {
	resScale = newRes;
}

bool Mesh :: isIOOpen() {
	return (ioProcess != IO_NULL);
}

bool Mesh :: isSaving() {
	return (ioProcess == IO_SAVE);
}

bool Mesh :: isLoading() {
	return (ioProcess == IO_LOAD);
}

int Mesh :: saveToFile(GraphicsOGL* gl, string fileName) {

	if(isIOOpen())
		return true;

	string fN = "/sdcard/";
		fN = fN + fileName;

	// Open File for Writing
	curFile = fopen(fN.c_str(), "w");

	// If File Could Not be Opened, Return False
	if(curFile == NULL)
		return false;

	ioPos = 0;
	ioProcess = IO_SAVE;

	return true;
}

int Mesh :: continueSaving(GraphicsOGL* gl) {

	if(!isSaving())
		return false;

	float bX, bY, bW,bH;
	bX = bW = gl->getScreenWidth()*.333;
	bY = bH = gl->getScreenHeight()*.333;

	// Print all Point Clouds to File
		if(ioPos == 0)
			fprintf(curFile, "%i\n", curCloud);

		int endPos = min(ioPos+2,curCloud);

		int i, k;
		for(k = 0; ioPos < endPos; ioPos++) {

			if(ioPos+1 >= curCloud) {
				ioPos = -1;
				ioProcess = IO_NULL;
				fprintf(curFile, "%i\n", -1);
				fclose(curFile);
				curFile = NULL;
				return true;
			}

			if(ioPos+1 == endPos) {
				float perc = 1.*ioPos/curCloud;
				string percStr = "Saving " + to_string(100*perc) + " %";
				string progStr = "Cloud " + to_string(ioPos) + " / " + to_string(curCloud);
				string workStr = "";
					workStr += "Point Number: " + to_string(pointArraySize[ioPos]/3) + "\n";
					workStr += "Memory Size: " + to_string(1.*pointArraySize[ioPos]*4/(1024*1024)) + "MB \n";

				gl->setColor(.2,.2,.2,.85);
				gl->fillRect(bX,bY,bW,bH);
				gl->setColor(1,1,1,1);
				gl->drawRect(bX,bY,bW,bH);
				gl->setColor(0,0,0,1);
				gl->fillRect(bX+25,bY+100, bW-50,30);
				gl->setColor(1,1,1,1);
				gl->fillRect(bX+25,bY+100, perc*(bW-50),30);
				gl->drawStringCentered(bX+bW*.5,bY+50,2,2,percStr);
				gl->drawStringCentered(bX+bW*.5,bY+150,1.5,1.5,progStr);
				gl->drawStringScaled(bX+40,bY+200,1.4,1.4,workStr);
			}

			fprintf(curFile, "%i\n", pointArraySize[ioPos]);

			// If Current Point Cloud Empty, Skip
			if(pointArraySize[ioPos] == 0)
				continue;

			// Print Points as x y z Data
			for(int l = 0; l < pointArraySize[ioPos]; l += 3, k++)
				fprintf(curFile, "%i %f %f %f\n", ioPos, pointArray[ioPos][l], pointArray[ioPos][l+1], pointArray[ioPos][l+2]);
		}
}

int Mesh :: loadFile(GraphicsOGL* gl, string fileName) {

	if(isIOOpen())
			return false;

	string fN = "/sdcard/";
		fN = fN + fileName;


	// Open File for Reading
	curFile = fopen(fN.c_str(), "r");

	// If File Could Not be Opened, Return False
	if(curFile == NULL)
		return false;

	clear();
	ioPos = 0;
	ioProcess = IO_LOAD;

	return true;
}

int Mesh :: continueLoading(GraphicsOGL* gl) {

	if(!isLoading())
			return false;

	float bX, bY, bW,bH;
	bX = bW = gl->getScreenWidth()*.333;
	bY = bH = gl->getScreenHeight()*.333;

	// Print all Point Clouds to File
		if(ioPos == 0)
			fscanf(curFile, "%i\n", &curCloud);

		int endPos = min(ioPos+2,curCloud);

		int i, k;
		for(k = 0; ioPos < endPos; ioPos++) {

			if(ioPos+1 >= curCloud) {
				ioPos = -1;
				ioProcess = IO_NULL;
				fclose(curFile);
				curFile = NULL;
				return true;
			}

			if(ioPos+1 == endPos) {
				float perc = 1.*ioPos/curCloud;
				string percStr = "Loading: " + to_string(100*perc) + " %";

				gl->setColor(.2,.2,.2,.85);
				gl->fillRect(bX,bY,bW,bH);
				gl->setColor(0,0,0,1);
				gl->fillRect(bX+25,bY+100, bW-50,30);
				gl->setColor(1,1,1,1);
				gl->fillRect(bX+25,bY+100, perc*(bW-50),30);
				gl->drawStringCentered(bX+bW*.5,bY+50,2,2,percStr);
			}

			int curN;
			fscanf(curFile, "%i\n", &curN);

			if(curN == -1) {
				ioPos = -1;
				ioProcess = IO_NULL;
				fclose(curFile);
				curFile = NULL;
				return true;
				break;
			}

			delete [] pointArray[ioPos];
			pointArraySize[ioPos] = curN;
			pointArray[ioPos] = new float[pointArraySize[ioPos]];

			// Print Points as x y z Data
			for(int l = 0; l < pointArraySize[ioPos]; l += 3, k++) {
				int curI;
				float curX, curY, curZ;
				fscanf(curFile, "%i %f %f %f\n", &curI, &curX, &curY, &curZ);
				pointArray[ioPos][l] = curX;
				pointArray[ioPos][l+1] = curY;
				pointArray[ioPos][l+2] = curZ;
			}
		}

	return true;
}


int Mesh :: getCurCloud() {
	return curCloud;
}

void Mesh :: addPoints(float* depthBuffer, int depthBufferSize, const glm::mat4 model_mat) {

	if(isIOOpen() || isDrawing || isAdding || depthBufferSize < 100)
		return;

	delete [] pointArray[curCloud];

	isAdding = true;


	// Find Size of Current Point Cloud
	pointArraySize[curCloud] = 0;

	if(resScale == 1)
		pointArraySize[curCloud] = depthBufferSize;
	else
		for(int i = 0; i < depthBufferSize; i += 3*resScale)
			pointArraySize[curCloud] += 3;


	// Get Model Matrix
	glm::mat4 curMat = model_mat * inverse_z_mat;
	glm::vec4 curVec;


	// Create New Array for Current Cloud
	pointArray[curCloud] = new float[pointArraySize[curCloud]];


	// Add Points to Cloud
	int k = 0;
	for(int i = 0; i < depthBufferSize; i += 3*resScale) {

		// Get Point Vector
		curVec = glm::vec4(depthBuffer[i],depthBuffer[i+1],depthBuffer[i+2],1);

		// Multiply Position by Model Matrix to Move it Into Position
		curVec = curMat*curVec;

		// Add Point to Cloud
		pointArray[curCloud][k] = curVec[0];
		pointArray[curCloud][k+1] = curVec[1];
		pointArray[curCloud][k+2] = curVec[2];

		// Increment to Next Point Position
		k += 3;
	}


	// Increment Through Clouds
	curCloud++;
	if(curCloud >= numClouds)
		curCloud = 0;

	isAdding = false;
}

int Mesh :: getPointNumber() {
	int number = 0;

	for(int i = 0; i < numClouds; i++)
		number += pointArraySize[i];

	return number;
}

int Mesh :: getPointCloudNumber() {

	return numClouds;
}

int Mesh :: getMaxCloudPoints() {

	return maxPointNum;
}

void Mesh :: Render(GraphicsOGL* gl, glm::mat4 projection_mat, glm::mat4 view_mat) {
	//isDrawing = true;

	Renderable :: Render(projection_mat, view_mat);

	// Calculate model view projection matrix.
	glm::mat4 mvp_mat = projection_mat * view_mat;


	glUseProgram(shader_program_);

	GLint ptSize = glGetUniformLocation(shader_program_, "pointSize");

	// Lock xyz_ij mutex.
	pthread_mutex_lock(&TangoData::GetInstance().xyzij_mutex);


	for(int i = 0; i < curCloud; i++) {

		if(pointArraySize[i] == 0)
			continue;

		if(ioPos != -1 && abs(i - ioPos) < 2)
			glUniform1f(ptSize, 2.);
		else
			glUniform1f(ptSize, 4.);

		glUniformMatrix4fv(uniform_mvp_mat_, 1, GL_FALSE, glm::value_ptr(mvp_mat));

		// Bind vertex buffer.
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * pointArraySize[i],
				   pointArray[i], GL_STREAM_DRAW);
		glEnableVertexAttribArray(attrib_vertices_);
		glVertexAttribPointer(attrib_vertices_, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_POINTS, 0, 3 * pointArraySize[i]);
	}

	// Unlock xyz_ij mutex.
	pthread_mutex_unlock(&TangoData::GetInstance().xyzij_mutex);

	GlUtil::CheckGlError("glDrawArray()");
	glUseProgram(0);
	GlUtil::CheckGlError("glUseProgram()");


	continueSaving(gl);
	continueLoading(gl);
	//isDrawing = false;
}
