// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras/top_block.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

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

    //now format the xml result
    std::string xml;
    xml += str(boost::format("  <now>%llu</now>\n") % time_now());
    xml += str(boost::format("  <tps>%llu</tps>\n") % time_tps());
    BOOST_FOREACH(const GetStatsMessage &message, receiver.messages)
    {
        const BlockStats &stats = message.stats;
        std::string block_xml;
        block_xml += str(boost::format("    <tps>%llu</tps>\n") % time_tps());
        block_xml += str(boost::format("    <stats_time>%llu</stats_time>\n") % message.stats_time);
        block_xml += str(boost::format("    <init_time>%llu</init_time>\n") % stats.init_time);
        block_xml += str(boost::format("    <start_time>%llu</start_time>\n") % stats.start_time);
        block_xml += str(boost::format("    <stop_time>%llu</stop_time>\n") % stats.stop_time);
        block_xml += str(boost::format("    <work_count>%llu</work_count>\n") % stats.work_count);
        block_xml += str(boost::format("    <time_last_work>%llu</time_last_work>\n") % stats.time_last_work);
        block_xml += str(boost::format("    <total_time_prep>%llu</total_time_prep>\n") % stats.total_time_prep);
        block_xml += str(boost::format("    <total_time_work>%llu</total_time_work>\n") % stats.total_time_work);
        block_xml += str(boost::format("    <total_time_post>%llu</total_time_post>\n") % stats.total_time_post);
        block_xml += str(boost::format("    <total_time_input>%llu</total_time_input>\n") % stats.total_time_input);
        block_xml += str(boost::format("    <total_time_output>%llu</total_time_output>\n") % stats.total_time_output);
        for (size_t i = 0; i < stats.items_consumed.size(); i++)
        {
            block_xml += str(boost::format("    <items_consumed>%llu</items_consumed>\n") % stats.items_consumed[i]);
        }
        for (size_t i = 0; i < stats.tags_consumed.size(); i++)
        {
            block_xml += str(boost::format("    <tags_consumed>%llu</tags_consumed>\n") % stats.tags_consumed[i]);
        }
        for (size_t i = 0; i < stats.msgs_consumed.size(); i++)
        {
            block_xml += str(boost::format("    <msgs_consumed>%llu</msgs_consumed>\n") % stats.msgs_consumed[i]);
        }
        for (size_t i = 0; i < stats.items_produced.size(); i++)
        {
            block_xml += str(boost::format("    <items_produced>%llu</items_produced>\n") % stats.items_produced[i]);
        }
        for (size_t i = 0; i < stats.tags_produced.size(); i++)
        {
            block_xml += str(boost::format("    <tags_produced>%llu</tags_produced>\n") % stats.tags_produced[i]);
        }
        for (size_t i = 0; i < stats.msgs_produced.size(); i++)
        {
            block_xml += str(boost::format("    <msgs_produced>%llu</msgs_produced>\n") % stats.msgs_produced[i]);
        }
        xml += str(boost::format("  <block id=\"%s\">\n%s</block>\n") % message.block_id % block_xml);
    }
    return str(boost::format("<gras_stats id=\"%s\">\n%s</gras_stats>") % this->to_string() % xml);
}
