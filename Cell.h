#pragma once

#include <vector>
#include <queue>

#include "QuadtreeNode.h"
#include "GeometryTypes.h"

using namespace geometry;

class Particle;

struct CellProperties {
	Rectangle corners;
	Vector2d midpoint;
	Vector2d com;
	double totalMass;

	std::vector<Particle *> particles;
	bool valuesUpdated = false;
};

bool rectangle_contains(const Rectangle & r, const Vector2d & p);

class Cell : public QuadtreeNode<CellProperties>
{
public:
	Cell(Rectangle & corners, std::vector<Particle*>& particles, bool is_root = true);
	
	Vector2d find_com();

	void init_root_cell();
	
private:
	
	void validate_corners(Rectangle & corners);

	Vector2d get_midpoint();

	bool subdivide_quadrants();

};

