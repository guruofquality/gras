// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_DETAIL_BLOCK_HPP
#define INCLUDED_GRAS_DETAIL_BLOCK_HPP

namespace gras
{

/*!
 * The following functions implement the templated methods in Block
 */

template <typename ClassType, typename ValueType>
inline void Block::register_property(
    const std::string &key,
    ValueType(ClassType::*get)(void),
    void(ClassType::*set)(const ValueType &)
)
{
    PropertyRegistrySptr pr;
    pr.reset(new PropertyRegistryImpl<ClassType, ValueType>((ClassType *)this, get, set));
    this->_register_property(key, pr);
}

template <typename ValueType>
inline void Block::set(const std::string &key, const ValueType &value)
{
    this->_set_property(key, PMC_M(value));
}

template <typename ValueType>
inline void Block::get(const std::string &key, ValueType &value)
{
    value = this->_get_property(key).as<ValueType>();
}

template <typename ValueType>
inline ValueType Block::get(const std::string &key)
{
    return this->_get_property(key).as<ValueType>();
}

} //namespace gras

#endif /*INCLUDED_GRAS_DETAIL_BLOCK_HPP*/
