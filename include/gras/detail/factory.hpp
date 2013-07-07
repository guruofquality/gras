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

/***********************************************************************
 * Templated registration - 0 args
 **********************************************************************/
template <typename ReturnType>
struct FactoryRegistryEntryImpl0 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(void);
    FactoryRegistryEntryImpl0(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &)
    {
        return _fcn();
    }
    Fcn _fcn;
};

template <typename ReturnType>
void Factory::register_make(const std::string &name, ReturnType(*fcn)(void))
{
    void *r = new FactoryRegistryEntryImpl0<ReturnType>(fcn);
    Factory::_register_make(name, r);
}

/***********************************************************************
 * Templated registration - 1 args
 **********************************************************************/
template <typename ReturnType, typename Arg0>
struct FactoryRegistryEntryImpl1 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const Arg0 &);
    FactoryRegistryEntryImpl1(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        return _fcn(a[0].safe_as<Arg0>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename Arg0>
void Factory::register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &))
{
    void *r = new FactoryRegistryEntryImpl1<ReturnType, Arg0>(fcn);
    Factory::_register_make(name, r);
}

/***********************************************************************
 * Templated registration - 2 args
 **********************************************************************/
template <typename ReturnType, typename Arg0, typename Arg1>
struct FactoryRegistryEntryImpl2 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const Arg0 &, const Arg1 &);
    FactoryRegistryEntryImpl2(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        return _fcn(a[0].safe_as<Arg0>(), a[1].safe_as<Arg1>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename Arg0, typename Arg1>
void Factory::register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &, const Arg1 &))
{
    void *r = new FactoryRegistryEntryImpl2<ReturnType, Arg0, Arg1>(fcn);
    Factory::_register_make(name, r);
}

/***********************************************************************
 * Templated registration - 3 args
 **********************************************************************/
template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2>
struct FactoryRegistryEntryImpl3 : FactoryRegistryEntry
{
    typedef ReturnType(*Fcn)(const Arg0 &, const Arg1 &, const Arg2 &);
    FactoryRegistryEntryImpl3(Fcn fcn):_fcn(fcn){}
    Element *make(const PMCC &args)
    {
        const PMCList &a = args.as<PMCList>();
        return _fcn(a[0].safe_as<Arg0>(), a[1].safe_as<Arg1>(), a[2].safe_as<Arg2>());
    }
    Fcn _fcn;
};

template <typename ReturnType, typename Arg0, typename Arg1, typename Arg2>
void Factory::register_make(const std::string &name, ReturnType(*fcn)(const Arg0 &, const Arg1 &, const Arg2 &))
{
    void *r = new FactoryRegistryEntryImpl3<ReturnType, Arg0, Arg1, Arg2>(fcn);
    Factory::_register_make(name, r);
}

/***********************************************************************
 * Templated make implementations
 **********************************************************************/
inline
Element *Factory::make(const std::string &name)
{
    PMCList args(0);
    return Factory::_make(name, PMC_M(args));
}

template <typename Arg0>
Element *Factory::make(const std::string &name, const Arg0 &a0)
{
    PMCList args(1);
    args[0] = PMC_M(a0);
    return Factory::_make(name, PMC_M(args));
}

template <typename Arg0, typename Arg1>
Element *Factory::make(const std::string &name, const Arg0 &a0, const Arg1 &a1)
{
    PMCList args(2);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    return Factory::_make(name, PMC_M(args));
}

template <typename Arg0, typename Arg1, typename Arg2>
Element *Factory::make(const std::string &name, const Arg0 &a0, const Arg1 &a1, const Arg2 &a2)
{
    PMCList args(3);
    args[0] = PMC_M(a0);
    args[1] = PMC_M(a1);
    args[2] = PMC_M(a2);
    return Factory::_make(name, PMC_M(args));
}

}

#endif /*INCLUDED_GRAS_DETAIL_FACTORY_HPP*/
