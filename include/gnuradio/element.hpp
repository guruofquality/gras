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

#ifndef INCLUDED_GNURADIO_ELEMENT_HPP
#define INCLUDED_GNURADIO_ELEMENT_HPP

#include <gnuradio/runtime_api.h>
#include <gruel/pmt.h>
#include <gnuradio/io_signature.hpp>

namespace gnuradio
{

struct GR_RUNTIME_API Element : boost::shared_ptr<ElementImpl>
{

    //! Create an empty element
    Element(void);

    Element(const std::string &name);

    /*!
     * Create an element from a shared pointer to an element.
     * Good for that factory function/shared ptr paradigm.
     */
    template <typename T>
    Element(const boost::shared_ptr<T> &elem)
    {
        *this = *elem;
    }

    long unique_id(void) const;

    std::string name(void) const;

    void set_output_signature(const gnuradio::IOSignature &sig);

    void set_input_signature(const gnuradio::IOSignature &sig);

    const gnuradio::IOSignature &input_signature(void) const;

    const gnuradio::IOSignature &output_signature(void) const;

};

} //namespace gnuradio

#endif /*INCLUDED_GNURADIO_ELEMENT_HPP*/
