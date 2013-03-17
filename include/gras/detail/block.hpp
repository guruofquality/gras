// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_DETAIL_BLOCK_HPP
#define INCLUDED_GRAS_DETAIL_BLOCK_HPP

namespace gras
{

template <typename ClassType, typename ValueType>
GRAS_FORCE_INLINE void Block::register_property(
    const std::string &key,
    ValueType(ClassType::*get)(void),
    void(ClassType::*set)(const ValueType &)
)
{
    boost::shared_ptr<PropertyRegistry> pr;
    pr.reset(new PropertyRegistryImpl<ClassType, ValueType>(this, get, set));
    this->property_interface_register(key, pr);
}

template <typename ValueType>
GRAS_FORCE_INLINE void Block::set_property(const std::string &key, const ValueType &value)
{
    return this->property_interface_set(key, PMC_M(value));
}

template <typename ValueType>
GRAS_FORCE_INLINE ValueType Block::get_property(const std::string &key)
{
    return this->property_interface_get(key).as<ValueType>();
}

} //namespace gras

#endif /*INCLUDED_GRAS_DETAIL_BLOCK_HPP*/
