// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_WORK_BUFFER_HPP
#define INCLUDED_GRAS_WORK_BUFFER_HPP

#include <gras/gras.hpp>
#include <cstdlib>
#include <vector>

namespace gras
{

template <typename PtrType>
class WorkBuffer
{
public:
    //! get a pointer of the desired type to this buffer
    template <typename T> T cast(void) const;

    //! get a native pointer type to this buffer
    PtrType get(void) const;

    //! Get the memory pointer reference
    PtrType &get(void);

    //! get the number of items in this buffer
    size_t size(void) const;

    //! Get the buffer length reference
    size_t &size(void);

private:
    PtrType _mem;
    size_t _len;
};

template <typename PtrType>
class WorkBufferArray : public std::vector<WorkBuffer<PtrType> >
{
public:
    //! Get the std::min all item sizes
    size_t min_items(void) const;

    //! Get a reference to the min items
    size_t &min_items(void);

    //! Get the std::max of all item sizes
    size_t max_items(void) const;

    //! Get a reference to the max items
    size_t &max_items(void);

private:
    size_t _min_items;
    size_t _max_items;
};

} //namespace gras

#include <gras/work_buffer.ipp>

#endif /*INCLUDED_GRAS_WORK_BUFFER_HPP*/
