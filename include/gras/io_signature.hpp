// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_IO_SIGNATURE_HPP
#define INCLUDED_GRAS_IO_SIGNATURE_HPP

#include <vector>
#include <stdexcept>

namespace gras
{

/*!
 * An IO signature describes the input or output specifications
 * for the streaming input or output ports of a block.
 * Properties are a maximum and minimum number of ports,
 * and an item size in bytes for each port.
 */
struct IOSignature : std::vector<int>
{
    static const int IO_INFINITE = -1;

    //! Create an empty signature with infinite IO
    IOSignature(void)
    {
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
    IOSignature(const std::vector<int> &sig)
    {
        this->assign(sig.begin(), sig.end());
        this->set_min_streams(this->size());
        this->set_max_streams(this->size());
    }

    //! Construct from pointer for backwards compatible shared_ptr usage.
    IOSignature(const IOSignature *sig)
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

    int sizeof_stream_item(const int index) const
    {
        if (this->empty()) return 0;
        if (this->size() > unsigned(index))
        {
            return this->at(index);
        }
        return this->back();
    }

    std::vector<int> sizeof_stream_items(void) const
    {
        return *this;
    }

    int _min_streams;
    int _max_streams;
};

} //namespace gras

#endif /*INCLUDED_GRAS_IO_SIGNATURE_HPP*/
