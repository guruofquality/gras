// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_DETAIL_FACTORY_HPP
#define INCLUDED_GRAS_DETAIL_FACTORY_HPP

#include <PMC/Containers.hpp> //PMCList

namespace gras
{

/***********************************************************************
 * Factory entry base class
 **********************************************************************/
struct GRAS_API FactoryRegistryEntry
{
    FactoryRegistryEntry(void);
    virtual ~FactoryRegistryEntry(void);
    virtual Element *make(const PMCC &args) = 0;
};

#for $NARGS in range($MAX_ARGS)
/***********************************************************************
 * Templated registration - $NARGS args
 **********************************************************************/
template <typename ReturnType, $expand('typename Arg%d', $NARGS)>
struct FactoryRegistryEntryImpl$(NARGS) : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)($expand('const Arg%d &', $NARGS));
    FactoryRegistryEntryImpl$(NARGS)(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        if (a.size() < $NARGS) throw a;
        return _fcn($expand('a[%d].safe_as<Arg%d>()', $NARGS));
    }
    Fcn _fcn;
};

template <typename ReturnType, $expand('typename Arg%d', $NARGS)>
void Factory::register_make(const std::string &name, ReturnType(*fcn)($expand('const Arg%d &', $NARGS)))
{
    void *r = new FactoryRegistryEntryImpl$(NARGS)<ReturnType, $expand('Arg%d', $NARGS)>(fcn);
    Factory::_register_make(name, r);
}

#end for
/***********************************************************************
 * Templated make implementations
 **********************************************************************/
#for $NARGS in range($MAX_ARGS)
template <$expand('typename Arg%d', $NARGS)>
Element *Factory::make(const std::string &name, $expand('const Arg%d &a%d', $NARGS))
{
    PMCList args($NARGS);
    #for $i in range($NARGS):
    args[$i] = PMC_M(a$i);
    #end for
    return Factory::_handle_make(name, PMC_M(args));
}

#end for
}

#endif /*INCLUDED_GRAS_DETAIL_FACTORY_HPP*/
