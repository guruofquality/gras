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

#include <Theron/Register.h>
#include <gras_impl/messages.hpp>
#include <gras_impl/interruptible_thread.hpp>
#include <Apology/Worker.hpp>

THERON_REGISTER_MESSAGE(Apology::WorkerTopologyMessage);
THERON_REGISTER_MESSAGE(gnuradio::TopAllocMessage);
THERON_REGISTER_MESSAGE(gnuradio::TopActiveMessage);
THERON_REGISTER_MESSAGE(gnuradio::TopInertMessage);
THERON_REGISTER_MESSAGE(gnuradio::TopTokenMessage);
THERON_REGISTER_MESSAGE(gnuradio::TopHintMessage);
THERON_REGISTER_MESSAGE(gnuradio::SharedThreadGroup);

THERON_REGISTER_MESSAGE(gnuradio::InputTagMessage);
THERON_REGISTER_MESSAGE(gnuradio::InputBufferMessage);
THERON_REGISTER_MESSAGE(gnuradio::InputTokenMessage);
THERON_REGISTER_MESSAGE(gnuradio::InputCheckMessage);
THERON_REGISTER_MESSAGE(gnuradio::InputAllocMessage);

THERON_REGISTER_MESSAGE(gnuradio::OutputBufferMessage);
THERON_REGISTER_MESSAGE(gnuradio::OutputTokenMessage);
THERON_REGISTER_MESSAGE(gnuradio::OutputCheckMessage);
THERON_REGISTER_MESSAGE(gnuradio::OutputHintMessage);
THERON_REGISTER_MESSAGE(gnuradio::OutputAllocMessage);

THERON_REGISTER_MESSAGE(gnuradio::SelfKickMessage);
THERON_REGISTER_MESSAGE(gnuradio::UpdateInputsMessage);
