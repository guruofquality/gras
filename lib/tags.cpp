// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/tags.hpp>

using namespace gras;

Tag::Tag(void):
    offset(0)
{
    //NOP
}

Tag::Tag(const item_index_t &offset, const PMCC &key, const PMCC &value, const PMCC &srcid):
    offset(offset), key(key), value(value), srcid(srcid)
{
    //NOP
}

bool gras::operator<(const Tag &lhs, const Tag &rhs)
{
    return lhs.offset < rhs.offset;
}
