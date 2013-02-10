// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_LIBGRAS_IMPL_COOL_QUEUE_HPP
#define INCLUDED_LIBGRAS_IMPL_COOL_QUEUE_HPP

#include <gras_impl/debug.hpp>
#include <Theron/Align.h> //alignment macros
#include <vector>

namespace gras
{

template <typename T>
struct CoolQueue
{
    CoolQueue(const size_t size = 4):
        _index(0), _mask(0), _size(0)
    {
        this->set_capacity(size);
    }

    GRAS_FORCE_INLINE void clear(void)
    {
        const size_t size = _container.size();
        _container.clear();
        _container.resize(size);
    }

    GRAS_FORCE_INLINE bool empty(void) const
    {
        return _size == 0;
    }

    GRAS_FORCE_INLINE bool full(void) const
    {
        return _size == _container.size();
    }

    GRAS_FORCE_INLINE const T &front(void) const
    {
        return (*this)[0];
    }

    GRAS_FORCE_INLINE T &front(void)
    {
        return (*this)[0];
    }

    GRAS_FORCE_INLINE const T &back(void) const
    {
        return (*this)[_size-1];
    }

    GRAS_FORCE_INLINE T &back(void)
    {
        return (*this)[_size-1];
    }

    GRAS_FORCE_INLINE void push_back(const T &e)
    {
        _size++;
        this->back() = e;
    }

    GRAS_FORCE_INLINE void pop_back(void)
    {
        this->back() = T();
        _size--;
    }

    GRAS_FORCE_INLINE void push_front(const T &e)
    {
        _size++;
        _index++;
        this->front() = e;
    }

    GRAS_FORCE_INLINE void pop_front(void)
    {
        this->front() = T();
        _index--;
        _size--;
    }

    GRAS_FORCE_INLINE void set_capacity(size_t size)
    {
        const size_t l2c = size_t(std::ceil(std::log(double(size))/std::log(2.0)));
        _container.resize(1 << l2c);
        _mask = (1 << l2c) - 1;
        _index = std::min(_index, _container.size()-1);
        _size = std::min(_size, _container.size());
    }

    GRAS_FORCE_INLINE size_t size(void) const
    {
        return _size;
    }

    GRAS_FORCE_INLINE const T &operator[](const size_t i) const
    {
        return _container[(_index-i)&_mask].e;
    }

    GRAS_FORCE_INLINE T &operator[](const size_t i)
    {
        return _container[(_index-i)&_mask].e;
    }

    struct ElemT
    {
        ElemT(void){}
        ElemT(const T &e): e(e){}
        THERON_PREALIGN(GRAS_MAX_ALIGNMENT)
            T e
        THERON_POSTALIGN(GRAS_MAX_ALIGNMENT);
    };

    std::vector<ElemT> _container;
    size_t _index, _mask, _size;
};

} //namespace gras

#endif /*INCLUDED_LIBGRAS_IMPL_COOL_QUEUE_HPP*/
