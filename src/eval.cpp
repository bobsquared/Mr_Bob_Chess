#include "eval.h"

Eval::Eval(KPNNUE *model) : model(model) {
    InitLightSquares();
    InitDistanceArray();
}


void Eval::InitLightSquares() {
    lightSquares = 0;
    for (int i = 0; i < 64; i++) {
        if (((i / 8) + (i % 8)) % 2) {
            lightSquares |= 1ULL << i;
        }
    }
}



// Initialize the distance arrays
void Eval::InitDistanceArray() {

  for (int i = 0; i < 64; i++) {
    for (int j = 0; j < 64; j++) {
      int colI = i % 8;
      int rowI = i / 8;
      int colJ = j % 8;
      int rowJ = j / 8;
      manhattanArray[i][j] = std::abs(colI - colJ) + std::abs(rowI - rowJ);
      chebyshevArray[i][j] = std::max(abs(colI - colJ), std::abs(rowI - rowJ));
    }
  }

}



// get the Phase value
int Eval::getPhase(Bitboard &board) {
    int phase = TOTALPHASE;
    phase -= (board.pieceCount[0] + board.pieceCount[1]) * PAWNPHASE;
    phase -= (board.pieceCount[2] + board.pieceCount[3]) * KNIGHTPHASE;
    phase -= (board.pieceCount[4] + board.pieceCount[5]) * BISHOPPHASE;
    phase -= (board.pieceCount[6] + board.pieceCount[7]) * ROOKPHASE;
    phase -= (board.pieceCount[8] + board.pieceCount[9]) * QUEENPHASE;

    return (phase * 256 + (TOTALPHASE / 2)) / TOTALPHASE;
}



int Eval::scaleEndgame(Bitboard &board, int eval) {

    uint64_t pawns = board.pieces[0] | board.pieces[1];
    uint64_t knights = board.pieces[2] | board.pieces[3];
    uint64_t bishops = board.pieces[4] | board.pieces[5];
    uint64_t rooks = board.pieces[6] | board.pieces[7];
    uint64_t queens = board.pieces[8] | board.pieces[9];

    bool attackingColor = eval < 0;
    uint64_t attacking = board.color[attackingColor];
    uint64_t defending = board.color[!attackingColor];
    uint64_t minors = knights | bishops;
    uint64_t majors = rooks | queens;
    uint64_t minorAndMajors = minors | majors;

    int numMajorMinors = count_population(minorAndMajors);
    int ret = 256;

    // Opposite color bishop
    if (check_bit(board.pieces[4]) && check_bit(board.pieces[5]) && check_bit(bishops & lightSquares) && minorAndMajors == bishops) {
        return 212;
    }

    if ((attacking & pawns) == 0) {
        // KR vs KRM
        if (queens == 0 && numMajorMinors == 3 && check_bit(board.pieces[6]) && check_bit(board.pieces[7])) {
            return 0;
        }

        // KR vs KB
        if (queens == 0 && numMajorMinors == 2 && check_bit(bishops) && check_bit(board.color[0] & minorAndMajors) ) {
            return 0;
        }

        // KR vs KN
        if (queens == 0 && numMajorMinors == 2 && check_bit(knights) && check_bit(board.color[0] & minorAndMajors) ) {
            return 115;
        }

        // K vs KNN
        if (numMajorMinors == 2 && minorAndMajors == board.pieces[2 + attackingColor]) {
            return 0;
        }

        // KB vs KNN or KN vs KNN
        if (numMajorMinors == 3 && check_bit(defending & minors) && (defending & majors) == 0 && (attacking & minorAndMajors) == board.pieces[2 + attackingColor]) {
            return 0;
        }
    }

    if ((defending & pawns) == 0 && check_bit(defending)) {
        ret += 16 * (7 - chebyshevArray[board.kingLoc[0]][board.kingLoc[1]]);
        if (check_bit(board.pieces[4 + attackingColor])) {
            if (lightSquares & board.pieces[4 + attackingColor]) {
                int brDistance = manhattanArray[board.kingLoc[!attackingColor]][7];
                int tlDistance = manhattanArray[board.kingLoc[!attackingColor]][56];
                ret += 16 * (7 - std::min(brDistance, tlDistance));
            }
            if (~lightSquares & board.pieces[4 + attackingColor]) {
                int brDistance = manhattanArray[board.kingLoc[!attackingColor]][0];
                int tlDistance = manhattanArray[board.kingLoc[!attackingColor]][63];
                ret += 16 * (7 - std::min(brDistance, tlDistance));
            }
        }
    }

    return ret;

}



// Evaluate the position
int Eval::evaluate(Bitboard &board) {

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

    int retm = model->evaluate(board);
    
    return retm;

}










//
