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
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef INCLUDED_GNURADIO_BLOCK_HPP
#define INCLUDED_GNURADIO_BLOCK_HPP

#include <gnuradio/element.hpp>
#include <gnuradio/sbuffer.hpp>
#include <gnuradio/tags.hpp>
#include <gruel/pmt.h>
#include <vector>
#include <string>

namespace gnuradio
{

//! Configuration parameters for an input port
struct GRAS_API InputPortConfig
{
    InputPortConfig(void);

    /*!
     * Set buffer inlining for this port config.
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
     *  * the input port has a buffer look-ahead of 0
     *
     * Default = false.
     */
    bool inline_buffer;

    /*!
     * Set the number of input buffer look-ahead items.
     * When num look-ahead items are not consumed,
     * they will be available for the next work call.
     * This is used to implement sample memory for
     * things like sliding dot products/FIR filters.
     *
     * Default = 0.
     */
    size_t lookahead_items;
};

//! Configuration parameters for an output port
struct GRAS_API OutputPortConfig
{
    OutputPortConfig(void);

    /*!
     * Set an output reserve requirement such that work is called
     * with an output buffer at least reserve items in size.
     *
     * Default = 1.
     */
    size_t reserve_items;

    /*!
     * Constrain the maximum number of items that
     * work can be called with for this port.
     *
     * Default = 0 aka disabled.
     */
    size_t maximum_items;
};

template <typename PtrType> struct WorkBuffer
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

    //! Get the memory pointer reference
    inline PtrType &get(void)
    {
        return _mem;
    }

    //! Get the buffer length reference
    inline size_t &size(void)
    {
        return _len;
    }

    PtrType _mem;
    size_t _len;
};

struct GRAS_API Block : Element
{

    //! Contruct an empty/null block
    Block(void);

    //! Create a new block given the name
    Block(const std::string &name);

    /*******************************************************************
     * Deal with input and output port configuration
     ******************************************************************/

    //! Get the configuration rules of an input port
    InputPortConfig input_config(const size_t which_input = 0) const;

    //! Set the configuration rules for an input port
    void set_input_config(const InputPortConfig &config, const size_t which_input = 0);

    //! Get the configuration rules of an output port
    OutputPortConfig output_config(const size_t which_output = 0) const;

    //! Set the configuration rules for an output port
    void set_output_config(const OutputPortConfig &config, const size_t which_output = 0);

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
     * Deal with data production and consumption
     ******************************************************************/

    //! Return options for the work call
    enum
    {
        WORK_CALLED_PRODUCE = -2,
        WORK_DONE = -1
    };

    //! Call during work to consume items
    void consume(const size_t which_input, const size_t how_many_items);

    //! Call during work to consume items
    void consume_each(const size_t how_many_items);

    //! Call during work to produce items, must return WORK_CALLED_PRODUCE
    void produce(const size_t which_output, const size_t how_many_items);

    /*******************************************************************
     * Deal with tag handling and tag configuration
     ******************************************************************/

    enum tag_propagation_policy_t
    {
        TPP_DONT = 0,
        TPP_ALL_TO_ALL = 1,
        TPP_ONE_TO_ONE = 2
    };

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

    typedef std::vector<WorkBuffer<const void *> > InputItems;
    typedef std::vector<WorkBuffer<void *> > OutputItems;

    //! The official call into the work routine (overload please)
    virtual int work(
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

    /*!
     * Set if the work call should be interruptible by stop().
     * Some work implementations block with the expectation of
     * getting a boost thread interrupt in a blocking call.
     * Set set_interruptible_work(true) if this is the case.
     * By default, work implementations are not interruptible.
     */
    void set_interruptible_work(const bool enb);

    /*******************************************************************
     * routines related to affinity and allocation
     ******************************************************************/

    /*!
     * Set the node affinity of this block.
     * This call affects how output buffers are allocated.
     * By default memory is allocated by malloc.
     * When the affinity is set, virtual memory
     * will be locked to a physical CPU/memory node.
     */
    void set_buffer_affinity(const Affinity &affinity);

    /*!
     * The output buffer allocator method.
     * This method is called by the scheduler to allocate output buffers.
     * The user may overload this method to create a custom allocator.
     *
     * Example use case:
     * //TODO code example
     *
     * \param which_output the output port index number
     * \param token the token for the buffer's returner
     * \param recommend_length the schedulers recommended length in bytes
     * \return the token used for the buffer allocation (may be the same)
     */
    virtual SBufferToken output_buffer_allocator(
        const size_t which_output,
        const SBufferToken &token,
        const size_t recommend_length
    );

    /*!
     * The input buffer allocator method.
     * This method is special and very different from allocate output buffers.
     * Typically, blocks do not have control of their input buffers.
     * When overloaded, an upstream block will ask this block
     * to allocate its output buffers. This way, this block will get
     * input buffers which were actually allocated by this method.
     *
     * \param which_input the input port index number
     * \param token the token for the buffer's returner
     * \param recommend_length the schedulers recommended length in bytes
     * \return the token used for the buffer allocation (may be the same)
     */
    virtual SBufferToken input_buffer_allocator(
        const size_t which_input,
        const SBufferToken &token,
        const size_t recommend_length
    );

};

} //namespace gnuradio

#endif /*INCLUDED_GNURADIO_BLOCK_HPP*/
