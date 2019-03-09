
/*
* ParticleGenerator.cpp
* CPSC 8170 Physically Based Animation
* Author: Caroline Requierme (crequie@clemson.edu)
* Date: 9/21/2018
* Version 1.1
*
* In the particle system, the ParticleGenerator is responsible
* for adding new particles to the scene and initializing them
* with randomized attributes. A systen may have multiple 
* ParticleGenerators and these generators come in a number of
* common forms including spheres, points, and disks. A 
* ParticleGenerator is not rendered in the scene, but does 
* determine where and how the particles move throughout the scene.
* Implemeted here is a spherical ParticleGenerator with particles
* being generated at the surface of the sphere.
*
*/
#include "ParticleGenerator.h"
#include "Particle.h"
#include "ParticleList.h"
#include "Vector.h"
#include <math.h>


using namespace std;

//-----------------------------------------------------------------
/*
ParticleGenerator::ParticleGenerator();
*
* PURPOSE : Default constructor
* INPUTS :  NONE
* OUTPUTS : NONE, initializes ParticleGenerator, sets everything to 0
*/
//-----------------------------------------------------------------

ParticleGenerator::ParticleGenerator()
{
   Vector3d x;
   x.set(0.0, 0.0, 0.0);
   
   position = x;
   timeStart = 0.0;
   timeStop = 0.0;
   generationRate = 0;

   meanInitSpeed = 0.0;
   speedRange = 0.0;
   radius = 0.0;
   meanLifespan = 0.0;
   lifespanRange = 0.0;

   plPointer = NULL;
}

//-----------------------------------------------------------------
/*
ParticleGenerator::ParticleGenerator(Vector3d x, float start_t, float stop_t, int gen_r)
*
* PURPOSE : Variable constructor
* INPUTS :  Vector3d x, position of center of generator
*           float start_t, time generator should start generating particles
*           float stop_t, time generator should stop generating particles
*           int gen_r, number of particles generated per second
* OUTPUTS : NONE, initializes ParticleGenerator
*/
//-----------------------------------------------------------------

ParticleGenerator::ParticleGenerator(int numParticles, Vector3d x, float start_t, float stop_t, int gen_r)
{
   pl = ParticleList(numParticles);
   plPointer = &pl;
   
   position = x;
   radius = 1.0;		

   timeStart = start_t;	
   timeStop = stop_t;
   generationRate = gen_r;
   f = 0.0;

   meanInitSpeed = 0.5;	
   speedRange = 0.2;
   meanLifespan = 1.0;	
   lifespanRange = 0.3;
}

//-----------------------------------------------------------------
/*
* ParticleGenerator "Setter" Functions
* These functions are not necessarily only setting one variable -- rather they are 
* categorized by usage.
*
* PURPOSE : Set private class variables with parameters
* INPUTS :  SpeedParams - mean, range - defines gaussian randomization for particle speed
	    LifespanParams - mean, range - defines gaussian randomization for particle lifespan
 	    Radius - rad - defines radius of spherical generator
	    Position - pos - defines center position of generator
	    StartStopTimes - start, stop - times to turn generator on/off
* OUTPUTS : NONE
*/
//-----------------------------------------------------------------
void ParticleGenerator::setSpeedParams(float mean, float range)
{
   meanInitSpeed = mean;
   speedRange = range;
}

void ParticleGenerator::setLifespanParams(float mean, float range)
{
   meanLifespan = mean;
   lifespanRange = range;
}

void ParticleGenerator::setRadius(float rad)
{
   radius = rad;
}

void ParticleGenerator::setPosition(Vector3d pos)
{
   position = pos;
}

void ParticleGenerator::setStartStopTimes(float start, float stop)
{
   timeStart = start;
   timeStop = stop;
}

//-----------------------------------------------------------------
/*
ParticleGenerator::gauss(double mean, double std, int seed)
*
* Function code provided by Donald House, Clemson University (dhouse@clemson.edu)
*
* This function takes as parameters real valued mean and standard-deviation,
* and an integer valued seed (makes sure you receive different numbers).  
* It returns a real number which may be interpreted as a sample of
* a normally distributed (Gaussian) random variable with the specified mean 
* and standard deviation. After the first call to gauss, the seed parameter 
* is ignored.
*/
//-----------------------------------------------------------------

double ParticleGenerator::gauss(double mean, double std, int seed)
{
   const int itblmax = 20;	// length - 1 of table describing F inverse
   const double didu = 40.0;	// delta table position/delta ind. variable

   // interpolation table for F inverse
   static double tbl[] =
      {0.00000E+00, 6.27500E-02, 1.25641E-01, 1.89000E-01,
       2.53333E-01, 3.18684E-01, 3.85405E-01, 4.53889E-01,
       5.24412E-01, 5.97647E-01, 6.74375E-01, 7.55333E-01,
       8.41482E-01, 9.34615E-01, 1.03652E+00, 1.15048E+00,
       1.28167E+00, 1.43933E+00, 1.64500E+00, 1.96000E+00,
       3.87000E+00};
  
   static int first_time = 1;

   double u;
   double di;
   int index, minus;
   double delta, gaussian_random_value;

   if (first_time){
#ifdef WIN32
      srand((unsigned)seed);
#else
      srand48(seed);
#endif
      first_time = 0;
  }

   // compute uniform random number between 0.0 - 0.5, and a sign with 
   // probability 1/2 of being either + or -
   
#ifdef WIN32
   int rn = rand();
   u = double(rn) / double(RAND_MAX);
#else
   u = drand48();
#endif
   if (u >= 0.5){
      minus = 0;
      u = u - 0.5;
   }
   else
      minus = 1;

  // interpolate gaussian random number using table

   index = (int)(di = (didu * u));
   if(index == itblmax)
      delta = tbl[index];
   else{
      di -= index;
      delta =  tbl[index] + (tbl[index + 1] - tbl[index]) * di;
    }
   gaussian_random_value = mean + std * (minus? -delta : delta);

   return gaussian_random_value;
}

//-----------------------------------------------------------------
/*
double ParticleGenerator::uniform(double min, double max)
* PURPOSE : Generate a random real number within a range of
*           numbers that can be interpreted as a sample of a 
*           uniform distribution of numbers.
* INPUTS :  double min, minimum value possible in range of random numbers
*           double max, maximum value possible in range of random numbers
* OUTPUTS : double, real, random number between min and max
*/
//-----------------------------------------------------------------

double ParticleGenerator::uniform(double min, double max)
{
   double r = ((max - min) * drand48() + min);	// drand48() returns value between 0 and 1
   return r;
}

//-----------------------------------------------------------------
/*
Vector3d ParticleGenerator::randSphereVec()
* PURPOSE : Generate a random directional vector off of the surface
*           of a given sphere. Called to assign random direction 
*           for particles with a omnidirectional or spherical 
*           ParticleGenerator
* INPUTS :  NONE, uses radius inherent in sphere generator
* OUTPUTS : Vector3d, radial directional vector
*/
//-----------------------------------------------------------------

Vector3d ParticleGenerator::randSphereVec()
{
   double theta = uniform(-PI, PI);	// azimuth angle
   double y = uniform(-1.0, 1.0); 	// height

   float r = sqrt(1 - pow(y,2));
   Vector3d v;
   v.set((r * cos(theta)), y, (-r * sin(theta)));

   return v;
}

//-----------------------------------------------------------------
/*
bool ParticleGenerator::shouldGenerate(float t)
* PURPOSE : Determines whether or not the ParticleGenerator should
*	    be currently generating particles into the scene
* INPUTS :  float t, current time in the simulation
* OUTPUTS : bool, true if ParticleGenerator should be generating
*		  false if ParticleGenerator shouldn't be generating
*/
//-----------------------------------------------------------------

bool ParticleGenerator::shouldGenerate(float t){

   bool generating = false;
   bool restart = false;
   float delay = 2.0;
   float duration = timeStop - timeStart;
   if (t <= duration){
      if ((t >= timeStart) && (t < timeStop)){
         generating = true;
      }
   }
   else{
      if ((fmod(t, duration)>= (timeStart + delay)) && (fmod(t, duration) < timeStop)){
         generating = true;
      }

   }

   return generating;
}

//-----------------------------------------------------------------
/*
ParticleGenerator::generateParticles(ParticleList pl, float t, float h)
* PURPOSE : Generate randomized particles and add them to the scene.
*	    These particles will come from the pre-allocated ParticleList
*           and are initialized with random values.
* INPUTS :  ParticleList pl, ParticleList which holds all the particles
*           that are available for the scene.
*	    float t, current time in simulation
*	    float h, timestep in simulation
* OUTPUTS : NONE, update particle attributes
*/
//-----------------------------------------------------------------

void ParticleGenerator::generateParticles(float t, float h){
   if (shouldGenerate(t) == true){		// Is the generator on?

      //std::cout << "IC1: " << pl.inactiveCount << std::endl;
      int n = floor(generationRate * h);      	// Get number of particles to generate
      
      f = f + ((generationRate * h) - n);	// Update accumulative fraction (leftover)
      if (f > 1){
         n = n + 1;
         f = f - 1.0;
      }



      for(int i = 0; i < n; i ++){		// For number of particles to be generated
         
         if (pl.inactiveCount > 0){			// As long as there are still particles left to activate..
	    // speed
            float s = gauss(meanInitSpeed, speedRange/3, 1);	// Randomize initial values
	    // direction 
	    Vector3d u = randSphereVec();
            //velocity
            Vector3d v = fabs(s) * u;
	    // position
            Vector3d x = position + (radius * u);
	    // lifespan 
	    float l = gauss(meanLifespan, lifespanRange/3, 2);

	    pl.activateTopParticle(x, v, l, t);

         }
      }
   }

}

