// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras/block.hpp>

using namespace gras;

PropertyRegistry::PropertyRegistry(void){}
PropertyRegistry::~PropertyRegistry(void){}

void BlockActor::handle_prop_access(
    const PropAccessMessage &message,
    const Theron::Address from
)
{
    ASSERT(this->prio_count.Load() != 0);
    this->prio_count.Decrement();

    PropAccessMessage reply;
    reply.set = not message.set;
    reply.key = message.key;

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

    this->Send(reply, from); //ACK
}

void Block::_register_property(const std::string &key, PropertyRegistrySptr pr)
{
    (*this)->block->prop_registry[key] = pr;
}

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

PMCC BlockActor::prop_access_dispatcher(const std::string &key, const PMCC &value, const bool set)
{
    PropAccessReceiver receiver;
    PropAccessMessage message;
    message.set = set;
    message.key = key;
    message.value = value;
    this->Push(message, receiver.GetAddress());
    this->prio_count.Increment();
    receiver.Wait();
    if (not receiver.message.error.empty())
    {
        throw std::runtime_error(receiver.message.error);
    }
    return receiver.message.value;
}

void Block::_set_property(const std::string &key, const PMCC &value)
{
    (*this)->block->prop_access_dispatcher(key, value, true);
}

PMCC Block::_get_property(const std::string &key)
{
    return (*this)->block->prop_access_dispatcher(key, PMCC(), false);
}
