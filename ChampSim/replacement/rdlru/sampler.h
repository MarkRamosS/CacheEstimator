// vim: set ts=4:
// vim: set shiftwidth=4:
// vim: set expandtab:

#ifndef SAMPLER_H
#define SAMPLER_H

#include "common.h"

constexpr int GET_INDEX(md_addr_t tag) {return tag & ((SAMPLER_WIDTH * 1ULL) - 1);}

#endif //SAMPLER_H
