// vim: set ts=4:
// vim: set shiftwidth=4:
// vim: set expandtab:

#ifndef COMMON_H
#define COMMON_H
//#include "pin.H"
#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <forward_list>
#include <mutex>
#include <shared_mutex>
#include <iostream>
#include <optional>
#include <unordered_map>
#include <unordered_set>

//#define SAMPLER_STATS

#define MAX_THREADS 1

constexpr int SAMPLER_WIDTH = (MAX_THREADS > 1) ? 256 : 1;
constexpr int MAX_BUCKET = 896 - 1;

//------------------------------------------------------------------------------
//-----> MACROS
//------------------------------------------------------------------------------
#define MSB(x) (x ? 63 - __builtin_clzll(x) : 63)
#define BITS_QUANTIZATION(distance) (distance > 63 ? MSB(distance) - 5 : 0)

#define GET_BUCKET_INTERNAL(distance) ((distance >> BITS_QUANTIZATION(distance)) + (32 * BITS_QUANTIZATION(distance)))
#define GET_BUCKET(distance) ((GET_BUCKET_INTERNAL(distance) <= MAX_BUCKET) ? GET_BUCKET_INTERNAL(distance) : MAX_BUCKET)

#define Q(x) ((x) >> 5)
#define M(x) ((x) & 0x1f)
#define GET_DISTANCE(bucket) ((bucket < 64) ? (1ULL * bucket) : ((1ULL << (Q(bucket) + 4)) + ((1ULL << (Q(bucket) - 1)) * M(bucket))))
#define MAX_DISTANCE (uint64_t) (GET_DISTANCE(MAX_BUCKET + 1))

typedef uint64_t md_addr_t;
typedef uint64_t counter_t;

/*  the main structure of each node */
struct sample {
    md_addr_t tag{0};
    counter_t memstamp{0};
    sample() = default;
    sample(md_addr_t tag, counter_t memstamp) : tag{tag}, memstamp{memstamp} {};
};


class Sampler
{
    private:
        std::array<std::unordered_map<md_addr_t, counter_t>, SAMPLER_WIDTH> sampler;
        std::array<std::shared_mutex, SAMPLER_WIDTH> mutexes;

        #ifdef SAMPLER_STATS
        int samples{0};
        int sampled{0};
        std::array<counter_t, SAMPLER_WIDTH> occupancy;
        #endif

        void stats_add(void);
        void stats_remove(int idx, counter_t memstamp, counter_t now);

    public:
        Sampler();
        bool has (md_addr_t tag);
        void add (md_addr_t tag, counter_t now);
        std::optional<sample> remove (md_addr_t tag, counter_t now);
        std::optional<sample> remove_expired (md_addr_t idx, counter_t now);
        void stats_print(FILE *fp, counter_t now);
        void print_sum();
};

#endif //COMMON_H
