//
// Copyright 2012 Josh Blum
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef INCLUDED_LIBGRAS_TAG_HANDLERS_HPP
#define INCLUDED_LIBGRAS_TAG_HANDLERS_HPP

#include <gras_impl/block_actor.hpp>
#include <algorithm>

namespace gras
{

GRAS_FORCE_INLINE void BlockActor::sort_tags(const size_t i)
{
    if (not this->input_tags_changed[i]) return;
    std::vector<Tag> &tags_i = this->input_tags[i];
    std::sort(tags_i.begin(), tags_i.end());
    this->input_tags_changed[i] = false;
}

GRAS_FORCE_INLINE void BlockActor::trim_tags(const size_t i)
{
    //------------------------------------------------------------------
    //-- trim the input tags that are past the consumption zone
    //-- and post trimmed tags to the downstream based on policy
    //------------------------------------------------------------------

    std::vector<Tag> &tags_i = this->input_tags[i];
    const size_t items_consumed_i = this->items_consumed[i];
    size_t last = 0;
    while (last < tags_i.size() and tags_i[last].offset < items_consumed_i)
    {
        last++;
    }

    if (last == 0) return;

    //call the overloaded propagate_tags to do the dirty work
    this->block_ptr->propagate_tags(i, boost::make_iterator_range(tags_i.begin(), tags_i.begin()+last));

    //now its safe to perform the erasure
    tags_i.erase(tags_i.begin(), tags_i.begin()+last);
}

} //namespace gras

#endif /*INCLUDED_LIBGRAS_TAG_HANDLERS_HPP*/
