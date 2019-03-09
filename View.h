/*
 View.h
 CPSC 8170 Physically Based Animation
 Donald H. House     8/23/2018
 Modified by Caroline Requierme (crequie@clemson.edu), 8/31/2018

 Interface for Physical Simulation Viewer
*/

#include <cstdlib>
#include <cstdio>

#include "Camera.h"
#include "Model.h"

#ifndef __VIEW_H__
#define __VIEW_H__

class View{
  private:
    const int width;                // initial window dimensions
    const int height;               // proportions match 1080p proportions but 1/2 scale
  
    const float near;               // distance of near clipping plane
    const float far;                // distance of far clipping plane
    const float fov;                // camera vertical field of view
  
    const float modelsize;          // dimension of the teapot
    const float modeldepth;         // initial distance of center of teapot from camera
  
    const float diffuse_fraction;   // fraction of reflected light that is diffuse
    const float specular_fraction;  // fraction of reflected light that is specular
    const float shininess;          // specular exponent

    // colors used for lights, and screen background
    const float white[4];
    const float dim_white[4];
    const float grey_background[4];

    // colors used for materials
    const float base_color[3];
    const float highlight_color[3];
    const float box_color[3];
    // The perspective camera
    Camera *camera;
  
    // The simulation model
    Model *themodel;

    // Switches to turn lights on and off
    bool KeyOn;
    bool FillOn;
    bool BackOn;
  
    // Switch to determine background color
    bool BackgroundGrey;

    // Current window dimensions
    int Width;
    int Height;

    // position the lights, never called outside of this class
    void setLights();
  
   // draw the model, never called outside of this class
    void drawModel();
  
  public:
    View(Model *model = NULL);
  
    // initialize the state of the viewer to start-up defaults
    void setInitialView();

    // Toggle lights on/off
    void toggleKeyLight();
    void toggleFillLight();
    void toggleBackLight();
  
    // Toggle background color grey/black
    void toggleBackColor();

    // Handlers for mouse events
    void handleButtons(int button, int state, int x, int y, bool shiftkey);
    void handleMotion(int x, int y);
  
    // redraw the display
    void updateDisplay();

    // handle window resizing, to keep consistent viewscreen proportions
    void reshapeWindow(int width, int height);
  
    // accessors to determine current screen width and height
    int getWidth(){return Width;}
    int getHeight(){return Height;}
};

#endif

