/*
* ParticleList.cpp
* CPSC 8170 Physically Based Animation
* Author: Caroline Requierme (crequie@clemson.edu)
* Date: 9/19/2018
* Version 1.1
*
* ParticleList contains the data structures necessary to
* hold and access all of the particles in the particle system.
* Because of the large number of particles that may be generated
* with any given system, it is important to allocate space for
* the particles statically and efficiently (only once at the
* beginning of the simulation). In addition, particles need to be
* activated and deactivated when they are no longer contributing to 
* the scene in order to increase performance efficiency.
* To achieve this, a ParticleList contains:
*
* particles, a basic array containing Particle objects-- represents
*            ALL of the particles that may be present in the whole
*            throughout the simulation. Each particle has a boolean
*	     attribute that tells the ParticleList whether the particle
*	     is currently active in the scene. The number of particles
*            is determined in the Model before the simulation begins
*            so that this memory is only allocated once.
*
* inactiveStack, an array equal in length to particles (length = number
*                of particles) which acts like a stack and holds the
*                indeces of inactive Particle objects in particles
*                list. Initially, this will contain the indeces of all of
*                particles in the system (all Particles are initialized as 
*                inactive). Then, when the ParticleGenerator is adding particles
*                to the scene, it will "pop" the top index from the 
*                inactiveStack, and activate the Particle at that index 
*		 in particles list.
*
* inactiveCount, int which stores the index of the "top" of the 
*		 inactiveStack. Note that this is not an index in the 
*                particles list, but rather to the inactiveStack array 
*                and allows us to use the array like a stack. When the 
*                ParticleGenerator wants to activate a Particle, it will 
*                call the inactiveCount to find the index of the next
*                inactive Particle in inactiveStack. From here, it can 
*		 index the particles list. When a Particle is activated,
*		 inactiveCount is decremented by one. When a Particle is
*		 deactivated, inactiveCount is incremented and the index 
*                of that particle is made added to the top of the inactiveStack.
*
* Note that this data structure does not have a way of looping through just the
* active particles, so when particles need to be accessed, you must loop through
* the entire particles list. 
*
***********************************************************************************************/

#include "Particle.h"
#include "ParticleList.h"
#include "Vector.h"
#include <assert.h>

using namespace std;

//-----------------------------------------------------------------
/*
ParticleList::ParticleList()
* PURPOSE : Default constructor
* INPUTS :  NONE
* OUTPUTS : NONE, initaializes empty ParticleList
*/
//-----------------------------------------------------------------

ParticleList::ParticleList()
{
   numParticles = 0;
   particles = NULL;
   inactiveStack = NULL;
   inactiveCount = 0;

}
		
//-----------------------------------------------------------------
/*
ParticleList::ParticleList(int np)
* PURPOSE : Variable constructor
* INPUTS :  int np, number of particles in the system
* OUTPUTS : NONE, initializes ParticleList
*/
//-----------------------------------------------------------------

ParticleList::ParticleList(int np)
{
   numParticles = np;
   particles = new Particle[numParticles];
   inactiveStack = new int[numParticles];
   inactiveCount = numParticles;

   for (int i=0; i < numParticles; i++){  // Construct list of particles
      particles[i] = Particle();	  // Use default constructor, all particles are inactive
      inactiveStack[i] = i;		  // B/c all particles are inactive, fill inactive stack with all indeces
   }

}

//-----------------------------------------------------------------
/*
ParticleList::clear()
* PURPOSE : Deactivate all particles in the system
* INPUTS :  NONE
* OUTPUTS : NONE, updates particles, inactiveStack, inactiveCount
*/
//-----------------------------------------------------------------

void ParticleList::clear()
{
   for (int i = 0; i < numParticles; i++){	// Deactivate all particles
      particles[i].isActive = false;
      inactiveStack[i] = i;			// Reset inactive stack with all indeces		
   }

   inactiveCount = numParticles;		// Update inactive count
}

//-----------------------------------------------------------------
/*
bool ParticleList::inStack(int i)
* PURPOSE : 
* INPUTS :  
* OUTPUTS : 
*/
//-----------------------------------------------------------------

bool ParticleList::inStack(int i)
{
   bool inIS = false;
   for (int j=0; j < numParticles; j++){
      if (i == inactiveStack[j]){
         inIS = true;
      }
   }

   return inIS;
}

//-----------------------------------------------------------------
/*
ParticleList::shouldKill(Particle p)
* PURPOSE : Decide whether a particle should be deactivated based
*           on the system's death conditions (defined here). These will
*           vary based on the system.
* INPUTS :  Particle p, current particle being tested
            float t, current time
* OUTPUTS : bool, true if should be deactivated, false if can stay active
*/
//-----------------------------------------------------------------

bool ParticleList::shouldKill(Particle p, float t){				// **DEFINE PARTICLE DEATH CONDITIONS HERE**
   bool dead = false;
   float particleAge = t - p.timestamp;

   if (particleAge >= p.lifespan){
      dead = true;
   }
   
   return dead;
}

//-----------------------------------------------------------------
/*
ParticleList::testAndDeactivate(float h)
* PURPOSE : Loop through the list of particles, testing if each 
*           active particle should be killed based on timestamp, 
*           collision, etc. If so, deactivate the particle. If not,
*           increment the particle's timestamp. This function will
*           be called at every timestep in the simulation. 
* INPUTS :  float h, timestep of the simulation
* OUTPUTS : NONE, particles, inactiveStack, and inactiveCount are
*           updated
*/
//-----------------------------------------------------------------

void ParticleList::testAndDeactivate(float h, float t)
{

   for (int i = 0; i < numParticles; i++){		// Loop through list of particles

      if (particles[i].isActive == true){		// Look for active particles
         if (shouldKill(particles[i], t) == true){ 	// Ask if particle should be killed

	    particles[i].isActive = false;		// Deactivate dead particles
               
             //if(inStack(i) == false){
               //std::cout << i << std::endl;
	       inactiveStack[inactiveCount] = i;	// Push to inactive stack
              // std::cout << "inactive before: " << inactiveCount << std::endl;
	       inactiveCount = inactiveCount + 1;	// Update inactive count
              // std::cout << "inactive after: " << inactiveCount << std::endl;
	    //}
         }

      } 
   }
}

//-----------------------------------------------------------------
/*
ParticleList::computeAccelerations(float drag)
* PURPOSE : Ask each active particle to calculate and store current
*           acceleration based on the forces in the scene and 
*           the particle's current velocity
* INPUTS :  float drag, property that defines air resistance
* OUTPUTS : NONE, updates Particle acceleration values
*/
//-----------------------------------------------------------------

void ParticleList::computeAccelerations(float drag)
{
   Vector3d Fg, Fa, Fair;						
   Fa.set(0.0, -9.8, 0.0);				// Force of gravity defined here

   for (int i = 0; i < numParticles; i++){		// For all active particles
      if (particles[i].isActive  == true){
         Fg = particles[i].mass * Fa;
         Fair = -drag * particles[i].velocity;
         Vector3d Ftotal = Fg + Fair;			// Forces present here include gravity and air resistance
 
         particles[i].acceleration = Ftotal/particles[i].mass;  // Update acceleration
      }
   }
}

//-----------------------------------------------------------------
/*
ParticleList::integrate(float h)
* PURPOSE : Perform Euler integration on active particles to update
*           velocity and position.
* INPUTS :  float h, simulation timestep
* OUTPUTS : NONE, update Particle attributes
*/
//-----------------------------------------------------------------

void ParticleList::integrate(float h){
   for (int i = 0; i < numParticles; i++){
 						// Loop through active particles
      if(particles[i].isActive == true){
         Vector3d v_new = particles[i].velocity + (h * particles[i].acceleration);	// Euler integration
         Vector3d x_new = particles[i].position + (h * particles[i].velocity);

         particles[i].prev_position = particles[i].position;
         
         particles[i].velocity = v_new;							// Update pos. and veloc.
	 particles[i].position = x_new;
      }
   }
}

//-----------------------------------------------------------------
/*
ParticleList::popInactiveStack()
* PURPOSE : Return the index of the particle that is at the top of
*           the inactive stack, called when the ParticleGenerator
*           needs to add particles to the scene
* INPUTS :  NONE
* OUTPUTS : int, index of next available inactive particle in particles
*           list
*/
//-----------------------------------------------------------------

int ParticleList::topInactiveStack(){
   int topIndex;

   if (inactiveCount >= 0){
      topIndex = inactiveStack[inactiveCount];	// Pop inactiveStack to get top index	
   }
   else{
      topIndex = -1;	// Rare case: all particles are active, cannot generate more
   }

   return topIndex;
}

//-----------------------------------------------------------------
/*
ParticleList::activateParticle(int pIndex, Vector3d pos, Vector3d vel, float ls)
* PURPOSE : Activate particle at the top of the inactivestack
* INPUTS :  int 
* OUTPUTS : int, index of next available inactive particle in particles
*           list
*/
//-----------------------------------------------------------------

void ParticleList::activateTopParticle(Vector3d pos, Vector3d vel, float ls, float ts)
{
   //std::cout << "IC2: " << inactiveCount << std::endl;

   if (inactiveCount > 0 && inactiveCount <= numParticles){
      int pIndex = inactiveStack[inactiveCount-1];
      
      particles[pIndex].position = pos;
      particles[pIndex].velocity = vel;
      particles[pIndex].lifespan = ls;
      particles[pIndex].timestamp = ts;
      particles[pIndex].isActive = true;
      
      inactiveCount = inactiveCount - 1;
   }
   //std::cout << "IC3: " << inactiveCount << std::endl;
}


