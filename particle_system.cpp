/*
 particle_system.cpp
 
 CPSC 8170 Physically Based Animation
 Donald H. House     8/23/2018
 Example OpenGL/GLUT 3D Animation Program Demonstrating:
   - animation loop using model-view-controller design
   - use of camera
   - hidden surface removal using depth buffer
   - lighting using 3 light studio lighting setup
   - materials and shading

 Keyboard keypresses have the following effects:
   s: start the simulation
   k: toggle key light on and off
   f: toggle fill light on and off
   r: toggle back (rim) light on and off
   g: toggle window background color between grey and black
   i: reinitialize (reset program to initial default state)
   q or Esc: quit
 
 Camera and model controls following the mouse:
 model yaw   - left-button, horizontal motion, rotation of the model around its y axis
 model tilt  - left-button, vertical motion, rotation of the model about its x axis
 camera trolly - middle-button, horizontal motion
 camera raise	 - middle-button, vertical motion
 trolly    - right-button, vertical or horizontal motion, trolly camera in and out
 
 usage: 
*/

#include "Model.h"
#include "View.h"

#include <cstdlib>
#include <iostream>

#ifdef __APPLE__
#  pragma clang diagnostic ignored "-Wdeprecated-declarations"
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

using namespace std;

//===========================================================================
// Model
//===========================================================================

// Create the model for simulating the particle system
Model particleSystem;

//===========================================================================
// View
//===========================================================================

// Create the scene viewer with pointer to particle system model
View psView(&particleSystem);

//===========================================================================
// Controller
//===========================================================================

// global needed to share parameter filename among callbacks
char *paramfilename;

//
// Keyboard callback routine.
// Send model and view commands based on key presses
//
void handleKey(unsigned char key, int x, int y){
  const int ESC = 27;
  
  switch(key){
    case 's':            
      particleSystem.initSimulation();     // reinitialize the simulation
      particleSystem.startSimulation();        // start the action
      break;

    case 'k':           // toggle key light on and off
      psView.toggleKeyLight();
      break;
      
    case 'f':           // toggle fill light on and off
      psView.toggleFillLight();
      break;
      
    case 'r':           // toggle back light on and off
      psView.toggleBackLight();
      break;
      
    case 'g':           // toggle background color from grey to black
      psView.toggleBackColor();
      break;

    case 'i':			// I -- reinitialize view
    case 'I':
      psView.setInitialView();
      break;
      
    case 'q':			// Q or Esc -- exit program
    case 'Q':
    case ESC:
      exit(0);
  }
  
  // always refresh the display after a key press
  glutPostRedisplay();
}

//
// let the View handle mouse button events
// but pass along the state of the shift key also
//
void handleButtons(int button, int state, int x, int y) {
  bool shiftkey = (glutGetModifiers() == GLUT_ACTIVE_SHIFT);

  psView.handleButtons(button, state, x, y, shiftkey);
  glutPostRedisplay();
}

//
// let the View handle mouse motion events
//
void handleMotion(int x, int y) {
  psView.handleMotion(x, y);
  glutPostRedisplay();
}

//
// let the View handle display events
//
void doDisplay(){
  psView.updateDisplay();
}

//
// let the View handle reshape events
//
void doReshape(int width, int height){
  psView.reshapeWindow(width, height);
}

//
// idle callback: let the Model handle simulation timestep events
//
void doSimulation(){
  static int count = 0;

  particleSystem.timeStep();

  if(count == 0)         // only update the display after every displayInterval time steps
    glutPostRedisplay();
  
  count = (count + 1) % particleSystem.displayInterval();
}

//
// Main program to create window, initiate GLUT, setup callbacks,
// and initialize Model and View
//
int main(int argc, char* argv[]){

  
  // start up the glut utilities
  glutInit(&argc, argv);
  
  // create the graphics window, giving width, height, and title text
  // and establish double buffering, RGBA color
  // Depth buffering must be available for drawing the shaded model
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(psView.getWidth(), psView.getHeight());
  glutCreateWindow("Particle System Simulation");
  
  // register callback to handle events
  glutDisplayFunc(doDisplay);
  glutReshapeFunc(doReshape);
  glutKeyboardFunc(handleKey);
  glutMouseFunc(handleButtons);
  glutMotionFunc(handleMotion);

  // idle function is called whenever there are no other events to process
  glutIdleFunc(doSimulation);
  
  // set up the camera viewpoint, materials, and lights
  psView.setInitialView();

  // load parameters and initialize the ball model
  particleSystem.initSimulation();
  
  glutMainLoop();
}
