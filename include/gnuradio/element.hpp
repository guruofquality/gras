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

#include <gruel/pmt.h>
#include <gnuradio/io_signature.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

//this is part of core for now, treat it as such
#include <gr_core_api.h>
#define GR_RUNTIME_API GR_CORE_API

namespace gnuradio
{

struct ElementImpl;

struct GR_RUNTIME_API Element : boost::shared_ptr<ElementImpl>
{

    //! Create an empty element
    Element(void);

    //! Creates a new element given the name
    Element(const std::string &name);

    /*!
     * Create an element from a shared pointer to an element.
     * Good for that factory function/shared ptr paradigm.
     */
    template <typename T>
    Element(const boost::shared_ptr<T> &elem)
    {
        *this = *elem;
        weak_self = elem;
    }

    //! Get the derived class as an element
    const Element &get_base(void) const;

    //! for internal use only
    boost::weak_ptr<Element> weak_self;

    //! An integer ID that is unique across the process
    long unique_id(void) const;

    //! Get the name of this element
    std::string name(void) const;

    void set_output_signature(const gnuradio::IOSignature &sig);

    void set_input_signature(const gnuradio::IOSignature &sig);

    const gnuradio::IOSignature &input_signature(void) const;

    const gnuradio::IOSignature &output_signature(void) const;

};

} //namespace gnuradio

#endif /*INCLUDED_GNURADIO_ELEMENT_HPP*/
