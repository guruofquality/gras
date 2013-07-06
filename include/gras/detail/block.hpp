// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_DETAIL_BLOCK_HPP
#define INCLUDED_GRAS_DETAIL_BLOCK_HPP

namespace gras
{

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
