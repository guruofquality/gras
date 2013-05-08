// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%{
#include <gras/thread_pool.hpp>
%}

////////////////////////////////////////////////////////////////////////
// remove base class warning
////////////////////////////////////////////////////////////////////////
#pragma SWIG nowarn=401

%include <gras/exception.i>

%include <std_string.i>

%include <gras/gras.hpp>
%include <gras/thread_pool.hpp>
