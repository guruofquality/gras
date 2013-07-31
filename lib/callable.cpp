// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/callable.hpp>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
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

std::vector<std::string> Callable::get_registered_names(void) const
{
    CallableRegistry *cr = reinterpret_cast<CallableRegistry *>(_call_registry.get());
    std::vector<std::string> names;
    BOOST_FOREACH(const CallableRegistryPair &p, (*cr))
    {
        names.push_back(p.first);
    }
    return names;
}

void Callable::unregister_call(const std::string &name)
{
    CallableRegistry *cr = reinterpret_cast<CallableRegistry *>(_call_registry.get());
    if (cr->count(name) == 0) throw std::invalid_argument("Callable - no method registered for name: " + name);
    cr->erase(name);
}

void Callable::_register_call(const std::string &name, void *entry)
{
    CallableRegistry *cr = reinterpret_cast<CallableRegistry *>(_call_registry.get());
    if (cr->count(name) != 0) throw std::invalid_argument("Callable - method already registered for name: " + name);
    (*cr)[name].reset(reinterpret_cast<CallableRegistryEntry *>(entry));
}

PMCC Callable::_handle_call(const std::string &name, const PMCC &args)
{
    CallableRegistry *cr = reinterpret_cast<CallableRegistry *>(_call_registry.get());
    if (cr->count(name) == 0) throw std::invalid_argument("Callable - no method registered for name: " + name);
    return (*cr)[name]->call(args);
}

CallableRegistryEntry::CallableRegistryEntry(void)
{
    //NOP
}

CallableRegistryEntry::~CallableRegistryEntry(void)
{
    //NOP
}

void CallableRegistryEntry::arg_check(const PMCList &args, const size_t nargs)
{
    if (args.size() != nargs) throw std::runtime_error(str(boost::format(
        "callable expected %u arguments but got %u") % nargs % args.size()));
}
