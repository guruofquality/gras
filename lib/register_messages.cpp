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

#include <gnuradio/top_block.hpp>
#include <gras_impl/messages.hpp>
#include <gras_impl/interruptible_thread.hpp>

THERON_DEFINE_REGISTERED_MESSAGE(gnuradio::TopAllocMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gnuradio::TopActiveMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gnuradio::TopInertMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gnuradio::TopTokenMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gnuradio::GlobalBlockConfig);
THERON_DEFINE_REGISTERED_MESSAGE(gnuradio::SharedThreadGroup);

THERON_DEFINE_REGISTERED_MESSAGE(gnuradio::InputTagMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gnuradio::InputBufferMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gnuradio::InputTokenMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gnuradio::InputCheckMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gnuradio::InputAllocMessage);

THERON_DEFINE_REGISTERED_MESSAGE(gnuradio::OutputBufferMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gnuradio::OutputTokenMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gnuradio::OutputCheckMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gnuradio::OutputHintMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gnuradio::OutputAllocMessage);

THERON_DEFINE_REGISTERED_MESSAGE(gnuradio::SelfKickMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gnuradio::UpdateInputsMessage);
