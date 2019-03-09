/*
* Particle.h
* CPSC 8170 Physically Based Animation
* Author: Caroline Requierme (crequie@clemson.edu)
* Date: 9/18/2018
* Version 1.0
*/

#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "Vector.h"

class Particle{				// Particle attributes are made public for benefit of the View
	public:
		Vector3d position;
                Vector3d prev_position;	
		Vector3d velocity;
		Vector3d acceleration;
		float mass;
		float timestamp;	// timestamp, How long has the particle been active?
		bool isActive;
		float lifespan;		// lifespan, Particle is deactivated after this time

		Particle();						// Default constructor
		Particle(Vector3d x, Vector3d v, float m, float ls);	// Variable Constructor
};

#endif
