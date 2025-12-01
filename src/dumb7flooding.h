// Algorithms adapted from https://www.chessprogramming.org/Dumb7Fill#OccludedFill

#pragma once
#include <cstdint>


static inline constexpr uint64_t dumb7FloodingN(uint64_t index, uint64_t empty) {
    empty = ~empty;
    uint64_t flood = index;
    index = (index << 8) & empty; flood |= index;
    index = (index << 8) & empty; flood |= index;
    index = (index << 8) & empty; flood |= index;
    index = (index << 8) & empty; flood |= index;
    index = (index << 8) & empty; flood |= index;
    index = (index << 8) & empty; flood |= index;
    index = (index << 8) & empty; flood |= index;

    return flood << 8;
}



static inline constexpr uint64_t dumb7FloodingS(uint64_t index, uint64_t empty) {
    empty = ~empty;
    uint64_t flood = index;
    index = (index >> 8) & empty; flood |= index;
    index = (index >> 8) & empty; flood |= index;
    index = (index >> 8) & empty; flood |= index;
    index = (index >> 8) & empty; flood |= index;
    index = (index >> 8) & empty; flood |= index;
    index = (index >> 8) & empty; flood |= index;
    index = (index >> 8) & empty; flood |= index;

    return flood >> 8;
}



static inline constexpr uint64_t dumb7FloodingE(uint64_t index, uint64_t empty) {
    empty = ~empty & 0xfefefefefefefefe;
    uint64_t flood = index;
    index = (index << 1) & empty; flood |= index;
    index = (index << 1) & empty; flood |= index;
    index = (index << 1) & empty; flood |= index;
    index = (index << 1) & empty; flood |= index;
    index = (index << 1) & empty; flood |= index;
    index = (index << 1) & empty; flood |= index;
    index = (index << 1) & empty; flood |= index;

    return (flood << 1) & 0xfefefefefefefefe;
}



static inline constexpr uint64_t dumb7FloodingW(uint64_t index, uint64_t empty) {
    empty = ~empty & 0x7f7f7f7f7f7f7f7f;
    uint64_t flood = index;
    index = (index >> 1) & empty; flood |= index;
    index = (index >> 1) & empty; flood |= index;
    index = (index >> 1) & empty; flood |= index;
    index = (index >> 1) & empty; flood |= index;
    index = (index >> 1) & empty; flood |= index;
    index = (index >> 1) & empty; flood |= index;
    index = (index >> 1) & empty; flood |= index;

    return (flood >> 1) & 0x7f7f7f7f7f7f7f7f;
}



static inline constexpr uint64_t dumb7FloodingNE(uint64_t index, uint64_t empty) {
    empty = ~empty & 0xfefefefefefefefe;
    uint64_t flood = index;
    index = (index << 9) & empty; flood |= index;
    index = (index << 9) & empty; flood |= index;
    index = (index << 9) & empty; flood |= index;
    index = (index << 9) & empty; flood |= index;
    index = (index << 9) & empty; flood |= index;
    index = (index << 9) & empty; flood |= index;
    index = (index << 9) & empty; flood |= index;

    return (flood << 9) & 0xfefefefefefefefe;
}



static inline constexpr uint64_t dumb7FloodingSE(uint64_t index, uint64_t empty) {
    empty = ~empty & 0xfefefefefefefefe;
    uint64_t flood = index;
    index = (index >> 7) & empty; flood |= index;
    index = (index >> 7) & empty; flood |= index;
    index = (index >> 7) & empty; flood |= index;
    index = (index >> 7) & empty; flood |= index;
    index = (index >> 7) & empty; flood |= index;
    index = (index >> 7) & empty; flood |= index;
    index = (index >> 7) & empty; flood |= index;

    return (flood >> 7) & 0xfefefefefefefefe;
}



static inline constexpr uint64_t dumb7FloodingNW(uint64_t index, uint64_t empty) {
    empty = ~empty & 0x7f7f7f7f7f7f7f7f;
    uint64_t flood = index;
    index = (index << 7) & empty; flood |= index;
    index = (index << 7) & empty; flood |= index;
    index = (index << 7) & empty; flood |= index;
    index = (index << 7) & empty; flood |= index;
    index = (index << 7) & empty; flood |= index;
    index = (index << 7) & empty; flood |= index;
    index = (index << 7) & empty; flood |= index;

    return (flood << 7) & 0x7f7f7f7f7f7f7f7f;
}



static inline constexpr uint64_t dumb7FloodingSW(uint64_t index, uint64_t empty) {
    empty = ~empty & 0x7f7f7f7f7f7f7f7f;
    uint64_t flood = index;
    index = (index >> 9) & empty; flood |= index;
    index = (index >> 9) & empty; flood |= index;
    index = (index >> 9) & empty; flood |= index;
    index = (index >> 9) & empty; flood |= index;
    index = (index >> 9) & empty; flood |= index;
    index = (index >> 9) & empty; flood |= index;
    index = (index >> 9) & empty; flood |= index;

    return (flood >> 9) & 0x7f7f7f7f7f7f7f7f;
}


//
