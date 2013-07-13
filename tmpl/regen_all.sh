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
SRCDIR="`dirname $SCRIPT`"
DESTDIR=${SRCDIR}/../include/gras

python ${SRCDIR}/expand_template.py ${SRCDIR}/factory.tmpl.hpp ${DESTDIR}/factory.hpp
python ${SRCDIR}/expand_template.py ${SRCDIR}/factory_detail.tmpl.hpp ${DESTDIR}/detail/factory.hpp
python ${SRCDIR}/expand_template.py ${SRCDIR}/callable.tmpl.hpp ${DESTDIR}/callable.hpp
python ${SRCDIR}/expand_template.py ${SRCDIR}/callable_detail.tmpl.hpp ${DESTDIR}/detail/callable.hpp
