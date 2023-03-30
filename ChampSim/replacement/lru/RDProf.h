// vim: set ts=4:
// vim: set shiftwidth=4:
// vim: set expandtab:

#ifndef PROF_H
#define PROF_H

#include "common.h"

#define SAMPLING_PERIOD (1024 * 1024 * 1024)
#define SAMPLING_PERIOD_MSK (SAMPLING_PERIOD - 1)

#define REFRESH_PERIOD (MAX_DISTANCE >> 1)
#define REFRESH_PERIOD_MSK (REFRESH_PERIOD - 1)

#define CACHELINE_SZ 64
#define BITS_OFFSET 6

#define GET_TAG(addr) (addr >> BITS_OFFSET)

VOID update (VOID *addr, int is_read, int threadid);
inline bool early_return(md_addr_t tag, counter_t now);
void update_histogram(counter_t distance, int read);
counter_t real_now();
void remove_all_expired(counter_t now);
void init();

#endif //PROF_H
