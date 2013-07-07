// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/callable.hpp>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <stdexcept>
#include <map>

using namespace gras;

typedef std::map<std::string, boost::shared_ptr<CallableRegistryEntry> > CallableRegistry;
typedef std::pair<std::string, boost::shared_ptr<CallableRegistryEntry> > CallableRegistryPair;

Callable::Callable(void)
{
    _call_registry.reset(new CallableRegistry());
}

Callable::~Callable(void)
{
    _call_registry.reset();
}

std::vector<std::string> Callable::get_registered_keys(void) const
{
    CallableRegistry *cr = reinterpret_cast<CallableRegistry *>(_call_registry.get());
    std::vector<std::string> keys;
    BOOST_FOREACH(const CallableRegistryPair &p, (*cr))
    {
        keys.push_back(p.first);
    }
    return keys;
}

void Callable::unregister_call(const std::string &key)
{
    CallableRegistry *cr = reinterpret_cast<CallableRegistry *>(_call_registry.get());
    if (cr->count(key) == 0) throw std::invalid_argument("Callable - no method registered for key: " + key);
    cr->erase(key);
}

void Callable::_register_call(const std::string &key, void *entry)
{
    CallableRegistry *cr = reinterpret_cast<CallableRegistry *>(_call_registry.get());
    (*cr)[key].reset(reinterpret_cast<CallableRegistryEntry *>(entry));
}

PMCC Callable::_handle_call(const std::string &key, const PMCC &args)
{
    CallableRegistry *cr = reinterpret_cast<CallableRegistry *>(_call_registry.get());
    if (cr->count(key) == 0) throw std::invalid_argument("Callable - no method registered for key: " + key);
    return (*cr)[key]->call(args);
}

CallableRegistryEntry::CallableRegistryEntry(void)
{
    //NOP
}

CallableRegistryEntry::~CallableRegistryEntry(void)
{
    //NOP
}
