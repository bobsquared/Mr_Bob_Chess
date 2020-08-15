#include "eval.h"



int pieceValues[6] = {S(85, 100), S(305, 305), S(305, 305), S(475, 535), S(925, 985), S(2000, 2000)};
// int pieceValues[6] = {S(157, 173), S(423, 390), S(422, 401), S(622, 694), S(974, 1211), S(2000, 2000)};



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
        // kingZoneMask[0][i] |= kingZoneMask[0][i] << 8;
        // kingZoneMask[0][i] |= kingZoneMask[0][i] >> 8;
        // kingZoneMaskWhite[i] |= kingZoneMaskWhite[i] << 8;

        kingZoneMask[1][i] = tempBitBoard | (1ULL << i);
        // kingZoneMask[1][i] |= kingZoneMask[1][i] << 8;
        // kingZoneMask[1][i] |= kingZoneMask[1][i] >> 8;
        // kingZoneMaskBlack[i] |= kingZoneMaskBlack[i] >> 8;
    }

}



// Piece square tables
void Eval::InitPieceBoards() {

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            pieceSquare[0][i * 8 + j] = S(WHITE_PAWN_TABLE[(7 - i) * 8 + j], WHITE_PAWN_TABLE[(7 - i) * 8 + j]);
            pieceSquare[1][i * 8 + j] = S(BLACK_PAWN_TABLE[(7 - i) * 8 + j], BLACK_PAWN_TABLE[(7 - i) * 8 + j]);
            pieceSquare[2][i * 8 + j] = S(WHITE_KNIGHT_TABLE[(7 - i) * 8 + j], WHITE_KNIGHT_TABLE[(7 - i) * 8 + j]);
            pieceSquare[3][i * 8 + j] = S(BLACK_KNIGHT_TABLE[(7 - i) * 8 + j], BLACK_KNIGHT_TABLE[(7 - i) * 8 + j]);
            pieceSquare[4][i * 8 + j] = S(WHITE_BISHOP_TABLE[(7 - i) * 8 + j], WHITE_BISHOP_TABLE[(7 - i) * 8 + j]);
            pieceSquare[5][i * 8 + j] = S(BLACK_BISHOP_TABLE[(7 - i) * 8 + j], BLACK_BISHOP_TABLE[(7 - i) * 8 + j]);
            pieceSquare[6][i * 8 + j] = S(WHITE_ROOK_TABLE[(7 - i) * 8 + j], WHITE_ROOK_TABLE[(7 - i) * 8 + j]);
            pieceSquare[7][i * 8 + j] = S(BLACK_ROOK_TABLE[(7 - i) * 8 + j], BLACK_ROOK_TABLE[(7 - i) * 8 + j]);
            pieceSquare[8][i * 8 + j] = S(WHITE_QUEEN_TABLE[(7 - i) * 8 + j], WHITE_QUEEN_TABLE[(7 - i) * 8 + j]);
            pieceSquare[9][i * 8 + j] = S(BLACK_QUEEN_TABLE[(7 - i) * 8 + j], BLACK_QUEEN_TABLE[(7 - i) * 8 + j]);
            pieceSquare[10][i * 8 + j] = S(WHITE_KING_TABLE[(7 - i) * 8 + j], WHITE_KING_TABLE_EG[(7 - i) * 8 + j]);
            pieceSquare[11][i * 8 + j] = S(BLACK_KING_TABLE[(7 - i) * 8 + j], BLACK_KING_TABLE_EG[(7 - i) * 8 + j]);

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
int Eval::evaluate(int *material, uint64_t *pieces, Magics *magics, uint64_t *knightMoves, int *pieceCount, uint64_t occ, bool col) {

    // Asserts for debugging mode
    #ifndef NDEBUG
    int debugMaterialCount = 0;
    int pawnCount = count_population(pieces[0]);
    int knightCount = count_population(pieces[2]);
    int bishopCount = count_population(pieces[4]);
    int rookCount = count_population(pieces[6]);
    int queenCount = count_population(pieces[8]);
    int kingCount = count_population(pieces[10]);
    debugMaterialCount += pawnCount * MGVAL(pieceValues[0]);
    debugMaterialCount += knightCount * MGVAL(pieceValues[1]);
    debugMaterialCount += bishopCount * MGVAL(pieceValues[2]);
    debugMaterialCount += rookCount * MGVAL(pieceValues[3]);
    debugMaterialCount += queenCount * MGVAL(pieceValues[4]);
    debugMaterialCount += kingCount * MGVAL(pieceValues[5]);
    assert(debugMaterialCount == MGVAL(material[0]));
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
    debugMaterialCount = pawnCount * EGVAL(pieceValues[0]);
    debugMaterialCount += knightCount * EGVAL(pieceValues[1]);
    debugMaterialCount += bishopCount * EGVAL(pieceValues[2]);
    debugMaterialCount += rookCount * EGVAL(pieceValues[3]);
    debugMaterialCount += queenCount * EGVAL(pieceValues[4]);
    debugMaterialCount += kingCount * EGVAL(pieceValues[5]);
    assert(debugMaterialCount == EGVAL(material[1]));
    assert(pawnCount == pieceCount[1]);
    assert(knightCount == pieceCount[3]);
    assert(bishopCount == pieceCount[5]);
    assert(rookCount == pieceCount[7]);
    assert(queenCount == pieceCount[9]);
    assert(kingCount == pieceCount[11]);
    #endif


    // King safety
    unsafeSquares[0] = 0;
    unsafeSquares[1] = 0;

    KSAttackersWeight[0] = 0;
    KSAttackersWeight[1] = 0;

    KSAttackersCount[1] = 0;
    KSAttackersCount[0] = 0;

    uint64_t pawnAttacksW = pawnAttacksAll(pieces[0], 0);
    uint64_t pawnAttacksB = pawnAttacksAll(pieces[1], 1);

    uint64_t knightAttacksW = knightAttacks(pieces[2]);
    uint64_t knightAttacksB = knightAttacks(pieces[3]);

    // Mobility
    mobilityUnsafeSquares[0] = pawnAttacksB | pieces[0] | pieces[10];
    mobilityUnsafeSquares[1] = pawnAttacksW | pieces[1] | pieces[11];

    minorUnsafe[0] = mobilityUnsafeSquares[0] | pieces[8];
    minorUnsafe[1] = mobilityUnsafeSquares[1] | pieces[9];

    queenUnsafe[0] = mobilityUnsafeSquares[0] | knightAttacksB;
    queenUnsafe[1] = mobilityUnsafeSquares[1] | knightAttacksW;

    tempUnsafe[0] = ~(pawnAttacksB | knightAttacksB | pieces[0]) & kingZoneMask[0][bitScan(pieces[11])];
    tempUnsafe[1] = ~(pawnAttacksW | knightAttacksW | pieces[1]) & kingZoneMask[1][bitScan(pieces[10])];
    occupied = occ;

    int ret = 0;
    ret += evaluateTrappedRook(pieces, false) - evaluateTrappedRook(pieces, true);
    ret += col? -16 : 16;

    int evalMidgame = ret;
    int evalEndgame = ret;
    int pieceSquareEval = evaluate_piece_square_values(pieces, false) - evaluate_piece_square_values(pieces, true);
    int passedPawnsEval = evaluatePassedPawns(pieces, false) - evaluatePassedPawns(pieces, true);
    int pawnsEval = evaluatePawns(pieces, false) - evaluatePawns(pieces, true);
    int imbalanceEval = evaluateImbalance(pieceCount, false) - evaluateImbalance(pieceCount, true);
    int pawnShieldEval = evaluatePawnShield(pieces, false) - evaluatePawnShield(pieces, true);
    int knightsEval = evaluateKnights(pieces, knightMoves, false) - evaluateKnights(pieces, knightMoves, true);
    int bishopsEval = evaluateBishops(pieces, magics, false) - evaluateBishops(pieces, magics, true);
    int rooksEval = evaluateRooks(pieces, magics, false) - evaluateRooks(pieces, magics, true);
    int queensEval = evaluateQueens(pieces, magics, false) - evaluateQueens(pieces, magics, true);

    evalMidgame += MGVAL(material[0] - material[1]);
    evalMidgame += MGVAL(pieceSquareEval);
    evalMidgame += MGVAL(passedPawnsEval);
    evalMidgame += MGVAL(pawnsEval);
    evalMidgame += MGVAL(imbalanceEval);
    evalMidgame += MGVAL(knightsEval);
    evalMidgame += MGVAL(bishopsEval);
    evalMidgame += MGVAL(rooksEval);
    evalMidgame += MGVAL(queensEval);
    evalMidgame += MGVAL(pawnShieldEval);

    KSAttackersCount[0] = std::min(KSAttackersCount[0], 7);
    KSAttackersCount[1] = std::min(KSAttackersCount[1], 7);
    evalMidgame += pieces[8] > 0? KSAttackersWeight[0] * pieceAttackWeight[KSAttackersCount[0]] / 100 : 0;
    evalMidgame -= pieces[9] > 0? KSAttackersWeight[1] * pieceAttackWeight[KSAttackersCount[1]] / 100 : 0;

    evalEndgame += EGVAL(material[0] - material[1]);
    evalEndgame += EGVAL(pieceSquareEval);
    evalEndgame += EGVAL(passedPawnsEval);
    evalEndgame += EGVAL(pawnsEval);
    evalEndgame += EGVAL(imbalanceEval);
    evalEndgame += EGVAL(knightsEval);
    evalEndgame += EGVAL(bishopsEval);
    evalEndgame += EGVAL(rooksEval);
    evalEndgame += EGVAL(queensEval);
    evalEndgame += EGVAL(pawnShieldEval);

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
int Eval::evaluate_debug(int *material, uint64_t *pieces, Magics *magics, uint64_t *knightMoves, int *pieceCount, uint64_t occ) {

    #ifndef NDEBUG
    int debugMaterialCount = 0;
    int pawnCount = count_population(pieces[0]);
    int knightCount = count_population(pieces[2]);
    int bishopCount = count_population(pieces[4]);
    int rookCount = count_population(pieces[6]);
    int queenCount = count_population(pieces[8]);
    int kingCount = count_population(pieces[10]);
    debugMaterialCount += pawnCount * MGVAL(pieceValues[0]);
    debugMaterialCount += knightCount * MGVAL(pieceValues[1]);
    debugMaterialCount += bishopCount * MGVAL(pieceValues[2]);
    debugMaterialCount += rookCount * MGVAL(pieceValues[3]);
    debugMaterialCount += queenCount * MGVAL(pieceValues[4]);
    debugMaterialCount += kingCount * MGVAL(pieceValues[5]);
    assert(debugMaterialCount == MGVAL(material[0]));
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
    debugMaterialCount = pawnCount * EGVAL(pieceValues[0]);
    debugMaterialCount += knightCount * EGVAL(pieceValues[1]);
    debugMaterialCount += bishopCount * EGVAL(pieceValues[2]);
    debugMaterialCount += rookCount * EGVAL(pieceValues[3]);
    debugMaterialCount += queenCount * EGVAL(pieceValues[4]);
    debugMaterialCount += kingCount * EGVAL(pieceValues[5]);
    assert(debugMaterialCount == EGVAL(material[1]));
    assert(pawnCount == pieceCount[1]);
    assert(knightCount == pieceCount[3]);
    assert(bishopCount == pieceCount[5]);
    assert(rookCount == pieceCount[7]);
    assert(queenCount == pieceCount[9]);
    assert(kingCount == pieceCount[11]);
    #endif


    int ret = MGVAL(material[0] - material[1]);

    int evalMidgame = ret;
    int evalEndgame = ret;
    occupied = occ;

    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "White piece square: " << evaluate_piece_square_values(pieces, false) << std::endl;
    std::cout << "White trapped rook: " << evaluateTrappedRook(pieces, false) << std::endl;
    std::cout << "White imbalance: " << evaluateImbalance(pieceCount, false) << std::endl;
    std::cout << "White pawns: " << evaluatePawns(pieces, false) << std::endl;
    std::cout << "White passed pawns: " << evaluatePassedPawns(pieces, false) << std::endl;
    std::cout << "White outposts: " << evaluateKnights(pieces, knightMoves, false) << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "Black piece square: " << evaluate_piece_square_values(pieces, true) << std::endl;
    std::cout << "Black trapped rook: " << evaluateTrappedRook(pieces, true) << std::endl;
    std::cout << "Black imbalance: " << evaluateImbalance(pieceCount, true) << std::endl;
    std::cout << "Black pawns: " << evaluatePawns(pieces, true) << std::endl;
    std::cout << "Black passed pawns: " << evaluatePassedPawns(pieces, true) << std::endl;
    std::cout << "Black outposts: " << evaluateKnights(pieces, knightMoves, true) << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "All trapped rook: " << evaluateTrappedRook(pieces, false) - evaluateTrappedRook(pieces, true) << std::endl;
    std::cout << "All piece square: " << evaluate_piece_square_values(pieces, false) - evaluate_piece_square_values(pieces, true) << std::endl;
    std::cout << "All imbalance: " << evaluateImbalance(pieceCount, false) - evaluateImbalance(pieceCount, true) << std::endl;
    std::cout << "All pawns: " << evaluatePawns(pieces, false) - evaluatePawns(pieces, true) << std::endl;
    std::cout << "All passed pawns: " << evaluatePassedPawns(pieces, false) - evaluatePassedPawns(pieces, true) << std::endl;
    std::cout << "All outposts: " << evaluateKnights(pieces, knightMoves, false) - evaluateKnights(pieces, knightMoves, true) << std::endl;
    std::cout << "All bishops: " << evaluateBishops(pieces, magics, false) - evaluateBishops(pieces, magics, true) << std::endl;
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
int Eval::evaluate_piece_square_values(uint64_t *pieces, bool col) {

    int ret = 0;
    uint64_t piece;

    for (int i = col; i < 12; i += 2) {
        piece = pieces[i];
        while (piece) {
            ret += pieceSquare[i][bitScan(piece)];
            piece &= piece - 1;
        }
    }

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
        ret += bishopWeight[pieceCount[0] + pieceCount[1]];
    }

    // Knight pair
    if (pieceCount[2 + col] >= 2) {
        ret -= S(14, 19);
    }

    // Rook pair
    if (pieceCount[6 + col] >= 2) {
        ret -= S(18, 18);
    }

    // Pawn count
    if (pieceCount[col] == 0) {
        ret -= S(22, 35);
    }

    ret += knightWeight[pieceCount[col]] * pieceCount[2 + col];
    ret += rookWeight[pieceCount[col]] * pieceCount[6 + col];
    ret += queenWeight[std::min(pieceCount[2 + col] + pieceCount[4 + col] + pieceCount[6 + col], 6)] * pieceCount[8 + col];

    return ret;
}



int Eval::evaluatePawns(uint64_t *pieces, bool col) {

    int ret = 0;
    uint64_t supportedPawns = pieces[col] & pawnAttacksAll(pieces[col], col);
    uint64_t adjacentPawns = pieces[col] & adjacentMask(pieces[col]);
    uint64_t doubledPawns = col? ((pieces[col] ^ supportedPawns) << 8) & pieces[col] : ((pieces[col] ^ supportedPawns) >> 8) & pieces[col];

    ret -= doublePawnValue * count_population(doubledPawns);

    uint64_t piece = pieces[col] & (!supportedPawns & !adjacentPawns);
    while (piece) {
        int bscan = bitScan(piece);
        if ((isolatedPawnMask[bscan] & pieces[col]) == 0) {
            ret -= isolatedPawnValue;
        }
        piece &= piece - 1;
    }

    while (supportedPawns) {
        int bscan = bitScan(supportedPawns) / 8;
        ret += supportedPawnWeight[col? 7 - bscan : bscan];
        supportedPawns &= supportedPawns - 1;
    }

    while (adjacentPawns) {
        int bscan = bitScan(adjacentPawns) / 8;
        ret += adjacentPawnWeight[col? 7 - bscan : bscan];
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
            ret += col? passedPawnWeight[(7 - (bscan / 8))] : passedPawnWeight[(bscan / 8)];
            if (columnMask[bscan] & pieces[6 + col]) {
                ret += S(6, 12);
            }
        }
        piece &= piece - 1;
    }

    return ret;

}



int Eval::evaluateKnights(uint64_t *pieces, uint64_t *knightMoves, bool col) {

    int ret = 0;
    uint64_t piece = pieces[2 + col];
    uint64_t holes = pawnAttacksAll(pieces[col], col);
    uint64_t defendedKnight = piece & holes;

    while (piece) {
        int bscan = bitScan(piece);

        // Mobility
        ret += knightMobilityBonus[count_population(knightMoves[bscan] & ~minorUnsafe[col])];

        // King safety
        unsafeSquares[!col] |= knightMoves[bscan];
        int attacks = count_population(knightMoves[bscan] & tempUnsafe[col]);
        if (attacks) {
            KSAttackersWeight[col] += attacks * pieceAttackValue[1];
            (KSAttackersCount[col])++;
        }

        // Outposts
        if (defendedKnight & (1ULL << bscan) && (outpostMask[col][bscan] & pieces[!col]) == 0) {
            ret += S(outpostPotential[col][bscan], 0);
        }

        piece &= piece - 1;
    }

    return ret;

}



int Eval::evaluateBishops(uint64_t *pieces, Magics *magics, bool col) {

    int ret = 0;
    uint64_t piece = pieces[4 + col];

    while (piece) {
        int bscan = bitScan(piece);
        uint64_t bishopAttacks = magics->bishopAttacksMask(occupied ^ pieces[8 + col], bscan);

        // Mobility
        ret += bishopMobilityBonus[count_population(bishopAttacks & ~minorUnsafe[col])];

        // King safety
        unsafeSquares[!col] |= bishopAttacks;
        int attacks = count_population(bishopAttacks & tempUnsafe[col]);
        if (attacks) {
            KSAttackersWeight[col] += attacks * pieceAttackValue[2];
            (KSAttackersCount[col])++;
        }

        piece &= piece - 1;
    }

    return ret;

}



int Eval::evaluateRooks(uint64_t *pieces, Magics *magics, bool col) {

    int ret = 0;
    uint64_t piece = pieces[6 + col];

    while (piece) {
        int bscan = bitScan(piece);
        uint64_t rookAttacks = magics->rookAttacksMask(occupied ^ pieces[8 + col], bscan);

        // Mobility
        ret += rookMobilityBonus[count_population(rookAttacks & ~mobilityUnsafeSquares[col])];

        // King safety
        unsafeSquares[!col] |= rookAttacks;
        int attacks = count_population(rookAttacks & tempUnsafe[col]);
        if (attacks) {
            KSAttackersWeight[col] += attacks * pieceAttackValue[3];
            (KSAttackersCount[col])++;
        }

        // Rook on open file
        if ((columnMask[bscan] & pieces[col]) == 0) {
            ret += S(18, 0);
            if ((columnMask[bscan] & pieces[!col]) == 0) {
                ret += S(12, 6);
            }
        }

        // Rook on enemy queen file
        if ((columnMask[bscan] & pieces[8 + !col]) == 0) {
            ret += S(5, 3);
        }

        piece &= piece - 1;
    }

    return ret;

}



int Eval::evaluateQueens(uint64_t *pieces, Magics *magics, bool col) {

    int ret = 0;
    uint64_t piece = pieces[8 + col];

    while (piece) {
        int bscan = bitScan(piece);
        uint64_t queenAttacks = magics->queenAttacksMask(occupied, bscan);

        // Mobility
        ret += queenMobilityBonus[count_population(queenAttacks & ~queenUnsafe[col])];

        // King safety
        unsafeSquares[!col] |= queenAttacks;
        int attacks = count_population(queenAttacks & tempUnsafe[col]);
        if (attacks) {
            KSAttackersWeight[col] += attacks * pieceAttackValue[4];
            (KSAttackersCount[col])++;
        }

        piece &= piece - 1;
    }

    return S(ret, 0);

}



int Eval::evaluatePawnShield(uint64_t *pieces, bool col) {

    int ret = 0;
    uint64_t piece = pieces[10 + col];
    int bscan = bitScan(piece);
    uint64_t shield = passedPawnMask[col][bscan] & (rowMask[col? std::max(bscan - 8, 0) : std::min(bscan + 8, 63)] | rowMask[col? std::max(bscan - 16, 0) : std::min(bscan + 16, 63)]);

    ret += count_population(shield & pieces[col]) * 24;


    if ((forwardMask[col][bscan] & pieces[col]) != 0) {
        ret += 32;
        if ((forwardMask[col][bscan] & pieces[!col]) != 0) {
            ret += 8;
        }
    }

    return S(ret, 0);

}
