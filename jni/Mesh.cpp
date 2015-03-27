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

#include "Mesh.h"
#include "Renderable.h"
#include "Graphics/GraphicsOGL.h"
#include <stdio.h>


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
	resScale = 1;//00;


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

int Mesh :: saveToFile(GraphicsOGL* gl, string fileName) {

	string fN = "/sdcard/";
		fN = fN + fileName;


	FILE *f = fopen(fN.c_str(), "w");

	if(f == NULL)
		return false;

	int k = 0;

		for(int i = 0; i < numClouds; i++) {

			if(pointArraySize[i] == 0)
				continue;

			for(int l = 0; l < pointArraySize[i]; l += 3) {
				fprintf(f, "%f %f %f\n", pointArray[i][l], pointArray[i][l+1], pointArray[i][l+2]);
			}
		}

	fclose(f);

	return true;
}

int Mesh :: getCurCloud() {
	return curCloud;
}

void Mesh :: addPoints(float* depthBuffer, int depthBufferSize, const glm::mat4 model_mat) {

	if(isDrawing || isAdding || depthBufferSize < 100)
		return;

	delete [] pointArray[curCloud];

	isAdding = true;


	// Find Size of Current Point Cloud
	pointArraySize[curCloud] = 0;

	if(resScale == 1)
		pointArraySize[curCloud] = depthBufferSize;
	else
		for(int i = 0; i < depthBufferSize; i += resScale)
			pointArraySize[curCloud]++;


	// Get Model Matrix
	glm::mat4 curMat = model_mat * inverse_z_mat;
	glm::vec4 curVec;


	// Create New Array for Current Cloud
	pointArray[curCloud] = new float[pointArraySize[curCloud]];


	// Add Points to Cloud
	int k = 0;
	for(int i = 0; i < pointArraySize[curCloud]; i += 3*resScale) {

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

void Mesh :: Render(glm::mat4 projection_mat, glm::mat4 view_mat) {
	//isDrawing = true;

	Renderable :: Render(projection_mat, view_mat);

	// Calculate model view projection matrix.
	glm::mat4 mvp_mat = projection_mat * view_mat;

	glUseProgram(shader_program_);


	// Lock xyz_ij mutex.
	pthread_mutex_lock(&TangoData::GetInstance().xyzij_mutex);


	for(int i = 0; i < numClouds; i++) {

		if(pointArraySize[i] == 0)
			continue;

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
	//isDrawing = false;
}
