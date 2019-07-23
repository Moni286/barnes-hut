#pragma once

#include "GeometryTypes.h"

using namespace geometry;

typedef unsigned long long uint64_t;

class Particle
{
public:
	Particle(uint64_t id, Vector2d position=Vector2d(), Vector2d velocity=Vector2d(), double mass=1) : 
		id(id), position(position), velocity(velocity), mass(mass) {};

	bool operator==(Particle & right);

	~Particle() {};
	
	uint64_t id;
	double mass = 1;
	Vector2d position = Vector2d();
	Vector2d velocity = Vector2d();
	Vector2d acc = Vector2d();
	
	bool updated = false;

};

