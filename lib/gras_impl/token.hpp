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

#ifndef INCLUDED_LIBGRAS_IMPL_TOKEN_HPP
#define INCLUDED_LIBGRAS_IMPL_TOKEN_HPP

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace gnuradio
{

typedef boost::weak_ptr<int> WeakToken;

struct Token : boost::shared_ptr<int>
{
    static Token make(void)
    {
        Token tok;
        tok.reset(new int(0));
        return tok;
    }
};

} //namespace gnuradio

#endif /*INCLUDED_LIBGRAS_IMPL_TOKEN_HPP*/
