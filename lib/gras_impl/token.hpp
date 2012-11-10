// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#ifndef INCLUDED_LIBGRAS_IMPL_TOKEN_HPP
#define INCLUDED_LIBGRAS_IMPL_TOKEN_HPP

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace gras
{

typedef boost::weak_ptr<int> WeakToken;

struct Token : boost::shared_ptr<int>
{
    static Token make(void)
    {
        Token tok;
        tok.reset(new int(0));
        return tok;
    }
};

} //namespace gras

#endif /*INCLUDED_LIBGRAS_IMPL_TOKEN_HPP*/
