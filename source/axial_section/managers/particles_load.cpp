#include "./fields_additionals/magnetic_mirror.hpp"
#include "../particles/species_description.hpp"
#include <iostream>

double frand() { return ((double)rand()/RAND_MAX); }

void coordinate_loader(string& XY_distrib, int i,
	double cX, double cY, double Xm, double Ym, double& x, double& y)
{
	if (XY_distrib == "random") {
		x = cX + (2*Xm*frand() - Xm);  
		y = cY + (2*Ym*frand() - Ym);  
	}
	else if (XY_distrib == "periodic") {
		// i%Np -- номер от нуля до Np-1 частицы в одной ячейке
		
		// сколько по длинне в ячейку влазит? Это надо искать делители Np
		// при том лучше, конечно, от половины Np начинать в обе стороны искать

		// NOTE: пока эта проблема решена так: divider -- число, показывающее 
		//		 сколько частиц будет уложено в одну ячейку вдоль оси Ox; 
		
		// ((i%int(Np)) % divider) -- индекс по ширине
		// ((i%int(Np)) / divider) -- индекс по высоте

		// делим на divider, чтобы влазить в одну ячейку (по ширине)

		int divider = 2;
		x = (cX - Xm) + ((i%int(Np)) % divider)*dx/divider;
		y = (cY - Ym) + ((i%int(Np)) / divider)*dy/(int(Np)/divider); 
	}
}


void load_p02d_particles(Species_description& sort, int Np,
		string XY_distrib, double cX, double cY, double Xm, double Ym, const vector2& p0)
{
	sort.Np_ = Np;

	srand(time(NULL));
	int err = 0;
	
	for (int i = 0; i < int(Np*(2*Xm/dx)*(2*Ym/dy)) + err; ++i) {

		double x, y;
		coordinate_loader(XY_distrib, i, cX, cY, Xm, Ym, x, y);
		
		double px = p0.x + sin(2.*M_PI*frand())*sqrt(-2.*(Tx*sort.m_/mec2)*log(frand())); 
		double py = p0.y + sin(2.*M_PI*frand())*sqrt(-2.*(Ty*sort.m_/mec2)*log(frand()));

		if (isinf(px) | isinf(py)) { 
			++err;
			continue;
		}
		
		vector2 r(x, y);
		vector3 p(px, py, 0);
		
		Particle temp_(r, p);
		sort.particles_.push_back(temp_);
	}
}

void load_p03d_particles(Species_description& sort, int Np,
		string XY_distrib, double cX, double cY, double Xm, double Ym, const vector3& p0)
{	
	sort.Np_ = Np;

	srand(time(NULL));

	// ####### constant flux initialization ###############################################################
	int index = string("constant_flux").size() + 1;
	int end   = XY_distrib.size();
	string distrib = XY_distrib.substr(index, end);
	
	double mirror_flux = 0;	// (поток магнитого поля в пробке ч/з поперечное сечение)/2
	vector2 R(x_m1, 0); 
	
	for (int ny = SIZE_Y/2; ny <= int(roundf(0.5*(SIZE_Y + plasma_width/dy))); ++ny) {
		R.y = ny*dy;
		mirror_flux += dy*( mirror1.return_field(R) + mirror2.return_field(R) ).x;
	}
	
	double flux = 0;
	int ny_flux[(nx_m2-nx_m1)];
	R.x = x_m1;
	
	// вычисление формы магнитной поверхности (ny_flux) между пробками
	for (int nx = nx_m1; nx < nx_m2; ++nx) {
		R.x = nx*dx;
		for (int ny = SIZE_Y/2; flux < mirror_flux; ++ny){
			R.y = ny*dy;
			flux += dy*( mirror1.return_field(R) + mirror2.return_field(R) ).x;
			ny_flux[(nx - nx_m1)] = ny;
		}
		flux = 0;
	}
	

	int err = 0;
	for (int nx = nx_m1; nx < nx_m2; ++nx) {
		
		for (int ny = (SIZE_Y - ny_flux[nx]); ny < ny_flux[nx]; ++ny) {
			
			err = 0;
			for (int i = 0; i < (int(Np) + err); ++i) {

				double x, y;
				coordinate_loader(distrib, i, (nx+0.5)*dx, (ny+0.5)*dy, Xm, Ym, x, y);
				
				double px = p0.x + sin(2.*M_PI*frand())*sqrt(-2.*(Tx*sort.m_/mec2)*log(frand())); 
				double py = p0.y + sin(2.*M_PI*frand())*sqrt(-2.*(Ty*sort.m_/mec2)*log(frand()));
				double pz = p0.z + sin(2.*M_PI*frand())*sqrt(-2.*(Tz*sort.m_/mec2)*log(frand()));
		
				if (isinf(px) || isinf(py) || isinf(pz)) { 
					++err;
					continue;
				}
				vector2 r(x, y);
				vector3 p(px, py, pz);
					
				Particle temp_(r, p);
				sort.particles_.push_back(temp_);
			}
		}
	}		
	// ####################################################################################################
}

void load_chosen_distribution(Species_description& sort, int Np,
		string XY_distrib, double cX, double cY, double Xm, double Ym, string P_distrib)
{
	sort.Np_ = Np;
	
	srand(time(NULL));
	int err = 0;
	for (int i = 0; i < int(Np*(2*Xm/dx)*(2*Ym/dy)) + err; ++i) {

		double x, y;
		coordinate_loader(XY_distrib, i, cX, cY, Xm, Ym, x, y);
		
		double px, py;

		if (P_distrib == string("X_Oscillations")) {
			px = 0.01*cos(2*M_PI*x/(SIZE_X*dx)) + sin(2.*M_PI*frand())*sqrt(-2.*(Tx*sort.m_/mec2)*log(frand())); 
			py = 								 sin(2.*M_PI*frand())*sqrt(-2.*(Ty*sort.m_/mec2)*log(frand()));
			
			if (isinf(px) | isinf(py)) { 
				++err;
				continue;
			}
		}
		else if (P_distrib == string("XY_Oscillations")) {
			px = 0.01*cos(2*M_PI*x/(SIZE_X*dx)) + sin(2.*M_PI*frand())*sqrt(-2.*(Tx*sort.m_/mec2)*log(frand())); 
			py = 0.01*cos(2*M_PI*y/(SIZE_Y*dy)) + sin(2.*M_PI*frand())*sqrt(-2.*(Ty*sort.m_/mec2)*log(frand()));
			
			if (isinf(px) | isinf(py)) { 
				++err;
				continue;
			}		
		}
		
		vector2 r(x, y);
		vector3 p(px, py, 0);		
		
		Particle temp_(r, p);
		sort.particles_.push_back(temp_);		
	}
}