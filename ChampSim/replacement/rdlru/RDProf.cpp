// vim: set ts=4:
// vim: set shiftwidth=4:
// vim: set expandtab:

#include "sampler.cpp"
#include "RDProf.h"
#include <atomic>
#include <fstream>
#include <numeric>
#include <mutex>

FILE *profile;

Sampler *smpl = new Sampler;
#if MAX_THREADS > 1
std::array<std::atomic<unsigned>, MAX_BUCKET + 1> histogram;
std::array<std::atomic<unsigned>, MAX_BUCKET + 1> histogram_read;
#else
std::array<unsigned, MAX_BUCKET + 1> histogram;
std::array<unsigned, MAX_BUCKET + 1> histogram_read;
#endif
std::array<counter_t, MAX_THREADS> now_lcl;

//PIN_LOCK glock;
std::mutex glock;
//-----------------------------------------------------------------------------
//                           SAMPLER INTERFACE
//-----------------------------------------------------------------------------
//VOID PIN_FAST_ANALYSIS_CALL update (VOID *addr, int is_read, int threadid) {
void update (uint64_t addr, int is_read, int threadid) {
    counter_t distance;
    md_addr_t tag = GET_TAG(addr);

    now_lcl[threadid]++;
    if (early_return(tag, now_lcl[threadid]))
        return;
    
#if MAX_THREADS > 1
    //PIN_GetLock(&glock, 1);
    glock.lock();
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
    //if ((now_lcl[threadid] & REFRESH_PERIOD_MSK) == 0)
    //    remove_all_expired(now);

#if MAX_THREADS > 1
    //PIN_ReleaseLock(&glock);
    glock.unlock();
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
    return std::accumulate(now_lcl.begin(), now_lcl.end(), 0.0);
}

//void remove_all_expired(counter_t now) {
//    std::optional<sample> entry;
//    for (int idx = 0; idx < SAMPLER_WIDTH; idx++) {
//        do {
//            entry = smpl->remove_expired (idx, now);
//            if (entry) { 
//                histogram[MAX_BUCKET]++;
//                histogram_read[MAX_BUCKET]++;
//            }
//        } while (entry);
//    }
//}

void rdprof_init()
{

    profile = fopen("prof.out", "w");
    fprintf(profile, "Max Distance:\t%ld\tRefresh Period:\t%ld\tRPeriod Mask:\t%lx\n\n", 
            MAX_DISTANCE, REFRESH_PERIOD, REFRESH_PERIOD_MSK);

}

void rdprof_fini()
{
    //smpl->stats_print(profile, real_now());

    std::cout<<"~~~~~~~~~~Reuse Distance Profiler output~~~~~~~~~~"<<std::endl;
    for (int i = 0; i <= MAX_BUCKET; i++)
        std::cout<<GET_DISTANCE(i) << "\t"<< 
                SAMPLING_PERIOD * 1LLU * histogram[i]<< "\t" << 
                SAMPLING_PERIOD * 1LLU * histogram_read[i]<<std::endl;
    std::cout<<"~~~~~~~~~~Reuse Distance Profiler output end~~~~~~~~~~\n"<<std::endl;
    smpl->print_sum();

    //delete smpl;
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */
   
int32_t Usage()
{
    //PIN_ERROR( "This Pintool collects a reuse distance profile\n" + KNOB_BASE::StringKnobSummary() + "\n");
    printf( "This Pintool collects a reuse distance profile\n" );
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int rdprof(int argc, char *argv[])
{
    //if (PIN_Init(argc, argv)) return Usage();


    //INS_AddInstrumentFunction(Instruction, 0);
    //PIN_AddFiniFunction(Fini, 0);

    //init();

    //// Never returns
    //PIN_StartProgram();



   
    return 0;
}
