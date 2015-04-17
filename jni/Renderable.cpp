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

#include "Renderable.h"
#include "tango_data.h"

static const char kVertexShader[] =
    "attribute vec4 vertex;\n"
	"uniform float pointSize;\n"
    "uniform mat4 mvp;\n"
    "varying vec4 v_color;\n"
    "void main() {\n"
	"  gl_PointSize = pointSize;\n"
    "  gl_Position = mvp*vertex;\n"
    "  v_color = vertex;\n"
    "}\n";

static const char kFragmentShader[] = "varying vec4 v_color;\n"
    "void main() {\n"
    "  gl_FragColor = vec4(v_color);\n"
    "}\n";


Renderable::Renderable() {
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
  shader_program_ = GlUtil::CreateProgram(kVertexShader, kFragmentShader);
  if (!shader_program_) {
    LOGE("Could not create program.");
  }
  uniform_mvp_mat_ = glGetUniformLocation(shader_program_, "mvp");
  attrib_vertices_ = glGetAttribLocation(shader_program_, "vertex");
  glGenBuffers(1, &vertex_buffer_);
}

void Renderable::Render(glm::mat4 projection_mat, glm::mat4 view_mat) {
}
