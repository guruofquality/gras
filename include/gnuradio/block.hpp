//
// Copyright 2012 Josh Blum
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with io_sig program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef INCLUDED_GNURADIO_BLOCK_HPP
#define INCLUDED_GNURADIO_BLOCK_HPP

#include <gnuradio/runtime_api.h>
#include <gnuradio/element.hpp>
#include <gnuradio/tags.hpp>
#include <gruel/pmt.h>
#include <vector>

namespace gnuradio
{

template <typename PtrType> struct Buffer
{
    //! get a native pointer type to this buffer
    inline PtrType get(void) const
    {
        return _mem;
    }

    //! get a pointer of the desired type to this buffer
    template <typename T> inline T cast(void) const
    {
        return reinterpret_cast<T>(this->get());
    }

    //! get the number of items in this buffer
    inline size_t size(void) const
    {
        return _len;
    }

//private:
    PtrType _mem;
    size_t _len;
};

struct GR_RUNTIME_API Block : Element
{
    enum
    {
        WORK_CALLED_PRODUCE = -2,
        WORK_DONE = -1
    };

    enum tag_propagation_policy_t
    {
        TPP_DONT = 0,
        TPP_ALL_TO_ALL = 1,
        TPP_ONE_TO_ONE = 2
    };

    Block(void);

    /*!
     * Set the block's work mode (how it produces and consumes, and the ratio).
     * When automatic, consume is automatically called, and forecast handled.
     * \param automatic true to call consume and forecast automatically
     */
    void set_auto_consume(const bool automatic);

    /*******************************************************************
     * Basic routines from basic block
     ******************************************************************/

    long unique_id(void) const;

    std::string name(void) const;

    unsigned history(void) const;

    void set_history(unsigned history);

    void set_output_multiple(int multiple);

    int output_multiple(void) const;

    void consume(int which_input, int how_many_items);

    void consume_each(int how_many_items);

    void produce(int which_output, int how_many_items);

    /*!
     * The relative rate can be thought of as interpolation/decimation.
     * In other words, relative rate is the ratio of output items to input items.
     */
    void set_relative_rate(double relative_rate);

    double relative_rate(void) const;

    /*******************************************************************
     * Tag related routines from basic block
     ******************************************************************/

    uint64_t nitems_read(unsigned int which_input);

    uint64_t nitems_written(unsigned int which_output);

    tag_propagation_policy_t tag_propagation_policy(void);

    void set_tag_propagation_policy(tag_propagation_policy_t p);

    void add_item_tag(
        unsigned int which_output, const Tag &tag
    );

    void add_item_tag(
        unsigned int which_output,
        uint64_t abs_offset,
        const pmt::pmt_t &key,
        const pmt::pmt_t &value,
        const pmt::pmt_t &srcid=pmt::PMT_F
    );

    void get_tags_in_range(
        std::vector<Tag> &tags,
        unsigned int which_input,
        uint64_t abs_start,
        uint64_t abs_end
    );

    void get_tags_in_range(
        std::vector<Tag> &tags,
        unsigned int which_input,
        uint64_t abs_start,
        uint64_t abs_end,
        const pmt::pmt_t &key
    );

    /*******************************************************************
     * Work related routines from basic block
     ******************************************************************/

    //! Called when the flow graph is started, can overload
    virtual bool start(void);

    //! Called when the flow graph is stopped, can overload
    virtual bool stop(void);

    typedef std::vector<Buffer<const void *> > InputItems;
    typedef std::vector<Buffer<void *> > OutputItems;

    //! The official call into the work routine (overload please)
    virtual int work(
        const InputItems &input_items,
        const OutputItems &output_items
    ) = 0;

    //! forcast requirements, can be overloaded
    virtual void forecast(
        int noutput_items,
        std::vector<size_t> &ninput_items_required
    );

};

} //namespace gnuradio

#endif /*INCLUDED_GNURADIO_BLOCK_HPP*/
