// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/factory.hpp>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <stdexcept>
#include <iostream>
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

void FactoryRegistryEntry::arg_check(const PMCList &args, const size_t nargs)
{
    if (args.size() != nargs) throw std::runtime_error(str(boost::format(
        "factory expected %u arguments but got %u") % nargs % args.size()));
}

typedef std::map<std::string, boost::shared_ptr<FactoryRegistryEntry> > FactoryRegistryType;

static FactoryRegistryType &get_factory_registry(void)
{
    static FactoryRegistryType r;
    return r;
}

static boost::mutex mutex;

void gras::_register_factory(const std::string &path, void *entry)
{
    boost::mutex::scoped_lock l(mutex);
    if (get_factory_registry().count(path) != 0)
    {
        std::cerr << "Warning: Factory - function already registered for path: " + path << std::endl;
    }
    get_factory_registry()[path].reset(reinterpret_cast<FactoryRegistryEntry *>(entry));
}

Element *gras::_handle_make(const std::string &path, const PMCC &args)
{
    boost::mutex::scoped_lock l(mutex);
    if (get_factory_registry().count(path) == 0)
    {
        throw std::invalid_argument("Factory - no function registered for path: " + path);
    }
    return get_factory_registry()[path]->make(args);
}
