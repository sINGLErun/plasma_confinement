#include "../solvers/solvers.hpp"
#include "../vectors/vector3_field.hpp"
#include "../particles/class_particles.hpp"
#include "../constants.h"

#include <memory>
#include <functional>


using namespace std;
using up_v3f = unique_ptr<vector3_field>;

#ifndef PARTICLES_MANAGER_H
#define PARTICLES_MANAGER_H

//#################################################################################################

class Particles_manager {
private:
	vector<class_particles> particles;

	function<void(const class_particles&, particle&, const vector3_field& E, const vector3_field& B)> Particle_push_;
	function<void(const class_particles&, const particle&, const vector2& r0, vector3_field& j)> Density_decomposition_;
	function<void(particle&, double size_x, double size_y)> Boundaries_processing_;

	function<void(particle&, double)> x_boundary;
	function<void(particle&, double)> y_boundary;

public:
	Particles_manager() = default;
	void initialisation(vector<string> solvers, vector<string> configuration);

	void Particle_push(const class_particles&, particle&, const up_v3f& E, const up_v3f& B); 
	void Density_decomposition(const class_particles&, const particle&, const vector2& r0, up_v3f& j);
	void Boundaries_processing(particle&, double size_x, double size_y);
};

//#################################################################################################

#endif