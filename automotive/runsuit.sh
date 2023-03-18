CONFIG_ROOT=~/pin/pin-3.26-98690-g1fc9d60e6-gcc-linux
TOOL_ROOT=$CONFIG_ROOT/source/tools/RDProf
SUITE_ROOT=~/mt23/automotive

$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/RDProf.so -- $SUITE_ROOT/bitcount/bitcnts 100000
mv prof.out prof_out/bitcount.out
echo "Bitcnts done."
$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/RDProf.so -- $SUITE_ROOT/qsort/qsort_small $SUITE_ROOT/qsort/input_small.dat > $SUITE_ROOT/qsort/small.out
mv prof.out prof_out/qsort_small.out
echo "Qsort small done."

$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/RDProf.so -- $SUITE_ROOT/qsort/qsort_large $SUITE_ROOT/qsort/input_large.dat > $SUITE_ROOT/qsort/large.out
mv prof.out prof_out/qsort_large.out
echo "Qsort large done."

$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/RDProf.so -- $SUITE_ROOT/basicmath/basicmath_small > $SUITE_ROOT/basicmath/output_small.txt
mv prof.out prof_out/basicmath_small.out
echo "Basicmath small done."

$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/RDProf.so -- $SUITE_ROOT/basicmath/basicmath_large > $SUITE_ROOT/basicmath/output_large.txt 
mv prof.out prof_out/basicmath_large.out
echo "Basicmath large done."

cd $SUITE_ROOT/susan
$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/RDProf.so -- ./susan input_small.pgm output_small.smoothing.pgm -s
mv prof.out ../prof_out/susan_small_smoothing.out
$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/RDProf.so -- ./susan input_small.pgm output_small.corners.pgm -c
mv prof.out ../prof_out/susan_small_corners.out
$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/RDProf.so -- ./susan input_small.pgm output_small.edges.pgm -e
mv prof.out ../prof_out/susan_small_edges.out
echo "Susan small done."

$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/RDProf.so -- ./susan input_large.pgm output_large.smoothing.pgm -s
mv prof.out ../prof_out/susan_large_smoothing.out
$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/RDProf.so -- ./susan input_large.pgm output_large.corners.pgm -c
mv prof.out ../prof_out/susan_large_corners.out
$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/RDProf.so -- ./susan input_large.pgm output_large.edges.pgm -e
mv prof.out ../prof_out/susan_large_edges.out
echo "Susan large done."
