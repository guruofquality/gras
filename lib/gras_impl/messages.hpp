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

#ifndef INCLUDED_LIBGRAS_IMPL_MESSAGES_HPP
#define INCLUDED_LIBGRAS_IMPL_MESSAGES_HPP

#include <gnuradio/sbuffer.hpp>

namespace gnuradio
{

struct TopBlockMessage
{
    enum
    {
        ALLOCATE,
        ACTIVE,
        INERT,
        HINT,
        TOKEN_TIME,
    } what;
    size_t hint;
    Token token;
};

struct CheckTokensMessage
{
    //empty
};

struct SelfKickMessage
{
    //empty
};

struct BufferReturnMessage
{
    size_t index;
    SBuffer buffer;
};

struct BufferHintMessage
{
    size_t history_bytes;
    size_t reserve_bytes;
    WeakToken token;
};

} //namespace gnuradio

#endif /*INCLUDED_LIBGRAS_IMPL_MESSAGES_HPP*/
