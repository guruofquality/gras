#!/bin/bash

# This script generates the multi-argument template code
# for the callable and factory interfaces.
# This is the magic that makes the API calls
# so natural to call into as a C++ client app.
# This script is manually run when the source tmpl files are changed.
# And the generated sources are checked in.
# This way, the build system does not generate public headers.
# And the build system does not depend on Cheetah templates.

SCRIPT="`readlink -e $0`"
SCRIPTPATH="`dirname $SCRIPT`"
DEST=${SCRIPTPATH}/../include/gras

python expand_template.py factory.tmpl.hpp ${DEST}/factory.hpp
python expand_template.py factory_detail.tmpl.hpp ${DEST}/detail/factory.hpp
python expand_template.py callable.tmpl.hpp ${DEST}/callable.hpp
python expand_template.py callable_detail.tmpl.hpp ${DEST}/detail/callable.hpp
