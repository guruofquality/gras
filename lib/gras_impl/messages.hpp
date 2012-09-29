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
#include <gnuradio/tags.hpp>
#include <gnuradio/sbuffer.hpp>
#include <gras_impl/token.hpp>

namespace gnuradio
{

//----------------------------------------------------------------------
//-- message from the top block/executor
//-- these messages must be ack'd
//----------------------------------------------------------------------

struct TopAllocMessage
{
    //empty
};

struct TopActiveMessage
{
    //empty
};

struct TopInertMessage
{
    //empty
};

struct TopTokenMessage
{
    Token token;
};

struct TopHintMessage
{
    size_t hint;
};

//----------------------------------------------------------------------
//-- message to an input port
//----------------------------------------------------------------------

struct InputTagMessage
{
    size_t index;
    Tag tag;
};

struct InputBufferMessage
{
    size_t index;
    SBuffer buffer;
};

struct InputTokenMessage
{
    size_t index;
    Token token;
};

struct InputAllocatorMessage
{
    size_t index;
    SBufferToken token;
    size_t recommend_length;
};

struct InputCheckMessage
{
    size_t index;
};

//----------------------------------------------------------------------
//-- message to an output port
//----------------------------------------------------------------------

struct OutputBufferMessage
{
    size_t index;
    SBuffer buffer;
};

struct OutputTokenMessage
{
    size_t index;
    Token token;
};

struct OutputCheckMessage
{
    size_t index;
};

struct OutputHintMessage
{
    size_t index;
    size_t history_bytes;
    size_t reserve_bytes;
    WeakToken token;
};

//----------------------------------------------------------------------
//-- message to just the block
//----------------------------------------------------------------------

struct SelfKickMessage
{
    //empty
};

struct CheckTokensMessage
{
    //empty
};

struct UpdateInputsMessage
{
    //empty
};

} //namespace gnuradio

#endif /*INCLUDED_LIBGRAS_IMPL_MESSAGES_HPP*/
