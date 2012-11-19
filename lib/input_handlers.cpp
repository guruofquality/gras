// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras_impl/block_actor.hpp>
#include <boost/foreach.hpp>

using namespace gras;

void BlockActor::handle_input_tag(const InputTagMessage &message, const Theron::Address)
{
    MESSAGE_TRACER();
    const size_t index = message.index;

    //handle incoming stream tag, push into the tag storage
    this->input_tags[index].push_back(message.tag);
    this->input_tags_changed[index] = true;
}

void BlockActor::handle_input_buffer(const InputBufferMessage &message, const Theron::Address)
{
    MESSAGE_TRACER();
    const size_t index = message.index;

    //handle incoming stream buffer, push into the queue
    if (this->block_state == BLOCK_STATE_DONE) return;
    this->input_queues.push(index, message.buffer);
    this->handle_task();
}

void BlockActor::handle_input_token(const InputTokenMessage &message, const Theron::Address)
{
    MESSAGE_TRACER();
    ASSERT(message.index < this->get_num_inputs());

    //store the token of the upstream producer
    this->token_pool.insert(message.token);
}

void BlockActor::handle_input_check(const InputCheckMessage &message, const Theron::Address)
{
    MESSAGE_TRACER();
    const size_t index = message.index;

    //an upstream block declared itself done, recheck the token
    this->inputs_done.set(index, this->input_tokens[index].unique());
    if (this->is_input_done(index)) //missing an upstream provider
    {
        this->mark_done();
    }
    //or re-enter handle task so fail logic can mark done
    else
    {
        this->handle_task();
    }
}

void BlockActor::handle_input_alloc(const InputAllocMessage &message, const Theron::Address)
{
    MESSAGE_TRACER();
    const size_t index = message.index;

    //handle the upstream block allocation request
    OutputAllocMessage new_msg;
    new_msg.token = block_ptr->input_buffer_allocator(
        index, message.token, message.recommend_length
    );
    if (new_msg.token) this->post_upstream(index, new_msg);
}
