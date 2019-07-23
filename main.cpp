
#include "Universe.h"
#include "GeometryTypes.h"

#include <iostream>
#include <fstream>
#include <streambuf>
#include <vector>
#include <memory>
#include <string>
#include <chrono>
#include <Windows.h>
#include <thread>

#define USE_TIME std::chrono::high_resolution_clock::time_point iMpOssiBlEt1, iMpOssiBlEt2
#define TIME_START iMpOssiBlEt1 = std::chrono::high_resolution_clock::now()
#define TIME_END iMpOssiBlEt2 = std::chrono::high_resolution_clock::now()
#define TIME_DIFF std::chrono::duration_cast<std::chrono::microseconds>(iMpOssiBlEt2 - iMpOssiBlEt1).count() / 1000000.0

bool _RUNNING = true;

BOOL WINAPI consoleHandler(DWORD signal);
void log_particles(const std::vector<Particle *> & particles);

int main(int argc, char *argv[]) {
	USE_TIME;

	std::ios::sync_with_stdio(false);

	if (!SetConsoleCtrlHandler(consoleHandler, TRUE)) {
		fprintf(stderr, "ERROR: Could not set control handler\n");
		return -1;
	}

	static size_t NPARTICLES = 10000;
	static double G = 1.559 * std::pow(10, -13);
	static double TIMESTEP = 100000;
	static geometry::Rectangle CORNERS = geometry::Rectangle(Vector2d(0, 0), Vector2d(89000, 89000));

	std::unique_ptr<Universe::Builder> builder = Universe::get_builder();
	builder->set_particle_count(NPARTICLES)
		.set_G(G)
		.set_timestep(TIMESTEP)
		.set_corners(CORNERS);

	if (argc == 2) {
		std::ifstream builderFile(argv[1]);
		std::string json((std::istreambuf_iterator<char>(builderFile)),
			std::istreambuf_iterator<char>());

		fprintf(stderr, "Building universe with parameters:\n%s\n", json.c_str());
		builder->fromJson(json);
	} else {
		fprintf(stderr, "Building universe with parameters {particles=%zu, G=%e, timestep=%f} ...\n", NPARTICLES, G, TIMESTEP);
	}

	std::unique_ptr<Universe> universe;
	TIME_START;
	universe = builder->build();
	TIME_END;
	double elapsedSeconds = TIME_DIFF;

	fprintf(stderr, "Universe constructed in %.3f seconds\n", elapsedSeconds);

	std::thread outputThread = std::thread(log_particles, universe->get_particles());

	int nIterations = 0;
	while (_RUNNING) {

		universe->update_particles();
		universe->update_cells();

		nIterations++;
	}

	fprintf(stderr, "Completed %d iterations\n", nIterations);
	std::cout.flush();
	return 0;
	
}

BOOL WINAPI consoleHandler(DWORD signal) {

	if (signal == CTRL_C_EVENT) {
		_RUNNING = false;
	}

	return TRUE;
}

void log_particles(const std::vector<Particle*>& particles)
{
	while (_RUNNING) {
		for (auto i = particles.begin(); i != particles.end(); i++)
			printf("%f, %f, %f\n", (*i)->position.x, (*i)->position.y, (*i)->mass);
		printf("ENDITER\n");
		fflush(stdout);
	}
}


