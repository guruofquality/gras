// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_LIBGRAS_IMPL_BLOCK_ACTOR_HPP
#define INCLUDED_LIBGRAS_IMPL_BLOCK_ACTOR_HPP

#include <gras_impl/debug.hpp>
#include <gras/block.hpp>
#include <gras/top_block.hpp>
#include <gras/thread_pool.hpp>
#include <Apology/Worker.hpp>
#include <gras_impl/messages.hpp>
#include <gras_impl/block_data.hpp>

namespace gras
{

struct BlockActor : Theron::Actor
{
    static BlockActor *make(const ThreadPool &tp = ThreadPool());
    BlockActor(const ThreadPool &tp);
    ~BlockActor(void);
    std::string name; //for debug
    ThreadPool thread_pool;
    Token prio_token;
    boost::shared_ptr<BlockData> data;
    Apology::Worker *worker;

    //do it here so we can match w/ the handler declarations
    void register_handlers(void)
    {
        this->RegisterHandler(this, &BlockActor::handle_topology);

        this->RegisterHandler(this, &BlockActor::handle_top_alloc);
        this->RegisterHandler(this, &BlockActor::handle_top_active);
        this->RegisterHandler(this, &BlockActor::handle_top_inert);
        this->RegisterHandler(this, &BlockActor::handle_top_token);
        this->RegisterHandler(this, &BlockActor::handle_top_config);
        this->RegisterHandler(this, &BlockActor::handle_top_thread_group);

        this->RegisterHandler(this, &BlockActor::handle_input_tag);
        this->RegisterHandler(this, &BlockActor::handle_input_msg);
        this->RegisterHandler(this, &BlockActor::handle_input_buffer);
        this->RegisterHandler(this, &BlockActor::handle_input_token);
        this->RegisterHandler(this, &BlockActor::handle_input_check);
        this->RegisterHandler(this, &BlockActor::handle_input_alloc);
        this->RegisterHandler(this, &BlockActor::handle_input_update);

        this->RegisterHandler(this, &BlockActor::handle_output_buffer);
        this->RegisterHandler(this, &BlockActor::handle_output_token);
        this->RegisterHandler(this, &BlockActor::handle_output_check);
        this->RegisterHandler(this, &BlockActor::handle_output_hint);
        this->RegisterHandler(this, &BlockActor::handle_output_alloc);
        this->RegisterHandler(this, &BlockActor::handle_output_update);

        this->RegisterHandler(this, &BlockActor::handle_callable);
        this->RegisterHandler(this, &BlockActor::handle_self_kick);
        this->RegisterHandler(this, &BlockActor::handle_get_stats);
    }

    //handlers
    void handle_topology(const Apology::WorkerTopologyMessage &, const Theron::Address);

    void handle_top_alloc(const TopAllocMessage &, const Theron::Address);
    void handle_top_active(const TopActiveMessage &, const Theron::Address);
    void handle_top_inert(const TopInertMessage &, const Theron::Address);
    void handle_top_token(const TopTokenMessage &, const Theron::Address);
    void handle_top_config(const TopConfigMessage &, const Theron::Address);
    void handle_top_thread_group(const TopThreadMessage &, const Theron::Address);

    void handle_input_tag(const InputTagMessage &, const Theron::Address);
    void handle_input_msg(const InputMsgMessage &, const Theron::Address);
    void handle_input_buffer(const InputBufferMessage &, const Theron::Address);
    void handle_input_token(const InputTokenMessage &, const Theron::Address);
    void handle_input_check(const InputCheckMessage &, const Theron::Address);
    void handle_input_alloc(const InputAllocMessage &, const Theron::Address);
    void handle_input_update(const InputUpdateMessage &, const Theron::Address);

    void handle_output_buffer(const OutputBufferMessage &, const Theron::Address);
    void handle_output_token(const OutputTokenMessage &, const Theron::Address);
    void handle_output_check(const OutputCheckMessage &, const Theron::Address);
    void handle_output_hint(const OutputHintMessage &, const Theron::Address);
    void handle_output_alloc(const OutputAllocMessage &, const Theron::Address);
    void handle_output_update(const OutputUpdateMessage &, const Theron::Address);

    void handle_callable(const CallableMessage &, const Theron::Address);
    void handle_self_kick(const SelfKickMessage &, const Theron::Address);
    void handle_get_stats(const GetStatsMessage &, const Theron::Address);

    //helpers
    void mark_done(void);
    void task_main(void);
    void input_fail(const size_t index);
    void output_fail(const size_t index);
    void produce(const size_t index, const size_t items);
    void consume(const size_t index, const size_t items);
    void task_kicker(void);
    void update_input_avail(const size_t index);
    bool is_work_allowed(void);

    //work helpers
    inline void task_work(void)
    {
        data->block->work(data->input_items, data->output_items);
    }
};

//-------------- common functions from this BlockActor class ---------//

GRAS_FORCE_INLINE void BlockActor::task_kicker(void)
{
    if (this->is_work_allowed()) this->Send(SelfKickMessage(), this->GetAddress());
}

GRAS_FORCE_INLINE void BlockActor::update_input_avail(const size_t i)
{
    const bool has_input_bufs = not data->input_queues.empty(i) and data->input_queues.ready(i);
    const bool has_input_msgs = not data->input_msgs[i].empty();
    data->inputs_available.set(i, has_input_bufs or has_input_msgs);
    data->input_queues.update_has_msg(i, has_input_msgs);
}

GRAS_FORCE_INLINE bool BlockActor::is_work_allowed(void)
{
    return (
        this->prio_token.unique() and
        data->block_state == BLOCK_STATE_LIVE and
        data->inputs_available.any() and
        data->input_queues.all_ready() and
        data->output_queues.all_ready()
    );
}

} //namespace gras

#endif /*INCLUDED_LIBGRAS_IMPL_BLOCK_ACTOR_HPP*/
