// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/sbuffer.hpp>
#include <gras/tags.hpp>
#include <gras/time_tag.hpp>
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
    //save null
    bool null = not b;
    ar & null;
    if (null) return;

    //save length
    size_t length = b.length;
    ar & length;

    //save bytes
    const char *ptr = reinterpret_cast<const char *>(b.get(0));
    for (size_t i = 0; i < length; i++) ar & (ptr[i]);
}
template<class Archive>
void load(Archive & ar, gras::SBuffer & b, unsigned int version)
{
    //load null
    bool null = false;
    ar & null;
    if (null) b.reset();
    if (null) return;

    //load length
    size_t length = 0;
    ar & length;

    //alloc sbuffer
    gras::SBufferConfig config;
    config.length = length;
    b = gras::SBuffer(config);

    //load bytes
    char *ptr = reinterpret_cast<char *>(b.get(0));
    for (size_t i = 0; i < length; i++) ar & (ptr[i]);
}
}}

BOOST_SERIALIZATION_SPLIT_FREE(gras::SBuffer)
PMC_SERIALIZE_EXPORT(gras::SBuffer, "PMC<gras::SBuffer>")

/***********************************************************************
 * support for tag type
 **********************************************************************/
namespace boost { namespace serialization {
template <class Archive>
void serialize(Archive &ar, gras::Tag &t, const unsigned int)
{
    ar & t.offset;
    ar & t.object;
}
}}

PMC_SERIALIZE_EXPORT(gras::Tag, "PMC<gras::Tag>")

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

/***********************************************************************
 * support for time tag type
 **********************************************************************/
namespace boost { namespace serialization {
template <class Archive>
void serialize(Archive &ar, gras::TimeTag &t, const unsigned int)
{
    ar & t._fsecs;
    ar & t._ticks;
}
}}

PMC_SERIALIZE_EXPORT(gras::TimeTag, "PMC<gras::TimeTag>")
