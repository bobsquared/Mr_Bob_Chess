#include "eval.h"
#include "nnue/accumulator/accumulator.h"
#include "nnue/nncpp.h"


namespace EVAL {

    NNCPP *model = new NNCPP();


    void InitEval(std::string nnueFile) {
        model->LoadNetwork(nnueFile);
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

        int kingWhiteSquare = bitScan(board.pieces[10]);
        int kingBlackSquare = bitScan(board.pieces[11]) ^ 56;

        uint8_t w = model->getHalfKing(kingWhiteSquare);
        uint8_t bl = model->getHalfKing(kingBlackSquare);

        if (b.acc.updateKingBucket(w, bl, kingWhiteSquare, kingBlackSquare)) {
            BITBOARD::InitFeatures(board, b.acc);
        }

        int queenBit = ((board.pieceCount[8] + board.pieceCount[9]) > 0) << 4;
        int bucket = (getPhaseEval(b) >> 4) | queenBit;

        b.acc.Accumulate(model->getL1White(kingWhiteSquare), model->getL1Black(kingBlackSquare), false);
        int retm = model->evaluate(b.acc.getOutputVectorWhite(), b.acc.getOutputVectorBlack(), board.toMove, std::max(0, bucket));

        const int m = 41  * (board.pieceCount[0] + board.pieceCount[1])
                    + 218 * (board.pieceCount[2] + board.pieceCount[3])
                    + 229 * (board.pieceCount[4] + board.pieceCount[5])
                    + 331 * (board.pieceCount[6] + board.pieceCount[7])
                    + 656 * (board.pieceCount[8] + board.pieceCount[9]);

        retm = retm * (14746 + m) / 16384;
        retm = std::clamp(retm, -MATE_VALUE_MAX + 1, MATE_VALUE_MAX - 1);

        return retm;
    }


}







//
