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

#ifndef INCLUDED_LIBGNURADIO_ELEMENT_IMPL_HPP
#define INCLUDED_LIBGNURADIO_ELEMENT_IMPL_HPP

#include <tsbe/block.hpp>
#include <tsbe/topology.hpp>
#include <tsbe/executor.hpp>
#include <gnuradio/element.hpp>
#include <gnuradio/block.hpp>
#include <gr_types.h>
#include <vector>
#include <iostream>

#define HERE() std::cerr << __FILE__ << ":" << __LINE__ << std::endl << std::flush;
#define VAR(x) std::cerr << #x << " = " << (x) << std::endl << std::flush;
#define ASSERT(x) if(not (x)){HERE(); std::cerr << "assert failed: " << #x << std::endl << std::flush;}

static inline unsigned long myulround(const double x)
{
    return (unsigned long)(x + 0.5);
}

static inline unsigned long long myullround(const double x)
{
    return (unsigned long long)(x + 0.5);
}

//! return true if an instance was found and removed
template <typename V, typename T>
bool remove_one(V &v, const T &t)
{
    for (size_t i = 0; i < v.size(); i++)
    {
        if (v[i] == t)
        {
            v.erase(v.begin() + i);
            return true;
        }
    }
    return false;
}

typedef boost::shared_ptr<int> Token;
static inline Token make_token(void)
{
    return Token(new int(0));
}

struct TopBlockMessage
{
    enum
    {
        ACTIVE,
        INERT,
        HINT,
    } what;
    size_t hint;
    Token token;
};

struct CheckTokensMessage
{
    //empty
};

namespace gnuradio
{

struct ElementImpl
{
    ElementImpl(void)
    {
        //NOP
    }

    ~ElementImpl(void)
    {
        children.clear();
    }

    std::vector<boost::shared_ptr<Element> > children;

    //stuff for when its a block
    std::string name;
    long unique_id;

    //per port properties
    std::vector<size_t> input_items_sizes;
    std::vector<size_t> output_items_sizes;
    IOSignature input_signature;
    IOSignature output_signature;
    std::vector<size_t> input_history_items;
    std::vector<size_t> output_multiple_items;

    //keeps track of production
    std::vector<uint64_t> items_consumed;
    std::vector<uint64_t> items_produced;

    //work buffers for the classic interface
    gr_vector_const_void_star work_input_items;
    gr_vector_void_star work_output_items;
    gr_vector_int work_ninput_items;

    //work buffers for the new work interface
    Block::InputItems input_items;
    Block::OutputItems output_items;

    //track work's calls to produce and consume
    std::vector<size_t> produce_items;
    std::vector<size_t> consume_items;

    //state for partial input buffer consumption
    std::vector<size_t> input_buff_offsets;

    //special buffer for dealing with history
    std::vector<tsbe::Buffer> history_buffs;

    //track the subscriber counts
    std::vector<Token> input_tokens;
    std::vector<Token> output_tokens;
    std::vector<Token> token_pool;

    //tag tracking
    std::vector<bool> input_tags_changed;
    std::vector<std::vector<Tag> > input_tags;
    std::vector<std::vector<Tag> > output_tags;
    Block::tag_propagation_policy_t tag_prop_policy;

    //topological things
    tsbe::Block block;
    tsbe::Topology topology;
    tsbe::Executor executor;
    const tsbe::Element &get_elem(void) const
    {
        if (block) return block;
        return topology;
    }
    //gets the handlers access for forecast and work
    Block *block_ptr;

    //handlers
    void handle_input_msg(const tsbe::TaskInterface &, const size_t, const tsbe::Wax &);
    void handle_output_msg(const tsbe::TaskInterface &, const size_t, const tsbe::Wax &);
    void topology_update(const tsbe::TaskInterface &, const tsbe::Wax &);
    void handle_allocation(const tsbe::TaskInterface &);
    void handle_task(const tsbe::TaskInterface &);
    void mark_done(const tsbe::TaskInterface &);
    void free_inputs(const tsbe::TaskInterface &);

    //is the fg running?
    bool active;
    Token token;
    size_t hint; //some kind of allocation hint

    //rate settings
    bool enable_fixed_rate;
    double relative_rate;
};

} //namespace gnuradio

#endif /*INCLUDED_LIBGNURADIO_ELEMENT_IMPL_HPP*/
