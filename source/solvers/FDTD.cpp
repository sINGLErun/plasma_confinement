#ifndef VECTOR3_FIELD_H
#define VECTOR3_FIELD_H
	#include "../vectors/vector3_field.hpp"
#endif

#include <omp.h>

void FDTD_2D(vector3_field& E, vector3_field& B, vector3_field& J,
		  	 double dt, double dx, double dy) 
{	// in 2D-FDTD fields are independent of z, so every %/dz fractions will be zero

	#pragma omp parallel for num_threads(8)
	// Bx(y, x+1/2) at t+1/2 ----------------------------------------------------
	for (int y = 0; y < B.size_y(); ++y) {
	for (int x = 0; x < B.size_x(); ++x) {
			B.x(y,x) -= 0.5*(E.z(y,x) - E.z(y-1,x))*dt/dy;			
	}
	}	

	#pragma omp parallel for num_threads(8)
	// By(y+1/2, x) at t+1/2 ----------------------------------------------------
	for (int y = 0; y < B.size_y(); ++y) {
	for (int x = 0; x < B.size_x(); ++x) {
			B.y(y,x) += 0.5*(E.z(y,x) - E.z(y,x-1))*dt/dx;
	}
	}

	#pragma omp parallel for num_threads(8)
	// Bz(y, x) at t+1/2 --------------------------------------------------------
	for (int y = 0; y < B.size_y(); ++y) {
	for (int x = 0; x < B.size_x(); ++x) {
			B.z(y,x) -= 0.5*((E.y(y,x+1) - E.y(y,x))/dx - (E.x(y+1,x) - E.x(y,x))/dy)*dt;
	}
	}

	#pragma omp parallel for num_threads(8)
	// Ex(y+1/2, x) at t+1 ------------------------------------------------------
	for (int y = 0; y < E.size_y(); ++y) {
	for (int x = 0; x < E.size_x(); ++x) {
		E.x(y,x) += -J.x(y,x)*dt + (B.z(y,x) - B.z(y-1,x))*dt/dy;
		J.x(y,x) = 0;
	}	
	}

	#pragma omp parallel for num_threads(8)
	// Ey(y, x+1/2) at t+1 ------------------------------------------------------
	for (int y = 0; y < E.size_y(); ++y) { 
	for (int x = 0; x < E.size_x(); ++x) {
		E.y(y,x) += -J.y(y,x)*dt - (B.z(y,x) - B.z(y,x-1))*dt/dx;
		J.y(y,x) = 0;
	}		
	}

	#pragma omp parallel for num_threads(8)
	// Ez(y+1/2,x+1/2) at t+1 ---------------------------------------------------
	for (int y = 0; y < E.size_y(); ++y) {
	for (int x = 0; x < E.size_x(); ++x) {
		E.z(y,x) += -J.z(y,x)*dt + ((B.y(y,x+1) - B.y(y,x))/dx - (B.x(y+1,x) - B.x(y,x))/dy)*dt;
		J.z(y,x) = 0;
	}	
	}

	#pragma omp parallel for num_threads(8)
	// Bx(y, x+1/2) at t+1/2 ----------------------------------------------------
	for (int y = 0; y < B.size_y(); ++y) {
	for (int x = 0; x < B.size_x(); ++x) {
			B.x(y,x) -= 0.5*(E.z(y,x) - E.z(y-1,x))*dt/dy;			
	}
	}	

	#pragma omp parallel for num_threads(8)
	// By(y+1/2, x) at t+1/2 ----------------------------------------------------
	for (int y = 0; y < B.size_y(); ++y) {
	for (int x = 0; x < B.size_x(); ++x) {
			B.y(y,x) += 0.5*(E.z(y,x) - E.z(y,x-1))*dt/dx;
	}
	}

	#pragma omp parallel for num_threads(8)
	// Bz(y, x) at t+1/2 --------------------------------------------------------
	for (int y = 0; y < B.size_y(); ++y) {
	for (int x = 0; x < B.size_x(); ++x) {
			B.z(y,x) -= 0.5*((E.y(y,x+1) - E.y(y,x))/dx - (E.x(y+1,x) - E.x(y,x))/dy)*dt;
	}
	}
}