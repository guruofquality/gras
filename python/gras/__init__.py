# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#add the current path of this module for installed client apps swig imports
import os
import sys
sys.path.append(os.path.dirname(__file__))

from PMC import *
from GRAS_SBuffer import SBufferConfig, SBuffer
from GRAS_Tags import Tag, StreamTag, PacketMsg
from GRAS_TimeTag import TimeTag
from GRAS_Element import Element
from GRAS_Factory import Factory
import GRAS_Block
import GRAS_HierBlock
import GRAS_TopBlock
from GRAS_PyBlock import PyBlock as Block
from GRAS_PyHierBlocks import PyHierBlock as HierBlock
from GRAS_PyHierBlocks import PyTopBlock as TopBlock
from GRAS_ThreadPool import ThreadPoolConfig, ThreadPool

#try to import module
#http://effbot.org/zone/import-string.htm
def __try_module_import(filename):
    directory, module_name = os.path.split(filename)
    module_name = os.path.splitext(module_name)[0]

    path = list(sys.path)
    sys.path.insert(0, directory)
    try:
        module = __import__(module_name)
    except Exception as ex:
        print 'Could not import', filename, ex
    finally:
        sys.path[:] = path # restore

#recursive search for modules in path
def __module_import(p):
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

#try to import everything in the modules dir
module_path = os.path.join(os.path.dirname(__file__), 'modules')
if os.path.exists(module_path): __module_import(os.path.abspath(module_path))
