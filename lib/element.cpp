// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras/element.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <cstdlib>

using namespace gras;

Element::Element(void)
{
    //NOP
}

Element::Element(const std::string &name)
{
    this->reset(new ElementImpl());
    (*this)->name = name;
    (*this)->self = this;
    size_t which = 0;
    while (true)
    {
        const std::string uid = str(boost::format("%s_%u") % name % which);
        try
        {
            this->set_uid(uid);
            break;
        }
        catch(const std::invalid_argument &)
        {
            which++;
        }
    }
    (*this)->repr = str(boost::format("%s (%u)") % name % which);

    if (GENESIS) std::cerr
        << "===================================================\n"
        << "== Element Created: " << to_string() << "\n"
        << "===================================================\n"
        << std::flush;
}

Element::~Element(void)
{
    //NOP
}

const Element &Element::to_element(void) const
{
    return *this;
}

Element &Element::to_element(void)
{
    return *this;
}

ElementImpl::~ElementImpl(void)
{
    if (this->executor) this->top_block_cleanup();
    if (this->topology) this->hier_block_cleanup();
    if (this->worker) this->block_cleanup();
    if (ARMAGEDDON) std::cerr
        << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
        << "xx Element Destroyed: " << name << "\n"
        << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
        << std::flush;
}

void Element::set_container(WeakContainer *container)
{
    (*this)->weak_self.reset(container);
}

bool Element::equals(const Element &rhs)
{
    return this->get() == rhs.get();
}

std::string Element::to_string(void) const
{
    return (*this)->repr;
}

const GlobalBlockConfig &Element::global_config(void) const
{
    return (*this)->global_config;
}

GlobalBlockConfig &Element::global_config(void)
{
    return (*this)->global_config;
}

void Element::adopt_element(const std::string &name, const Element &child)
{
    if (child->parent) throw std::invalid_argument(str(boost::format(
        "Could not register child %s into %s.\n"
        "The child %s already has parent %s.\n"
    )
        % child.to_string()
        % this->to_string()
        % child.to_string()
        % child->parent.to_string()
    ));

    if ((*this)->children.count(name)) throw std::invalid_argument(str(boost::format(
        "A child of name %s already registered to element %s"
    )
        % child.to_string()
        % this->to_string()
    ));

    child->parent = *this;
    (*this)->children[name] = child;
}

Element *Element::locate_element(const std::string &path)
{
    //split the paths into nodes
    std::vector<std::string> nodes;
    boost::split(nodes, path, boost::is_any_of("/"));

    //iterate through the path to find the element
    boost::shared_ptr<ElementImpl> elem = *this;
    size_t i = 0;
    BOOST_FOREACH(const std::string &node, nodes)
    {
        i++;
        if (node == "" and i == 1) //find root
        {
            while (elem->parent) elem = elem->parent;
            continue;
        }
        if (node == ".") //this dir
        {
            continue;
        }
        if (node == "..") //up a dir
        {
            if (not elem->parent) throw std::invalid_argument(
                "Element tree lookup fail - null parent: " + path
            );
            elem = elem->parent;
            continue;
        }
        if (elem->children.count(node) == 0) throw std::invalid_argument(
            "Element tree lookup fail - no such path: " + path
        );
        elem = elem->children[node];
    }

    //return block ptr as result
    return elem->self;
}
