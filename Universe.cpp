#include "Universe.h"

#include <memory>
#include <map>
#include <cctype>

#include <rapidjson\document.h>
#include <rapidjson\prettywriter.h>

const char *Universe::Builder::JSON_CORNERS = "corners";
const char *Universe::Builder::JSON_TOPLEFT = "topleft";
const char *Universe::Builder::JSON_BOTTOMRIGHT = "bottomright";
const char *Universe::Builder::JSON_X = "x";
const char *Universe::Builder::JSON_Y = "y";

const char *Universe::Builder::JSON_G = "g";
const char *Universe::Builder::JSON_TIMESTEP = "timestep";

const char *Universe::Builder::JSON_NPARTICLES = "nparticles";
const char *Universe::Builder::JSON_SEED = "seed";

const char *Universe::Builder::JSON_MINMASS = "minmass";
const char *Universe::Builder::JSON_MAXMASS = "maxmass";

const char *Universe::Builder::JSON_MINVELOCITY = "minVelocity";
const char *Universe::Builder::JSON_MAXVELOCITY = "maxVelocity";

Universe::Builder::Builder()
{
	Vector2d topLeft(0, 0);
	Vector2d bottomRight(1000, 1000);
	corners = Rectangle(topLeft, bottomRight);

	nParticles = 1024;
	G = 1.559 * std::pow(10, -13);
	timestep = 1;
	seed = 0;

	minMass = 1;
	maxMass = 1;

	minVelocity = 0;
	maxVelocity = 0.1;
}

Universe::Builder & Universe::Builder::fromJson(std::string & jsonStr)
{
	std::transform(jsonStr.begin(), jsonStr.end(), jsonStr.begin(), std::tolower);

	rapidjson::Document jsonDoc;
	jsonDoc.Parse<0>(jsonStr.c_str());

	auto &cornersJson = jsonDoc.FindMember(JSON_CORNERS);
	if (cornersJson != jsonDoc.MemberEnd()) {

		auto & bottomRightJson = cornersJson->value[JSON_BOTTOMRIGHT];
		corners.bottomRight.x = bottomRightJson[JSON_X].GetDouble();
		corners.bottomRight.y = bottomRightJson[JSON_Y].GetDouble();

		auto & topLeftJson = cornersJson->value[JSON_TOPLEFT];
		corners.topLeft.x = topLeftJson[JSON_X].GetDouble();
		corners.topLeft.y = topLeftJson[JSON_Y].GetDouble();
		
	}
	
	const auto &memberEnd = jsonDoc.MemberEnd();

	auto &gJson = jsonDoc.FindMember(JSON_G);
	G = (gJson == memberEnd) ? G : gJson->value.GetDouble();

	auto &timestepJson = jsonDoc.FindMember(JSON_TIMESTEP);
	timestep = (timestepJson == memberEnd) ? timestep : timestepJson->value.GetDouble();

	auto &nParticlesJson = jsonDoc.FindMember(JSON_NPARTICLES);
	nParticles = (nParticlesJson == memberEnd) ? nParticles : nParticlesJson->value.GetInt64();

	auto &seedJson = jsonDoc.FindMember(JSON_SEED);
	seed = (seedJson == memberEnd) ? seed : seedJson->value.GetInt();

	auto &minMassJson = jsonDoc.FindMember(JSON_MINMASS);
	minMass = (minMassJson == memberEnd) ? minMass : minMassJson->value.GetDouble();

	auto &maxMassJson = jsonDoc.FindMember(JSON_MAXMASS);
	maxMass = (maxMassJson == memberEnd) ? maxMass : maxMassJson->value.GetDouble();

	auto &minVelocityJson = jsonDoc.FindMember(JSON_MINVELOCITY);
	minVelocity = (minVelocityJson == memberEnd) ? minVelocity : minVelocityJson->value.GetDouble();

	auto &maxVelocityJson = jsonDoc.FindMember(JSON_MAXVELOCITY);
	maxVelocity = (maxVelocityJson == memberEnd) ? maxVelocity : maxVelocityJson->value.GetDouble();

	return *this;
}

Universe::Builder& Universe::Builder::set_particle_count(size_t nParticles)
{
	this->nParticles = nParticles;
	return *this;
}

Universe::Builder& Universe::Builder::set_corners(Rectangle& corners)
{
	this->corners = corners;
	return *this;
}

Universe::Builder & Universe::Builder::set_G(double G)
{
	this->G = G;
	return *this;
}

Universe::Builder & Universe::Builder::set_timestep(double timestep)
{
	this->timestep = timestep;
	return *this;
}

Universe::Builder & Universe::Builder::set_seed(int seed) {
	this->seed = seed;
	return *this;
}

double Universe::Builder::rand_double(double min, double max) {
	if (min == max) return min;

	int randInt = rand();
	return (max - min) * (double)randInt / RAND_MAX + min;
}

std::unique_ptr<Universe::Builder> Universe::get_builder()
{
	return std::make_unique<Builder>();
}

std::unique_ptr<Universe> Universe::Builder::build()
{
	srand(seed);

	std::vector<Particle *> particles;

	for (size_t i = 0; i < nParticles; i++) {
		Vector2d position;
		position.x = rand_double(corners.topLeft.x+0.1, corners.bottomRight.x-0.1);
		position.y = rand_double(corners.topLeft.y+0.1, corners.bottomRight.y-0.1);
		
		double velocityX = rand_double(minVelocity, maxVelocity);
		velocityX = rand() > RAND_MAX / 2 ? velocityX : -velocityX;

		double velocityMagnitude = rand_double(std::abs(velocityX), maxVelocity);

		double velocityY = std::sqrt(std::pow(velocityMagnitude, 2) - std::pow(velocityX, 2));
		velocityY = rand() > RAND_MAX / 2 ? velocityY : -velocityY;

		Vector2d velocity(velocityX, velocityY);

		double mass = rand_double(minMass, maxMass);

		particles.push_back(new Particle(i, position, velocity, mass));
	}

	Cell *root = new Cell(corners, particles);

	return std::make_unique<Universe>(root, G, timestep);
}


Universe::Universe(Cell *rootCell, double G, double timestep)
{
	this->G = G;
	this->timestep = timestep;
	this->rootCell = rootCell;
}

const Cell *Universe::get_root_cell() {
	return rootCell;
}

// 1. find acceleration for each particle in parallel
const std::vector<Particle *> & Universe::update_particles() {

	std::vector<Vector2d> nextPositions;
	std::vector<Particle *> & particles = rootCell->get_value().particles;
	for (auto i = particles.begin(); i != particles.end(); i++) {
		Vector2d fieldStrength = find_acceleration_at((*i)->position);
		Vector2d nextPos = find_next_position(*(*i), fieldStrength);
		nextPositions.push_back(nextPos);
	}

	for (int i = 0; i != particles.size(); i++) {
		particles.at(i)->position = nextPositions.at(i);
	}

	return particles;

}

const std::vector<Particle*>& Universe::get_particles()
{
	return rootCell->get_value().particles;
}

void Universe::update_cells() {
	rootCell->init_root_cell();
}

// parallelization: 
// 1. find com for all cells (parallel)
Vector2d Universe::find_acceleration_at(Vector2d & pos) {
	Cell *currentCell = rootCell;
	Cell *targetCell = nullptr;

	// Universe shouldn't know the number of children of a cell
	Vector2d acc(0, 0);
	while (currentCell->get_children()[0] != nullptr) {
		for (int i = 0; i < 4; i++) {
			Cell *childCell = (Cell *)currentCell->get_children()[i];
			if (!rectangle_contains(childCell->get_value().corners, pos)) {
				Vector2d com = childCell->find_com();
				Vector2d disp = com - pos;

				acc = acc + (G * childCell->get_value().totalMass / std::pow(disp.magnitude(), 2)) * disp.direction();
			}
			else {
				targetCell = childCell;
			}
		}

		currentCell = targetCell;
	}

	return acc;
}

Vector2d Universe::find_next_position(Particle & particle, Vector2d & fieldStrength) {

	Vector2d nextPosition = particle.position + timestep * particle.velocity + 0.5 * std::pow(timestep, 2) * particle.acc;
	particle.velocity = particle.velocity + (timestep*0.5) * (particle.acc + fieldStrength);
	particle.acc = fieldStrength;

	return nextPosition;
}

Universe::~Universe()
{
	for (auto i = rootCell->get_value().particles.begin(); i != rootCell->get_value().particles.end(); i++) {
		delete *i;
	}

	delete rootCell;
}