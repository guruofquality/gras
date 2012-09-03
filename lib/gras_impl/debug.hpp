//
// Copyright 2012 Josh Blum
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with io_sig program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef INCLUDED_LIBGRAS_IMPL_DEBUG_HPP
#define INCLUDED_LIBGRAS_IMPL_DEBUG_HPP

#include <iostream>

#define HERE() std::cerr << __FILE__ << ":" << __LINE__ << std::endl << std::flush;
#define VAR(x) std::cerr << #x << " = " << (x) << std::endl << std::flush;
#define ASSERT(x) if(not (x)){std::cerr << "ASSERT FAIL " << __FILE__ << ":" << __LINE__ << "\n\t" << #x << std::endl << std::flush;}

#endif /*INCLUDED_LIBGRAS_IMPL_DEBUG_HPP*/
