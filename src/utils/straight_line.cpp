/*************************************************************************
 * Copyright (C) 2011 by Saleh Dindar and the Swarm-NG Development Team  *
 *                                                                       *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 3 of the License.        *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program; if not, write to the                         *
 * Free Software Foundation, Inc.,                                       *
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ************************************************************************/

/*! \file straight_line.cpp
 *   \brief Implements a utility to create a default ensemble. 
 *
 */


#include "swarm/swarm.h"
#include <iostream>
using namespace swarm;
using namespace std;

const int nbod = 3;
const int nsys = 64;

/// main program
int main(int argc, char* argv[]){
	if(argc <= 1){
		cout << "Usage: straight_line <outputfilename>" << endl;
	}
	const string outputfn = argv[1];
	init(config());
	defaultEnsemble ens = defaultEnsemble::create(nbod,nsys);
	
	for(int s = 0; s < nsys ; s++){
		ens[s].id() = 0;
		ens[s].time() = 0;
		
		// Set all the bodies to zero
		for(int b = 0; b < nbod; b++){
			ens[s][b].mass() = 0;
			for(int c = 0; c < 3; c++)
				ens[s][b][c].pos() = 0, ens[s][b][c].vel() = 0;
		}
		
		// the central one has to pull everything together
		ens[s][0].mass() = 1;
		ens[s][1][0].pos() = -10;
		ens[s][2][0].pos() = +10;

	}
	snapshot::save_text(ens,outputfn);
}



