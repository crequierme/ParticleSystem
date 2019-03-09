//
// Camera.h
// Definition of OpenGL Camera Class
//
// Christopher Root, 2006
// Modifications by Donald House, 2009
//   corrected calculation of camera coordinates
// Modifications by Yujie Shu, 2012
//   simplified calculation of camera coordinates
// Simplifications and corrections by Donald House, 2018
// Camera viewing and positioning parameter setting seperated by Donald House, 2018

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Vector.h"

class Camera {
private:
  // all variables starting with 'Default' hold the initial camera values
  // these values are used if the camera is reset to its initial state
  Vector3d DefaultPos;
  Vector3d DefaultAim;
  Vector3d DefaultUp;

  double DefaultAzim;
  double DefaultElev;
  
  // Azimuth and elevation angles
  double CurrentAzim;
  double CurrentElev;
  
  double DeltaAzim;
  double DeltaElev;

  // current camera position and aim
  Vector3d Pos;
  Vector3d Aim;
  Vector3d Up;
  
  // current camera translation
  double TranslateX;
  double TranslateY;
  double TranslateZ;
  
  // Camera perspective projection parameters
  float NearPlane;
  float FarPlane;
  float Fov;
  
  // variables to hold mouse history data
  int MousePrevX;
  int MousePrevY;
  bool constrain;   // shift key down when mouse button pressed

  // current camera mouse interaction mode
  int InteractionMode;
  
  //flag to invert the coordinate
  int inverted;

  // methods called only within the Camera class
  void Initialize();
  void ComputeCoords(const Vector3d &P, const Vector3d &A, const Vector3d &U);

public:
  // default constructor
  Camera();

  // constructor setting up camera orientation and view volume
  // P is position in 3D, A is aim coordinate in 3D, U is up vector
  // Near is near clipping plane, Far is far clipping plane,
  // ViewAngle is field of view angle in degrees
  Camera(Vector3d P, Vector3d A, Vector3d U, float Near = 0.1, float Far = 1000.0, float ViewAngle = 60.0);

  // sets the clipping planes of the view volume
  void SetClippingPlanes(float Near, float Far);

  // sets the FOV, ViewAngle should be in degrees
  void SetFOV(float ViewAngle);

  // set routines for Pos, Aim, and Up vector
  void SetPos(const Vector3d &P);
  void SetAim(const Vector3d &A);
  void SetUp(const Vector3d &U);

  // reset the camera to its initial position
  void Reset();

  // change camera aim position, and also move camera to same
  // relative position with respect to new aim position
  void SetCenterOfFocus(const Vector3d &NewAim);

  // Set up the OpenGL camera view matrix for perspective viewing
  // W and H are the width and height of the window
  // Uses the current NearPlane, FarPlane, and Fov parameters
  void PerspectiveDisplay(int W, int H);

  // Positions camera using current position, aim, and up vector
  void AimCamera();
  // Positions camera and updates position, aim, and up vector
  void AimCamera(const Vector3d &P, const Vector3d &A, const Vector3d &U);
 
  // function that handles mouse button events
  void HandleMouseEvent(int button, int state, int x, int y, bool shiftkey);

  // function that handles mouse movements
  void HandleMouseMotion(int x, int y);
};

#endif
