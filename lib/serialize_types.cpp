// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/sbuffer.hpp>
#include <gras/tags.hpp>
#include <PMC/Serialize.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/string.hpp>

/***********************************************************************
 * support for sbuffer
 **********************************************************************/
namespace boost { namespace serialization {
template<class Archive>
void save(Archive & ar, const gras::SBuffer & b, unsigned int version)
{
    bool null = not b;
    ar & null;
    if (null) return;

    //TODO lazyness string
    std::string s((const char *)b.get(), b.length);
    ar & s;
}
template<class Archive>
void load(Archive & ar, gras::SBuffer & b, unsigned int version)
{
    bool null = false;
    ar & null;
    if (null) b.reset();
    if (null) return;

    //TODO lazyness string
    std::string s;
    ar & s;
    gras::SBufferConfig config;
    config.length = s.length();
    b = gras::SBuffer(config);
    b.length = s.length();
    std::memcpy(b.get(), s.c_str(), b.length);
}
}}

BOOST_SERIALIZATION_SPLIT_FREE(gras::SBuffer)
PMC_SERIALIZE_EXPORT(gras::SBuffer, "PMC<gras::SBuffer>")


/***********************************************************************
 * support for special packet msg type
 **********************************************************************/
namespace boost { namespace serialization {
template <class Archive>
void serialize(Archive &ar, gras::PacketMsg &t, const unsigned int)
{
    ar & t.info;
    ar & t.buff;
}
}}

PMC_SERIALIZE_EXPORT(gras::PacketMsg, "PMC<gras::PacketMsg>")

/***********************************************************************
 * support for special stream tag type
 **********************************************************************/
namespace boost { namespace serialization {
template <class Archive>
void serialize(Archive &ar, gras::StreamTag &t, const unsigned int)
{
    ar & t.key;
    ar & t.val;
    ar & t.src;
}
}}

PMC_SERIALIZE_EXPORT(gras::StreamTag, "PMC<gras::StreamTag>")
