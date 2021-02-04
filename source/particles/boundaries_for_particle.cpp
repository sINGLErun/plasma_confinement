#ifndef PARTICLES_H
#define PARTICLES_H
	#include "particles.hpp"
#endif

void reflective_Xboundaries_for(particle& _particle, double SIZE_X)
{
	if ( _particle.r().x() < 0 ) {
		_particle.r().x() = 0;
		_particle.p().x() = - _particle.p().x(); 
	} else if ( _particle.r().x() > SIZE_X ) {
		_particle.r().x() = SIZE_X;
		_particle.p().x() = - _particle.p().x();
	}
}

void reflective_Yboundaries_for(particle& _particle, double SIZE_Y)
{
	if ( _particle.r().y() < 0 ) {
		_particle.r().y() = 0;
		_particle.p().y() = - _particle.p().y(); 
	} else if ( _particle.r().y() > SIZE_Y ) {
		_particle.r().y() = SIZE_Y;
		_particle.p().y() = - _particle.p().y(); 
	}
}

void periodic_Xboundaries_for(particle& _particle, double SIZE_X)
{
	if ( _particle.r().x() < 0 ) { _particle.r().x() = SIZE_X; }
	else if ( _particle.r().x() > SIZE_X ) { _particle.r().x() = 0; }
}

void periodic_Yboundaries_for(particle& _particle, double SIZE_Y)
{
	if ( _particle.r().y() < 0 ) { _particle.r().y() = SIZE_Y; }
	else if ( _particle.r().y() > SIZE_Y ) { _particle.r().y() = 0; }
}