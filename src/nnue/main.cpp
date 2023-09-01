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
    else {
        Trainer trainer = Trainer();
        trainer.getFile("NNUE_d10_332M_rmn.epd");
        int modelSize[45] = {768, 512, 8, 1};
        KPNNUE model = KPNNUE(3, modelSize);
        trainer.train(model, "networks/test46/dog", 1000, 16384, 0.001);
    }

}