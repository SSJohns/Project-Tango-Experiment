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

#define GLM_FORCE_RADIANS

#include <cmath>
#include <jni.h>
#include <string>

#include "tango-gl-renderer/axis.h"
#include "tango-gl-renderer/camera.h"
#include "tango-gl-renderer/frustum.h"
#include "tango-gl-renderer/gl_util.h"
#include "tango-gl-renderer/grid.h"
#include "tango-gl-renderer/transform.h"
#include "Functions.h"

#include <sstream>
#include <cmath>
#include "pointcloud.h"
#include "Renderable.h"
#include "Mesh.h"
#include "Graphics/GraphicsOGL.h"
#include "tango_data.h"
using namespace std;


long long totalPointNumber = 0;

// SLIDER VARIABLES
	int resolutionSlider = 1;


// CURSOR VARIABLES
	int cursorX = 0,
		cursorY = 0,
		realCT = 0,
		cursorPressed = 0,
		cursorTouching = 0,
		cTTimerMax = 10, //5
		cTTimer = cTTimerMax;

// MENU VARIABLES
	bool toggleAddPoints = false;
	int updatePoints = 0;
	int saveFailed = false;


	bool glCreated = false;




float dir = 0;
GraphicsOGL* gl = NULL;


// Screen size.
GLuint screen_width;
GLuint screen_height;

// Render camera's parent transformation.
// This object is a pivot transformtion for render camera to rotate around.
Transform* cam_parent_transform;

// Render camera.
Camera* cam;
Mesh* mesh;

// Device frustum.
Frustum* frustum;

// Point cloud drawable object.
Pointcloud* pointcloud;

// Device axis (in OpenGL coordinates).
Axis* axis;

// Ground grid.
// Each block is 1 meter by 1 meter in real world.
Grid* grid;

// Single finger touch positional values.
// First element in the array is x-axis touching position.
// Second element in the array is y-axis touching position.
float cam_start_angle[2];
float cam_cur_angle[2];

// Double finger touch distance value.
float cam_start_dist;
float cam_cur_dist;

enum CameraType {
  FIRST_PERSON = 0,
  THIRD_PERSON = 1,
  TOP_DOWN = 2
};
CameraType camera_type;;


// Render and camera controlling constant values.
// Height offset is used for offset height of motion tracking
// pose data. Motion tracking start position is (0,0,0). Adding
// a height offset will give a more reasonable pose while a common
// human is holding the device. The units is in meters.
const glm::vec3 kHeightOffset = glm::vec3(0.0f, 1.3f, 0.0f);

// Render camera observation distance in third person camera mode.
const float kThirdPersonCameraDist = 7.0f;

// Render camera observation distance in top down camera mode.
const float kTopDownCameraDist = 5.0f;

// Zoom in speed.
const float kZoomSpeed = 10.0f;

// Min/max clamp value of camera observation distance.
const float kCamViewMinDist = 1.0f;
const float kCamViewMaxDist = 100.f;

// FOV set up values.
// Third and top down camera's FOV is 65 degrees.
// First person camera's FOV is 45 degrees.
const float kHighFov = 65.0f;
const float kLowFov = 45.0f;

// Frustum scale.
const glm::vec3 kFrustumScale = glm::vec3(0.4f, 0.3f, 0.5f);

// Set camera type, set render camera's parent position and rotation.
void SetCamera(CameraType camera_index) {
  camera_type = camera_index;
  cam_cur_angle[0] = cam_cur_angle[1] = cam_cur_dist = 0.0f;
  switch (camera_index) {
    case CameraType::FIRST_PERSON:
      cam->SetFieldOfView(kLowFov);
      cam_parent_transform->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
      cam_parent_transform->SetRotation(glm::quat(1.0f, 0.0f, 0.0, 0.0f));
      break;
    case CameraType::THIRD_PERSON:
      cam->SetFieldOfView(kHighFov);
      cam->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
      cam->SetRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
      cam_cur_dist = kThirdPersonCameraDist;
      cam_cur_angle[0] = -PI / 4.0f;
      cam_cur_angle[1] = PI / 4.0f;
      break;
    case CameraType::TOP_DOWN:
      cam->SetFieldOfView(kHighFov);
      cam->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
      cam->SetRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
      cam_cur_dist = kTopDownCameraDist;
      cam_cur_angle[1] = PI / 2.0f;
      break;
    default:
      break;
  }
}

bool InitGlContent() {
  camera_type = CameraType::FIRST_PERSON;

  cam = new Camera();
  pointcloud = new Pointcloud();
  frustum = new Frustum();
  axis = new Axis();
  grid = new Grid();
  mesh = new Mesh(10000);

  cam_parent_transform = new Transform();

  frustum->SetScale(kFrustumScale);

  // Set the parent-child camera transfromation.
  cam->SetParent(cam_parent_transform);

  // Put the grid at the resonable height since the motion
  // tracking pose always starts at (0, 0, 0).
  grid->SetPosition(kHeightOffset);

  SetCamera(CameraType::THIRD_PERSON);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  return true;
}

bool SetupGraphics(int w, int h) {
  screen_width = w;
  screen_height = h;

  if (h == 0) {
    LOGE("Setup graphic height not valid");
    return false;
  }

  cam->SetAspectRatio(static_cast<float>(w) / static_cast<float>(h));
  return true;
}

int checkCircleButton(int x, int y, int r, string str) {

	int pressed;
	float xDis, yDis;
	xDis = cursorX-x;
	yDis = cursorY-y;

	pressed = (cursorTouching && sqrt(xDis*xDis + yDis*yDis) < r);


	if(pressed)
		r += 8;

	glLineWidth(20);

	gl->setColor(1,1,1,1);
	gl->fillCircle(x,y,r);
	gl->setColor(0,0,0,1);
	gl->drawCircle(x,y,r);

	gl->setColor(0,0,0,1);
	gl->drawStringCentered(x,y,2,2,str);

	return pressed;
}

int checkRectButton(int x, int y, int w, int h, string str) {

	int pressed;
	float b = 5;

	pressed = (cursorTouching && (cursorX >= x && cursorX <= x+w && cursorY >= y && cursorY <= y+h));


	if(pressed) {
		float r = 5;
		x -= r;
		y -= r;
		w += 2*r;
		h += 2*r;
	}

	gl->setColor(0,0,0,1);
	gl->fillRect(x,y,w,h);

	if(!pressed) {
		gl->setColor(1,1,1,1);
		gl->fillRect(x+b,y+b,w-2*b,h-2*b);
	}

	gl->setColor(0,0,0,1);
	gl->drawStringCentered(x+w/2.,y+h/2.,2,2,str);

	return pressed;
}

float checkSlider(int x, int y, int w, int h, float curPerc) {

	int pressed;
	float r = h*.25;
	float prX, newPerc;

	pressed = (cursorTouching && (cursorX >= x && cursorX <= x+w && cursorY >= y && cursorY <= y+h));


	if(pressed)
		newPerc = max(0.,min(1.*(cursorX-x)/w,1.));

	prX = x + curPerc*w;

	gl->setColor(0,0,0,1);
	gl->fillRect(x,y+r,w,h-2*r);

	gl->fillRect(prX-3,y,6,h);

	if(!pressed)
		return -1;
	else
		return newPerc;
}


// GL render loop.
bool RenderFrame() {
	if(gl == NULL)
		  gl = new GraphicsOGL();

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);\
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  // XYZij dirty indicates that the XYZij data has been changed.
  if (TangoData::GetInstance().is_xyzij_dirty) {
    TangoData::GetInstance().UpdateXYZijData();
  }

  // Pose dirty indicates that the pose data has been changed.
  if (TangoData::GetInstance().is_pose_dirty) {
    TangoData::GetInstance().UpdatePoseData();
  }

  /// Viewport set to full screen, and camera at origin
  /// facing on negative z direction, y axis is the up
  /// vector of the camera.
  glViewport(0, 0, screen_width, screen_height);

  // Get OpenGL camera to OpenGL world transformation for motion tracking.
  // Computed based on pose callback.
  glm::mat4 oc_2_ow_mat_motion = glm::mat4(1.0f);

  // Get OpenGL camera to OpenGL world transformation for depth.
  // Note that this transformation is different from the oc_2_ow_mat_motion
  // due to the timestamp differences. This transformation is computed
  // based on the closest pose data of depth frame timestamp.
  glm::mat4 oc_2_ow_mat_depth = glm::mat4(1.0f);

  if (camera_type == CameraType::FIRST_PERSON) {
    // Get motion transformation.
    oc_2_ow_mat_motion = TangoData::GetInstance().GetOC2OWMat(false);

    // Set camera's pose to motion tracking's pose.
    cam->SetTransformationMatrix(oc_2_ow_mat_motion);

    // Get depth frame transformation.
    oc_2_ow_mat_depth = TangoData::GetInstance().GetOC2OWMat(true);
  } else {
    // Get parent camera's rotation from touch.
    // Note that the render camera is a child transformation
    // of the this transformation.
    // cam_cur_angle[0] is the x-axis touch, cooresponding to y-axis rotation.
    // cam_cur_angle[0] is the y-axis touch, cooresponding to x-axis rotation.
    glm::quat parent_cam_rot =
        glm::rotate(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), -cam_cur_angle[0],
                    glm::vec3(0, 1, 0));
    parent_cam_rot =
        glm::rotate(parent_cam_rot, -cam_cur_angle[1], glm::vec3(1, 0, 0));

    // Get motion transformation.
    oc_2_ow_mat_motion = TangoData::GetInstance().GetOC2OWMat(false);

    // Get depth frame transformation.
    oc_2_ow_mat_depth = TangoData::GetInstance().GetOC2OWMat(true);

    // Set render camera parent position and rotation.
    cam_parent_transform->SetRotation(parent_cam_rot);
    cam_parent_transform->SetPosition(
        GlUtil::GetTranslationFromMatrix(oc_2_ow_mat_motion));

    gl->setPerspective();

    frustum->SetTransformationMatrix(oc_2_ow_mat_motion);
    frustum->SetScale(kFrustumScale);
    frustum->Render(cam->GetProjectionMatrix(), cam->GetViewMatrix());

    // Set camera view distance, based on touch interaction.
    cam->SetPosition(glm::vec3(0.0f, 0.0f, cam_cur_dist));
  }

  //gl->update3D(cam->GetOrthoMatrix(), cam->GetViewMatrix());

  // Set axis transformation, axis representing device's pose.
  axis->SetTransformationMatrix(oc_2_ow_mat_motion);
  axis->Render(cam->GetProjectionMatrix(), cam->GetViewMatrix());

  // Render point cloud based on depth buffer and depth frame transformation.
  if(!mesh->isIOOpen())
	  pointcloud->Render(
			  cam->GetProjectionMatrix(), cam->GetViewMatrix(), oc_2_ow_mat_depth,
			  TangoData::GetInstance().depth_buffer_size * 3,
			  static_cast<float*>(TangoData::GetInstance().depth_buffer), resolutionSlider);

  //Only Update if the Cloud is New!
  if(((!toggleAddPoints && updatePoints) || (toggleAddPoints && !updatePoints)) && TangoData::GetInstance().hasNewCloud) {
	  totalPointNumber += TangoData::GetInstance().depth_buffer_size;

	  mesh->addPoints(TangoData::GetInstance().depth_buffer, TangoData::GetInstance().depth_buffer_size*3, oc_2_ow_mat_motion);
	  TangoData::GetInstance().hasNewCloud = false;
  }
  grid->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f) - kHeightOffset);
  // Render grid.
  grid->Render(cam->GetProjectionMatrix(), cam->GetViewMatrix());

  mesh->Render(gl, cam->GetProjectionMatrix(), cam->GetViewMatrix());

  gl->setColor(0,0,0,1);
  gl->setFont("8bit");
  gl->setOrtho();


  float s = 2, dY = 0;//-150;
  gl->drawStringScaled(0,dY,s,s,"Frame Delta Time (ms): " + to_string(TangoData::GetInstance().depth_frame_delta_time));
  	  dY += 12*s;
  gl->drawStringScaled(0,dY,s,s,"_Camera Distance: " + to_string(cam_cur_dist));
  	  dY += 12*s;
  gl->drawStringScaled(0,dY,s,s,"Point Number: " + to_string(mesh->getPointNumber()) + " / " + to_string(mesh->getMaxCloudPoints()*mesh->getPointCloudNumber()))	;
  	  dY += 12*s;
  gl->drawStringScaled(0,dY,s,s,"Current File Size: " + to_string(mesh->getPointNumber()*3.*4/1024/1024) + " / " + to_string(mesh->getMaxCloudPoints()*mesh->getPointCloudNumber()*3.*4/1024/1024) + " MB");
  	  dY += 12*s;
  gl->drawStringScaled(0,dY,s,s,"Total Point Number: " + to_string(totalPointNumber));
  	  dY += 12*s;
  gl->drawStringScaled(0,dY,s,s,"Estimated File Size: " + to_string(totalPointNumber*3.*4/1024/1024) + " MB");
  	  dY += 12*s;
  gl->drawStringScaled(0,dY,s,s,"Average Depth (m): " + to_string(TangoData::GetInstance().depth_average_length));
  	  dY += 12*s;
  gl->drawStringScaled(0,dY,s,s,TangoData::GetInstance().pose_string);
  	  dY += 12*s;
  gl->drawStringScaled(0,dY,s,s,TangoData::GetInstance().event_string);
  	  dY += 12*s;


  gl->drawStringScaled(0,0,s,s,TangoData::GetInstance().errorString);


  float barPerc;
  barPerc = 1.*mesh->getCurCloud()/mesh->getPointCloudNumber();
  gl->setColor(0,1,0,1);
  gl->fillRect(0,0,gl->getScreenWidth()*barPerc,5);
  gl->setColor(0,0,0,1);
  gl->drawRect(0,0,gl->getScreenWidth(),5);



  gl->setColor(0,0,0,1);

  if(realCT && !cursorTouching) {
	  cursorTouching = cursorPressed = true;

	  if(cTTimer == -1)
	  		  cTTimer = cTTimerMax;
  }
  else if(!realCT && cursorTouching) {
	  cursorPressed = false;

	  if(cTTimer > -1)
		  cTTimer--;

	  if(cTTimer == -1) {
		  cursorTouching = 0;
		  realCT = false;
	  }
  }
  else if(realCT && cursorTouching) {
  	  cursorPressed = false;
  }

  float lX1 = 20, lX2 = lX1+200, rX1 = 1500, w = 180, h = 90;
  float bYS = 110, bY3 = 1095, bY2 = bY3-bYS, bY1 = bY2-bYS;


  string resTxt;
  if(resolutionSlider == 1)
	  resTxt = "Using every available point.";
  else if(resolutionSlider == 2)
	  resTxt = "Using every 2nd available point.";
  else if(resolutionSlider == 3)
	  resTxt = "Using every 3rd available point.";
  else
	  resTxt = "Using every " + to_string(resolutionSlider) + "th available point.";

  gl->drawStringScaled(lX2+w+20,bY3-30,2,2,resTxt);

  int maxRes = 30;
  float slVal = checkSlider(lX2+w+20,bY3,1000,h,1.*(resolutionSlider-1)/(maxRes-1));
  if(slVal != -1) {
	  resolutionSlider = round(slVal*(maxRes-1)) + 1;

	  resolutionSlider = max(1, min(resolutionSlider, maxRes));

	  mesh->setResolution(resolutionSlider);
  }

  if(checkRectButton(lX1,bY1,w,h, "Save File"))
	  if(cursorPressed) {
		  if(!mesh->saveToFile(gl,"model.dat"))
			  saveFailed = true;
	  }

  if(checkRectButton(lX2,bY1,w,h, "Load File"))
 	  if(cursorPressed) {
 		  if(!mesh->loadFile(gl,"model.dat"))
 			  saveFailed = true;
 	  }

  // Draw Add Point Button
  if(!toggleAddPoints) {
	  if(checkRectButton(lX1,bY2,w,h, "Hold to\nAdd Points"))
		  updatePoints = true;
	  else
		  updatePoints = false;
  }
  else {
	  if(checkRectButton(lX1,bY2,w,h, "Hold to\nStop Adding"))
		  updatePoints = true;
	  else
		  updatePoints = false;
  }

  if(checkRectButton(lX2,bY2,w,h, "Toggle Adding\nPoints"))
	  if(cursorPressed)
		  toggleAddPoints = !toggleAddPoints;


  if(checkRectButton(lX1,bY3,w,h, "Clear Model"))
	  if(cursorPressed) {
		  mesh->clear();
		  totalPointNumber = 0;
	  }


  // RIGHT BUTTONS

  if(checkRectButton(rX1,bY3,w,h, "Reset Camera"))
  	  if(cursorPressed)
  		  cam->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));


  realCT = cursorPressed = false;

  return true;
}

#ifdef __cplusplus
extern "C" {
#endif
// Tango Service interfaces.
JNIEXPORT jint JNICALL
Java_com_projecttango_experiments_nativepointcloud_TangoJNINative_initialize(
    JNIEnv* env, jobject, jobject activity) {
  TangoErrorType err = TangoData::GetInstance().Initialize(env, activity);
  if (err != TANGO_SUCCESS) {
    if (err == TANGO_INVALID) {
      LOGE("Tango Service version mis-match");
    } else {
      LOGE("Tango Service initialize internal error");
    }
  }
  return static_cast<int>(err);
}

JNIEXPORT void JNICALL
Java_com_projecttango_experiments_nativepointcloud_TangoJNINative_setupConfig(
    JNIEnv*, jobject) {
  if (!TangoData::GetInstance().SetConfig()) {
    LOGE("Tango set config failed");
  }
}

JNIEXPORT void JNICALL
Java_com_projecttango_experiments_nativepointcloud_TangoJNINative_connectCallbacks(
    JNIEnv*, jobject) {
  if (!TangoData::GetInstance().ConnectCallbacks()) {
    LOGE("Tango ConnectCallbacks failed");
  }
}

JNIEXPORT jint JNICALL
Java_com_projecttango_experiments_nativepointcloud_TangoJNINative_connect(
    JNIEnv*, jobject) {
  TangoErrorType err = TangoData::GetInstance().Connect();
  if (err != TANGO_SUCCESS) {
    LOGE("Tango Service connect failed");
  }
  return static_cast<int>(err);
}

JNIEXPORT void JNICALL
Java_com_projecttango_experiments_nativepointcloud_TangoJNINative_setupExtrinsics(
    JNIEnv*, jobject) {
  // The extrinsics can only be queried after connected to service.
  if (!TangoData::GetInstance().SetupExtrinsicsMatrices()) {
    LOGE("Tango set extrinsics failed");
  }
}

JNIEXPORT void JNICALL
Java_com_projecttango_experiments_nativepointcloud_TangoJNINative_disconnect(
    JNIEnv*, jobject) {
  TangoData::GetInstance().Disconnect();
}

JNIEXPORT void JNICALL
Java_com_projecttango_experiments_nativepointcloud_TangoJNINative_freeGLContent(
    JNIEnv*, jobject) {
  if (cam != NULL) {
    delete cam;
  }
  cam = NULL;

  if (pointcloud != NULL) {
    delete pointcloud;
  }
  pointcloud = NULL;

  if (axis != NULL) {
    delete axis;
  }
  axis = NULL;

  if (grid != NULL) {
    delete grid;
  }
  grid = NULL;

  if (frustum != NULL) {
    delete frustum;
  }
  frustum = NULL;

  if (cam_parent_transform != NULL) {
    delete cam_parent_transform;
  }
  cam_parent_transform = NULL;
}

// Graphic interfaces.
JNIEXPORT void JNICALL
Java_com_projecttango_experiments_nativepointcloud_TangoJNINative_initGlContent(
    JNIEnv*, jobject) {
  InitGlContent();
}

JNIEXPORT void JNICALL
Java_com_projecttango_experiments_nativepointcloud_TangoJNINative_setupGraphic(
    JNIEnv*, jobject, jint width, jint height) {
  SetupGraphics(width, height);
}

JNIEXPORT void JNICALL
Java_com_projecttango_experiments_nativepointcloud_TangoJNINative_render(
    JNIEnv*, jobject) {
  RenderFrame();
}

JNIEXPORT void JNICALL
Java_com_projecttango_experiments_nativepointcloud_TangoJNINative_setCamera(
    JNIEnv*, jobject, int camera_index) {
  SetCamera(static_cast<CameraType>(camera_index));
}

JNIEXPORT void JNICALL
Java_com_projecttango_experiments_nativepointcloud_TangoJNINative_passTouchPos(
    JNIEnv*, jobject, float x, float y) {
  cursorX = x;
  cursorY = y+50;
}

JNIEXPORT void JNICALL
Java_com_projecttango_experiments_nativepointcloud_TangoJNINative_passTouch(
    JNIEnv*, jobject, int touched) {
  realCT = touched;
}

// Touching GL interface.
JNIEXPORT void JNICALL
Java_com_projecttango_experiments_nativepointcloud_TangoJNINative_startSetCameraOffset(
    JNIEnv*, jobject) {
  if (cam != NULL) {
    cam_start_angle[0] = cam_cur_angle[0];
    cam_start_angle[1] = cam_cur_angle[1];
    cam_start_dist = cam->GetPosition().z;
  }
}

JNIEXPORT void JNICALL
Java_com_projecttango_experiments_nativepointcloud_TangoJNINative_setCameraOffset(
    JNIEnv*, jobject, float rotation_x, float rotation_y, float dist) {
  if (cam != NULL) {
    cam_cur_angle[0] = cam_start_angle[0] + rotation_x;
    cam_cur_angle[1] = cam_start_angle[1] + rotation_y;
    dist = GlUtil::Clamp(cam_start_dist + dist * kZoomSpeed, kCamViewMinDist,
                         kCamViewMaxDist);
    cam_cur_dist = dist;
  }
}
#ifdef __cplusplus
}
#endif
