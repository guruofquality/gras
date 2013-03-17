// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras/block.hpp>

using namespace gras;

PropertyRegistry::PropertyRegistry(void){}
PropertyRegistry::~PropertyRegistry(void){}

/***********************************************************************
 * The actual thread-safe implementation of property handling
 **********************************************************************/
void BlockActor::handle_prop_access(
    const PropAccessMessage &message,
    const Theron::Address from
)
{
    //setup reply
    PropAccessMessage reply;
    reply.set = not message.set;
    reply.key = message.key;

    //try to call the property bound method
    PropertyRegistrySptr pr = prop_registry[message.key];
    if (not pr) reply.error = "no property registered for key: " + message.key;
    else try
    {
        if (message.set) pr->set(message.value);
        else reply.value = pr->get();
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
    this->highPrioAck();
}

/***********************************************************************
 * A special receiver to handle the property access result
 **********************************************************************/
struct PropAccessReceiver : Theron::Receiver
{
    PropAccessReceiver(void)
    {
        this->RegisterHandler(this, &PropAccessReceiver::handle_prop_access);
    }

    void handle_prop_access(const PropAccessMessage &msg, const Theron::Address)
    {
        this->message = msg;
    }

    PropAccessMessage message;
};

/***********************************************************************
 * Handle the get and set calls from the user's call-stack
 **********************************************************************/
template <typename ActorType>
static PMCC prop_access_dispatcher(ActorType &actor, const std::string &key, const PMCC &value, const bool set)
{
    PropAccessReceiver receiver;
    PropAccessMessage message;
    message.set = set;
    message.key = key;
    message.value = value;
    actor->Push(message, receiver.GetAddress());
    actor->highPrioPreNotify();
    receiver.Wait();
    if (not receiver.message.error.empty())
    {
        throw std::runtime_error(receiver.message.error);
    }
    return receiver.message.value;
}

void Block::_register_property(const std::string &key, PropertyRegistrySptr pr)
{
    (*this)->block->prop_registry[key] = pr;
}

void Block::_set_property(const std::string &key, const PMCC &value)
{
    prop_access_dispatcher((*this)->block, key, value, true);
}

PMCC Block::_get_property(const std::string &key)
{
    return prop_access_dispatcher((*this)->block, key, PMCC(), false);
}
