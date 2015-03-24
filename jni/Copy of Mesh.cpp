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



static const glm::mat4 inverse_z_mat = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                                                 0.0f, -1.0f, 0.0f, 0.0f,
                                                 0.0f, 0.0f, -1.0f, 0.0f,
                                                 0.0f, 0.0f, 0.0f, 1.0f);

static bool isDrawing = false, isAdding = false;

Mesh :: Mesh(int maxPoints) : Renderable() {
	maxPointNum = maxPoints;

	//Initialize Point Array
	pointArray = new float[0];
	pointArraySize = 0;
}

Mesh :: ~Mesh() {
	delete [] pointArray;
}


void Mesh :: addPoints(float* depthBuffer, int depthBufferSize, const glm::mat4 model_mat) {

	if(isDrawing || isAdding)// || pointArraySize > 100000)
		return;


	float* oldPtArray = pointArray;
	int oldStart = pointArraySize*3;

	isAdding = true;

	for(int i = 0; i < depthBufferSize; i += 3*resScale) {
		pointArraySize++;
	}
	//pointArraySize += depthBufferSize/3;


	glm::mat4 curMat = model_mat * inverse_z_mat;
	glm::vec4 curVec;

	pointArray = new float[pointArraySize*3];

	//Add Points from Old Array to New Array
	for(int i = 0; i < oldStart; i++)
		pointArray[i] = oldPtArray[i];


	int k = 0;
	for(int i = 0; i < depthBufferSize; i += 3*resScale) {
		curVec = glm::vec4(depthBuffer[i],depthBuffer[i+1],depthBuffer[i+2],1);

		curVec = curMat*curVec;

		pointArray[oldStart + k] = curVec[0];
		pointArray[oldStart + k+1] = curVec[1];
		pointArray[oldStart + k+2] = curVec[2];
		k += 3;
	}

	delete [] oldPtArray;

	isAdding = false;
}


void Mesh :: Render(glm::mat4 projection_mat, glm::mat4 view_mat) {
	//isDrawing = true;

	Renderable :: Render(projection_mat, view_mat);


	if(pointArraySize == 0)
		return;

	glUseProgram(shader_program_);

	// Lock xyz_ij mutex.
	pthread_mutex_lock(&TangoData::GetInstance().xyzij_mutex);

	// Calculate model view projection matrix.
	glm::mat4 mvp_mat = projection_mat * view_mat;
	glUniformMatrix4fv(uniform_mvp_mat_, 1, GL_FALSE, glm::value_ptr(mvp_mat));

	// Bind vertex buffer.
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * pointArraySize,
			   pointArray, GL_STATIC_DRAW);
	glEnableVertexAttribArray(attrib_vertices_);
	glVertexAttribPointer(attrib_vertices_, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_POINTS, 0, 3 * pointArraySize);

	// Unlock xyz_ij mutex.
	pthread_mutex_unlock(&TangoData::GetInstance().xyzij_mutex);

	GlUtil::CheckGlError("glDrawArray()");
	glUseProgram(0);
	GlUtil::CheckGlError("glUseProgram()");

	//isDrawing = false;
}
