// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras/top_block.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/regex.hpp>
#include <algorithm>
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

//http://stackoverflow.com/questions/13464383/boost-property-write-json-incorrect-behaviour
static std::string my_write_json(const boost::property_tree::ptree &pt)
{
    boost::regex exp("\"(null|true|false|[0-9]+(\\.[0-9]+)?)\"");
    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, pt);
    std::string rv = boost::regex_replace(ss.str(), exp, "$1");

    return rv;
}

static std::string query_blocks(ElementImpl *self, const boost::property_tree::ptree &)
{
    boost::property_tree::ptree root;
    boost::property_tree::ptree e;
    BOOST_FOREACH(Apology::Worker *worker, self->executor->get_workers())
    {
        boost::property_tree::ptree t;
        t.put_value(dynamic_cast<BlockActor *>(worker)->block_ptr->to_string());
        e.push_back(std::make_pair("", t));
    }
    root.push_back(std::make_pair("blocks", e));
    return my_write_json(root);
}

static std::string query_stats(ElementImpl *self, const boost::property_tree::ptree &query)
{

    //parse list of block ids needed in this query
    std::vector<std::string> block_ids;
    BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, query.get_child("args"))
    {
        if (v.first.data() == std::string("block"))
        {
            block_ids.push_back(v.second.get<std::string>(""));
        }
    }

    //get stats with custom receiver and set high prio
    GetStatsReceiver receiver;
    size_t outstandingCount(0);
    BOOST_FOREACH(Apology::Worker *worker, self->executor->get_workers())
    {
        //filter workers not needed in query
        const std::string id = dynamic_cast<BlockActor *>(worker)->block_ptr->to_string();
        if (std::find(block_ids.begin(), block_ids.end(), id) == block_ids.end()) continue;

        //send a message to the block's actor to query stats
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
    boost::property_tree::ptree blocks;
    BOOST_FOREACH(const GetStatsMessage &message, receiver.messages)
    {
        const BlockStats &stats = message.stats;
        boost::property_tree::ptree block;
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
        #define my_block_ptree_append(l) { \
            boost::property_tree::ptree e; \
            for (size_t i = 0; i < stats.l.size(); i++) { \
                boost::property_tree::ptree t; t.put_value(stats.l[i]); \
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
        blocks.push_back(std::make_pair(message.block_id, block));
    }
    root.push_back(std::make_pair("blocks", blocks));

    return my_write_json(root);
}

std::string TopBlock::query(const std::string &args)
{
    //why the fuck does no OS ever patch boost when there is a bug
    //https://svn.boost.org/trac/boost/ticket/6785
    //serialize the path args into xml -- but I just wanted json
    std::stringstream query_args_ss(args);
    boost::property_tree::ptree query_args_pt;
    boost::property_tree::xml_parser::read_xml(query_args_ss, query_args_pt);

    //dispatch based on path arg
    std::string path = query_args_pt.get<std::string>("args.path");
    if (path == "/blocks.json") return query_blocks(this->get(), query_args_pt);
    if (path == "/stats.json") return query_stats(this->get(), query_args_pt);
    return "";
}
