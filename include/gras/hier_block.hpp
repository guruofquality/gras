// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_HIER_BLOCK_HPP
#define INCLUDED_GRAS_HIER_BLOCK_HPP

#include <gras/element.hpp>

namespace gras
{

struct GRAS_API HierBlock : Element
{
    HierBlock(void);

    HierBlock(const std::string &name);

    virtual ~HierBlock(void);

    /*!
     * Commit changes to the global configuration.
     * Must be call to apply changes to the global config.
     */
    void commit_config(void);

    /*******************************************************************
     * connection flow interface
     ******************************************************************/

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

};

} //namespace gras

#endif /*INCLUDED_GRAS_HIER_BLOCK_HPP*/
