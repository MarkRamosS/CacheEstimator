CONFIG_ROOT=~/pin/pin-3.26-98690-g1fc9d60e6-gcc-linux
TOOL_ROOT=~/mt23/ChampSim/tracer/pin
SUITE_ROOT=~/mt23/automotive
$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/champsim_tracer.so -o $TOOL_ROOT/traces/bitcnts.champsim -- $SUITE_ROOT/bitcount/bitcnts 100000
echo "Bitcnts done."
xz $TOOL_ROOT/traces/bitcnts.champsim
echo "Zipped."

$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/champsim_tracer.so -o $TOOL_ROOT/traces/qsort_small.champsim --- $SUITE_ROOT/qsort/qsort_small $SUITE_ROOT/qsort/input_small.dat > $SUITE_ROOT/qsort/small.out
echo "Qsort small done."
xz $TOOL_ROOT/traces/qsort_small.champsim
echo "Zipped."

$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/champsim_tracer.so -o $TOOL_ROOT/traces/qsort_large.champsim -- $SUITE_ROOT/qsort/qsort_large $SUITE_ROOT/qsort/input_large.dat > $SUITE_ROOT/qsort/large.out
echo "Qsort large done."
xz $TOOL_ROOT/traces/qsort_large.champsim
echo "Zipped."

$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/champsim_tracer.so -o $TOOL_ROOT/traces/basicmath_small.champsim -- $SUITE_ROOT/basicmath/basicmath_small > $SUITE_ROOT/basicmath/output_small.txt
echo "Basicmath small done."
xz $TOOL_ROOT/traces/basicmath_small.champsim
echo "Zipped."

$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/champsim_tracer.so -o $TOOL_ROOT/traces/basicmath_large.champsim -- $SUITE_ROOT/basicmath/basicmath_large > $SUITE_ROOT/basicmath/output_large.txt 
echo "Basicmath large done."
xz $TOOL_ROOT/traces/basicmath_large.champsim
echo "Zipped."

#cd $SUITE_ROOT/susan
#$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/RDProf.so -- ./susan input_small.pgm output_small.smoothing.pgm -s
#mv prof.out ../prof_out/susan_small_smoothing.out
#$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/RDProf.so -- ./susan input_small.pgm output_small.corners.pgm -c
#mv prof.out ../prof_out/susan_small_corners.out
#$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/RDProf.so -- ./susan input_small.pgm output_small.edges.pgm -e
#mv prof.out ../prof_out/susan_small_edges.out
#echo "Susan small done."

#$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/RDProf.so -- ./susan input_large.pgm output_large.smoothing.pgm -s
#$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/RDProf.so -- ./susan input_large.pgm output_large.corners.pgm -c
#$CONFIG_ROOT/pin -t $TOOL_ROOT/obj-intel64/RDProf.so -- ./susan input_large.pgm output_large.edges.pgm -e
#echo "Susan large done."
