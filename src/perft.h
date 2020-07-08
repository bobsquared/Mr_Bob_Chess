#pragma once

#include <iostream>
#include <chrono>
#include <string>
#include "bitboard.h"


extern void SeeTest(Bitboard &x, std::string fen, int from, int to, int result, MOVE flags=0);
extern void PerftTest(Bitboard & b);
extern void Perft(Bitboard & b, int depth, uint64_t numNodes, std::string fen);
uint64_t PerftCall(Bitboard & b, int depth);
