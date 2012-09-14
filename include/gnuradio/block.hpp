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

#include <gnuradio/element.hpp>
#include <gnuradio/tags.hpp>
#include <gruel/pmt.h>
#include <vector>
#include <string>

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

struct GRAS_API Block : Element
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

    Block(const std::string &name);

    /*******************************************************************
     * Basic routines from basic block
     ******************************************************************/

    //! Get the number of history items (default 0)
    size_t input_history(const size_t which_input = 0) const;

    /*!
     * Set the number of items that will be saved from the previous run.
     * Input buffers will begin with an overlap of the previous's buffer's
     * num history items. This is used to implement sample memory for
     * things like sliding dot products/FIR filters.
     */
    void set_input_history(const size_t history, const size_t which_input = 0);

    void set_output_multiple(const size_t multiple, const size_t which_output = 0);

    size_t output_multiple(const size_t which_output = 0) const;

    void consume(const size_t which_input, const size_t how_many_items);

    void consume_each(const size_t how_many_items);

    void produce(const size_t which_output, const size_t how_many_items);

    /*!
     * Set buffer inlining for this input.
     * Inlining means that the input buffer can be used as an output buffer.
     * The goal is to make better use of cache and memory bandwidth.
     *
     * By default, inlining is disabled on all input ports.
     * The user should enable inlining on an input port
     * when it is understood that the work function will read
     * before writting to a particular section of the buffer.
     *
     * The scheduler will inline a buffer when
     *  * inlining is enabled on the particular input port
     *  * block holds the only buffer reference aka unique
     *  * the input buffer has the same affinity as the block
     *  * the input port has a buffer history of 0 items
     */
    void set_input_inline(const size_t which_input, const bool enb);

    //! Get the buffer inlining state
    bool input_inline(const size_t which_input) const;

    /*!
     * Enable fixed rate logic.
     * When enabled, relative rate is assumed to be set,
     * and forecast is automatically called.
     * Also, consume will be called automatically.
     */
    void set_fixed_rate(const bool fixed_rate);

    /*!
     * The relative rate can be thought of as interpolation/decimation.
     * In other words, relative rate is the ratio of output items to input items.
     */
    void set_relative_rate(double relative_rate);

    double relative_rate(void) const;

    /*******************************************************************
     * Tag related routines from basic block
     ******************************************************************/

    uint64_t nitems_read(const size_t which_input);

    uint64_t nitems_written(const size_t which_output);

    tag_propagation_policy_t tag_propagation_policy(void);

    void set_tag_propagation_policy(tag_propagation_policy_t p);

    void add_item_tag(
        const size_t which_output, const Tag &tag
    );

    void add_item_tag(
        const size_t which_output,
        uint64_t abs_offset,
        const pmt::pmt_t &key,
        const pmt::pmt_t &value,
        const pmt::pmt_t &srcid=pmt::PMT_F
    );

    void get_tags_in_range(
        std::vector<Tag> &tags,
        const size_t which_input,
        uint64_t abs_start,
        uint64_t abs_end
    );

    void get_tags_in_range(
        std::vector<Tag> &tags,
        const size_t which_input,
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
    virtual int Work(
        const InputItems &input_items,
        const OutputItems &output_items
    ) = 0;

    //! forcast requirements, can be overloaded
    virtual void forecast(
        int noutput_items,
        std::vector<int> &ninput_items_required
    );

    //! scheduler calls when the topology is updated, can be overloaded
    virtual bool check_topology(int ninputs, int noutputs);

};

} //namespace gnuradio

#endif /*INCLUDED_GNURADIO_BLOCK_HPP*/
