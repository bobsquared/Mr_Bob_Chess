#include <iostream>
#include "trainer.h"


int main(int argc, char **argv) {
    InitColumnsMask();
    InitRowsMask();

    if (argc == 3) {
        Bitboard board = Bitboard();
        std::string network = argv[1];
        std::string fen = argv[2];
        KPNNUE model = KPNNUE();
        model.setNetwork(network);
        int ret = model.evaluate(fen, board);

        std::cout << "Score: " << ret << std::endl;
    } 
    else {
        Trainer trainer = Trainer();
        trainer.getFile("NNUE_d10_384M_rm.epd");
        KPNNUE model = KPNNUE();
        int modelSize[45] = {768, 768, 1024, 1};
        KPNNUE model = KPNNUE(3, modelSize);
        trainer.train(model, "networks/", 1000, 16384, 0.001);
    }

}