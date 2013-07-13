#!/bin/bash

SCRIPT="`readlink -e $0`"
SCRIPTPATH="`dirname $SCRIPT`"
DEST=${SCRIPTPATH}/../include/gras

python expand_template.py factory.tmpl.hpp ${DEST}/factory.hpp
python expand_template.py factory_detail.tmpl.hpp ${DEST}/detail/factory.hpp
python expand_template.py callable.tmpl.hpp ${DEST}/callable.hpp
python expand_template.py callable_detail.tmpl.hpp ${DEST}/detail/callable.hpp
