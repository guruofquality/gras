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

#ifndef INCLUDED_LIBGRAS_IMPL_VECTOR_UTILS_HPP
#define INCLUDED_LIBGRAS_IMPL_VECTOR_UTILS_HPP

//! return true if an instance was found and removed
template <typename V, typename T>
bool remove_one(V &v, const T &t)
{
    for (size_t i = 0; i < v.size(); i++)
    {
        if (v[i] == t)
        {
            v.erase(v.begin() + i);
            return true;
        }
    }
    return false;
}

template <typename V, typename T>
void resize_fill_grow(V &v, const size_t new_len, const T &fill)
{
    if (v.size() >= new_len) return; //dont ever shrink it
    v.resize(new_len, fill);
}

template <typename V>
void resize_fill_back(V &v, const size_t new_len)
{
    if (v.empty()) v.push_back(0);
    resize_fill_grow(v, new_len, v.back());
}

template <typename V, typename Sig>
void fill_item_sizes_from_sig(V &v, const Sig &s, const size_t size)
{
    v.resize(size);
    for (size_t i = 0; i < v.size(); i++)
    {
        v[i] = s->sizeof_stream_item(i);
    }
}

#endif /*INCLUDED_LIBGRAS_IMPL_VECTOR_UTILS_HPP*/
