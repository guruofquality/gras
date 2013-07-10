// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras/block.hpp>

using namespace gras;

/***********************************************************************
 * The actual thread-safe implementation of property handling
 **********************************************************************/
void BlockActor::handle_callable(
    const CallableMessage &message,
    const Theron::Address from
)
{
    //setup reply
    CallableMessage reply;
    reply.key = message.key;

    //call into the handler overload to do the property access
    try
    {
        reply.ret = data->block->_handle_call_ts(message.key, message.args);
    }
    catch (const std::exception &e)
    {
        reply.error = e.what();
    }
    catch (...)
    {
        reply.error = "unknown error";
    }

    //send the reply
    this->Send(reply, from); //ACK

    //work could have been skipped by a high prio msg
    //forcefully kick the task to recheck in a new call
    this->Send(SelfKickMessage(), this->GetAddress());
}

PMCC Block::_handle_call_ts(const std::string &key, const PMCC &args)
{
    //got here, so we call the base class unless this got overloaded
    return Callable::_handle_call(key, args);
}

/***********************************************************************
 * A special receiver to handle the property access result
 **********************************************************************/
struct CallableReceiver : Theron::Receiver
{
    CallableReceiver(void)
    {
        this->RegisterHandler(this, &CallableReceiver::handle_callable);
    }

    void handle_callable(const CallableMessage &msg, const Theron::Address)
    {
        this->message = msg;
    }

    CallableMessage message;
};

/***********************************************************************
 * Handle the get and set calls from the user's call-stack
 **********************************************************************/
PMCC Block::_handle_call(const std::string &key, const PMCC &args)
{
    CallableReceiver receiver;
    CallableMessage message;
    boost::shared_ptr<BlockActor> actor = (*this)->block_actor;
    message.prio_token = actor->prio_token;
    message.key = key;
    message.args = args;
    actor->GetFramework().Send(message, receiver.GetAddress(), actor->GetAddress());
    receiver.Wait();
    if (not receiver.message.error.empty())
    {
        throw std::runtime_error(receiver.message.error);
    }
    return receiver.message.ret;
}
