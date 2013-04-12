#!/bin/bash

SUFFIX=$1

ABSOLUTE_PATH=$(cd `dirname "${BASH_SOURCE[0]}"` && pwd)/`basename "${BASH_SOURCE[0]}"`
SOURCEDIR=`dirname $ABSOLUTE_PATH`
BUILDDIR=$(pwd)

OUT_PDF="gras_benchmark_${SUFFIX}.pdf"
OUT_PNG="gras_benchmark_${SUFFIX}.png"

echo "source directory ${SOURCEDIR}"
echo "build directory ${BUILDDIR}"

python -B $SOURCEDIR/run_benchmarks.py ${OUT_PDF}
convert ${OUT_PDF} /tmp/tmp_gras_bm_${SUFFIX}.png
convert /tmp/tmp_gras_bm_${SUFFIX}*.png -append ${OUT_PNG}
rm /tmp/tmp_gras_bm_${SUFFIX}*.png
