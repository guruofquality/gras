########################################################################
## GRAS - The GNU Radio Advanced Scheduler
########################################################################

Read all about the project goals here:
https://github.com/guruofquality/gras/wiki

------------------------------------------------------------------------
-- Prerequisites
------------------------------------------------------------------------
* https://github.com/guruofquality/tsbe
* standard gnuradio prereqs

------------------------------------------------------------------------
-- Get the source code
------------------------------------------------------------------------
git clone https://github.com/guruofquality/gras.git
cd grruntime
git submodule init
git submodule update

------------------------------------------------------------------------
-- Installation instructions
------------------------------------------------------------------------
mkdir <build dir>
cd <build dir>
cmake <source dir>/gnuradio
make
sudo make install
