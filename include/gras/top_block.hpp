// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_TOP_BLOCK_HPP
#define INCLUDED_GRAS_TOP_BLOCK_HPP

#include <gras/hier_block.hpp>

namespace gras
{

struct GRAS_API GlobalBlockConfig
{
    GlobalBlockConfig(void);

    /*!
     * Constrain the maximum number of items that
     * work can be called with for all output ports.
     *
     * Default = 0 aka disabled.
     */
    size_t maximum_output_items;

    /*!
     * Set the global memory node affinity.
     * Blocks that have not been explicitly set,
     * will take on this new buffer_affinity.
     *
     * Default = -1 aka no affinity.
     */
    long buffer_affinity;
};

struct GRAS_API TopBlock : HierBlock
{
    TopBlock(void);

    TopBlock(const std::string &name);

    //! Get the global block config settings
    GlobalBlockConfig global_config(void) const;

    //! Set the global block config settings
    void set_global_config(const GlobalBlockConfig &config);

    /*!
     * Commit changes to the overall flow graph.
     * Call this after modifying connections.
     * Commit is called automatically by start/stop/run.
     */
    void commit(void);

    /*!
     * Run is for finite flow graph executions.
     * Mostly for testing purposes only.
     */
    void run(void);

    //! Start a flow graph execution (does not block)
    virtual void start(void);

    //! Stop a flow graph execution (does not block)
    virtual void stop(void);

    /*!
     * Wait for threads to exit after stop() or run().
     * This is a blocking call and will not return until
     * all blocks in the topology have been marked done.
     */
    virtual void wait(void);

    /*!
     * Wait for threads to exit after stop() or run().
     * This is call will block until timeout or done.
     *
     * \param timeout the timeout in seconds
     * \return true of execution completed
     */
    virtual bool wait(const double timeout);

    //! Deprecated
    void start(const size_t max_items);

    //! Deprecated
    void run(const size_t max_items);

    //! Deprecated
    int max_noutput_items(void) const;

    //! Deprecated
    void set_max_noutput_items(int max_items);

};

} //namespace gras

#endif /*INCLUDED_GRAS_TOP_BLOCK_HPP*/
