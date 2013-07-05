// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_DETAIL_BLOCK_HPP
#define INCLUDED_GRAS_DETAIL_BLOCK_HPP

#include <typeinfo>

namespace gras
{

struct GRAS_API PropertyRegistry
{
    PropertyRegistry(void);
    virtual ~PropertyRegistry(void);
    virtual void set(const PMCC &) = 0;
    virtual PMCC get(void) = 0;
    virtual const std::type_info &type(void) const = 0;
};

template <typename ClassType, typename ValueType>
class PropertyRegistryImpl : public PropertyRegistry
{
public:
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
        (_my_class->*_setter)(value.safe_as<ValueType>());
    }

    PMCC get(void)
    {
        return PMC_M((_my_class->*_getter)());
    }

    const std::type_info &type(void) const
    {
        return typeid(ValueType);
    }

private:
    ClassType *_my_class;
    ValueType(ClassType::*_getter)(void);
    void(ClassType::*_setter)(const ValueType &);
};

/*!
 * The following functions implement the templated methods in Block
 */

template <typename ClassType, typename ValueType>
inline void Block::register_getter(
    const std::string &key,
    ValueType(ClassType::*get)(void)
)
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *pr = new PropertyRegistryImpl<ClassType, ValueType>(obj, get, NULL);
    this->_register_getter(key, pr);
}

template <typename ClassType, typename ValueType>
inline void Block::register_setter(
    const std::string &key,
    void(ClassType::*set)(const ValueType &)
)
{
    ClassType *obj = dynamic_cast<ClassType *>(this);
    void *pr = new PropertyRegistryImpl<ClassType, ValueType>(obj, NULL, set);
    this->_register_setter(key, pr);
}

template <typename ValueType>
inline void Block::set(const std::string &key, const ValueType &value)
{
    this->_set_property(key, PMC_M(value));
}

template <typename ValueType>
inline void Block::get(const std::string &key, ValueType &value)
{
    value = this->_get_property(key).safe_as<ValueType>();
}

template <typename ValueType>
inline ValueType Block::get(const std::string &key)
{
    return this->_get_property(key).safe_as<ValueType>();
}

template <typename ValueType>
inline void Block::post_output_msg(const size_t i, const ValueType &value)
{
    this->_post_output_msg(i, PMC_M(value));
}

template <>
inline void Block::post_output_msg(const size_t i, const PMCC &value)
{
    this->_post_output_msg(i, value);
}

template <>
inline void Block::post_output_msg(const size_t i, const PMC &value)
{
    this->_post_output_msg(i, value);
}

template <typename ValueType>
inline void Block::post_input_msg(const size_t i, const ValueType &value)
{
    this->_post_input_msg(i, PMC_M(value));
}

template <>
inline void Block::post_input_msg(const size_t i, const PMCC &value)
{
    this->_post_input_msg(i, value);
}

template <>
inline void Block::post_input_msg(const size_t i, const PMC &value)
{
    this->_post_input_msg(i, value);
}

} //namespace gras

#endif /*INCLUDED_GRAS_DETAIL_BLOCK_HPP*/
