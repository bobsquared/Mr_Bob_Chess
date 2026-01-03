#pragma once

#include <iostream>
#include <chrono>
#include <string>
#include "board/bitboard.h"
#include "board/movepick.h"
#include "board/legality.h"
#include "board/move.h"
#include "board/fen.h"
#include "board/see.h"

extern void SeeTest(Board &x, std::string fen, int from, int to, int result, MOVE flags=0);
extern void Perft(Board & b, int depth);
uint64_t PerftCall(Board & b, int depth);
