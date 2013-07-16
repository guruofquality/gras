// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/top_block.hpp>
#include <gras_impl/messages.hpp>
#include <gras_impl/interruptible_thread.hpp>

THERON_DEFINE_REGISTERED_MESSAGE(gras::TopAllocMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::TopActiveMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::TopInertMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::TopTokenMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::TopConfigMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::TopThreadMessage);

THERON_DEFINE_REGISTERED_MESSAGE(gras::InputTagMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::InputMsgMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::InputBufferMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::InputTokenMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::InputCheckMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::InputAllocMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::InputUpdateMessage);

THERON_DEFINE_REGISTERED_MESSAGE(gras::OutputBufferMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::OutputTokenMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::OutputCheckMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::OutputHintMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::OutputAllocMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::OutputUpdateMessage);

THERON_DEFINE_REGISTERED_MESSAGE(gras::CallableMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::SelfKickMessage);
THERON_DEFINE_REGISTERED_MESSAGE(gras::GetStatsMessage);
