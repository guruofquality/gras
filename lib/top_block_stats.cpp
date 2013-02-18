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

std::string TopBlock::get_stats_xml(void)
{
    GetStatsReceiver receiver;
    (*this)->executor->post_all(GetStatsMessage(), receiver);
    std::string xml;
    xml += str(boost::format("  <now>%ull</now>\n") % time_now());
    xml += str(boost::format("  <tps>%ull</tps>\n") % time_tps());
    BOOST_FOREACH(const GetStatsMessage &message, receiver.messages)
    {
        const BlockStats &stats = message.stats;
        std::string block_xml;
        block_xml += str(boost::format("    <id>%s</id>\n") % message.block_id);
        block_xml += str(boost::format("    <start_time>%llu</start_time>\n") % stats.start_time);
        block_xml += str(boost::format("    <stop_time>%llu</stop_time>\n") % stats.stop_time);
        block_xml += str(boost::format("    <work_count>%llu</work_count>\n") % stats.work_count);
        block_xml += str(boost::format("    <time_last_work>%llu</time_last_work>\n") % stats.time_last_work);
        block_xml += str(boost::format("    <total_time_work>%llu</total_time_work>\n") % stats.total_time_work);
        block_xml += str(boost::format("    <total_time_work_other>%llu</total_time_work_other>\n") % stats.total_time_work_other);
        for (size_t i = 0; i < stats.items_consumed.size(); i++)
        {
            block_xml += str(boost::format("    <input_items_consumed>%llu</input_items_consumed>\n") % stats.items_consumed[i]);
        }
        for (size_t i = 0; i < stats.tags_consumed.size(); i++)
        {
            block_xml += str(boost::format("    <tags_consumed>%llu</tags_consumed>\n") % stats.tags_consumed[i]);
        }
        for (size_t i = 0; i < stats.items_produced.size(); i++)
        {
            block_xml += str(boost::format("    <items_produced>%llu</items_produced>\n") % stats.items_produced[i]);
        }
        for (size_t i = 0; i < stats.tags_produced.size(); i++)
        {
            block_xml += str(boost::format("    <tags_produced>%llu</tags_produced>\n") % stats.tags_produced[i]);
        }
        xml += str(boost::format("  <block>\n%s</block>\n") % block_xml);
    }
    return str(boost::format("<gras_stats>\n%s</gras_stats>") % xml);
}
