#include "./fields.hpp" 
#include <iostream>



enum CONF { BOUND, SX, SY };
enum FD_DESC { FIELD, AXIS, PX, PY };

void Fields::initialize(string solver, vector<string> configuration,
	string dir_name, multimap<string, vector<string>> diagnostics_description)
{
	// initalization of solver
	if ( solver == "FDTD_2D" ) {
		propogate_ = FDTD_2D;			
	}

	// initalization of configuration
	int size_x = stoi(configuration[SX]);
	int size_y = stoi(configuration[SY]);
	string boundaries = configuration[BOUND];
	
	if ( boundaries == "px_py" ) {
		E_ = make_unique<px_py_vector3_field>(size_x, size_y);
		B_ = make_unique<px_py_vector3_field>(size_x, size_y);
		j_ = make_unique<px_py_vector3_field>(size_x, size_y);
	}
	else if ( boundaries == "rx_ry" ) {
		E_ = make_unique<rx_ry_Electric_field>(size_x, size_y);
		B_ = make_unique<rx_ry_Magnetic_field>(size_x, size_y);
		j_ = make_unique<rx_ry_Electric_field>(size_x, size_y);
	}
	else if ( boundaries == "rx_py" ) {
		E_ = make_unique<rx_py_Electric_field>(size_x, size_y);
		B_ = make_unique<rx_py_Magnetic_field>(size_x, size_y);
		j_ = make_unique<rx_py_Electric_field>(size_x, size_y);
	}
	else if ( boundaries == "px_ry" ) {
		E_ = make_unique<px_ry_Electric_field>(size_x, size_y);
		B_ = make_unique<px_ry_Magnetic_field>(size_x, size_y);
		j_ = make_unique<px_ry_Electric_field>(size_x, size_y);
	}


	// initalization of diagnostics
	if (fields_are_diagnosed) {
		for (auto& diagnostic : diagnostics_description) {
			if ( diagnostic.first == "energy" ) {
				diagnostics_.push_back(make_unique<fields_energy>(dir_name + "/"
					+ diagnostic.first));
			}
			else if ( diagnostic.first == "whole_field" ) {
				diagnostics_.push_back(make_unique<whole_field>(dir_name + "/fields/"
					+ diagnostic.first,
					diagnostic.second[FIELD] + diagnostic.second[AXIS],
					diagnostic.second[FIELD], diagnostic.second[AXIS] ) );
			}
			else if ( diagnostic.first == "field_along_X" ) {
				diagnostics_.push_back(make_unique<field_along_X>(dir_name + "/fields/"
					+ diagnostic.first,
					diagnostic.second[FIELD] + diagnostic.second[AXIS],
					diagnostic.second[FIELD], diagnostic.second[AXIS],
					stoi(diagnostic.second[PX]) ) );
			}
			else if ( diagnostic.first == "field_along_Y" ) {
				diagnostics_.push_back(make_unique<field_along_Y>(dir_name + "/fields/"
					+ diagnostic.first,
					diagnostic.second[FIELD] + diagnostic.second[AXIS],
					diagnostic.second[FIELD], diagnostic.second[AXIS],
					stoi(diagnostic.second[PX]) ) );
			} 
			else if ( diagnostic.first == "field_at_point" ) {
				diagnostics_.push_back(make_unique<field_at_point>(dir_name + "/fields/"
					+ diagnostic.first,
					diagnostic.second[FIELD] + diagnostic.second[AXIS],
					diagnostic.second[FIELD], diagnostic.second[AXIS], 
					stoi(diagnostic.second[PX]), stoi(diagnostic.second[PY]) ) );
			}
		}
		diagnostics_.shrink_to_fit();
	}
	
}

void Fields::propogate()
{
	propogate_(*E_, *B_, *j_);
}

void Fields::diagnose()
{
	for (auto& diagnostic : diagnostics_) {
		(*diagnostic).diagnose(*E_, *B_, *j_);
	}
}