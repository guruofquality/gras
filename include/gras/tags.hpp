// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_GRAS_TAGS_HPP
#define INCLUDED_GRAS_TAGS_HPP

#include <gras/gras.hpp>
#include <gras/sbuffer.hpp>
#include <boost/operators.hpp>
#include <PMC/PMC.hpp>

namespace gras
{

/*!
 * A Tag is a combination of absolute item count and associated object.
 *
 * When used with an item stream, a tag is used to decorate a stream item.
 * The offset associates the object with the absolute position of an item.
 *
 * When used for message passing, only the object is used.
 * The offset should always be set to zero.
 */
struct GRAS_API Tag : boost::less_than_comparable<Tag>
{
    //! Make a tag from parameters to initialize the members
    Tag
    (
        const item_index_t &offset = 0,
        const PMCC &value = PMCC()
    );

    //! the absolute item count associated with this tag
    item_index_t offset;

    /*!
     * The object contained in this tag, which could be anything.
     * Common types for this value are StreamTag and PacketMsg
     */
    PMCC object;
};

GRAS_API bool operator<(const Tag &lhs, const Tag &rhs);
GRAS_API bool operator==(const Tag &lhs, const Tag &rhs);

/*!
 * A stream tag is a commonly used structure
 * used to decorate a stream item with metadata.
 */
struct GRAS_API StreamTag
{
    //! Initialization constructor for stream tag
    StreamTag
    (
        const PMCC &key = PMCC(),
        const PMCC &val = PMCC(),
        const PMCC &src = PMCC()
    );

    //! A symbolic name identifying the type of tag
    PMCC key;

    //! The value of this tag -> the sample metadata
    PMCC val;

    //! The optional source ID -> something unique
    PMCC src;
};

GRAS_API bool operator==(const StreamTag &lhs, const StreamTag &rhs);

/*!
 * A packet message is a commonly used structure
 * used to pass packet-based data between blocks.
 */
struct GRAS_API PacketMsg
{
    //! Initialization constructor for packet message
    PacketMsg
    (
        const PMCC &info = PMCC(),
        const SBuffer &buff = SBuffer()
    );

    //! Convenience contructor to initialize from buffer
    PacketMsg(const SBuffer &buff);

    //! Metadata info associated with this packet
    PMCC info;

    //! The raw bytes of the packet - ref counted buffer
    SBuffer buff;
};

GRAS_API bool operator==(const PacketMsg &lhs, const PacketMsg &rhs);

} //namespace gras

#endif /*INCLUDED_GRAS_TAGS_HPP*/
