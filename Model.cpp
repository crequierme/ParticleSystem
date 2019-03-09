//********************************************************************************
/*
 Model.cpp
 CPSC 8170 Physically Based Animation
 Donald H. House     8/23/2018
 Modified by Caroline Requierme (crequie@clemson.edu), 9/22/2018

 Implementation for Particle System Simulation
*/
//********************************************************************************

#include "Model.h"
#include "Vector.h"
#include "Particle.h"
#include "ParticleList.h"
#include "ParticleGenerator.h"

#include <cstdlib>
#include <cstdio>
#include <cmath>


using namespace std;

//-----------------------------------------------------------------
/*
Model::Model()
* PURPOSE : Default constructor
* INPUTS :  None
* OUTPUTS : None, initiates the simulation
*/
//-----------------------------------------------------------------

Model::Model(){
  initSimulation();
}

//-----------------------------------------------------------------
/*
Model::initSimulation()
* PURPOSE : Initiate the particle system and simulation properties
* INPUTS :  NONE, uses class variables
* OUTPUTS : NONE, defines class variables
*/
//-----------------------------------------------------------------

void Model::initSimulation(){

   h = 0.01;		// simulation timestep
   dispinterval = 1;	// display interval

   drag = 0.2;		

//***** DEFINE PARTICLE LIST HERE *********************
   numParticles = 50000;	// total number of particles in the system
   numGenerators = 3;
   generators = new ParticleGenerator [numGenerators];

//*****************************************************

//***** DEFINE PARTICLE GENERATORS HERE ****************
   Vector3d x;
   x.set(0.0, 10.0, 0.0);
   float start_t = 0.0;
   float stop_t = 4.5;
   int gen_r = 5000;

   pg = ParticleGenerator(numParticles, x, start_t, stop_t, gen_r);
  
   float mean_s = 5.0;
   float s_range = 1.0;
   float mean_ls = 1.5;
   float ls_range = 0.5;
   float rad = 12.0;

   pg.setSpeedParams(mean_s, s_range);
   pg.setLifespanParams(mean_ls, ls_range);
   pg.setRadius(rad);
//--------------------------------------------------------

   Vector3d x2;
   x2.set(10.0, 0.0, 0.0);
   float start_t2 = 0.0;
   float stop_t2 = 3.5;
   int gen_r2 = 4000;

   pg2 = ParticleGenerator(numParticles, x2, start_t2, stop_t2, gen_r2);

  
   float mean_s2 = 20.0;
   float s_range2 = 8.0;
   float mean_ls2 = 1.0;
   float ls_range2 = 0.5;
   float rad2 = 6.0;

   pg2.setSpeedParams(mean_s2, s_range2);
   pg2.setLifespanParams(mean_ls2, ls_range2);
   pg2.setRadius(rad2);
//--------------------------------------------------------
   Vector3d x3;
   x3.set(-18.0, -5.0, 4.0);
   float start_t3 = 0.0;
   float stop_t3 = 2.5;
   int gen_r3 = 3000;

   pg3 = ParticleGenerator(numParticles, x3, start_t3, stop_t3, gen_r3);

  
   float mean_s3 = 12.0;
   float s_range3 = 3.0;
   float mean_ls3 = 0.8;
   float ls_range3 = 0.2;
   float rad3 = 3.0;

   pg3.setSpeedParams(mean_s3, s_range3);
   pg3.setLifespanParams(mean_ls3, ls_range3);
   pg3.setRadius(rad3);

   generators[0] = pg;
   generators[1] = pg2;
   generators[2] = pg3;
//*****************************************************
   for (int i = 0; i < numGenerators; i++){
      ParticleList *pl = generators[i].getParticleList();
      pl->clear();
   }



   running = false;	// flag to mark simulation is running
   t = 0.0; 		// current time
   n = 0;		// current step

}

//-----------------------------------------------------------------
/*
Model::timeStep()
* PURPOSE : Perform one time step in the simulation
* INPUTS :  None
* OUTPUTS : None, updates particles 
*/
//-----------------------------------------------------------------

void Model::timeStep(){

  if(running){
     for (int i = 0; i < numGenerators; i++)
     {   
        generators[i].generateParticles(t, h);	// generate particles
        generators[i].testAndDeactivate(h, t);  	// deactivate dead particles
        generators[i].computeAccelerations(drag);	// compute accelerations of particles
        generators[i].integrate(h);			// Euler integration
        n = n + 1;				// update time
        t = n * h; 
     }
  }
}

//-----------------------------------------------------------------
/*
Model::startSimulation()
* PURPOSE : Start the simulation
* INPUTS :  None
* OUTPUTS : None, changes state of boolean flag
*/
//-----------------------------------------------------------------

void Model::startSimulation(){
  running = true;
}
