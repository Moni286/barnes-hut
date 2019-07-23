#include "Cell.h"

#include "Particle.h"

#include <stdexcept>
#include <iostream>
#include <queue>
#include <algorithm>
#include <iterator>

bool rectangle_contains(const Rectangle & r, const Vector2d & p) {
	return (r.topLeft.x < p.x) && (r.topLeft.y < p.y) &&
		(r.bottomRight.x >= p.x) && (r.bottomRight.y >= p.y);
};

Cell::Cell(Rectangle & corners, std::vector<Particle *>& particles, bool is_root)
{

	validate_corners(corners);
	CellProperties cellProperties;

	cellProperties.particles = particles;
	
	cellProperties.corners = Rectangle(corners);
	cellProperties.midpoint = Vector2d((corners.topLeft + corners.bottomRight) / 2);

	cellProperties.com = Vector2d(0, 0);
	cellProperties.valuesUpdated = false;

	this->set_value(cellProperties);
	find_com();
	if (is_root)
		init_root_cell();
	
}

void Cell::init_root_cell() {
	if (this->value.particles.size() <= 1) {
		return;
	}

	auto minXP = std::min_element(value.particles.begin(), value.particles.end(), 
		[](Particle *p1, Particle *p2) { return p1->position.x < p2->position.x; });
	
	auto minYP = std::min_element(value.particles.begin(), value.particles.end(),
		[](Particle *p1, Particle *p2) { return p1->position.y < p2->position.y; });

	auto maxXP = std::max_element(value.particles.begin(), value.particles.end(),
		[](Particle *p1, Particle *p2) { return p1->position.x < p2->position.x; });

	auto maxYP = std::max_element(value.particles.begin(), value.particles.end(),
		[](Particle *p1, Particle *p2) { return p1->position.y < p2->position.y; });

	double minX = isinf((*minXP)->position.x) ? value.corners.topLeft.x: (*minXP)->position.x;
	double minY = isinf((*minYP)->position.y) ? value.corners.topLeft.y: (*minYP)->position.y;
	double maxX = isinf((*maxXP)->position.x) ? value.corners.bottomRight.x: (*maxXP)->position.x;
	double maxY = isinf((*maxYP)->position.y) ? value.corners.bottomRight.y: (*maxYP)->position.y;


	value.corners = Rectangle(Vector2d(minX, minY) - Vector2d(0.1, 0.1),
		Vector2d(maxX, maxY) + Vector2d(0.1, 0.1));

	std::queue<QuadtreeNode *> queue;
	queue.push(this);
	while (!queue.empty()) {
		Cell *currentCell = (Cell *)queue.front();
		queue.pop();
		
		bool hasChildren = currentCell->subdivide_quadrants();
		if (hasChildren) {
			for (int i = 0; i < 4; i++) {
				queue.push(currentCell->get_children()[i]);
			}
		}
	}
	
}

bool Cell::subdivide_quadrants() {
	value.valuesUpdated = false;
	if (value.particles.size() <= 1) {
		clearChildren();
		return false;
	}

	const Rectangle& corners = value.corners;
	const Vector2d& midpoint = get_midpoint();

	Rectangle topLeftQ, topRightQ, bottomRightQ, bottomLeftQ;
	std::vector<Particle *> topLeftP, topRightP, bottomRightP, bottomLeftP;

	topLeftQ = Rectangle(corners.topLeft, midpoint);
	bottomRightQ = Rectangle(midpoint, corners.bottomRight);

	topRightQ = Rectangle(Vector2d(midpoint.x, corners.topLeft.y),
		Vector2d(corners.bottomRight.x, midpoint.y));

	bottomLeftQ = Rectangle(Vector2d(corners.topLeft.x, midpoint.y),
		Vector2d(midpoint.x, corners.bottomRight.y));

	auto i = value.particles.begin();
	while (i != value.particles.end()) {
		if (rectangle_contains(topLeftQ, (*i)->position))
			topLeftP.push_back(*i);

		else if (rectangle_contains(topRightQ, (*i)->position))
			topRightP.push_back(*i);

		else if (rectangle_contains(bottomRightQ, (*i)->position))
			bottomRightP.push_back(*i);

		else if (rectangle_contains(bottomLeftQ, (*i)->position))
			bottomLeftP.push_back(*i);

		else
			throw new std::out_of_range("wtf");

		i++;

	}

	if (get_children()[0] == nullptr) {
		get_children()[0] = new Cell(topLeftQ, std::move(topLeftP), false);
		get_children()[1] = new Cell(topRightQ, std::move(topRightP), false);
		get_children()[2] = new Cell(bottomRightQ, std::move(bottomRightP), false);
		get_children()[3] = new Cell(bottomLeftQ, std::move(bottomLeftP), false);
	}
	else {
		get_children()[0]->get_value().particles = std::move(topLeftP);
		get_children()[1]->get_value().particles = std::move(topRightP);
		get_children()[2]->get_value().particles = std::move(bottomRightP);
		get_children()[3]->get_value().particles = std::move(bottomLeftP);

		get_children()[0]->get_value().corners = topLeftQ;
		get_children()[1]->get_value().corners = topRightQ;
		get_children()[2]->get_value().corners = bottomRightQ;
		get_children()[3]->get_value().corners = bottomLeftQ;

		for (int i = 0; i < 4; i++) {
			get_children()[i]->get_value().valuesUpdated = false;
		}
	}

	return true;
}


void Cell::validate_corners(Rectangle & corners) {
	if (corners.topLeft.x > corners.bottomRight.x) {
		throw std::invalid_argument("bottomRight.x <= topLeft.x");
	}

	if (corners.topLeft.y > corners.bottomRight.y) {
		throw std::invalid_argument("bottomRight.y <= topLeft.y");
	}
}

Vector2d Cell::get_midpoint()
{
	return (value.corners.topLeft + value.corners.bottomRight) / 2;
}

Vector2d Cell::find_com() {
	if (value.valuesUpdated == true) {
		return value.com;
	}

	value.totalMass = 0;
	value.com = Vector2d(0, 0);

	for (auto i = value.particles.begin(); i != value.particles.end(); i++) {
		value.com = value.com + ((*i)->mass * (*i)->position);
		value.totalMass += (*i)->mass;
	}

	value.com = (value.totalMass == 0) ? get_midpoint() : (value.com / value.totalMass);
	value.valuesUpdated = true;
	return value.com;
}
