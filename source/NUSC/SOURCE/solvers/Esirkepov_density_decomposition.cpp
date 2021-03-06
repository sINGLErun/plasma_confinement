#include "../vectors/vector_classes.hpp"
#include "../vectors/vector3_field.hpp"
#include "../particles/species_description.hpp"
#include "../constants.h"

#include <functional>
#include <cmath>
#include <omp.h>
#include <iostream>

/*

	NOTE: try to add 1/2 to calculate fields at shifted points

*/

void Esirkepov_density_decomposition(const Species_description& sort,
									 const Particle& particle,
									 const vector2& r0,
									 vector3_field& j)
{
	double q  = sort.q();
	double n  = sort.n();
	double Np = sort.Np();
	int charge_cloud = sort.charge_cloud();
	function<double(double, double)> shape_at = sort.form_factor();


	double gamma = sqrt(1 + particle.p().dot(particle.p())/(sort.m()*sort.m()));
	double vz = particle.p().z/gamma;
	vector2 r = particle.r();

	px_py_vector3_field temp_j(2*charge_cloud+1, 2*charge_cloud+1); 

	size_t nearest_edge_to_rx = size_t(roundf(r.x/dx));
	size_t nearest_edge_to_ry = size_t(roundf(r.y/dy));

	{	// collecting jx(y,x) -------------------------------------------------------------------------
		long int node_x, node_y;
		node_x = nearest_edge_to_rx - charge_cloud;
		
		for (int x = -charge_cloud, y = -charge_cloud; y <= +charge_cloud; ++y) {
			node_y = nearest_edge_to_ry + y;

			temp_j.x(y,x) = - q*n/Np*0.5*dx/dt*(shape_at(r.x - node_x*dx, dx) - shape_at(r0.x - node_x*dx, dx))*
									 		   (shape_at(r.y - node_y*dy, dy) + shape_at(r0.y - node_y*dy, dy));
	
			#pragma omp atomic
			j.x(node_y,node_x) += temp_j.x(y,x);
		}
	
	
		for (int y = -charge_cloud; y <= +charge_cloud; ++y) {
			node_y = nearest_edge_to_ry + y;
			
			for (int x = -charge_cloud+1; x <= +charge_cloud; ++x) {
				node_x = nearest_edge_to_rx + x;
	
				temp_j.x(y,x) = temp_j.x(y,x-1) - q*n/Np*0.5*dx/dt*(shape_at(r.x - node_x*dx, dx) - shape_at(r0.x - node_x*dx, dx))*
																   (shape_at(r.y - node_y*dy, dy) + shape_at(r0.y - node_y*dy, dy));
	
				#pragma omp atomic
				j.x(node_y, node_x) += temp_j.x(y,x);
	
			}
		}
	}

	{	// collecting jy(y,x) -------------------------------------------------------------------------
		long int node_x, node_y;
		node_y = nearest_edge_to_ry - charge_cloud;
		
		for (int y = -charge_cloud, x = -charge_cloud; x <= +charge_cloud; ++x) {
			node_x = nearest_edge_to_rx + x;
	
			temp_j.y(y,x) = - q*n/Np*0.5*dy/dt*(shape_at(r.x - node_x*dx, dx) + shape_at(r0.x - node_x*dx, dx))*
											   (shape_at(r.y - node_y*dy, dy) - shape_at(r0.y - node_y*dy, dy));
		
			#pragma omp atomic
			j.y(node_y, node_x) += temp_j.y(y,x);
		}
	
		for (int y = -charge_cloud+1; y <= +charge_cloud; ++y) {
			node_y = nearest_edge_to_ry + y;
	
			for (int x = -charge_cloud; x <= +charge_cloud; ++x) {
				node_x = nearest_edge_to_rx + x;
	
				temp_j.y(y,x) += temp_j.y(y-1,x) - q*n/Np*0.5*dy/dt*(shape_at(r.x - node_x*dx, dx) + shape_at(r0.x - node_x*dx, dx))*
																    (shape_at(r.y - node_y*dy, dy) - shape_at(r0.y - node_y*dy, dy));
			
				#pragma omp atomic
				j.y(node_y, node_x) += temp_j.y(y,x);
			}
		}
	}

	{	// collecting jz(y,x) -------------------------------------------------------------------------
		long int node_x, node_y;
		
		for (long int y = -charge_cloud; y <= +charge_cloud; ++y) {
			node_y = nearest_edge_to_ry + y;
	
			for (long int x = -charge_cloud; x <= +charge_cloud; ++x) {
				node_x = nearest_edge_to_rx + x;
	
				#pragma omp atomic
				j.z(node_y, node_x) += + q*n/Np*vz/3.*((	 shape_at(r.x - node_x*dx, dx) + 0.5*shape_at(r0.x - node_x*dx, dx))*shape_at(r.y  -  node_y*dy, dy)+
									    				(0.5*shape_at(r.x - node_x*dx, dx) +	 shape_at(r0.x - node_x*dx, dx))*shape_at(r0.y -  node_y*dy, dy));
			}
		}
	}
}