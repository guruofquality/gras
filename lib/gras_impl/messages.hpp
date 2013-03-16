// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_LIBGRAS_IMPL_MESSAGES_HPP
#define INCLUDED_LIBGRAS_IMPL_MESSAGES_HPP

#include <gras/buffer_queue.hpp>
#include <gras/sbuffer.hpp>
#include <gras/tags.hpp>
#include <gras/sbuffer.hpp>
#include <gras_impl/token.hpp>
#include <gras_impl/stats.hpp>

namespace gras
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

//----------------------------------------------------------------------
//-- message to an input port
//-- do not ack
//----------------------------------------------------------------------

struct InputTagMessage
{
    InputTagMessage(const Tag &tag):tag(tag){}
    size_t index;
    Tag tag;
};

struct InputMsgMessage
{
    InputMsgMessage(const PMCC &msg):msg(msg){}
    size_t index;
    PMCC msg;
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

struct InputAllocMessage
{
    size_t index;
    SBufferConfig config;
    SBufferToken token; //holds reference
};

struct InputCheckMessage
{
    size_t index;
};

struct InputUpdateMessage
{
    size_t index;
};

//----------------------------------------------------------------------
//-- message to an output port
//-- do not ack
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
    size_t reserve_bytes;
    WeakToken token;
};

struct OutputAllocMessage
{
    size_t index;
    BufferQueueSptr queue;
};

struct OutputUpdateMessage
{
    size_t index;
};

//----------------------------------------------------------------------
//-- message to just the block
//-- do not ack
//----------------------------------------------------------------------

struct SelfKickMessage
{
    //empty
};

struct GetStatsMessage
{
    std::string block_id;
    BlockStats stats;
    time_ticks_t stats_time;
};

} //namespace gras

#include <Theron/Register.h>
#include <gras/top_block.hpp>
#include <gras_impl/messages.hpp>
#include <gras_impl/interruptible_thread.hpp>

THERON_DECLARE_REGISTERED_MESSAGE(gras::TopAllocMessage);
THERON_DECLARE_REGISTERED_MESSAGE(gras::TopActiveMessage);
THERON_DECLARE_REGISTERED_MESSAGE(gras::TopInertMessage);
THERON_DECLARE_REGISTERED_MESSAGE(gras::TopTokenMessage);
THERON_DECLARE_REGISTERED_MESSAGE(gras::GlobalBlockConfig);
THERON_DECLARE_REGISTERED_MESSAGE(gras::SharedThreadGroup);

THERON_DECLARE_REGISTERED_MESSAGE(gras::InputTagMessage);
THERON_DECLARE_REGISTERED_MESSAGE(gras::InputMsgMessage);
THERON_DECLARE_REGISTERED_MESSAGE(gras::InputBufferMessage);
THERON_DECLARE_REGISTERED_MESSAGE(gras::InputTokenMessage);
THERON_DECLARE_REGISTERED_MESSAGE(gras::InputCheckMessage);
THERON_DECLARE_REGISTERED_MESSAGE(gras::InputAllocMessage);
THERON_DECLARE_REGISTERED_MESSAGE(gras::InputUpdateMessage);

THERON_DECLARE_REGISTERED_MESSAGE(gras::OutputBufferMessage);
THERON_DECLARE_REGISTERED_MESSAGE(gras::OutputTokenMessage);
THERON_DECLARE_REGISTERED_MESSAGE(gras::OutputCheckMessage);
THERON_DECLARE_REGISTERED_MESSAGE(gras::OutputHintMessage);
THERON_DECLARE_REGISTERED_MESSAGE(gras::OutputAllocMessage);
THERON_DECLARE_REGISTERED_MESSAGE(gras::OutputUpdateMessage);

THERON_DECLARE_REGISTERED_MESSAGE(gras::SelfKickMessage);
THERON_DECLARE_REGISTERED_MESSAGE(gras::GetStatsMessage);

#endif /*INCLUDED_LIBGRAS_IMPL_MESSAGES_HPP*/
