// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_DETAIL_PROPERTY_HPP
#define INCLUDED_GRAS_DETAIL_PROPERTY_HPP

#include <PMC/PMC.hpp>

namespace gras
{

struct PropertyRegistry
{
    PropertyRegistry(void){}
    ~PropertyRegistry(void){}
    virtual void set(const PMCC &) = 0;
    virtual PMCC get(void) = 0;
};

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
    ~PropertyRegistryImpl(void){}

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

struct PropertyInterface
{
    virtual void property_interface_register(
        const std::string &key,
        boost::shared_ptr<PropertyRegistry> pr
    ) = 0;

    virtual void property_interface_set(const std::string &key, const PMCC &value) = 0;

    virtual PMCC property_interface_get(const std::string &key) = 0;

};

} //namespace gras

#endif /*INCLUDED_GRAS_DETAIL_PROPERTY_HPP*/
