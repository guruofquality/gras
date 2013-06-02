// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include "element_impl.hpp"
#include <gras/element.hpp>
#include <boost/thread/mutex.hpp>
#include <stdexcept>
#include <set>

using namespace gras;

static boost::mutex uid_mutex;
static std::set<std::string> &get_uid_set(void)
{
    static std::set<std::string> uid_set;
    return uid_set;
}

static void free_uid(std::string *s)
{
    boost::mutex::scoped_lock l(uid_mutex);
    get_uid_set().erase(*s);
    delete s;
}

static std::string *alloc_uid(const std::string &uid)
{
    boost::mutex::scoped_lock l(uid_mutex);
    if (get_uid_set().count(uid) > 0)
    {
        throw std::invalid_argument("Element::set_uid - uid already in use: " + uid);
    }
    get_uid_set().insert(uid);
    return new std::string(uid);
}

void Element::set_uid(const std::string &uid)
{
    (*this)->uid.reset(alloc_uid(uid), &free_uid);
}

std::string Element::get_uid(void) const
{
    boost::mutex::scoped_lock l(uid_mutex);
    return *((*this)->uid);
}
