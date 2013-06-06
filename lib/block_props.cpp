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

    //call into the handler overload to do the property access
    try
    {
        reply.value = block_ptr->_handle_prop_access(message.key, message.value, message.set);
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

PMCC Block::_handle_prop_access(const std::string &key, const PMCC &value, const bool set)
{
    const PropertyRegistryPair &pair = (*this)->block_data->property_registry[key];
    PropertyRegistrySptr pr = (set)? pair.setter : pair.getter;
    if (not pr) throw std::invalid_argument("no property registered for key: " + key);
    if (set)
    {
        pr->set(value);
        return PMCC();
    }
    return pr->get();
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
PMCC BlockActor::prop_access_dispatcher(const std::string &key, const PMCC &value, const bool set)
{
    PropAccessReceiver receiver;
    PropAccessMessage message;
    message.prio_token = this->prio_token;
    message.set = set;
    message.key = key;
    message.value = value;
    this->GetFramework().Send(message, receiver.GetAddress(), this->GetAddress());
    receiver.Wait();
    if (not receiver.message.error.empty())
    {
        throw std::runtime_error(receiver.message.error);
    }
    return receiver.message.value;
}

void Block::_register_getter(const std::string &key, void *pr)
{
    (*this)->block_data->property_registry[key].getter.reset(reinterpret_cast<PropertyRegistry *>(pr));
}

void Block::_register_setter(const std::string &key, void *pr)
{
    (*this)->block_data->property_registry[key].setter.reset(reinterpret_cast<PropertyRegistry *>(pr));
}

void Block::_set_property(const std::string &key, const PMCC &value)
{
    (*this)->block_actor->prop_access_dispatcher(key, value, true);
}

PMCC Block::_get_property(const std::string &key)
{
    return (*this)->block_actor->prop_access_dispatcher(key, PMCC(), false);
}
