// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_DETAIL_PROPERTY_HPP
#define INCLUDED_GRAS_DETAIL_PROPERTY_HPP

#include <gras/gras.hpp>
#include <PMC/PMC.hpp>
#include <boost/shared_ptr.hpp>

namespace gras
{

struct GRAS_API PropertyRegistry
{
    PropertyRegistry(void);
    virtual ~PropertyRegistry(void);
    virtual void set(const PMCC &) = 0;
    virtual PMCC get(void) = 0;
};

typedef boost::shared_ptr<PropertyRegistry> PropertyRegistrySptr;

template <typename ClassType, typename ValueType>
struct PropertyRegistryImpl : PropertyRegistry
{
    PropertyRegistryImpl(
        ClassType *my_class,
        ValueType(ClassType::*getter)(void),
        void(ClassType::*setter)(const ValueType &)
    ):
        _my_class(my_class),
        _getter(getter),
        _setter(setter)
    {}
    virtual ~PropertyRegistryImpl(void){}

    void set(const PMCC &value)
    {
        return _setter(_my_class, value.as<ValueType>());
    }

    PMCC get(void)
    {
        return PMC_M(_getter(_my_class));
    }

    ClassType *_my_class;
    ValueType(ClassType::*_getter)(void);
    void(ClassType::*_setter)(const ValueType &);
};

} //namespace gras

#endif /*INCLUDED_GRAS_DETAIL_PROPERTY_HPP*/
