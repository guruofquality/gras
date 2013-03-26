// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras/top_block.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <sstream>

using namespace gras;

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

std::string TopBlock::query(const std::string &)
{
    //get stats with custom receiver and set high prio
    GetStatsReceiver receiver;
    size_t outstandingCount(0);
    BOOST_FOREACH(Apology::Worker *worker, (*this)->executor->get_workers())
    {
        dynamic_cast<BlockActor *>(worker)->highPrioPreNotify();
        worker->Push(GetStatsMessage(), receiver.GetAddress());
        outstandingCount++;
    }
    while (outstandingCount) outstandingCount -= receiver.Wait(outstandingCount);

    //create root level node
    boost::property_tree::ptree root;
    root.put("now", time_now());
    root.put("tps", time_tps());

    //iterate through blocks
    BOOST_FOREACH(const GetStatsMessage &message, receiver.messages)
    {
        const BlockStats &stats = message.stats;
        boost::property_tree::ptree block;
        block.put("<xmlattr>.id", message.block_id);
        block.put("id", message.block_id);
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
        #define my_block_ptree_append(l) \
        for (size_t i = 0; i < stats.l.size(); i++) { \
            boost::property_tree::ptree t; t.put_value(stats.l[i]); \
            block.push_back(std::make_pair(#l, t)); \
        }
        my_block_ptree_append(items_consumed);
        my_block_ptree_append(tags_consumed);
        my_block_ptree_append(msgs_consumed);
        my_block_ptree_append(items_produced);
        my_block_ptree_append(tags_produced);
        my_block_ptree_append(msgs_produced);
        root.push_back(std::make_pair("block", block));
    }

    //create top tag and write xml
    boost::property_tree::ptree top;
    top.put_child("gras_stats", root);
    std::stringstream ss;
    write_xml(ss, top);
    return ss.str();
}
