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
// along with io_sig program.  If not, see <http://www.gnu.org/licenses/>.

#include "element_impl.hpp"
#include <boost/foreach.hpp>
#include <algorithm>

using namespace gnuradio;

void ElementImpl::handle_task(const tsbe::TaskInterface &task_iface)
{
    //------------------------------------------------------------------
    //-- Decide if its possible to continue any processing:
    //-- Handle task may get called for incoming buffers,
    //-- however, not all ports may have available buffers.
    //------------------------------------------------------------------
    const bool all_inputs_ready = (~task_iface.get_inputs_ready()).none();
    const bool all_outputs_ready = (~task_iface.get_outputs_ready()).none();
    if (not (this->active and all_inputs_ready and all_outputs_ready)) return;

    const size_t num_inputs = task_iface.get_num_inputs();
    const size_t num_outputs = task_iface.get_num_outputs();
    const bool is_source = (num_inputs == 0);

    //------------------------------------------------------------------
    //-- sort the input tags before working
    //------------------------------------------------------------------
    for (size_t i = 0; i < num_inputs; i++)
    {
        if (not this->input_tags_changed[i]) continue;
        std::vector<Tag> &tags_i = this->input_tags[i];
        std::sort(tags_i.begin(), tags_i.end(), Tag::offset_compare);
        this->input_tags_changed[i] = false;
    }

    //------------------------------------------------------------------
    //-- Processing time!
    //------------------------------------------------------------------

    HERE();


    //0) figure out what we have for input data
    //1) calculate the possible num output items
    //2) take into account the item multiple
    //3) allocate some buffers
    //4) work....

    //block_ptr->forecast(100


    //TODO set deactive when work returns DONE

    //TODO source blocks should call work again until exhausted output buffers

    //------------------------------------------------------------------
    //-- trim the input tags that are past the consumption zone
    //-- and post trimmed tags to the downstream based on policy
    //------------------------------------------------------------------
    for (size_t i = 0; i < num_inputs; i++)
    {
        std::vector<Tag> &tags_i = this->input_tags[i];
        const size_t items_consumed_i = this->items_consumed[i];
        size_t last = 0;
        while (last < tags_i.size() and tags_i[last].offset < items_consumed_i)
        {
            last++;
        }

        //follow the tag propagation policy before erasure
        switch (this->tag_prop_policy)
        {
        case Block::TPP_DONT: break; //well that was ez
        case Block::TPP_ALL_TO_ALL:
            for (size_t out_i = 0; out_i < num_outputs; out_i++)
            {
                for (size_t tag_i = 0; tag_i < last; tag_i++)
                {
                    Tag t = tags_i[tag_i];
                    t.offset *= this->relative_rate;
                    task_iface.post_downstream(out_i, t);
                }
            }
            break;
        case Block::TPP_ONE_TO_ONE:
            if (i < num_outputs)
            {
                for (size_t tag_i = 0; tag_i < last; tag_i++)
                {
                    Tag t = tags_i[tag_i];
                    t.offset *= this->relative_rate;
                    task_iface.post_downstream(i, t);
                }
            }
            break;
        };

        //now its safe to perform the erasure
        if (last != 0) tags_i.erase(tags_i.begin(), tags_i.begin()+last);
    }

    //------------------------------------------------------------------
    //-- now commit all tags in the output queue to the downstream
    //------------------------------------------------------------------
    for (size_t i = 0; i < num_outputs; i++)
    {
        BOOST_FOREACH(const Tag &t, this->output_tags[i])
        {
            task_iface.post_downstream(i, t);
        }
        this->output_tags[i].clear();
    }
}
