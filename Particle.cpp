#include "Particle.h"

#include "Cell.h"

bool Particle::operator==(Particle & right){
	return this->id == right.id;
}

