// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_IO_SIGNATURE_HPP
#define INCLUDED_GRAS_IO_SIGNATURE_HPP

#include <vector>
#include <stdexcept>
#include <cstdlib>

namespace gras
{

/*!
 * An IO signature describes the input or output specifications
 * for the streaming input or output ports of a block.
 * Properties are a maximum and minimum number of ports,
 * and an item size in bytes for each port.
 */
struct IOSignature : std::vector<size_t>
{
    static const int IO_INFINITE = ~0;

    //! Create an empty signature
    IOSignature(void)
    {
        this->set_min_streams(IO_INFINITE);
        this->set_max_streams(IO_INFINITE);
    }

    //! Create a signature with a single item size
    IOSignature(const size_t size)
    {
        this->push_back(size);
        this->set_min_streams(IO_INFINITE);
        this->set_max_streams(IO_INFINITE);
    }

    //! Create a signature with the specified min and max streams
    IOSignature(const int min_streams, const int max_streams)
    {
        if (min_streams > max_streams and max_streams != IO_INFINITE)
        {
            throw std::invalid_argument("io signature fail: min_streams > max_streams");
        }
        this->set_min_streams(min_streams);
        this->set_max_streams(max_streams);
    }

    //! Create a signature from a vector of IO widths
    IOSignature(const std::vector<size_t> &sig)
    {
        this->assign(sig.begin(), sig.end());
        this->set_min_streams(IO_INFINITE);
        this->set_max_streams(IO_INFINITE);
    }

    //! Construct from pointer for backwards compatible shared_ptr usage.
    explicit IOSignature(const IOSignature *sig)
    {
        *this = *sig;
    }

    //! Overloaded arrow operator for backwards compatible shared_ptr usage.
    IOSignature* operator->(void)
    {
        return this;
    };

    //! Overloaded arrow operator for backwards compatible shared_ptr usage.
    const IOSignature* operator->(void) const
    {
        return this;
    };

    const size_t &at(const size_t index) const
    {
        if (this->empty())
        {
            throw std::invalid_argument("io signature fail: indexing empty vector");
        }
        if (this->size() > index)
        {
            return std::vector<size_t>::at(index);
        }
        return this->back();
    }

    const size_t &operator[](const size_t index) const
    {
        return this->at(index);
    }

    void set_min_streams(const int val)
    {
        _min_streams = val;
    }

    void set_max_streams(const int val)
    {
        _max_streams = val;
    }

    int min_streams(void) const
    {
        return _min_streams;
    }

    int max_streams(void) const
    {
        return _max_streams;
    }

    int sizeof_stream_item(const size_t index) const
    {
        return this->at(index);
    }

    const std::vector<size_t> &sizeof_stream_items(void) const
    {
        return *this;
    }

    int _min_streams;
    int _max_streams;
};

} //namespace gras

#endif /*INCLUDED_GRAS_IO_SIGNATURE_HPP*/
