/*
 Model.h
 CPSC 8170 Physically Based Animation
 Donald H. House     8/26/2018
 Modified by Caroline Requierme (crequie@clemson.edu), 8/31/2018
*/

#ifndef __MODEL_H__
#define __MODEL_H__

#include "Vector.h"
#include "Particle.h"
#include "ParticleList.h"
#include "ParticleGenerator.h"

class Model{
  private:

    float h; 		// h, timestep
    int dispinterval;	
    float drag;		// drag, defines air resistance
    int numParticles;	// total number of particles in system


    bool running;	// flag to start simulation
    float t;		// t, Current time
    int n;		// number timesteps

    ParticleGenerator pg;
    ParticleGenerator pg2;
    ParticleGenerator pg3;

    int numGenerators;
    
  public:
    ParticleGenerator *generators;

    Model();
    void initSimulation();
    void timeStep();
    void startSimulation();       

    int getNumParticles(){return numParticles;}
    ParticleGenerator getGen1(){return pg;}
    ParticleGenerator getGen2(){return pg2;}
    ParticleGenerator getGen3(){return pg3;}

    bool isSimRunning(){return running;}
    int displayInterval(){return dispinterval;}

};

#endif
