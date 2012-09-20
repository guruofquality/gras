########################################################################
## GRAS - The GNU Radio Advanced Scheduler
########################################################################

Read all about the project goals here:
https://github.com/guruofquality/gras/wiki

------------------------------------------------------------------------
-- Prerequisites
------------------------------------------------------------------------
* standard gnuradio prereqs
* libnuma (linux only)

------------------------------------------------------------------------
-- Get the source code
------------------------------------------------------------------------
git clone https://github.com/guruofquality/gras.git gras
cd gras
git submodule init
git submodule update

------------------------------------------------------------------------
-- Build and Install instructions (unix makefiles)
------------------------------------------------------------------------
mkdir <build-dir>
cd <build-dir>
cmake <gras-source-dir>/gnuradio
make
sudo make install
sudo ldconfig

------------------------------------------------------------------------
-- Build and Install instructions (MSVC)
------------------------------------------------------------------------
Untested. Minor fixes may be needed but it should work.
