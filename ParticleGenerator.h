/*
* ParticleGenerator.h
* CPSC 8170 Physically Based Animation
* Author: Caroline Requierme (crequie@clemson.edu)
* Date: 9/21/2018
* Version 1.0
*/

#ifndef __PARTICLEGENERATOR_H__
#define __PARTICLEGENERATOR_H__

#include "Vector.h"
#include "Particle.h"
#include "ParticleList.h"

class ParticleGenerator{
   private:

      ParticleList pl;
      ParticleList* plPointer;
      Vector3d position;	// center position of generator
      float radius;		// radius of spherical generator

      float timeStart; 		// time when generator should start generating particles
      float timeStop;		// time when generator should stop generating particles
      int generationRate;	// rate of generation of particles per second
      float f; 			// accumulated fraction of particles

      float meanInitSpeed;	// average initial speed of particles
      float speedRange;		// range of particle speeds
      float meanLifespan;	// average lifespan of particles
      float lifespanRange;


   public:
      ParticleGenerator();
      ParticleGenerator( int numParticles, Vector3d x, float start_t, float stop_t, int gen_r);

      void setSpeedParams(float mean, float range);
      void setLifespanParams(float mean, float range);
      void setRadius(float rad);
      void setPosition(Vector3d pos);
      void setStartStopTimes(float start, float stop);

      double gauss(double mean, double std, int seed);
      double uniform(double min, double max);
      Vector3d randSphereVec();
      bool shouldGenerate(float t);
      void generateParticles(float t, float h);
      
      void testAndDeactivate(float h, float t){pl.testAndDeactivate(h, t);}
      void computeAccelerations(float drag){pl.computeAccelerations(drag);}
      void integrate(float h){pl.integrate(h);}

      ParticleList* getParticleList(){plPointer = &pl; return plPointer;}
      int getNumParticles(){return pl.getNumParticles();}
};

#endif
