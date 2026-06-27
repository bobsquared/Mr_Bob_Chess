#include "eval.h"
#include "nnue/accumulator/accumulator.h"
#include "nnue/nncpp.h"


namespace EVAL {

    NNCPP model = NNCPP();



    void InitEval(std::string nnueFile) {
        model.LoadNetwork(nnueFile);
    }
    


    // Evaluate the position
    int evaluate(Board &b) {
        
        BoardState &board = b.state;

        // Asserts for debugging mode
        #ifndef NDEBUG
        int pawnCount = count_population(board.pieces[0]);
        int knightCount = count_population(board.pieces[2]);
        int bishopCount = count_population(board.pieces[4]);
        int rookCount = count_population(board.pieces[6]);
        int queenCount = count_population(board.pieces[8]);
        int kingCount = count_population(board.pieces[10]);
        assert(pawnCount == board.pieceCount[0]);
        assert(knightCount == board.pieceCount[2]);
        assert(bishopCount == board.pieceCount[4]);
        assert(rookCount == board.pieceCount[6]);
        assert(queenCount == board.pieceCount[8]);
        assert(kingCount == board.pieceCount[10]);

        pawnCount = count_population(board.pieces[1]);
        knightCount = count_population(board.pieces[3]);
        bishopCount = count_population(board.pieces[5]);
        rookCount = count_population(board.pieces[7]);
        queenCount = count_population(board.pieces[9]);
        kingCount = count_population(board.pieces[11]);
        assert(pawnCount == board.pieceCount[1]);
        assert(knightCount == board.pieceCount[3]);
        assert(bishopCount == board.pieceCount[5]);
        assert(rookCount == board.pieceCount[7]);
        assert(queenCount == board.pieceCount[9]);
        assert(kingCount == board.pieceCount[11]);
        #endif

        int queenBit = ((board.pieceCount[8] + board.pieceCount[9]) > 0) << 2;
        b.acc.Accumulate(model.getL1White(), model.getL1Black(), false);
        int retm = model.evaluate(b.acc.getOutputVectorWhite(), b.acc.getOutputVectorBlack(), board.toMove, std::max(0, ((getPhaseEval(b) >> 6) | queenBit)));
        
        return retm;
    }


}







//
