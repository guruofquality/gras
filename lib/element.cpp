// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras/element.hpp>
#include <boost/format.hpp>
#include <boost/detail/atomic_count.hpp>

static boost::detail::atomic_count unique_id_pool(0);

using namespace gras;

Element::Element(void)
{
    //NOP
}

Element::Element(const std::string &name)
{
    this->reset(new ElementImpl());
    (*this)->name = name;
    (*this)->unique_id = ++unique_id_pool;
    (*this)->id = str(boost::format("%s(%d)") % this->name() % this->unique_id());

    if (GENESIS) std::cerr << "New element: " << to_string() << std::endl;
}

ElementImpl::~ElementImpl(void)
{
    if (this->executor) this->top_block_cleanup();
    if (this->topology) this->hier_block_cleanup();
    if (this->block) this->block_cleanup();
}

long Element::unique_id(void) const
{
    return (*this)->unique_id;
}

std::string Element::name(void) const
{
    return (*this)->name;
}

std::string Element::to_string(void) const
{
    return (*this)->id;
}
