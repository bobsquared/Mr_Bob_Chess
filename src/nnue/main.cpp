#include <iostream>
#include "trainer.h"


int main(int argc, char **argv) {
    InitColumnsMask();
    InitRowsMask();
    InitLateMoveArray();

    if (argc == 3) {
        #ifdef KPTrainer
        Trainer trainer = Trainer();
        std::string infile = argv[1];
        std::string outfile = argv[2];
        trainer.extractFile(infile, outfile);
        #else
        Bitboard board = Bitboard();
        std::string network = argv[1];
        std::string fen = argv[2];
        KPNNUE model = KPNNUE(network);
        int ret = model.evaluate(fen, board);

        std::cout << "Score: " << ret << std::endl;
        #endif
    }
    else {
        Trainer trainer = Trainer();
        trainer.getFile("NNUE_d8_70M_r.epd");
        // trainer.getFileSigmoid("../texeldata.fen");
        // KPNNUE model = KPNNUE("networks/test25/dog_47.bin");
        int modelSize[45] = {768, 256, 8, 1};
        KPNNUE model = KPNNUE(3, modelSize);
        trainer.train(model, "networks/test26/dog");
    }
    

}