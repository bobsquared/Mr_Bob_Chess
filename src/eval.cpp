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
int isolatedPawnValue = S(10, 2);

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

int trappedRook = S(40, 0);




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



// Initialize variables in evaluation
void Eval::InitializeEval(Bitboard &board) {

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

    // Mobility
    mobilityUnsafeSquares[0] = pawnAttacksAll(board.pieces[1], 1) | board.pieces[0] | board.pieces[10];
    mobilityUnsafeSquares[1] = pawnAttacksAll(board.pieces[0], 0) | board.pieces[1] | board.pieces[11];

    minorUnsafe[0] = mobilityUnsafeSquares[0] | board.pieces[8];
    minorUnsafe[1] = mobilityUnsafeSquares[1] | board.pieces[9];

    queenUnsafe[0] = mobilityUnsafeSquares[0] | knightAttacks(board.pieces[3]);
    queenUnsafe[1] = mobilityUnsafeSquares[1] | knightAttacks(board.pieces[2]);

    tempUnsafe[0] = ~(pawnAttacksAll(board.pieces[1], 1) | knightAttacks(board.pieces[3]) | board.pieces[0]) & kingZoneMask[0][bitScan(board.pieces[11])];
    tempUnsafe[1] = ~(pawnAttacksAll(board.pieces[0], 0) | knightAttacks(board.pieces[2]) | board.pieces[1]) & kingZoneMask[1][bitScan(board.pieces[10])];
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



// Evaluate the position
int Eval::evaluate(Bitboard &board) {

    // Asserts for debugging mode
    #ifndef NDEBUG
    int debugMaterialCount = 0;
    int pawnCount = count_population(board.pieces[0]);
    int knightCount = count_population(board.pieces[2]);
    int bishopCount = count_population(board.pieces[4]);
    int rookCount = count_population(board.pieces[6]);
    int queenCount = count_population(board.pieces[8]);
    int kingCount = count_population(board.pieces[10]);
    debugMaterialCount += pawnCount * MGVAL(pieceValues[0]);
    debugMaterialCount += knightCount * MGVAL(pieceValues[1]);
    debugMaterialCount += bishopCount * MGVAL(pieceValues[2]);
    debugMaterialCount += rookCount * MGVAL(pieceValues[3]);
    debugMaterialCount += queenCount * MGVAL(pieceValues[4]);
    debugMaterialCount += kingCount * MGVAL(pieceValues[5]);
    assert(debugMaterialCount == MGVAL(board.material[0]));
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
    debugMaterialCount = pawnCount * EGVAL(pieceValues[0]);
    debugMaterialCount += knightCount * EGVAL(pieceValues[1]);
    debugMaterialCount += bishopCount * EGVAL(pieceValues[2]);
    debugMaterialCount += rookCount * EGVAL(pieceValues[3]);
    debugMaterialCount += queenCount * EGVAL(pieceValues[4]);
    debugMaterialCount += kingCount * EGVAL(pieceValues[5]);
    assert(debugMaterialCount == EGVAL(board.material[1]));
    assert(pawnCount == board.pieceCount[1]);
    assert(knightCount == board.pieceCount[3]);
    assert(bishopCount == board.pieceCount[5]);
    assert(rookCount == board.pieceCount[7]);
    assert(queenCount == board.pieceCount[9]);
    assert(kingCount == board.pieceCount[11]);
    #endif



    InitializeEval(board);

    int ret = 0;
    ret += board.toMove? S(-16, -16) : S(16, 16);
    ret += board.material[0] - board.material[1];
    ret += evaluateImbalance(board, false) - evaluateImbalance(board, true);
    ret += evaluatePawnShield(board, false) - evaluatePawnShield(board, true);

    ret += evaluatePawns(board, false) - evaluatePawns(board, true);
    ret += evaluateKnights(board, false) - evaluateKnights(board, true);
    ret += evaluateBishops(board, false) - evaluateBishops(board, true);
    ret += evaluateRooks(board, false) - evaluateRooks(board, true);
    ret += evaluateQueens(board, false) - evaluateQueens(board, true);
    ret += evaluateKing(board, false) - evaluateKing(board, true);

    int phase = getPhase(board);
    ret = ((MGVAL(ret) * (256 - phase)) + (EGVAL(ret) * phase)) / 256;
    return board.toMove? -ret : ret;
}



// Evaluate the position with debugging
int Eval::evaluate_debug(Bitboard &board) {

    #ifndef NDEBUG
    int debugMaterialCount = 0;
    int pawnCount = count_population(board.pieces[0]);
    int knightCount = count_population(board.pieces[2]);
    int bishopCount = count_population(board.pieces[4]);
    int rookCount = count_population(board.pieces[6]);
    int queenCount = count_population(board.pieces[8]);
    int kingCount = count_population(board.pieces[10]);
    debugMaterialCount += pawnCount * MGVAL(pieceValues[0]);
    debugMaterialCount += knightCount * MGVAL(pieceValues[1]);
    debugMaterialCount += bishopCount * MGVAL(pieceValues[2]);
    debugMaterialCount += rookCount * MGVAL(pieceValues[3]);
    debugMaterialCount += queenCount * MGVAL(pieceValues[4]);
    debugMaterialCount += kingCount * MGVAL(pieceValues[5]);
    assert(debugMaterialCount == MGVAL(board.material[0]));
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
    debugMaterialCount = pawnCount * EGVAL(pieceValues[0]);
    debugMaterialCount += knightCount * EGVAL(pieceValues[1]);
    debugMaterialCount += bishopCount * EGVAL(pieceValues[2]);
    debugMaterialCount += rookCount * EGVAL(pieceValues[3]);
    debugMaterialCount += queenCount * EGVAL(pieceValues[4]);
    debugMaterialCount += kingCount * EGVAL(pieceValues[5]);
    assert(debugMaterialCount == EGVAL(board.material[1]));
    assert(pawnCount == board.pieceCount[1]);
    assert(knightCount == board.pieceCount[3]);
    assert(bishopCount == board.pieceCount[5]);
    assert(rookCount == board.pieceCount[7]);
    assert(queenCount == board.pieceCount[9]);
    assert(kingCount == board.pieceCount[11]);
    #endif


    int ret = MGVAL(board.material[0] - board.material[1]);

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

    uint64_t pawnAttacksW = pawnAttacksAll(board.pieces[0], 0);
    uint64_t pawnAttacksB = pawnAttacksAll(board.pieces[1], 1);

    uint64_t knightAttacksW = knightAttacks(board.pieces[2]);
    uint64_t knightAttacksB = knightAttacks(board.pieces[3]);

    // Mobility
    mobilityUnsafeSquares[0] = pawnAttacksB | board.pieces[0] | board.pieces[10];
    mobilityUnsafeSquares[1] = pawnAttacksW | board.pieces[1] | board.pieces[11];

    minorUnsafe[0] = mobilityUnsafeSquares[0] | board.pieces[8];
    minorUnsafe[1] = mobilityUnsafeSquares[1] | board.pieces[9];

    queenUnsafe[0] = mobilityUnsafeSquares[0] | knightAttacksB;
    queenUnsafe[1] = mobilityUnsafeSquares[1] | knightAttacksW;

    tempUnsafe[0] = ~(pawnAttacksB | knightAttacksB | board.pieces[0]) & kingZoneMask[0][bitScan(board.pieces[11])];
    tempUnsafe[1] = ~(pawnAttacksW | knightAttacksW | board.pieces[1]) & kingZoneMask[1][bitScan(board.pieces[10])];

    ret += evaluateKnights(board, false) - evaluateKnights(board, true);
    ret += evaluateBishops(board, false) - evaluateBishops(board, true);
    ret += evaluateRooks(board, false) - evaluateRooks(board, true);
    ret += evaluateQueens(board, false) - evaluateQueens(board, true);
    ret += evaluatePawns(board, false) - evaluatePawns(board, true);

    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "White imbalance: " << evaluateImbalance(board, false) << std::endl;
    std::cout << "White pawns: " << evaluatePawns(board, false) << std::endl;
    std::cout << "White Kings: " << MGVAL(evaluateKing(board, false)) << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "Black imbalance: " << evaluateImbalance(board, true) << std::endl;
    std::cout << "Black pawns: " << evaluatePawns(board, true) << std::endl;
    std::cout << "Black Kings: " << MGVAL(evaluateKing(board, true)) << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;
    std::cout << "All imbalance: " << evaluateImbalance(board, false) - evaluateImbalance(board, true) << std::endl;
    std::cout << "All pawns: " << evaluatePawns(board, false) - evaluatePawns(board, true) << std::endl;
    std::cout << "All Kings: " << MGVAL(evaluateKing(board, false) - evaluateKing(board, true)) << std::endl;
    std::cout << "----------------------------------------------------------" << std::endl;

    int phase = TOTALPHASE;
    phase -= (board.pieceCount[0] + board.pieceCount[1]) * PAWNPHASE;
    phase -= (board.pieceCount[2] + board.pieceCount[3]) * KNIGHTPHASE;
    phase -= (board.pieceCount[4] + board.pieceCount[5]) * BISHOPPHASE;
    phase -= (board.pieceCount[6] + board.pieceCount[7]) * ROOKPHASE;
    phase -= (board.pieceCount[8] + board.pieceCount[9]) * QUEENPHASE;

    phase = (phase * 256 + (TOTALPHASE / 2)) / TOTALPHASE;
    ret = ((evalMidgame * (256 - phase)) + (evalEndgame * phase)) / 256;

    return ret;
}




// Evaluate material imbalance
int Eval::evaluateImbalance(Bitboard &board, bool col) {

    int ret = 0;

    // Bishop pair
    if (board.pieceCount[4 + col] >= 2) {
        ret += bishopWeight[board.pieceCount[0] + board.pieceCount[1]];
    }

    // Knight pair
    if (board.pieceCount[2 + col] >= 2) {
        ret -= knightPair;
    }

    // Rook pair
    if (board.pieceCount[6 + col] >= 2) {
        ret -= rookPair;
    }

    // Pawn count
    if (board.pieceCount[col] == 0) {
        ret -= noPawns;
    }

    ret += knightWeight[board.pieceCount[col]] * board.pieceCount[2 + col];
    ret += rookWeight[board.pieceCount[col]] * board.pieceCount[6 + col];
    ret += queenWeight[std::min(board.pieceCount[2 + col] + board.pieceCount[4 + col] + board.pieceCount[6 + col], 6)] * board.pieceCount[8 + col];

    return ret;
}



int Eval::evaluatePawns(Bitboard &board, bool col) {

    int ret = 0;
    int dist = 0;
    int distFinal = 0;
    uint64_t piece = board.pieces[col];
    int ourKing = bitScan(board.pieces[10 + col]);
    int theirKing = bitScan(board.pieces[10 + !col]);

    unsafeSquares[!col] |= pawnAttacksAll(board.pieces[col], col);
    uint64_t supportedPawns = board.pieces[col] & pawnAttacksAll(board.pieces[col], col);
    uint64_t adjacentPawns = board.pieces[col] & adjacentMask(board.pieces[col]);
    uint64_t doubledPawns = col? ((board.pieces[col] ^ supportedPawns) << 8) & board.pieces[col] : ((board.pieces[col] ^ supportedPawns) >> 8) & board.pieces[col];

    ret -= doublePawnValue * count_population(doubledPawns);

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

    while (piece) {
        int bscan = bitScan(piece);

        // PST
        ret += pieceSquare[col][bscan];

        dist = manhattanArray[bscan][ourKing] * 2;
        dist -= manhattanArray[bscan][theirKing] * 2;

        // Passed pawns
        if ((passedPawnMask[col][bscan] & board.pieces[!col]) == 0 && (forwardMask[col][bscan] & board.pieces[col]) == 0) {
            ret += col? passedPawnWeight[(7 - (bscan / 8))] : passedPawnWeight[(bscan / 8)];
            if (columnMask[bscan] & board.pieces[6 + col]) {
                ret += S(6, 12);
            }
            dist *= 3;
        }

        // Isolated pawns
        // if ((isolatedPawnMask[bscan] & board.pieces[col]) == 0) {
        //     ret -= isolatedPawnValue;
        // }

        distFinal += dist;
        piece &= piece - 1;
    }

    if (board.pieceCount[col]) {
        ret += S(0, -distFinal / board.pieceCount[col]);
    }

    return ret;

}



int Eval::evaluateKnights(Bitboard &board, bool col) {

    int ret = 0;
    uint64_t piece = board.pieces[2 + col];
    uint64_t holes = pawnAttacksAll(board.pieces[col], col);
    uint64_t defendedKnight = piece & holes;

    while (piece) {
        int bscan = bitScan(piece);

        // PST
        ret += pieceSquare[2 + col][bscan];

        // Mobility
        ret += knightMobilityBonus[count_population(board.knightMoves[bscan] & ~minorUnsafe[col])];

        // King safety
        unsafeSquares[!col] |= board.knightMoves[bscan];
        attacksKnight[col] |= board.knightMoves[bscan];
        int attacks = count_population(board.knightMoves[bscan] & tempUnsafe[col]);
        if (attacks) {
            KSAttackersWeight[col] += pieceAttackValue[1];
            KSAttacks[col] += attacks;
            (KSAttackersCount[col])++;
        }

        // Outposts
        if (defendedKnight & (1ULL << bscan) && (outpostMask[col][bscan] & board.pieces[!col]) == 0) {
            ret += S(outpostPotential[col][bscan], 0);
        }

        piece &= piece - 1;
    }

    return ret;

}



int Eval::evaluateBishops(Bitboard &board, bool col) {

    int ret = 0;
    uint64_t piece = board.pieces[4 + col];

    while (piece) {
        int bscan = bitScan(piece);
        uint64_t bishopAttacks = magics->bishopAttacksMask(board.occupied ^ board.pieces[8 + col], bscan);

        // PST
        ret += pieceSquare[4 + col][bscan];

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



int Eval::evaluateRooks(Bitboard &board, bool col) {

    int ret = 0;
    uint64_t piece = board.pieces[6 + col];

    while (piece) {
        int bscan = bitScan(piece);
        uint64_t rookAttacks = magics->rookAttacksMask(board.occupied ^ board.pieces[8 + col], bscan);

        // PST
        ret += pieceSquare[6 + col][bscan];

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
        if ((columnMask[bscan] & board.pieces[col]) == 0) {
            ret += rookOnSemiOpen;
            if ((columnMask[bscan] & board.pieces[!col]) == 0) {
                ret += rookOnOpen;
            }
        }

        // Rook on enemy queen file
        if ((columnMask[bscan] & board.pieces[8 + !col]) == 0) {
            ret += rookOnQueen;
        }

        // Trapped rooks
        uint64_t pieceLoc = piece & -piece;
        if (rowMask[col * 56] & pieceLoc) {
            if (board.pieces[col + 10] > 1ULL << (3 + (col * 56)) && pieceLoc > board.pieces[col + 10]) {
                ret -= trappedRook;
            }
            if (board.pieces[col + 10] < 1ULL << (4 + (col * 56)) && pieceLoc < board.pieces[col + 10]) {
                ret -= trappedRook;
            }
        }

        piece &= piece - 1;
    }

    return ret;

}



int Eval::evaluateQueens(Bitboard &board, bool col) {

    int ret = 0;
    uint64_t piece = board.pieces[8 + col];

    while (piece) {
        int bscan = bitScan(piece);
        uint64_t queenAttacks = magics->queenAttacksMask(board.occupied, bscan);

        // PST
        ret += pieceSquare[8 + col][bscan];

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



int Eval::evaluateKing(Bitboard &board, bool col) {

    int ret = 0;
    int theirKing = bitScan(board.pieces[10 + !col]);

    // PST
    ret += pieceSquare[10 + col][bitScan(board.pieces[10 + col])];

    if (KSAttackersCount[col] > 1) {

        uint64_t knightChecks = knightAttacks(board.pieces[10 + !col]) & attacksKnight[col] & ~unsafeSquares[col];
        uint64_t bishopChecks = magics->bishopAttacksMask(board.occupied, theirKing) & attacksBishop[col] & ~unsafeSquares[col];
        uint64_t rookChecks = magics->rookAttacksMask(board.occupied, theirKing) & attacksRook[col] & ~unsafeSquares[col];
        uint64_t queenChecks = magics->queenAttacksMask(board.occupied, theirKing) & attacksQueen[col] & ~unsafeSquares[col];

        // King safety
        int kingSafe = KSAttackersWeight[col];
        kingSafe += queenCheckVal * count_population(queenChecks);
        kingSafe += rookCheckVal * count_population(rookChecks);
        kingSafe += bishopCheckVal * count_population(bishopChecks);
        kingSafe += knightCheckVal * count_population(knightChecks);
        kingSafe += KSAttacks[col] * attacksSafety / (count_population(kingZoneMask[!col][theirKing] & board.pieces[!col]) + 1);
        kingSafe += (board.pieces[8 + col] == 0) * -175;
        kingSafe -= KSOffset;

        if (kingSafe > 0) {
            ret += S(kingSafe, 0);
        }
    }

    return ret;

}



int Eval::evaluatePawnShield(Bitboard &board, bool col) {

    int ret = 0;
    uint64_t piece = board.pieces[10 + col];
    int bscan = bitScan(piece);
    uint64_t shield = passedPawnMask[col][bscan] & (rowMask[col? std::max(bscan - 8, 0) : std::min(bscan + 8, 63)] | rowMask[col? std::max(bscan - 16, 0) : std::min(bscan + 16, 63)]);

    ret += S(count_population(shield & board.pieces[col]) * 24, 0);

    if ((forwardMask[col][bscan] & board.pieces[col]) != 0) {
        ret += kingPawnFront;
        if ((forwardMask[col][bscan] & board.pieces[!col]) != 0) {
            ret += kingPawnFrontN;
        }
    }

    if (bscan % 8 > 0 && (forwardMask[col][bscan - 1] & board.pieces[col]) != 0) {
        ret += kingPawnAdj;
        if ((forwardMask[col][bscan - 1] & board.pieces[!col]) != 0) {
            ret += kingPawnAdjN;
        }
    }

    if (bscan % 8 < 7 && (forwardMask[col][bscan + 1] & board.pieces[col]) != 0) {
        ret += kingPawnAdj;
        if ((forwardMask[col][bscan + 1] & board.pieces[!col]) != 0) {
            ret += kingPawnAdjN;
        }
    }

    return ret;

}
