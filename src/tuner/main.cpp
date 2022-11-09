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
    
    KPNNUE model = KPNNUE("../nnue/networks/test13/dog_32.bin");
    Eval eval = Eval(model);
    Tuner tuner = Tuner(eval, board, model, "../texeldata.fen");
    tuner.tune_adam(10000, 8192, 0.001, 0.9, 0.999);
}