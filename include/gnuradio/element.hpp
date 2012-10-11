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

#ifndef INCLUDED_GNURADIO_ELEMENT_HPP
#define INCLUDED_GNURADIO_ELEMENT_HPP

#include <gnuradio/gras.hpp>
#include <gnuradio/io_signature.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace gnuradio
{

class ElementImpl;

struct GRAS_API Element : boost::shared_ptr<ElementImpl>, boost::enable_shared_from_this<Element>
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
        *this = elem->shared_to_element();
    }

    //! Convert a shared ptr of a derived class to an Element
    Element &shared_to_element(void);

    //! for internal use only
    boost::weak_ptr<Element> weak_self;

    //! An integer ID that is unique across the process
    long unique_id(void) const;

    //! Get the name of this element
    std::string name(void) const;

    //! get a canonical name for this element
    std::string to_string(void) const;

    void set_output_signature(const gnuradio::IOSignature &sig);

    void set_input_signature(const gnuradio::IOSignature &sig);

    const gnuradio::IOSignature &input_signature(void) const;

    const gnuradio::IOSignature &output_signature(void) const;

};

} //namespace gnuradio

#endif /*INCLUDED_GNURADIO_ELEMENT_HPP*/
