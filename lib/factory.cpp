// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/factory.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <stdexcept>
#include <map>

using namespace gras;

FactoryRegistryEntry::FactoryRegistryEntry(void)
{
    //NOP
}

FactoryRegistryEntry::~FactoryRegistryEntry(void)
{
    //NOP
}

typedef std::map<std::string, boost::shared_ptr<FactoryRegistryEntry> > FactoryRegistryType;

static FactoryRegistryType &get_factory_registry(void)
{
    static FactoryRegistryType r;
    return r;
}

static boost::mutex mutex;

void Factory::_register_make(const std::string &name, void *entry)
{
    boost::mutex::scoped_lock l(mutex);
    if (get_factory_registry().count(name) != 0)
    {
        throw std::invalid_argument("Factory - function already registered for name: " + name);
    }
    get_factory_registry()[name].reset(reinterpret_cast<FactoryRegistryEntry *>(entry));
}

Element *Factory::_handle_make(const std::string &name, const PMCC &args)
{
    boost::mutex::scoped_lock l(mutex);
    if (get_factory_registry().count(name) == 0)
    {
        throw std::invalid_argument("Factory - no function registered for name: " + name);
    }
    return get_factory_registry()[name]->make(args);
}
