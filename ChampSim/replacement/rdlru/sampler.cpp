// vim: set ts=4:
// vim: set shiftwidth=4:
// vim: set expandtab:

#include "sampler.h"
#include <mutex>

//----------------------------------------------------------------------------
//                             SAMPLER
//----------------------------------------------------------------------------

Sampler::Sampler() = default;

// Check whether the sampler has a sample for this tag
bool Sampler::has (md_addr_t tag)
{
    md_addr_t idx = GET_INDEX(tag);
    bool found = false;

    ////---> Quick, cheap check
    if (sampler[idx].empty())
        return false;

#if MAX_THREADS > 1
    auto& m = mutexes[idx];
    m.lock_shared();
    //PIN_RWMutexReadLock(&m);
#endif

    //---> More expensive check, but while holding only a read mutex
    found = sampler[idx].count(tag) > 0;

#if MAX_THREADS > 1
    m.unlock_shared();
    //PIN_RWMutexUnlock(&m);
#endif

    return found;
}

// Add an entry for the new sample
void Sampler::add (md_addr_t tag, counter_t now)
{
    md_addr_t idx = GET_INDEX(tag);
    auto& l = sampler[idx];

#if MAX_THREADS > 1
    auto& m = mutexes[idx];
    //PIN_RWMutexReadLock(&m);
    m.lock();
#endif

    l[tag] = now;

    stats_add();


#if MAX_THREADS > 1
    m.unloc();
    //PIN_RWMutexUnlock(&m);
#endif
}

// Find and remove the entry for tag
std::optional<sample> Sampler::remove (md_addr_t tag, counter_t now)
{
    md_addr_t idx = GET_INDEX(tag);
    auto& l = sampler[idx];
    std::optional<sample> entry;

#if MAX_THREADS > 1
    //---> Try to do the actual removal, while holding a write mutex
    //---> Might not find the entry, if it was removed in the meantime
    auto& m = mutexes[idx];
    m.lock();
    //PIN_RWMutexReadLock(&m);
#endif

    auto it = l.find(tag);
    if (it != l.end()) {
        entry = {it->first, it->second};
        stats_remove(idx, it->second, now);
        l.erase(it);
    }

#if MAX_THREADS > 1
    m.unlock();
    //PIN_RWMutexUnlock(&m);
#endif

    return entry;
}

// Find and remove entries which even if reused will
// have a reuse distance greater than the MAXIMUM
std::optional<sample> Sampler::remove_expired (md_addr_t idx, counter_t now)
{
    auto& l = sampler[idx];
    std::optional<sample> entry;

    //---> Quick, cheap checks
    if (l.empty())
        return entry;

#if MAX_THREADS > 1
    auto& m = mutexes[idx];
    m.lock();
    //PIN_RWMutexReadLock(&m);
#endif

    // Check the list one by one item
    for (auto it = l.begin(); it != l.end(); ++it) {
        counter_t distance = now - it->second - 1;
        if (distance >= MAX_DISTANCE) {
            entry = {it->first, it->second};
            stats_remove(idx, it->second, now);
            l.erase(it);
            break;
        }
    }

#if MAX_THREADS > 1
    m.unlock();
    //PIN_RWMutexUnlock(&m);
#endif

    return entry;
}


// STATS METHODS 

void Sampler::stats_add(void)
{
    #ifdef SAMPLER_STATS
    samples++;
    sampled++;
    #endif
    return;
}

void Sampler::stats_remove(int idx, counter_t memstamp, counter_t now)
{
    #ifdef SAMPLER_STATS
    samples--;
    occupancy[idx] += (now - memstamp);
    assert(samples >= 0);
    #endif
    return;
}

void Sampler::stats_print(FILE *fp, counter_t now)
{
    #ifdef SAMPLER_STATS
    counter_t sum = 0;
    fprintf(fp, "Accesses:\t%lu\nSamples:\t%d\nLeft:\t%d\n", now, sampled, samples);

    for (int i = 0; i < SAMPLER_WIDTH; i++) {
        auto& l = sampler[i];
        for (auto& s: l)
            occupancy[i] += now - s.second;

        fprintf(fp, "Occupancy Way %d :\t%lf\n", i, (occupancy[i] * 1.0) / now);
        sum += occupancy[i];
    }

    fprintf(fp, "Occupancy Avg :\t%lf\n\n", (sum * 1.0) / now);
    #endif
    return;
}

void Sampler::print_sum ()
{
    std::cout<<"Sampler size:"<<sampler[0].size()<<std::endl;
    return;
}
