CONFIG_ROOT=~/pin/pin-3.26-98690-g1fc9d60e6-gcc-linux
TOOL_ROOT=$CONFIG_ROOT/source/tools/RDProf
SUITE_ROOT=~/mt23/automotive

#$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/RDProf.so -- $SUITE_ROOT/bitcount/bitcnts 100000000 
#mv prof.out prof_out/bitcount.out

#$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/RDProf.so -- $SUITE_ROOT/qsort/qsort_small $SUITE_ROOT/qsort/input_small.dat > $SUITE_ROOT/qsort/small.out  100000000 
#mv prof.out prof_out/qsort_small.out
#
#$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/RDProf.so -- $SUITE_ROOT/qsort/qsort_large $SUITE_ROOT/qsort/input_large.dat > $SUITE_ROOT/qsort/large.out 100000000 
#mv prof.out prof_out/qsort_large.out

$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/RDProf.so -- $SUITE_ROOT/basicmath/basicmath_small > $SUITE_ROOT/basicmath/output_small.txt 100000000 
mv prof.out prof_out/basicmath_small.out
#$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/RDProf.so -- $SUITE_ROOT/basicmath/basicmath_large > $SUITE_ROOT/basicmath/output_large.txt 100000000 
#mv prof.out prof_out/basicmath_large.out
