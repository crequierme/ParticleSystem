/*
* ParticleList.h
* CPSC 8170 Physically Based Animation
* Author: Caroline Requierme (crequie@clemson.edu)
* Date: 9/18/2018
* Version 1.0
*/

#ifndef __PARTICLELIST_H__
#define __PARTICLELIST_H__

#include "Vector.h"
#include "Particle.h"

class ParticleList{
	private:
		int numParticles;	// total number of particles in system
		
	public:
		ParticleList();			
                ParticleList(int np);

		Particle* particles;	// particles, array holds all Particle objects
		int* inactiveStack;	// inactiveStack, array acts a stack, holds indeces of inactive particles
		int inactiveCount;	// inactiveCount, number of inactive particles, indexes next empty space in inactiveStack
		
                int getNumParticles(){return numParticles;}
                int getInactiveCount(){return inactiveCount;}
		void clear();
		bool inStack(int i);
                bool shouldKill(Particle p, float t);
		void testAndDeactivate(float h, float t);
		void computeAccelerations(float drag);
		void integrate(float h);
		int topInactiveStack();
	        void activateTopParticle(Vector3d pos, Vector3d vel, float ls, float ts);

};	

#endif
