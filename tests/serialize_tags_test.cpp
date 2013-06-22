// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <boost/test/unit_test.hpp>
#include <algorithm>
#include <iostream>

#include <PMC/PMC.hpp>
#include <PMC/Serialize.hpp>
#include <gras/tags.hpp>
#include <gras/time_tag.hpp>

#include <cstdlib>

static PMCC loopback_test(PMCC p0)
{
    std::cout << "\ndoing loopback test on " << p0 << std::endl;
    const std::string data = PMC::serialize(p0, "TEXT");

    std::cout << "serialized object " << data << std::endl;
    PMCC p1 = PMC::deserialize(data, "TEXT");

    return p1;
}

static gras::SBuffer get_random_sbuff(void)
{
    gras::SBufferConfig config;
    config.length = 100;
    gras::SBuffer buff(config);
    for (size_t i = 0; i < buff.length/sizeof(long); i++)
    {
        reinterpret_cast<long *>(buff.get())[i] = std::rand();
    }
    return buff;
}

BOOST_AUTO_TEST_CASE(test_sbuffer_type)
{
    gras::SBuffer src_buff = get_random_sbuff();
    PMCC result = loopback_test(PMC_M(src_buff));
    const gras::SBuffer &result_buff = result.as<gras::SBuffer>();

    BOOST_CHECK_EQUAL(src_buff.length, result_buff.length);
    BOOST_CHECK(std::memcmp(src_buff.get(), result_buff.get(), src_buff.length) == 0);
}

BOOST_AUTO_TEST_CASE(test_null_sbuffer_type)
{
    gras::SBuffer src_buff;
    PMCC result = loopback_test(PMC_M(src_buff));
    const gras::SBuffer &result_buff = result.as<gras::SBuffer>();
    BOOST_CHECK(not src_buff);
    BOOST_CHECK(not result_buff);
}

BOOST_AUTO_TEST_CASE(test_pkt_msg_type)
{
    gras::PacketMsg pkt_msg;
    pkt_msg.buff = get_random_sbuff();
    pkt_msg.info = PMC_M(long(42));

    PMCC result = loopback_test(PMC_M(pkt_msg));
    const gras::PacketMsg &result_msg = result.as<gras::PacketMsg>();

    BOOST_CHECK_EQUAL(pkt_msg.info.type().name(), result_msg.info.type().name());
    BOOST_CHECK(pkt_msg.info.eq(result_msg.info));
    BOOST_CHECK(std::memcmp(pkt_msg.buff.get(), result_msg.buff.get(), pkt_msg.buff.length) == 0);
}

BOOST_AUTO_TEST_CASE(test_time_tag_type)
{
    const gras::TimeTag t0 = gras::TimeTag::from_ticks(42);
    PMCC result = loopback_test(PMC_M(t0));
    const gras::TimeTag &t1 = result.as<gras::TimeTag>();
    BOOST_CHECK(t0 == t1);
}
