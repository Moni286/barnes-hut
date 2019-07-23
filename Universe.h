#pragma once

#include "GeometryTypes.h"
#include "Cell.h"
#include "Particle.h"

#include <memory>

class Universe
{
public:
	class Builder 
	{
	public:

		Builder();

		Builder & fromJson(std::string & jsonStr);

		Builder& set_particle_count(size_t nParticles);

		Builder& set_corners(geometry::Rectangle & corners);

		Builder& set_G(double G);

		Builder& set_timestep(double timestep);

		Builder& set_seed(int seed);

		std::unique_ptr<Universe> build();

	private:
		double rand_double(double min, double max);

		size_t nParticles;
		geometry::Rectangle corners;
		double G;
		double timestep;

		double minMass;
		double maxMass;

		double minVelocity;
		double maxVelocity;

		int seed;

		static const char *JSON_CORNERS;
		static const char *JSON_TOPLEFT;
		static const char *JSON_BOTTOMRIGHT;
		static const char *JSON_X;
		static const char *JSON_Y;

		static const char *JSON_MINMASS;
		static const char *JSON_MAXMASS;
		static const char *JSON_MINVELOCITY;
		static const char *JSON_MAXVELOCITY;

		static const char *JSON_G;
		static const char *JSON_TIMESTEP;

		static const char *JSON_NPARTICLES;
		static const char *JSON_SEED;
	};

	static std::unique_ptr<Builder> get_builder();

	Universe(Cell * rootCell, double G, double timestep);

	const Cell *get_root_cell();

	const std::vector<Particle *> & update_particles();

	const std::vector<Particle *> & get_particles();

	void update_cells();

	Vector2d find_acceleration_at(Vector2d & pos);

	Vector2d find_next_position(Particle & particle, Vector2d & fieldStrength);

	~Universe();

private:

	Cell *rootCell;
	double G;
	double timestep;
		
};

