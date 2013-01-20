#!/bin/bash

ABSOLUTE_PATH=$(cd `dirname "${BASH_SOURCE[0]}"` && pwd)/`basename "${BASH_SOURCE[0]}"`
SOURCEDIR=`dirname $ABSOLUTE_PATH`
BUILDDIR=$(pwd)

OUT_PDF="gras_benchmark.pdf"
OUT_PNG="gras_benchmark.png"

echo "source directory ${SOURCEDIR}"
echo "build directory ${BUILDDIR}"

python -B $SOURCEDIR/run_benchmarks.py ${OUT_PDF}
convert ${OUT_PDF} /tmp/tmp_gras_bm.png
convert /tmp/tmp_gras_bm*.png -append ${OUT_PNG}
rm /tmp/tmp_gras_bm*.png
