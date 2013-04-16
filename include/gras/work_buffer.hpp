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
    size_t min(void) const;

    //! Get a reference to the min items
    size_t &min(void);

    //! Get the std::max of all item sizes
    size_t max(void) const;

    //! Get a reference to the max items
    size_t &max(void);

    //! Get a vector of just the work pointers
    const std::vector<PtrType> &vec(void) const;

    //! Get a reference to the work pointers
    std::vector<PtrType> &vec(void);

    //! Resize the work buffer array
    void resize(const size_t size);

private:
    size_t _min;
    size_t _max;
    std::vector<PtrType> _vec;
};

} //namespace gras

#include <gras/detail/work_buffer.hpp>

#endif /*INCLUDED_GRAS_WORK_BUFFER_HPP*/
