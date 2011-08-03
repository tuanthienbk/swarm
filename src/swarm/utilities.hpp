/*************************************************************************
 * Copyright (C) 2008-2011 by Saleh Dindar & Swarm-NG Development Team   *
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

/*! \file utilities.cpp
 *  \brief provides several utility  functions for public interface for swarm libaray
 *
*/
#pragma once


#include <cuda_runtime_api.h>
#include "config.hpp"
#include "common.hpp"


namespace swarm {

	bool configure_grid(dim3 &gridDim, int threadsPerBlock, int nthreads, int dynShmemPerThread, int staticShmemPerBlock);


	void find_best_factorization(unsigned int &bx, unsigned int &by, int nblocks);

	void load_config(config &cfg, const std::string &fn);
}