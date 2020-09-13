#include "eval.h"

extern Magics *magics;



int pieceValues[6] = {S(114, 94), S(491, 332), S(500, 328), S(694, 559), S(1271, 1259), S(5000, 5000)};

// Queen, Bishop, Knight and rook weights
int knightWeight[9] = {S(-30, -30), S(-24, -24), S(-18, -18), S(-12, -12), S(-6, -6), S(0, 0), S(6, 6) , S(12, 12), S(18, 18)};
int rookWeight[9] = {S(60, 60), S(48, 48), S(36, 36), S(24, 24), S(12, 12), S(0, 0), S(-12, -12), S(-24, -24), S(-36, -36)};
int bishopWeight[17] = {S(57, 57), S(56, 56), S(54, 54), S(52, 52), S(50, 50), S(46, 46), S(45, 45), S(43, 43), S(42, 42), S(40, 40), S(39, 39), S(37, 37), S(35, 35), S(33, 33), S(27, 27), S(26, 26), S(23, 23)};
int queenWeight[7] = {S(0, 0), S(12, 12), S(15, 15), S(18, 18), S(22, 22), S(26, 26), S(30, 30)};

// Supported and adjacent pawn weights
// int supportedPawnWeight[7] = {S(11, 0), S(0, 0), S(23, 24), S(18, 11), S(30, 2), S(47, 42), S(78, 81)};
// int adjacentPawnWeight[7]  = {S(0, 0), S(9, 7), S(7, 9), S(13, 7), S(21, 31), S(83, 132), S(109, 391)};
int supportedPawnWeight[7] = {S(0, 0), S(0, 0), S(25, 24), S(21, 13), S(32, 10), S(69, 44), S(354, -53)};
int adjacentPawnWeight[7]  = {S(0, 0), S(11, 7), S(7, 10), S(18, 7), S(31, 29), S(101, 82), S(-69, 451)};

// Passed Pawn weights
int passedPawnWeight[7] = {S(0, 0), S(5, 2), S(-3, 9), S(-7, 29), S(14, 60), S(29, 145), S(62, 213)};

// Doubled pawns and isolated pawns
int doublePawnValue = S(18, 12);
int isolatedPawnValue = S(10, 16);

// Mobility
int knightMobilityBonus[9] =  {S(-54, -42), S(-19, -31), S(-4, 8), -S(0, 8), S(1, 9), S(12, 11), S(13, 17), S(14, 21), S(31, 24)};
int bishopMobilityBonus[14] = {S(-27, -18), S(-25, -16), S(2, 1), S(2, 1), S(2, 3), S(16, 9), S(22, 26), S(27, 31), S(32, 35), S(36, 36), S(38, 37), S(47, 40), S(68, 48), S(82, 61)};
int rookMobilityBonus[15] =   {S(-66, -78), S(-21, -32), S(-17, 3), S(-8, 6), S(-6, 11), S(-1, 14), S(5, 17), S(7, 21), S(18, 22), S(29, 24), S(39, 27), S(49, 34), S(59, 36), S(60, 38), S(70, 40)};
int queenMobilityBonus[27] =  {S(-15, -19), S(-15, -17), S(-10, -10), S(-7, -2), S(10, 15), S(11, 17), S(12, 19), S(13, 21), S(15, 25),
                             S(18, 26), S(21, 29), S(23, 35), S(27, 39), S(27, 39), S(30, 45), S(32, 45), S(35, 49), S(35, 52), S(39, 55),
                             S(39, 55), S(41, 59), S(47, 62), S(50, 65), S(55, 71), S(62, 76), S(66, 82), S(70, 88)};


int pieceAttackValue[5] = {0, 38, 30, 48, 5};
int attacksSafety = 37;
int queenCheckVal = 69;
int rookCheckVal = 61;
int bishopCheckVal = 14;
int knightCheckVal = 102;
int KSOffset = 91;


int kingPawnFront = S(32, -11);
int kingPawnFrontN = S(5, -1);

int kingPawnAdj = S(17, -12);
int kingPawnAdjN = S(9, -9);

int rookOnOpen = S(27, -13);
int rookOnSemiOpen = S(24, 3);
int rookOnQueen = S(-20, 22);

int knightPair = S(14, 9);
int rookPair = S(17, 17);
int noPawns = S(0, 22);




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
int Eval::evaluate(int *material, uint64_t *pieces, uint64_t *knightMoves, int *pieceCount, uint64_t occ, bool col) {

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

    for (int i = 0; i < 2; i++) {
        unsafeSquares[i] = 0;
        KSAttackersWeight[i] = 0;
        KSAttacks[i] = 0;
        KSAttackersCount[i] = 0;
        attacksKnight[i] = 0;
        attacksBishop[i] = 0;
        attacksRook[i] = 0;
        attacksQueen[i] = 0;
    }

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
    ret += col? S(-16, -16) : S(16, 16);
    ret += material[0] - material[1];
    ret += evaluate_piece_square_values(pieces, false) - evaluate_piece_square_values(pieces, true);
    ret += evaluatePassedPawns(pieces, false) - evaluatePassedPawns(pieces, true);
    ret += evaluatePawns(pieces, false) - evaluatePawns(pieces, true);
    ret += evaluateImbalance(pieceCount, false) - evaluateImbalance(pieceCount, true);
    ret += evaluatePawnShield(pieces, false) - evaluatePawnShield(pieces, true);
    ret += evaluateKnights(pieces, knightMoves, false) - evaluateKnights(pieces, knightMoves, true);
    ret += evaluateBishops(pieces, false) - evaluateBishops(pieces, true);
    ret += evaluateRooks(pieces, false) - evaluateRooks(pieces, true);
    ret += evaluateQueens(pieces, false) - evaluateQueens(pieces, true);
    ret += evaluateKing(pieces, false) - evaluateKing(pieces, true);

    int evalMidgame = MGVAL(ret);
    int evalEndgame = EGVAL(ret);

    evalMidgame += evaluateTrappedRook(pieces, false) - evaluateTrappedRook(pieces, true);

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
int Eval::evaluate_debug(int *material, uint64_t *pieces, uint64_t *knightMoves, int *pieceCount, uint64_t occ) {

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

    for (int i = 0; i < 2; i++) {
        unsafeSquares[i] = 0;
        KSAttackersWeight[i] = 0;
        KSAttacks[i] = 0;
        KSAttackersCount[i] = 0;
        attacksKnight[i] = 0;
        attacksBishop[i] = 0;
        attacksRook[i] = 0;
        attacksQueen[i] = 0;
    }

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

    ret += evaluateKnights(pieces, knightMoves, false) - evaluateKnights(pieces, knightMoves, true);
    ret += evaluateBishops(pieces, false) - evaluateBishops(pieces, true);
    ret += evaluateRooks(pieces, false) - evaluateRooks(pieces, true);
    ret += evaluateQueens(pieces, false) - evaluateQueens(pieces, true);
    ret += evaluatePawns(pieces, false) - evaluatePawns(pieces, true);

    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "White piece square: " << evaluate_piece_square_values(pieces, false) << std::endl;
    std::cout << "White trapped rook: " << evaluateTrappedRook(pieces, false) << std::endl;
    std::cout << "White imbalance: " << evaluateImbalance(pieceCount, false) << std::endl;
    std::cout << "White pawns: " << evaluatePawns(pieces, false) << std::endl;
    std::cout << "White passed pawns: " << evaluatePassedPawns(pieces, false) << std::endl;
    std::cout << "White Kings: " << MGVAL(evaluateKing(pieces, false)) << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "Black piece square: " << evaluate_piece_square_values(pieces, true) << std::endl;
    std::cout << "Black trapped rook: " << evaluateTrappedRook(pieces, true) << std::endl;
    std::cout << "Black imbalance: " << evaluateImbalance(pieceCount, true) << std::endl;
    std::cout << "Black pawns: " << evaluatePawns(pieces, true) << std::endl;
    std::cout << "Black passed pawns: " << evaluatePassedPawns(pieces, true) << std::endl;
    std::cout << "Black Kings: " << MGVAL(evaluateKing(pieces, true)) << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "All trapped rook: " << evaluateTrappedRook(pieces, false) - evaluateTrappedRook(pieces, true) << std::endl;
    std::cout << "All piece square: " << evaluate_piece_square_values(pieces, false) - evaluate_piece_square_values(pieces, true) << std::endl;
    std::cout << "All imbalance: " << evaluateImbalance(pieceCount, false) - evaluateImbalance(pieceCount, true) << std::endl;
    std::cout << "All pawns: " << evaluatePawns(pieces, false) - evaluatePawns(pieces, true) << std::endl;
    std::cout << "All passed pawns: " << evaluatePassedPawns(pieces, false) - evaluatePassedPawns(pieces, true) << std::endl;
    std::cout << "All Kings: " << MGVAL(evaluateKing(pieces, false) - evaluateKing(pieces, true)) << std::endl;
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
        ret -= knightPair;
    }

    // Rook pair
    if (pieceCount[6 + col] >= 2) {
        ret -= rookPair;
    }

    // Pawn count
    if (pieceCount[col] == 0) {
        ret -= noPawns;
    }

    ret += knightWeight[pieceCount[col]] * pieceCount[2 + col];
    ret += rookWeight[pieceCount[col]] * pieceCount[6 + col];
    ret += queenWeight[std::min(pieceCount[2 + col] + pieceCount[4 + col] + pieceCount[6 + col], 6)] * pieceCount[8 + col];

    return ret;
}



int Eval::evaluatePawns(uint64_t *pieces, bool col) {

    int ret = 0;
    unsafeSquares[!col] |= pawnAttacksAll(pieces[col], col);
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
    int dist = 0;
    int distFinal = 0;
    int numPawns = 0;
    uint64_t piece = pieces[col];
    int ourKing = bitScan(pieces[10 + col]);
    int theirKing = bitScan(pieces[10 + !col]);
    while (piece) {
        int bscan = bitScan(piece);

        dist = manhattanArray[bscan][ourKing] * 2;
        dist -= manhattanArray[bscan][theirKing] * 2;
        if ((passedPawnMask[col][bscan] & pieces[!col]) == 0 && (forwardMask[col][bscan] & pieces[col]) == 0) {
            ret += col? passedPawnWeight[(7 - (bscan / 8))] : passedPawnWeight[(bscan / 8)];
            if (columnMask[bscan] & pieces[6 + col]) {
                ret += S(6, 12);
            }
            dist *= 3;
        }

        distFinal += dist;
        numPawns++;
        piece &= piece - 1;
    }

    if (numPawns > 0) {
        ret += S(0, -distFinal / numPawns);
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
        attacksKnight[col] |= knightMoves[bscan];
        int attacks = count_population(knightMoves[bscan] & tempUnsafe[col]);
        if (attacks) {
            KSAttackersWeight[col] += pieceAttackValue[1];
            KSAttacks[col] += attacks;
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



int Eval::evaluateBishops(uint64_t *pieces, bool col) {

    int ret = 0;
    uint64_t piece = pieces[4 + col];

    while (piece) {
        int bscan = bitScan(piece);
        uint64_t bishopAttacks = magics->bishopAttacksMask(occupied ^ pieces[8 + col], bscan);

        // Mobility
        ret += bishopMobilityBonus[count_population(bishopAttacks & ~minorUnsafe[col])];

        // King safety
        unsafeSquares[!col] |= bishopAttacks;
        attacksBishop[col] |= bishopAttacks;
        int attacks = count_population(bishopAttacks & tempUnsafe[col]);
        if (attacks) {
            KSAttackersWeight[col] += pieceAttackValue[2];
            KSAttacks[col] += attacks;
            (KSAttackersCount[col])++;
        }

        piece &= piece - 1;
    }

    return ret;

}



int Eval::evaluateRooks(uint64_t *pieces, bool col) {

    int ret = 0;
    uint64_t piece = pieces[6 + col];

    while (piece) {
        int bscan = bitScan(piece);
        uint64_t rookAttacks = magics->rookAttacksMask(occupied ^ pieces[8 + col], bscan);

        // Mobility
        ret += rookMobilityBonus[count_population(rookAttacks & ~mobilityUnsafeSquares[col])];

        // King safety
        unsafeSquares[!col] |= rookAttacks;
        attacksRook[col] |= rookAttacks;
        int attacks = count_population(rookAttacks & tempUnsafe[col]);
        if (attacks) {
            KSAttackersWeight[col] += pieceAttackValue[3];
            KSAttacks[col] += attacks;
            (KSAttackersCount[col])++;
        }

        // Rook on open file
        if ((columnMask[bscan] & pieces[col]) == 0) {
            ret += rookOnSemiOpen;
            if ((columnMask[bscan] & pieces[!col]) == 0) {
                ret += rookOnOpen;
            }
        }

        // Rook on enemy queen file
        if ((columnMask[bscan] & pieces[8 + !col]) == 0) {
            ret += rookOnQueen;
        }

        piece &= piece - 1;
    }

    return ret;

}



int Eval::evaluateQueens(uint64_t *pieces, bool col) {

    int ret = 0;
    uint64_t piece = pieces[8 + col];

    while (piece) {
        int bscan = bitScan(piece);
        uint64_t queenAttacks = magics->queenAttacksMask(occupied, bscan);

        // Mobility
        ret += queenMobilityBonus[count_population(queenAttacks & ~queenUnsafe[col])];

        // King safety
        unsafeSquares[!col] |= queenAttacks;
        attacksQueen[col] |= queenAttacks;
        int attacks = count_population(queenAttacks & tempUnsafe[col]);
        if (attacks) {
            KSAttackersWeight[col] += pieceAttackValue[4];
            KSAttacks[col] += attacks;
            (KSAttackersCount[col])++;
        }

        piece &= piece - 1;
    }

    return S(ret, 0);

}



int Eval::evaluateKing(uint64_t *pieces, bool col) {

    int ret = 0;
    int theirKing = bitScan(pieces[10 + !col]);

    if (KSAttackersCount[col] > 1) {

        uint64_t knightChecks = knightAttacks(pieces[10 + !col]) & attacksKnight[col] & ~unsafeSquares[col];
        uint64_t bishopChecks = magics->bishopAttacksMask(occupied, theirKing) & attacksBishop[col] & ~unsafeSquares[col];
        uint64_t rookChecks = magics->rookAttacksMask(occupied, theirKing) & attacksRook[col] & ~unsafeSquares[col];
        uint64_t queenChecks = magics->queenAttacksMask(occupied, theirKing) & attacksQueen[col] & ~unsafeSquares[col];

        // King safety
        ret += KSAttackersWeight[col];
        ret += queenCheckVal * count_population(queenChecks);
        ret += rookCheckVal * count_population(rookChecks);
        ret += bishopCheckVal * count_population(bishopChecks);
        ret += knightCheckVal * count_population(knightChecks);
        ret += KSAttacks[col] * attacksSafety / (count_population(kingZoneMask[!col][theirKing] & pieces[!col]) + 1);
        ret += (pieces[8 + col] == 0) * -175;
        ret -= KSOffset;
    }

    return ret > 0? S(ret, 0) : S(0, 0);

}



int Eval::evaluatePawnShield(uint64_t *pieces, bool col) {

    int ret = 0;
    uint64_t piece = pieces[10 + col];
    int bscan = bitScan(piece);
    uint64_t shield = passedPawnMask[col][bscan] & (rowMask[col? std::max(bscan - 8, 0) : std::min(bscan + 8, 63)] | rowMask[col? std::max(bscan - 16, 0) : std::min(bscan + 16, 63)]);

    ret += S(count_population(shield & pieces[col]) * 24, 0);

    if ((forwardMask[col][bscan] & pieces[col]) != 0) {
        ret += kingPawnFront;
        if ((forwardMask[col][bscan] & pieces[!col]) != 0) {
            ret += kingPawnFrontN;
        }
    }

    if (bscan % 8 > 0 && (forwardMask[col][bscan - 1] & pieces[col]) != 0) {
        ret += kingPawnAdj;
        if ((forwardMask[col][bscan - 1] & pieces[!col]) != 0) {
            ret += kingPawnAdjN;
        }
    }

    if (bscan % 8 < 7 && (forwardMask[col][bscan + 1] & pieces[col]) != 0) {
        ret += kingPawnAdj;
        if ((forwardMask[col][bscan + 1] & pieces[!col]) != 0) {
            ret += kingPawnAdjN;
        }
    }

    return ret;

}
