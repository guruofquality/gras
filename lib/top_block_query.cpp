// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include "gras_impl/query_common.hpp"
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <Theron/DefaultAllocator.h>
#include <algorithm>
#include <set>

using namespace gras;

using namespace boost::property_tree;

struct GetStatsReceiver : Theron::Receiver
{
    GetStatsReceiver(void)
    {
        this->RegisterHandler(this, &GetStatsReceiver::handle_get_stats);
    }

    void handle_get_stats(const GetStatsMessage &message, const Theron::Address)
    {
        this->messages.push_back(message);
    }

    std::vector<GetStatsMessage> messages;
};

static ptree query_blocks(ElementImpl *self, const ptree &)
{
    ptree root;
    ptree e;
    BOOST_FOREACH(Apology::Worker *w, self->executor->get_workers())
    {
        BlockActor *actor = dynamic_cast<BlockActor *>(w->get_actor());
        ptree prop_e;
        typedef std::pair<std::string, PropertyRegistryPair> PropRegistryKVP;
        BOOST_FOREACH(const PropRegistryKVP &p, actor->data->property_registry)
        {
            ptree prop_attrs;
            if (p.second.setter)
            {
                ptree type;
                type.put_value(p.second.setter->type().name());
                prop_attrs.push_back(std::make_pair("setter", type));
            }
            if (p.second.getter)
            {
                ptree type;
                type.put_value(p.second.getter->type().name());
                prop_attrs.push_back(std::make_pair("getter", type));
            }
            ptree block_attrs;
            block_attrs.push_back(std::make_pair(p.first, prop_attrs));
            prop_e.push_back(std::make_pair("props", block_attrs));
        }
        e.push_back(std::make_pair(actor->data->block->get_uid(), prop_e));
    }
    root.push_back(std::make_pair("blocks", e));
    return root;
}

static ptree query_stats(ElementImpl *self, const ptree &query)
{
    //parse list of block ids needed in this query
    std::vector<std::string> block_ids;
    if (query.count("blocks") != 0)
    {
        BOOST_FOREACH(const ptree::value_type &v, query.get_child("blocks"))
        {
            block_ids.push_back(v.second.get_value<std::string>());
        }
    }

    //get stats with custom receiver and set high prio
    GetStatsReceiver receiver;
    size_t outstandingCount(0);
    BOOST_FOREACH(Apology::Worker *w, self->executor->get_workers())
    {
        BlockActor *actor = dynamic_cast<BlockActor *>(w->get_actor());

        //filter workers not needed in query
        const std::string id = actor->data->block->get_uid();
        if (std::find(block_ids.begin(), block_ids.end(), id) == block_ids.end()) continue;

        //send a message to the block's actor to query stats
        GetStatsMessage message;
        message.prio_token = actor->prio_token;
        actor->GetFramework().Send(message, receiver.GetAddress(), actor->GetAddress());
        outstandingCount++;
    }
    while (outstandingCount) outstandingCount -= receiver.Wait(outstandingCount);

    //create root level node
    ptree root;
    root.put("now", time_now());
    root.put("tps", time_tps());

    //allocator debugs
    Theron::DefaultAllocator *allocator = dynamic_cast<Theron::DefaultAllocator *>(Theron::AllocatorManager::Instance().GetAllocator());
    if (allocator)
    {
        root.put("default_allocator_bytes_allocated", allocator->GetBytesAllocated());
        root.put("default_allocator_peak_bytes_allocated", allocator->GetPeakBytesAllocated());
        root.put("default_allocator_allocation_count", allocator->GetAllocationCount());
    }

    //thread pool counts
    std::set<ThreadPool> thread_pools;
    BOOST_FOREACH(Apology::Worker *w, self->executor->get_workers())
    {
        BlockActor *actor = dynamic_cast<BlockActor *>(w->get_actor());
        thread_pools.insert(actor->thread_pool);
    }
    ptree tp_e;
    BOOST_FOREACH(const ThreadPool &tp, thread_pools)
    {
        ptree t;
        t.put("framework_counter_messages_processed", tp->GetCounterValue(Theron::COUNTER_MESSAGES_PROCESSED));
        t.put("framework_counter_yields", tp->GetCounterValue(Theron::COUNTER_YIELDS));
        t.put("framework_counter_local_pushes", tp->GetCounterValue(Theron::COUNTER_LOCAL_PUSHES));
        t.put("framework_counter_shared_pushes", tp->GetCounterValue(Theron::COUNTER_SHARED_PUSHES));
        t.put("framework_counter_mailbox_queue_max", tp->GetCounterValue(Theron::COUNTER_MAILBOX_QUEUE_MAX));
        tp_e.push_back(std::make_pair("", t));
    }
    root.push_back(std::make_pair("thread_pools", tp_e));

    //iterate through blocks
    ptree blocks;
    BOOST_FOREACH(const GetStatsMessage &message, receiver.messages)
    {
        const BlockStats &stats = message.stats;
        ptree block;
        block.put("tps", time_tps());
        block.put("stats_time", message.stats_time);
        block.put("init_time", stats.init_time);
        block.put("start_time", stats.start_time);
        block.put("stop_time", stats.stop_time);
        block.put("work_count", stats.work_count);
        block.put("time_last_work", stats.time_last_work);
        block.put("total_time_prep", stats.total_time_prep);
        block.put("total_time_work", stats.total_time_work);
        block.put("total_time_post", stats.total_time_post);
        block.put("total_time_input", stats.total_time_input);
        block.put("total_time_output", stats.total_time_output);
        block.put("actor_queue_depth", stats.actor_queue_depth);
        #define my_block_ptree_append(l) { \
            ptree e; \
            for (size_t i = 0; i < stats.l.size(); i++) { \
                ptree t; t.put_value(stats.l[i]); \
                e.push_back(std::make_pair("", t)); \
            } \
            block.push_back(std::make_pair(#l, e)); \
        }
        my_block_ptree_append(items_enqueued);
        my_block_ptree_append(tags_enqueued);
        my_block_ptree_append(msgs_enqueued);
        my_block_ptree_append(items_consumed);
        my_block_ptree_append(tags_consumed);
        my_block_ptree_append(msgs_consumed);
        my_block_ptree_append(items_produced);
        my_block_ptree_append(tags_produced);
        my_block_ptree_append(msgs_produced);
        my_block_ptree_append(bytes_copied);
        my_block_ptree_append(inputs_idle);
        my_block_ptree_append(outputs_idle);
        blocks.push_back(std::make_pair(message.block_id, block));
    }
    root.push_back(std::make_pair("blocks", blocks));
    return root;
}

static ptree query_props(ElementImpl *self, const ptree &query)
{
    ptree root;
    const std::string block_id = query.get<std::string>("block");
    const std::string prop_key = query.get<std::string>("key");
    const bool set = query.count("value") != 0;
    BOOST_FOREACH(Apology::Worker *w, self->executor->get_workers())
    {
        BlockActor *actor = dynamic_cast<BlockActor *>(w->get_actor());
        if (actor->data->block->get_uid() != block_id) continue;
        if (set)
        {
            const std::type_info &t = actor->data->property_registry[prop_key].setter->type();
            const PMCC p = ptree_to_pmc(query.get_child("value"), t);
            actor->prop_access_dispatcher(prop_key, p, true);
        }
        else
        {
            PMCC p = actor->prop_access_dispatcher(prop_key, PMC(), false);
            ptree v = pmc_to_ptree(p);
            root.push_back(std::make_pair("block", query.get_child("block")));
            root.push_back(std::make_pair("key", query.get_child("key")));
            root.push_back(std::make_pair("value", v));
        }
    }
    return root;
}

std::string TopBlock::query(const std::string &args)
{
    //convert json args into property tree
    const ptree query = json_to_ptree(args);

    //dispatch based on path arg
    std::string path = query.get<std::string>("path");
    ptree result;
    if (path == "/blocks.json") result = query_blocks(this->get(), query);
    if (path == "/stats.json") result = query_stats(this->get(), query);
    if (path == "/props.json") result = query_props(this->get(), query);
    return ptree_to_json(result);
}
