//
// Camera.cpp
// Implementation of OpenGL Camera Class
// 
// Christopher Root, 2006
// Modifications by Donald House, 2009
//   corrected calculation of camera coordinates
// Modifications by Yujie Shu, 2012
//   simplified calculation of camera coordinates
// Simplifications and corrections by Donald House, 2018
//
#include "Camera.h"

#ifdef __APPLE__
#  pragma clang diagnostic ignored "-Wdeprecated-declarations"
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <cstdio>

using namespace std;

// camera mouse interaction states
#define INACTIVE 0
#define TRANSLATE 1
#define ROTATE 2
#define DOLLY 3

// set camera position, aim point, and up vector. verify ok, and make up the true up direction
void Camera::ComputeCoords(const Vector3d &P, const Vector3d &A, const Vector3d &U){
  const float epsilon = 0.0001;
  
  Vector3d zaxis = P - A;
  // if camera positoin and aim position coincident, no way to aim the camera
  if (zaxis.norm() < epsilon) {
    fprintf (stderr, "Camera position and aim position the same. Can't aim camera!\n");
    exit(1);
  }
  Vector3d dir = -zaxis.normalize();
  Vector3d xaxis = dir % U;

  // if up vector and aim vector parallel, no way to tell which way is up
  if (xaxis.norm() < epsilon) {
    fprintf (stderr, "Up parallel to aim. Can't tell which way is up!\n");
    exit(1);
  }

  Pos = P;
  Aim = A;
  Up = xaxis.normalize() % dir; // correct up vector to be perpendicular to dir
}

// Initialize routine setting up defaults
void Camera::Initialize() {
  Vector3d tmp, tmp1, tmp2;
  Vector3d axisOrigin, updatePos;
  double dist;

  InteractionMode = INACTIVE;
  
  DefaultPos = Pos;
  DefaultAim = Aim;
  DefaultUp = Up;

  // find the angle around the x axis
  updatePos = Pos - Aim;
  axisOrigin.set(updatePos.x, 0, 0);
  dist = (axisOrigin-updatePos).norm();
  tmp1.set(updatePos.x, 0, dist);

  tmp = updatePos.normalize();
  tmp1 = tmp1.normalize();

  CurrentElev = RadToDeg(acos(tmp * tmp1));

  // find the angle around the y axis
  axisOrigin.set(0, updatePos.y, 0);
  dist = (axisOrigin-updatePos).norm();

  tmp2.set(0, updatePos.y, dist);
  tmp2 = tmp2.normalize();

  CurrentAzim = 360.0 - RadToDeg(acos(tmp2*tmp));

  DefaultElev = CurrentElev;
  DefaultAzim = CurrentAzim;

  inverted = 0;
  TranslateX = 0;
  TranslateY = 0;
  TranslateZ = 0;
}

/* constructors */

// default constructor... sets position to 0, 0, 5, aimed at the origin
// with the up vector set to the y axis
Camera::Camera() {
  Pos.set(0, 0, 5);
  Aim.set(0, 0, 0);
  Up.set(0, 1, 0);

  // set default view volume
  NearPlane = 0.1;
  FarPlane = 1000.0;
  Fov = 60.0;

  Initialize();
}

/*
 * constructor to set a camera to a desired orientation
 * P is position in 3D
 * A is the aim coordinate
 * U is the up vector
 * Near is near clipping plane distance
 * Far is far clipping plane distance
 * ViewAngle is view frustum vertical angle in degrees
 */
Camera::Camera(Vector3d P, Vector3d A, Vector3d U,
               float Near, float Far, float ViewAngle) {
  ComputeCoords(P, A, U);

  SetClippingPlanes(Near, Far);
  SetFOV(ViewAngle);

  Initialize();
}

// set functions for the Pos, Aim, and Up vectors....
// just remember that |Aim - Pos| != 0, and (Aim - Pos) % Up != 0, and or you'll see problems
void Camera::SetPos(const Vector3d &P) {
  ComputeCoords(P, Aim, Up);
}

void Camera::SetAim(const Vector3d &A) {
  ComputeCoords(Pos, A, Up);
}

void Camera::SetUp(const Vector3d &U) {
  ComputeCoords(Pos, Aim, U);
}

/*
 * sets the near and far clipping planes for the camera view
 */
void Camera::SetClippingPlanes(float Near, float Far) {
  const float epsilon = 0.0001;
  
  NearPlane = Max(epsilon, Near); // Near must not be 0 or negative
  FarPlane = Max(Near, Far);      // Far must not be smaller than Near
}

/*
 * sets the field of view of the camera, ViewAngle is in degrees
 * Fov constrainted to be between 1 and 179 degrees
 */
void Camera::SetFOV(float ViewAngle) {
  Fov = Min(Max(ViewAngle, 1.0), 179.0);
}

/*
 * resets the camera to its original orientation
 */
void Camera::Reset() {
  Pos = DefaultPos;
  Aim = DefaultAim;
  Up = DefaultUp;

  CurrentElev = DefaultElev;
  CurrentAzim = DefaultAzim;
  TranslateX = TranslateY = TranslateZ = 0;
}

/*
 * sets the camera's aim to be the given vector v
 */
void Camera::SetCenterOfFocus(const Vector3d &NewAim) {
  Vector3d dif = NewAim - Aim;
  ComputeCoords(Pos + dif, NewAim, Up);
}

/*
 * Set up the OpenGL camera view matrix for perspective viewing
 * W and H are the width and height of the window
 * Uses the current NearPlane, FarPlane, and Fov parameters
*/
void Camera::PerspectiveDisplay(int W, int H) {

  // set up the projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(Fov, (float)W/(float)H, NearPlane, FarPlane);
}

// Position, aim, and orient the camera in the current modelview frame
// using current position, aim, and up vector
void Camera::AimCamera(){
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(Pos.x, Pos.y, Pos.z,
            Aim.x, Aim.y, Aim.z,
            Up.x, Up.y, Up.z);

  // Translate the camera by accumulated translation
  glTranslatef (TranslateX, TranslateY, TranslateZ);

  // Rotate the camera about its vertical axis (yaw), then about its x axis (pitch)
  glRotatef(CurrentElev, 1, 0, 0);
  glRotatef(CurrentAzim, 0, 1, 0);
}

// Position, aim, and orient the camera in the current modelview frame
// using updated position, aim, and up vectors
void Camera::AimCamera(const Vector3d &P, const Vector3d &A, const Vector3d &U){
  SetPos(P);
  SetAim(A);
  SetUp(U);

  AimCamera();
}

/*
 * mouse event handler function called from view
 *
*/
void Camera::HandleMouseEvent(int button, int state, int x, int y, bool shiftkey) {
  double realy, wx, wy, wz;

  //if the shift key is depressed, motion is constrained
  constrain = shiftkey;
  
  if (state == GLUT_UP && InteractionMode != INACTIVE) {
    // update the elevation and roll of the camera
    CurrentElev += DeltaElev;
    CurrentAzim += DeltaAzim;

    // reset the change in elevation and roll of the camera
    DeltaElev = DeltaAzim = 0.0;

    InteractionMode = INACTIVE;
  } else if (state == GLUT_DOWN){
    
    // set the new mouse state
    MousePrevX = x;
    MousePrevY = y;
    if (abs(CurrentElev) > 90)
      inverted = 1;
    else
      inverted = 0;

    // mouse button has been pressed, camera will move
    switch (button) {
      case GLUT_LEFT_BUTTON:
        // rotating camera
        InteractionMode = ROTATE;
        break;
      case GLUT_MIDDLE_BUTTON:
        // translating camera:
        InteractionMode = TRANSLATE;
        break;
      case GLUT_RIGHT_BUTTON:
        // dollying camera in/out:
        InteractionMode = DOLLY;
        break;
     }
  }
}

/*
 * Mouse Motion handler function... should be called in the
 * mouse motion function of your own code
 */
void Camera::HandleMouseMotion(int x, int y) {
  double mouse_dx, mouse_dy;
  double constrained_dx, constrained_dy;
  
  if (InteractionMode != INACTIVE) {
    // find the greatest change in mouse position
    // and constrains change to be in that direction
    mouse_dx = constrained_dx = x - MousePrevX;
    mouse_dy = constrained_dy = -(y - MousePrevY);   // fix for mouse y coordinates flipped

    if (Abs(constrained_dx) > Abs(constrained_dy))
      constrained_dy = 0;
    else
      constrained_dx = 0;

    switch (InteractionMode) {
      case DOLLY:
        // camera is dollying in or out
        TranslateZ += (constrained_dx + constrained_dy) * 0.05;
        break;
        
      case ROTATE:
        // camera is rotating

        // get rate of change in screen coordinates from when the
        // mouse was first pressed
        if(constrain){
          DeltaAzim = constrained_dx / 5.0;
          DeltaElev = constrained_dy / 5.0;
        }
        else{
          DeltaAzim = mouse_dx / 5.0;
          DeltaElev = mouse_dy / 5.0;
        }
        
        if (inverted)
          CurrentAzim -= DeltaAzim;
        else
          CurrentAzim += DeltaAzim;

        CurrentElev -= DeltaElev;

        while (CurrentElev < -180)
          CurrentElev += 360;
        while (CurrentElev > 180)
          CurrentElev -= 360;

        break;
        
      case TRANSLATE:
        if(constrain){
          TranslateX += constrained_dx * 0.05;
          TranslateY += constrained_dy * 0.05;
        }
        else{
          TranslateX += mouse_dx * 0.05;
          TranslateY += mouse_dy * 0.05;
        }
        break;
    }

    MousePrevX = x;
    MousePrevY = y;
  }
}

