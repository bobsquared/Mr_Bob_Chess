
// Algorithms adapted from https://www.chessprogramming.org/Dumb7Fill#OccludedFill
// Used to initialize magic bitboards
// Fill ones in a certain direction until a piece is blocking the way.


#include "dumb7flooding.h"



uint64_t dumb7FloodingN(uint64_t index, uint64_t empty) {
    empty ^= 18446744073709551615U;
    uint64_t flood = index;
    index = (index << 8) & empty;
    flood |= index;
    index = (index << 8) & empty;
    flood |= index;
    index = (index << 8) & empty;
    flood |= index;
    index = (index << 8) & empty;
    flood |= index;
    index = (index << 8) & empty;
    flood |= index;
    index = (index << 8) & empty;
    flood |= index;
    index = (index << 8) & empty;
    flood |= index;
    return (flood << 8) & 18446744073709551615U;
}

uint64_t dumb7FloodingS(uint64_t index, uint64_t empty) {
    empty ^= 18446744073709551615U;
    uint64_t flood = index;
    index = (index >> 8) & empty;
    flood |= index;
    index = (index >> 8) & empty;
    flood |= index;
    index = (index >> 8) & empty;
    flood |= index;
    index = (index >> 8) & empty;
    flood |= index;
    index = (index >> 8) & empty;
    flood |= index;
    index = (index >> 8) & empty;
    flood |= index;
    index = (index >> 8) & empty;
    flood |= index;

    return (flood >> 8) & 18446744073709551615U;
}

uint64_t dumb7FloodingE(uint64_t index, uint64_t empty) {
    empty ^= 18446744073709551615U;
    empty &= 0xfefefefefefefefe;
    uint64_t flood = index;
    index = (index << 1) & empty;
    flood |= index;
    index = (index << 1) & empty;
    flood |= index;
    index = (index << 1) & empty;
    flood |= index;
    index = (index << 1) & empty;
    flood |= index;
    index = (index << 1) & empty;
    flood |= index;
    index = (index << 1) & empty;
    flood |= index;
    index = (index << 1) & empty;
    flood |= index;

    return (flood << 1) & 0xfefefefefefefefe;
}

uint64_t dumb7FloodingW(uint64_t index, uint64_t empty) {
    empty ^= 18446744073709551615U;
    empty &= 0x7f7f7f7f7f7f7f7f;
    uint64_t flood = index;
    index = (index >> 1) & empty;
    flood |= index;
    index = (index >> 1) & empty;
    flood |= index;
    index = (index >> 1) & empty;
    flood |= index;
    index = (index >> 1) & empty;
    flood |= index;
    index = (index >> 1) & empty;
    flood |= index;
    index = (index >> 1) & empty;
    flood |= index;
    index = (index >> 1) & empty;
    flood |= index;

    return (flood >> 1) & 0x7f7f7f7f7f7f7f7f;
}

uint64_t dumb7FloodingNE(uint64_t index, uint64_t empty) {
    empty ^= 18446744073709551615U;
    empty &= 0xfefefefefefefefe;
    uint64_t flood = index;
    index = (index << 9) & empty;
    flood |= index;
    index = (index << 9) & empty;
    flood |= index;
    index = (index << 9) & empty;
    flood |= index;
    index = (index << 9) & empty;
    flood |= index;
    index = (index << 9) & empty;
    flood |= index;
    index = (index << 9) & empty;
    flood |= index;
    index = (index << 9) & empty;
    flood |= index;

    return (flood << 9) & 0xfefefefefefefefe;
}

uint64_t dumb7FloodingSE(uint64_t index, uint64_t empty) {
    empty ^= 18446744073709551615U;
    empty &= 0xfefefefefefefefe;
    uint64_t flood = index;
    index = (index >> 7) & empty;
    flood |= index;
    index = (index >> 7) & empty;
    flood |= index;
    index = (index >> 7) & empty;
    flood |= index;
    index = (index >> 7) & empty;
    flood |= index;
    index = (index >> 7) & empty;
    flood |= index;
    index = (index >> 7) & empty;
    flood |= index;
    index = (index >> 7) & empty;
    flood |= index;

    return (flood >> 7) & 0xfefefefefefefefe;
}

uint64_t dumb7FloodingNW(uint64_t index, uint64_t empty) {
    empty ^= 18446744073709551615U;
    empty &= 0x7f7f7f7f7f7f7f7f;
    uint64_t flood = index;
    index = (index << 7) & empty;
    flood |= index;
    index = (index << 7) & empty;
    flood |= index;
    index = (index << 7) & empty;
    flood |= index;
    index = (index << 7) & empty;
    flood |= index;
    index = (index << 7) & empty;
    flood |= index;
    index = (index << 7) & empty;
    flood |= index;
    index = (index << 7) & empty;
    flood |= index;

    return (flood << 7) & 0x7f7f7f7f7f7f7f7f;
}

uint64_t dumb7FloodingSW(uint64_t index, uint64_t empty) {
    empty ^= 18446744073709551615U;
    empty &= 0x7f7f7f7f7f7f7f7f;
    uint64_t flood = index;
    index = (index >> 9) & empty;
    flood |= index;
    index = (index >> 9) & empty;
    flood |= index;
    index = (index >> 9) & empty;
    flood |= index;
    index = (index >> 9) & empty;
    flood |= index;
    index = (index >> 9) & empty;
    flood |= index;
    index = (index >> 9) & empty;
    flood |= index;
    index = (index >> 9) & empty;
    flood |= index;

    return (flood >> 9) & 0x7f7f7f7f7f7f7f7f;
}























//
