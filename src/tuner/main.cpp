#include "../defs.h"
#include "../zobrist_hashing.h"
#include "../magic_bitboards.h"
#include "tuner.h"

Zobrist *zobrist = new Zobrist();
Magics *magics;

int main() {
    InitColumnsMask();
    InitRowsMask();

    magics = new Magics();
    Bitboard board = Bitboard();
    Eval eval = Eval();

    Tuner tuner = Tuner(eval, board, "../texeldata.fen");
    tuner.tune_adam(10000, 8192, 0.003, 0.9, 0.999);
}