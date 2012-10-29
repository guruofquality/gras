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

#include <gras/top_block.hpp>
#include <gras_impl/messages.hpp>
#include <gras_impl/interruptible_thread.hpp>

THERON_DEFINE_REGISTERED_MESSAGE(gras::TopAllocMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::TopActiveMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::TopInertMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::TopTokenMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::GlobalBlockConfig);
THERON_DEFINE_REGISTERED_MESSAGE(gras::SharedThreadGroup);

THERON_DEFINE_REGISTERED_MESSAGE(gras::InputTagMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::InputBufferMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::InputTokenMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::InputCheckMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::InputAllocMessage);

THERON_DEFINE_REGISTERED_MESSAGE(gras::OutputBufferMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::OutputTokenMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::OutputCheckMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::OutputHintMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::OutputAllocMessage);

THERON_DEFINE_REGISTERED_MESSAGE(gras::SelfKickMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::UpdateInputsMessage);
