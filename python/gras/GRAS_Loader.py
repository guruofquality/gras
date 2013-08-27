# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import os
import imp
import traceback

#try to import module
def __try_module_import(filename):
    directory, module_name = os.path.split(filename)
    module_name = os.path.splitext(module_name)[0]

    try:
        fp, pathname, description = imp.find_module(module_name, [directory])
        module = imp.load_module(module_name, fp, pathname, description)
    except Exception as ex:
        print 'Could not import', filename, ex
        print traceback.format_exc()

#recursive search for modules in path
def __module_import(p):
    if not os.path.exists(p): return
    if os.path.isfile(p):
        return __try_module_import(p)
    if not os.path.isdir(p): return
    if os.path.exists(os.path.join(p, '__init__.py')):
        return __try_module_import(p)
    for sub in os.listdir(p):
        name, ext = os.path.splitext(sub)
        #prefer .pyo over .pyc, prefer .pyc over .py
        has_pyc = os.path.exists(os.path.join(p, name+'.pyc'))
        has_pyo = os.path.exists(os.path.join(p, name+'.pyo'))
        if ext == ".py" and (has_pyc or has_pyo): continue
        if ext == ".pyc" and has_pyo: continue
        __module_import(os.path.join(p, sub))

#separate the paths and load each one
def __load_modules_from_paths(paths, suffix):
    if not paths: return
    for path in paths.split(os.pathsep):
        if not path: continue
        if suffix: path = os.path.join(path, suffix)
        __module_import(path)

__load_modules_from_paths(os.getenv("GRAS_ROOT", "@GRAS_ROOT@"), os.path.join("lib@LIB_SUFFIX@", "gras", "python"))
__load_modules_from_paths(os.getenv("GRAS_PATH", ""), os.path.join("lib@LIB_SUFFIX@", "gras", "python"))
__load_modules_from_paths(os.getenv("GRAS_PYTHON_PATH", ""), "")
