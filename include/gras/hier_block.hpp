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

#ifndef INCLUDED_GRAS_HIER_BLOCK_HPP
#define INCLUDED_GRAS_HIER_BLOCK_HPP

#include <gras/element.hpp>

namespace gras
{

struct GRAS_API HierBlock : Element
{
    HierBlock(void);

    HierBlock(const std::string &name);

    void connect(const Element &elem);

    void disconnect(const Element &elem);

    void connect(
        const Element &src,
        const size_t src_index,
        const Element &sink,
        const size_t sink_index
    );

    void disconnect(
        const Element &src,
        const size_t src_index,
        const Element &sink,
        const size_t sink_index
    );

    void disconnect_all(void);

    /*!
     * Commit changes to the overall flow graph.
     * Call this after modifying connections.
     */
    virtual void commit(void);

    /*!
     * The lock() call is deprecated.
     * Topology can be changed duing design execution.
     * The underlying implementation is literally a NOP.
     */
    inline void lock(void){}

    /*!
     * The unlock() call is deprecated.
     * Topology can be changed duing design execution.
     * The underlying implementation is this->commit().
     */
    inline void unlock(void){this->commit();}

};

} //namespace gras

#endif /*INCLUDED_GRAS_HIER_BLOCK_HPP*/
