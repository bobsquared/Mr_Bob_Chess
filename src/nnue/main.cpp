#include <iostream>
#include "trainer.h"


int main(int argc, char **argv) {
    InitColumnsMask();
    InitRowsMask();
    InitLateMoveArray();

    if (argc == 3) {
        Bitboard board = Bitboard();
        std::string network = argv[1];
        std::string fen = argv[2];
        KPNNUE model = KPNNUE(network);
        int ret = model.evaluate(fen, board);

        std::cout << "Score: " << ret << std::endl;
    } 

}