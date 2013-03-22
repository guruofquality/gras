// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras/block.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

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
    this->highPrioAck();
}

PMCC Block::_handle_prop_access(const std::string &key, const PMCC &value, const bool set)
{
    PropertyRegistrySptr pr = (*this)->block->prop_registry[key];
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
template <typename ActorType>
static PMCC prop_access_dispatcher(ActorType &actor, const std::string &path, const PMCC &value, const bool set)
{
    //split the paths into nodes
    std::vector<std::string> nodes;
    boost::split(nodes, path, boost::is_any_of("/"));

    //iterate through the path to find the element
    std::string key;
    Element elem = *(actor->block_ptr);
    size_t i = 0;
    BOOST_FOREACH(const std::string &node, nodes)
    {
        i++;
        if (node == "" and i == 1) //find root
        {
            while (elem->_parent) elem = elem->_parent;
            continue;
        }
        if (node == ".") //this dir
        {
            continue;
        }
        if (node == "..") //up a dir
        {
            if (not elem->_parent) throw std::runtime_error(
                "Property tree lookup fail - null parent: " + path
            );
            elem = elem->_parent;
            continue;
        }
        if (i == nodes.size() or elem->block) //leaf
        {
            key = node;
            if (i != nodes.size() or not elem->block) throw std::runtime_error(
                "Property tree lookup fail - beyond leaf: " + path
            );
            continue;
        }
        if (elem->_subelems.count(node) == 0) throw std::runtime_error(
            "Property tree lookup fail - no such path: " + path
        );
        elem = elem->_subelems[node];
    }

    //now send a message to the actor to perform the action
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

void Block::_register_property(const std::string &key, PMCC pr)
{
    (*this)->block->prop_registry[key] = pr.as<PropertyRegistrySptr>();
}

void Block::_set_property(const std::string &key, const PMCC &value)
{
    prop_access_dispatcher((*this)->block, key, value, true);
}

PMCC Block::_get_property(const std::string &key)
{
    return prop_access_dispatcher((*this)->block, key, PMCC(), false);
}
