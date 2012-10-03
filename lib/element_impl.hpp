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

#ifndef INCLUDED_LIBGRAS_ELEMENT_IMPL_HPP
#define INCLUDED_LIBGRAS_ELEMENT_IMPL_HPP

#include <gras_impl/block_actor.hpp>
#include <Apology/Topology.hpp>
#include <Apology/Executor.hpp>
#include <gnuradio/element.hpp>
#include <gnuradio/block.hpp>
#include <gras_impl/token.hpp>
#include <gras_impl/interruptible_thread.hpp>

namespace gnuradio
{

struct ElementImpl
{

    //deconstructor stuff
    ~ElementImpl(void);
    void top_block_cleanup(void);
    void hier_block_cleanup(void);

    //common element properties
    std::string name;
    long unique_id;
    IOSignature input_signature;
    IOSignature output_signature;

    //top block stuff
    SharedThreadGroup thread_group;
    Token token;

    //things may be in this element
    boost::shared_ptr<Apology::Topology> topology;
    boost::shared_ptr<Apology::Executor> executor;
    boost::shared_ptr<BlockActor> block;
    Apology::Base *get_elem(void) const
    {
        if (block) return block.get();
        return topology.get();
    }


};

} //namespace gnuradio

#endif /*INCLUDED_LIBGRAS_ELEMENT_IMPL_HPP*/
