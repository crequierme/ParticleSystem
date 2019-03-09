/*
* Particle.cpp
* CPSC 8170 Physically Based Animation
* Author: Caroline Requierme (crequie@clemson.edu)
* Date: 9/18/2018
* Version 1.0
*
* Particle is a C++ class object constructed to hold the
* attributes of a particle generated by a particle system.
* In the system, a ParticleGenerator is responsible for
* initializing these Particle objects with randomized values.
* Particle attributes are later accessed by the View to 
* represent the particles in the scene as GL_POINTS. 
*/

#include "Particle.h"
#include "Vector.h"

using namespace std;

//-----------------------------------------------------------------
/*
Particle::Particle()
* PURPOSE : Default constructor
* INPUTS :  None
* OUTPUTS : None, initializes a particle
*/
//-----------------------------------------------------------------

Particle::Particle()		// Default constructor
{	
   Vector3d x;
   x.set(0.0, 0.0, 0.0);	// Positioned at the origin
   Vector3d v;
   v.set(0.0, 0.0, 0.0);	// No velocity or initial acceleration
   Vector3d a;
   a.set(0.0, 0.0, 0.0);

   position = x;
   prev_position = x;
   velocity = v;
   acceleration = a;
   mass = 0.5;

   timestamp = 0.0;
   isActive = false;

   lifespan = 0.0;
}

//-----------------------------------------------------------------
/*
Particle::Particle(Vector3d x, Vector3d v, float m, float ls)
* PURPOSE : Variable constructor
* INPUTS :  Vector3d x, position of particle
            Vector3d v, initial velocity of particle
	    float m, mass of particle
	    float ls, lifespan of particle
* OUTPUTS : None, initializes a particle
*/
//-----------------------------------------------------------------

Particle::Particle(Vector3d x, Vector3d v, float m, float ls)	// Variable constructor
{
   position = x;
   prev_position = x;
   velocity = v;
   timestamp = 0.0;
   isActive = false;
   mass = fabs(m);						// Mass and lifespan must be positive
   lifespan = fabs(ls);
}

