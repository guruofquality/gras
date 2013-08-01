// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

%include <gras/exception.i>
%module (package="gras") GRAS_Factory

%{
#include <gras/factory.hpp>
%}

%newobject gras::_handle_make;

////////////////////////////////////////////////////////////////////////
// Export swig element comprehension
////////////////////////////////////////////////////////////////////////
%include <std_string.i>
%include <std_vector.i>

%template (StringVector) std::vector<std::string>;

%import <PMC/PMC.i>
%import <gras/element.i>
%include <gras/gras.hpp>
%include <gras/factory.hpp>

////////////////////////////////////////////////////////////////////////
// Create python make method for the factory
////////////////////////////////////////////////////////////////////////
%pythoncode %{
#TODO we need to register this into the real factory
_py_factory = dict()

def register_factory(path, fcn):
    #TODO we need to register this into the real factory
    _py_factory[path] = fcn

def make(path, *args, **kwargs):

    #first try the local to python py factory #TODO real factory
    if path in _py_factory: return _py_factory[path](*args, **kwargs)

    from PMC import PMC_M
    pmcargs = PMC_M(list(args))
    return _handle_make(path, pmcargs)

def try_load_dll(name):
    import ctypes
    import ctypes.util
    dll_path = ctypes.util.find_library(name)
    if not dll_path: dll_path = 'lib'+name+'.so'
    ctypes.CDLL(dll_path, ctypes.RTLD_GLOBAL)

def py_jit_factory(*args):
    #load the dlls with ctypes for the JIT factory
    try_load_dll("gras")
    try_load_dll("pmc")
    jit_factory(*args)
%}
