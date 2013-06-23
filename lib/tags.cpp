// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/tags.hpp>

using namespace gras;

Tag::Tag(const item_index_t &offset, const PMCC &object):
    offset(offset), object(object)
{
    //NOP
}

bool gras::operator<(const Tag &lhs, const Tag &rhs)
{
    return lhs.offset < rhs.offset;
}

bool gras::operator==(const Tag &lhs, const Tag &rhs)
{
    return
        lhs.offset == rhs.offset and
        lhs.object.eq(rhs.object);
}

StreamTag::StreamTag(const PMCC &key, const PMCC &val, const PMCC &src):
    key(key), val(val), src(src)
{
    //NOP
}

bool gras::operator==(const StreamTag &lhs, const StreamTag &rhs)
{
    return
        lhs.key.eq(rhs.key) and
        lhs.val.eq(rhs.val) and
        lhs.src.eq(rhs.src);
}

PacketMsg::PacketMsg(const PMCC &info, const SBuffer &buff):
    info(info), buff(buff)
{
    //NOP
}

PacketMsg::PacketMsg(const SBuffer &buff):
    buff(buff)
{
    //NOP
}

bool gras::operator==(const PacketMsg &lhs, const PacketMsg &rhs)
{
    return lhs.info.eq(rhs.info) and lhs.buff == rhs.buff;
}
