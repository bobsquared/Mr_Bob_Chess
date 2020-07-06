#include "eval.h"



Eval::Eval() {
    InitPieceBoards();
    InitKingZoneMask();
    InitPassedPawnsMask();
    InitForwardBackwardMask();
    InitDistanceArray();
    InitIsolatedPawnsMask();
    InitOutpostMask();
}



void Eval::InitKingZoneMask() {

    for (int i = 0; i < 64; i++) {
        uint64_t tempBitBoard = 0;
        uint64_t tempBitBoard1 = 0;
        uint64_t tempBitBoard2 = 0;

        tempBitBoard = 1ULL << i;

        tempBitBoard |= tempBitBoard >> 8;
        tempBitBoard |= tempBitBoard << 8;

        tempBitBoard1 |= tempBitBoard >> 1;
        tempBitBoard1 &= ~columnMask[7];

        tempBitBoard2 |= tempBitBoard << 1;
        tempBitBoard2 &= ~columnMask[0];

        tempBitBoard |= tempBitBoard1 | tempBitBoard2;
        tempBitBoard &= ~(1ULL << i);

        kingZoneMask[0][i] = tempBitBoard | (1ULL << i);
        // kingZoneMaskWhite[i] |= kingZoneMaskWhite[i] << 8;
        // kingZoneMaskWhite[i] |= kingZoneMaskWhite[i] << 8;
        // kingZoneMaskWhite[i] |= kingZoneMaskWhite[i] << 8;

        kingZoneMask[1][i] = tempBitBoard | (1ULL << i);
        // kingZoneMaskBlack[i] |= kingZoneMaskBlack[i] >> 8;
        // kingZoneMaskBlack[i] |= kingZoneMaskBlack[i] >> 8;
        // kingZoneMaskBlack[i] |= kingZoneMaskBlack[i] >> 8;
    }

}



// Piece square tables
void Eval::InitPieceBoards() {

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            pieceSquare[0][i * 8 + j] = WHITE_PAWN_TABLE[(7 - i) * 8 + j];
            pieceSquare[1][i * 8 + j] = BLACK_PAWN_TABLE[(7 - i) * 8 + j];
            pieceSquare[2][i * 8 + j] = WHITE_KNIGHT_TABLE[(7 - i) * 8 + j];
            pieceSquare[3][i * 8 + j] = BLACK_KNIGHT_TABLE[(7 - i) * 8 + j];
            pieceSquare[4][i * 8 + j] = WHITE_BISHOP_TABLE[(7 - i) * 8 + j];
            pieceSquare[5][i * 8 + j] = BLACK_BISHOP_TABLE[(7 - i) * 8 + j];
            pieceSquare[6][i * 8 + j] = WHITE_ROOK_TABLE[(7 - i) * 8 + j];
            pieceSquare[7][i * 8 + j] = BLACK_ROOK_TABLE[(7 - i) * 8 + j];
            pieceSquare[8][i * 8 + j] = WHITE_QUEEN_TABLE[(7 - i) * 8 + j];
            pieceSquare[9][i * 8 + j] = BLACK_QUEEN_TABLE[(7 - i) * 8 + j];
            pieceSquare[10][i * 8 + j] = WHITE_KING_TABLE[(7 - i) * 8 + j];
            pieceSquare[11][i * 8 + j] = BLACK_KING_TABLE[(7 - i) * 8 + j];

            pieceSquareEG[0][i * 8 + j] = WHITE_PAWN_TABLE[(7 - i) * 8 + j];
            pieceSquareEG[1][i * 8 + j] = BLACK_PAWN_TABLE[(7 - i) * 8 + j];
            pieceSquareEG[2][i * 8 + j] = WHITE_KNIGHT_TABLE[(7 - i) * 8 + j];
            pieceSquareEG[3][i * 8 + j] = BLACK_KNIGHT_TABLE[(7 - i) * 8 + j];
            pieceSquareEG[4][i * 8 + j] = WHITE_BISHOP_TABLE[(7 - i) * 8 + j];
            pieceSquareEG[5][i * 8 + j] = BLACK_BISHOP_TABLE[(7 - i) * 8 + j];
            pieceSquareEG[6][i * 8 + j] = WHITE_ROOK_TABLE[(7 - i) * 8 + j];
            pieceSquareEG[7][i * 8 + j] = BLACK_ROOK_TABLE[(7 - i) * 8 + j];
            pieceSquareEG[8][i * 8 + j] = WHITE_QUEEN_TABLE[(7 - i) * 8 + j];
            pieceSquareEG[9][i * 8 + j] = BLACK_QUEEN_TABLE[(7 - i) * 8 + j];
            pieceSquareEG[10][i * 8 + j] = WHITE_KING_TABLE_EG[(7 - i) * 8 + j];
            pieceSquareEG[11][i * 8 + j] = BLACK_KING_TABLE_EG[(7 - i) * 8 + j];

        }
    }

}



void Eval::InitPassedPawnsMask() {

  for (int i = 0; i < 64; i++) {

    passedPawnMask[0][i] = 0;
    passedPawnMask[1][i] = 0;

    if (i % 8 == 0) {

      if (i + 8 <= 63) {
        passedPawnMask[0][i] |= 1ULL << (i + 8);
        passedPawnMask[0][i] |= passedPawnMask[0][i] << 1;
      }

      if (i - 8 >= 0) {
        passedPawnMask[1][i] |= 1ULL << (i - 8);
        passedPawnMask[1][i] |= passedPawnMask[1][i] << 1;
      }

    }
    else if (i % 8 == 7) {

      if (i + 8 <= 63) {
        passedPawnMask[0][i] |= 1ULL << (i + 8);
        passedPawnMask[0][i] |= passedPawnMask[0][i] >> 1;
      }

      if (i - 8 >= 0) {
        passedPawnMask[1][i] |= 1ULL << (i - 8);
        passedPawnMask[1][i] |= passedPawnMask[1][i] >> 1;
      }
    }
    else {

      if (i + 8 <= 63) {
        passedPawnMask[0][i] |= 1ULL << (i + 8);
        passedPawnMask[0][i] |= passedPawnMask[0][i] << 1;
        passedPawnMask[0][i] |= passedPawnMask[0][i] >> 1;
      }

      if (i - 8 >= 0) {
        passedPawnMask[1][i] |= 1ULL << (i - 8);
        passedPawnMask[1][i] |= passedPawnMask[1][i] << 1;
        passedPawnMask[1][i] |= passedPawnMask[1][i] >> 1;
      }
    }

    passedPawnMask[0][i] |= passedPawnMask[0][i] << 8;
    passedPawnMask[0][i] |= passedPawnMask[0][i] << 16;
    passedPawnMask[0][i] |= passedPawnMask[0][i] << 32;

    passedPawnMask[1][i] |= passedPawnMask[1][i] >> 8;
    passedPawnMask[1][i] |= passedPawnMask[1][i] >> 16;
    passedPawnMask[1][i] |= passedPawnMask[1][i] >> 32;

  }

}



// Initialize the forward and backwards masks bitboard
void Eval::InitForwardBackwardMask() {

    for (int i = 0; i < 64; i++) {

        forwardMask[0][i] = 1ULL << i;
        forwardMask[1][i] = 1ULL << i;

        forwardMask[0][i] |= forwardMask[0][i] << 8;
        forwardMask[0][i] |= forwardMask[0][i] << 16;
        forwardMask[0][i] |= forwardMask[0][i] << 32;

        forwardMask[1][i] |= forwardMask[1][i] >> 8;
        forwardMask[1][i] |= forwardMask[1][i] >> 16;
        forwardMask[1][i] |= forwardMask[1][i] >> 32;

        forwardMask[0][i] ^= 1ULL << i;
        forwardMask[1][i] ^= 1ULL << i;

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



// Initialize the Isolated Pawns arrays
void Eval::InitIsolatedPawnsMask() {

  for (int i = 0; i < 64; i++) {

    isolatedPawnMask[i] = 0;

    if (i % 8 == 0) {
      isolatedPawnMask[i] |= 1ULL << (i + 1);
    }
    else if (i % 8 == 7) {
      isolatedPawnMask[i] |= 1ULL << (i - 1);
    }
    else {
      isolatedPawnMask[i] |= 1ULL << (i + 1);
      isolatedPawnMask[i] |= 1ULL << (i - 1);
    }

    isolatedPawnMask[i] |= isolatedPawnMask[i] << 8;
    isolatedPawnMask[i] |= isolatedPawnMask[i] << 16;
    isolatedPawnMask[i] |= isolatedPawnMask[i] << 32;

    isolatedPawnMask[i] |= isolatedPawnMask[i] >> 8;
    isolatedPawnMask[i] |= isolatedPawnMask[i] >> 16;
    isolatedPawnMask[i] |= isolatedPawnMask[i] >> 32;

  }

}



// Initialize the Outpost detection mask arrays
void Eval::InitOutpostMask() {

  for (int i = 0; i < 64; i++) {
    outpostMask[0][i] = isolatedPawnMask[i] & passedPawnMask[0][i];
    outpostMask[1][i] = isolatedPawnMask[i] & passedPawnMask[1][i];
  }

}



// Find all adjacent pawns
uint64_t Eval::adjacentMask(uint64_t pawns) {
    uint64_t ret = (pawns << 1) & ~columnMask[0];
    return ret | ((pawns >> 1) & ~columnMask[7]);
}



// Evaluate the position
int Eval::evaluate(int *material, uint64_t *pieces, Magics *magics, uint64_t *knightMoves, int *pieceCount, uint64_t occupied, bool col) {

    // Asserts for debugging mode
    #ifndef NDEBUG
    int debugMaterialCount = 0;
    int pawnCount = count_population(pieces[0]);
    int knightCount = count_population(pieces[2]);
    int bishopCount = count_population(pieces[4]);
    int rookCount = count_population(pieces[6]);
    int queenCount = count_population(pieces[8]);
    int kingCount = count_population(pieces[10]);
    debugMaterialCount += pawnCount * pieceValues[0];
    debugMaterialCount += knightCount * pieceValues[1];
    debugMaterialCount += bishopCount * pieceValues[2];
    debugMaterialCount += rookCount * pieceValues[3];
    debugMaterialCount += queenCount * pieceValues[4];
    debugMaterialCount += kingCount * pieceValues[5];
    assert(debugMaterialCount == material[0]);
    assert(pawnCount == pieceCount[0]);
    assert(knightCount == pieceCount[2]);
    assert(bishopCount == pieceCount[4]);
    assert(rookCount == pieceCount[6]);
    assert(queenCount == pieceCount[8]);
    assert(kingCount == pieceCount[10]);

    pawnCount = count_population(pieces[1]);
    knightCount = count_population(pieces[3]);
    bishopCount = count_population(pieces[5]);
    rookCount = count_population(pieces[7]);
    queenCount = count_population(pieces[9]);
    kingCount = count_population(pieces[11]);
    debugMaterialCount = pawnCount * pieceValues[0];
    debugMaterialCount += knightCount * pieceValues[1];
    debugMaterialCount += bishopCount * pieceValues[2];
    debugMaterialCount += rookCount * pieceValues[3];
    debugMaterialCount += queenCount * pieceValues[4];
    debugMaterialCount += kingCount * pieceValues[5];
    assert(debugMaterialCount == material[1]);
    assert(pawnCount == pieceCount[1]);
    assert(knightCount == pieceCount[3]);
    assert(bishopCount == pieceCount[5]);
    assert(rookCount == pieceCount[7]);
    assert(queenCount == pieceCount[9]);
    assert(kingCount == pieceCount[11]);
    #endif


    int ret = material[0] - material[1];
    ret += evaluateTrappedRook(pieces, false) - evaluateTrappedRook(pieces, true);
    ret += evaluateMobility(pieces, magics, knightMoves, occupied, false) - evaluateMobility(pieces, magics, knightMoves, occupied, true);
    ret += evaluateImbalance(pieceCount, false) - evaluateImbalance(pieceCount, true);
    ret += evaluatePawns(pieces, false) - evaluatePawns(pieces, true);
    ret += evaluatePassedPawns(pieces, false) - evaluatePassedPawns(pieces, true);
    ret += col? -16 : 16;


    int evalMidgame = ret;
    int evalEndgame = ret;


    evalMidgame += evaluate_piece_square_values(pieces, false, false) - evaluate_piece_square_values(pieces, false, true);
    evalMidgame += evaluateKingSafety(pieces, magics, knightMoves, occupied, false) - evaluateKingSafety(pieces, magics, knightMoves, occupied, true);
    evalEndgame += evaluate_piece_square_values(pieces, true, false) - evaluate_piece_square_values(pieces, true, true);

    int phase = TOTALPHASE;
    phase -= (pieceCount[0] + pieceCount[1]) * PAWNPHASE;
    phase -= (pieceCount[2] + pieceCount[3]) * KNIGHTPHASE;
    phase -= (pieceCount[4] + pieceCount[5]) * BISHOPPHASE;
    phase -= (pieceCount[6] + pieceCount[7]) * ROOKPHASE;
    phase -= (pieceCount[8] + pieceCount[9]) * QUEENPHASE;

    phase = (phase * 256 + (TOTALPHASE / 2)) / TOTALPHASE;
    ret = ((evalMidgame * (256 - phase)) + (evalEndgame * phase)) / 256;

    return ret;
}



// Evaluate the position with debugging
int Eval::evaluate_debug(int *material, uint64_t *pieces, Magics *magics, uint64_t *knightMoves, int *pieceCount, uint64_t occupied) {

    #ifndef NDEBUG
    int debugMaterialCount = 0;
    int pawnCount = count_population(pieces[0]);
    int knightCount = count_population(pieces[2]);
    int bishopCount = count_population(pieces[4]);
    int rookCount = count_population(pieces[6]);
    int queenCount = count_population(pieces[8]);
    int kingCount = count_population(pieces[10]);
    debugMaterialCount += pawnCount * pieceValues[0];
    debugMaterialCount += knightCount * pieceValues[1];
    debugMaterialCount += bishopCount * pieceValues[2];
    debugMaterialCount += rookCount * pieceValues[3];
    debugMaterialCount += queenCount * pieceValues[4];
    debugMaterialCount += kingCount * pieceValues[5];
    assert(debugMaterialCount == material[0]);
    assert(pawnCount == pieceCount[0]);
    assert(knightCount == pieceCount[2]);
    assert(bishopCount == pieceCount[4]);
    assert(rookCount == pieceCount[6]);
    assert(queenCount == pieceCount[8]);
    assert(kingCount == pieceCount[10]);

    pawnCount = count_population(pieces[1]);
    knightCount = count_population(pieces[3]);
    bishopCount = count_population(pieces[5]);
    rookCount = count_population(pieces[7]);
    queenCount = count_population(pieces[9]);
    kingCount = count_population(pieces[11]);
    debugMaterialCount = pawnCount * pieceValues[0];
    debugMaterialCount += knightCount * pieceValues[1];
    debugMaterialCount += bishopCount * pieceValues[2];
    debugMaterialCount += rookCount * pieceValues[3];
    debugMaterialCount += queenCount * pieceValues[4];
    debugMaterialCount += kingCount * pieceValues[5];
    assert(debugMaterialCount == material[1]);
    assert(pawnCount == pieceCount[1]);
    assert(knightCount == pieceCount[3]);
    assert(bishopCount == pieceCount[5]);
    assert(rookCount == pieceCount[7]);
    assert(queenCount == pieceCount[9]);
    assert(kingCount == pieceCount[11]);
    #endif


    int ret = material[0] - material[1];

    int evalMidgame = ret;
    int evalEndgame = ret;

    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "White piece square: " << evaluate_piece_square_values(pieces, false, false) << std::endl;
    std::cout << "White trapped rook: " << evaluateTrappedRook(pieces, false) << std::endl;
    std::cout << "White mobility: " << evaluateMobility(pieces, magics, knightMoves, occupied, false) << std::endl;
    std::cout << "White safety: " << evaluateKingSafety(pieces, magics, knightMoves, occupied, false) << std::endl;
    std::cout << "White imbalance: " << evaluateImbalance(pieceCount, false) << std::endl;
    std::cout << "White pawns: " << evaluatePawns(pieces, false) << std::endl;
    std::cout << "White passed pawns: " << evaluatePassedPawns(pieces, false) << std::endl;
    std::cout << "White outposts: " << evaluateOutposts(pieces, false) << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "Black piece square: " << evaluate_piece_square_values(pieces, false, true) << std::endl;
    std::cout << "Black trapped rook: " << evaluateTrappedRook(pieces, true) << std::endl;
    std::cout << "Black mobility: " << evaluateMobility(pieces, magics, knightMoves, occupied, true) << std::endl;
    std::cout << "Black safety: " << evaluateKingSafety(pieces, magics, knightMoves, occupied, true) << std::endl;
    std::cout << "Black imbalance: " << evaluateImbalance(pieceCount, true) << std::endl;
    std::cout << "Black pawns: " << evaluatePawns(pieces, true) << std::endl;
    std::cout << "Black passed pawns: " << evaluatePassedPawns(pieces, true) << std::endl;
    std::cout << "Black outposts: " << evaluateOutposts(pieces, true) << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "All trapped rook: " << evaluateTrappedRook(pieces, false) - evaluateTrappedRook(pieces, true) << std::endl;
    std::cout << "All piece square: " << evaluate_piece_square_values(pieces, false, false) - evaluate_piece_square_values(pieces, false, true) << std::endl;
    std::cout << "All mobility: " << evaluateMobility(pieces, magics, knightMoves, occupied, false) - evaluateMobility(pieces, magics, knightMoves, occupied, true) << std::endl;
    std::cout << "All safety: " << evaluateKingSafety(pieces, magics, knightMoves, occupied, false) - evaluateKingSafety(pieces, magics, knightMoves, occupied, true) << std::endl;
    std::cout << "All imbalance: " << evaluateImbalance(pieceCount, false) - evaluateImbalance(pieceCount, true) << std::endl;
    std::cout << "All pawns: " << evaluatePawns(pieces, false) - evaluatePawns(pieces, true) << std::endl;
    std::cout << "All passed pawns: " << evaluatePassedPawns(pieces, false) - evaluatePassedPawns(pieces, true) << std::endl;
    std::cout << "All outposts: " << evaluateOutposts(pieces, false) - evaluateOutposts(pieces, true) << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;

    int phase = TOTALPHASE;
    phase -= (pieceCount[0] + pieceCount[1]) * PAWNPHASE;
    phase -= (pieceCount[2] + pieceCount[3]) * KNIGHTPHASE;
    phase -= (pieceCount[4] + pieceCount[5]) * BISHOPPHASE;
    phase -= (pieceCount[6] + pieceCount[7]) * ROOKPHASE;
    phase -= (pieceCount[8] + pieceCount[9]) * QUEENPHASE;

    phase = (phase * 256 + (TOTALPHASE / 2)) / TOTALPHASE;
    ret = ((evalMidgame * (256 - phase)) + (evalEndgame * phase)) / 256;

    return ret;
}



// Evaluate piece square values
int Eval::evaluate_piece_square_values(uint64_t *pieces, bool eg, bool col) {

    int ret = 0;
    uint64_t piece;
    int (*psqtb)[64] = eg? pieceSquareEG : pieceSquare;

    for (int i = col; i < 12; i += 2) {
        piece = pieces[i];
        while (piece) {
            ret += psqtb[i][bitScan(piece)];
            piece &= piece - 1;
        }
    }

    return ret;
}



// Evaluate mobility
int Eval::evaluateMobility(uint64_t *pieces, Magics *magics, uint64_t *knightMoves, uint64_t occupied, bool col) {

    int ret = 0;
    uint64_t unsafeSquares = pawnAttacksAll(pieces[!col], !col) | pieces[col] | pieces[10 + col];
    uint64_t minorUnsafe = unsafeSquares | pieces[8 + col];
    uint64_t queenUnsafe = unsafeSquares | knightAttacks(pieces[2 + !col]);


    uint64_t piece = pieces[2 + col];
    while (piece) {
        ret += knightMobilityBonus[count_population(knightMoves[bitScan(piece)]) & ~minorUnsafe];
        piece &= piece - 1;
    }

    piece = pieces[4 + col];
    while (piece) {
        ret += bishopMobilityBonus[count_population(magics->bishopAttacksMask(occupied ^ pieces[8 + col], bitScan(piece)) & ~minorUnsafe)];
        piece &= piece - 1;
    }

    piece = pieces[6 + col];
    while (piece) {
        ret += rookMobilityBonus[count_population(magics->rookAttacksMask(occupied ^ pieces[8 + col], bitScan(piece)) & ~unsafeSquares)];
        piece &= piece - 1;
    }

    piece = pieces[8 + col];
    while (piece) {
        ret += queenMobilityBonus[count_population(magics->queenAttacksMask(occupied, bitScan(piece)) & ~queenUnsafe)];
        piece &= piece - 1;
    }

    return ret;
}



// Evaluate mobility
int Eval::evaluateKingSafety(uint64_t *pieces, Magics *magics, uint64_t *knightMoves, uint64_t occupied, bool col) {

    if (pieces[8 + col] == 0) {
        return 0;
    }

    int ret = 0;
    int attackers = 0;
    uint64_t unsafeSquares = kingZoneMask[col][bitScan(pieces[10 + !col])] & ~(pawnAttacksAll(pieces[!col], !col) | knightAttacks(pieces[2 + !col]) | pieces[col]);

    uint64_t piece = pieces[2 + col];
    while (piece) {
        int attacks = count_population(knightMoves[bitScan(piece)] & unsafeSquares);
        if (attacks) {
            ret += attacks * pieceAttackValue[1];
            attackers++;
        }
        piece &= piece - 1;
    }

    piece = pieces[4 + col];
    while (piece) {
        int attacks = count_population(magics->bishopAttacksMask(occupied ^ pieces[8 + col], bitScan(piece)) & unsafeSquares);
        if (attacks) {
            ret += attacks * pieceAttackValue[2];
            attackers++;
        }
        piece &= piece - 1;
    }

    piece = pieces[6 + col];
    while (piece) {
        int attacks = count_population(magics->rookAttacksMask(occupied ^ pieces[8 + col], bitScan(piece)) & unsafeSquares);
        if (attacks) {
            ret += attacks * pieceAttackValue[3];
            attackers++;
        }
        piece &= piece - 1;
    }

    piece = pieces[8 + col];
    while (piece) {
        int attacks = count_population(magics->queenAttacksMask(occupied, bitScan(piece)) & unsafeSquares);
        if (attacks) {
            ret += attacks * pieceAttackValue[4];
            attackers++;
        }
        piece &= piece - 1;
    }

    attackers = std::min(attackers, 7);
    ret = ret * pieceAttackWeight[attackers] / 100;
    return ret;
}



// Evaluate trapped rook
int Eval::evaluateTrappedRook(uint64_t *pieces, bool col) {

    int ret = 0;
    uint64_t piece = pieces[6 + col];

    while (piece) {
        uint64_t pieceLoc = piece & -piece;
        if (rowMask[col * 56] & pieceLoc) {
            if (pieces[col + 10] > 1ULL << (3 + (col * 56)) && pieceLoc > pieces[col + 10]) {
                ret -= 40;
            }
            if (pieces[col + 10] < 1ULL << (4 + (col * 56)) && pieceLoc < pieces[col + 10]) {
                ret -= 40;
            }
        }
        piece &= piece - 1;
    }

    return ret;
}



// Evaluate material imbalance
int Eval::evaluateImbalance(int *pieceCount, bool col) {

    int ret = 0;

    // Bishop pair
    if (pieceCount[4 + col] >= 2) {
        ret += 35;
    }

    // Knight pair
    if (pieceCount[2 + col] >= 2) {
        ret -= 19;
    }

    // Pawn count
    if (pieceCount[col] == 0) {
        ret -= 87;
    }

    ret += knightWeight[pieceCount[0 + col]] * pieceCount[2 + col];
    ret += rookWeight[pieceCount[0 + col]] * pieceCount[6 + col];

    return ret;
}



int Eval::evaluatePawns(uint64_t *pieces, bool col) {

    int ret = 0;
    uint64_t supportedPawns = pieces[col] & pawnAttacksAll(pieces[col], col);
    uint64_t adjacentPawns = pieces[col] & adjacentMask(pieces[col]);
    uint64_t doubledPawns = col? ((pieces[col] ^ supportedPawns) << 8) & pieces[col] : ((pieces[col] ^ supportedPawns) >> 8) & pieces[col];

    ret -= 18 * count_population(doubledPawns);

    uint64_t piece = pieces[col] & (!supportedPawns & !adjacentPawns);
    while (piece) {
        int bscan = bitScan(piece);
        if ((isolatedPawnMask[bscan] & pieces[col]) == 0) {
            ret -= 12;
        }
        piece &= piece - 1;
    }

    while (supportedPawns) {
        int bscan = bitScan(supportedPawns) / 8;
        ret += col? (7 - bscan) * 5 : (bscan) * 5;
        supportedPawns &= supportedPawns - 1;
    }

    while (adjacentPawns) {
        int bscan = bitScan(adjacentPawns) / 8;
        ret += col? (7 - bscan) * 3 : (bscan) * 3;
        adjacentPawns &= adjacentPawns - 1;
    }

    return ret;

}



int Eval::evaluatePassedPawns(uint64_t *pieces, bool col) {

    int ret = 0;
    uint64_t piece = pieces[col];
    while (piece) {
        int bscan = bitScan(piece);
        if ((passedPawnMask[col][bscan] & pieces[!col]) == 0 && (forwardMask[col][bscan] & pieces[col]) == 0) {
            ret += col? (7 - (bscan / 8)) * 12 : (bscan / 8) * 12;
        }
        piece &= piece - 1;
    }

    return ret;

}



// ELO LOSS FOR NOW????
int Eval::evaluateOutposts(uint64_t *pieces, bool col) {

    int ret = 0;
    uint64_t piece = pieces[2 + col];
    uint64_t holes = pawnAttacksAll(pieces[col], col);

    while (holes) {
        int bscan = bitScan(holes);
        if ((outpostMask[col][bscan] & pieces[!col]) == 0 && (piece & (1ULL << bscan))) {
            ret += outpostPotential[col][bscan];
        }
        holes &= holes - 1;
    }

    return ret;

}
