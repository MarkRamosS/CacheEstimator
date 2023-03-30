// vim: set ts=4:
// vim: set shiftwidth=4:
// vim: set expandtab:

#include "RDProf.h"
#include <atomic>
#include <fstream>
#include <numeric>

FILE *profile;

Sampler *smpl = NULL;
#if MAX_THREADS > 1
std::array<std::atomic<unsigned>, MAX_BUCKET + 1> histogram;
std::array<std::atomic<unsigned>, MAX_BUCKET + 1> histogram_read;
#else
std::array<unsigned, MAX_BUCKET + 1> histogram;
std::array<unsigned, MAX_BUCKET + 1> histogram_read;
#endif
std::array<counter_t, MAX_THREADS> now_lcl;

PIN_LOCK glock;

//-----------------------------------------------------------------------------
//                           SAMPLER INTERFACE
//-----------------------------------------------------------------------------
VOID PIN_FAST_ANALYSIS_CALL update (VOID *addr, int is_read, int threadid) {
    counter_t distance;
    md_addr_t tag = GET_TAG((uint64_t)addr);

    now_lcl[threadid]++;
    if (early_return(tag, now_lcl[threadid]))
        return;
    
#if MAX_THREADS > 1
    PIN_GetLock(&glock, 1);
#endif
    counter_t now = real_now();

    std::optional<sample> entry = smpl->remove(tag, now);
    // found matching sample, update the histograms
    if (entry) {
        distance = now - entry->memstamp - 1;
        distance = distance > 0 ? distance : 0;
        update_histogram(distance, is_read);
    }

    // Time for a new sample?
    if ((now_lcl[threadid] & SAMPLING_PERIOD_MSK) == 0)
          smpl->add(tag, now);

    // Time for removing stale entries from the sampler?
    if ((now_lcl[threadid] & REFRESH_PERIOD_MSK) == 0)
        remove_all_expired(now);

#if MAX_THREADS > 1
    PIN_ReleaseLock(&glock);
#endif
}

inline bool early_return(md_addr_t tag, counter_t now) {
    return ((!smpl->has(tag)) && \
        ((now & SAMPLING_PERIOD_MSK) != 0) && \
        ((now & REFRESH_PERIOD_MSK) != 0));
}

void update_histogram(counter_t distance, int is_read) {
    int distance_bucket = (int)(GET_BUCKET(distance));
    histogram[distance_bucket]++;
    if (is_read) 
        histogram_read[distance_bucket]++;
}

counter_t real_now() {
    return std::reduce(now_lcl.begin(), now_lcl.end());
}

void remove_all_expired(counter_t now) {
    std::optional<sample> entry;
    for (int idx = 0; idx < SAMPLER_WIDTH; idx++) {
        do {
            entry = smpl->remove_expired (idx, now);
            if (entry) { 
                histogram[MAX_BUCKET]++;
                histogram_read[MAX_BUCKET]++;
            }
        } while (entry);
    }
}

//-----------------------------------------------------------------------------
//                    INITIALIZATION AND FINISH                                
//-----------------------------------------------------------------------------
KNOB<std::string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "prof.out", "specify output file name");

void init()
{
    smpl = new Sampler;

    profile = fopen(KnobOutputFile.Value().c_str(), "w");
    fprintf(profile, "Max Distance:\t%ld\tRefresh Period:\t%ld\tRPeriod Mask:\t%lx\n\n", MAX_DISTANCE, REFRESH_PERIOD, REFRESH_PERIOD_MSK);

}


// Is called for every instruction and instruments reads and writes
VOID Instruction(INS ins, VOID *v)
{
    // Instruments memory accesses using a predicated call, i.e.
    // the instrumentation is called iff the instruction will actually be executed.
    //
    // On the IA-32 and Intel(R) 64 architectures conditional moves and REP 
    // prefixed instructions appear as predicated instructions in Pin.
    UINT32 memOperands = INS_MemoryOperandCount(ins);

    // Iterate over each memory operand of the instruction.
    for (UINT32 memOp = 0; memOp < memOperands; memOp++)
    {
        if (INS_MemoryOperandIsRead(ins, memOp))
            INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE, (AFUNPTR)update, IARG_FAST_ANALYSIS_CALL,
                IARG_MEMORYOP_EA, memOp, IARG_UINT32, 1, IARG_THREAD_ID, IARG_END);
        if (INS_MemoryOperandIsWritten(ins, memOp))
            INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE, (AFUNPTR)update, IARG_FAST_ANALYSIS_CALL,
                IARG_MEMORYOP_EA, memOp, IARG_UINT32, 0, IARG_THREAD_ID, IARG_END);
    }
}

VOID Fini(INT32 code, VOID *v)
{
    smpl->stats_print(profile, real_now());

    for (int i = 0; i <= MAX_BUCKET; i++)
        fprintf(profile, "%llu\t%llu\t%llu\n", GET_DISTANCE(i), SAMPLING_PERIOD * 1LLU * histogram[i],  SAMPLING_PERIOD * 1LLU * histogram_read[i]);
    fclose(profile);

    delete smpl;
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */
   
INT32 Usage()
{
    PIN_ERROR( "This Pintool collects a reuse distance profile\n" 
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int rdprof(int argc, char *argv[])
{
    if (PIN_Init(argc, argv)) return Usage();


    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    init();

    // Never returns
    PIN_StartProgram();
    
    return 0;
}
