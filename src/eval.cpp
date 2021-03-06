#include "eval.h"

extern Magics *magics;


int pieceValues[6] = {S(62, 267), S(315, 790), S(347, 830), S(434, 1371), S(1866, 1993), S(5000, 5000)};

int PAWN_TABLE[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                      S(287, 292), S(283, 289), S(297, 245), S(297, 192), S(311, 171), S(225, 191), S(22, 281), S(50, 288),
                      S(-15, 215), S(74, 203), S(98, 170), S(121, 135), S(131, 112), S(182, 83), S(87, 150), S(11, 153),
                      S(-9, 74), S(27, 58), S(21, 36), S(47, 2), S(69, -9), S(66, -2), S(58, 26), S(11, 18),
                      S(-17, 24), S(9, 25), S(6, -13), S(7, -15), S(25, -18), S(15, -14), S(45, 0), S(12, -19),
                      S(-19, 5), S(-1, 15), S(-7, -6), S(-13, 3), S(9, 3), S(3, -1), S(57, -3), S(24, -31),
                      S(-23, 26), S(-11, 32), S(-17, 18), S(-13, 14), S(-12, 24), S(30, 23), S(57, 17), S(19, -36),
                      S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int KNIGHT_TABLE[64] = {S(-120, -97), S(-16, -40), S(-63, 0), S(14, -18), S(31, -19), S(-36, 2), S(19, -33), S(-65, -97),
                        S(-4, -51), S(-1, -29), S(34, -13), S(50, 9), S(47, 6), S(80, -28), S(-1, -26), S(30, -47),
                        S(-19, -36), S(15, -4), S(44, 13), S(50, 18), S(77, 7), S(84, 19), S(42, 0), S(30, -34),
                        S(1, -27), S(11, 2), S(31, 20), S(33, 36), S(22, 40), S(43, 30), S(18, 20), S(27, -13),
                        S(-4, -33), S(3, 0), S(9, 22), S(6, 30), S(12, 28), S(10, 28), S(26, 4), S(3, -23),
                        S(-14, -65), S(-3, -17), S(0, -7), S(6, 9), S(7, 7), S(1, -8), S(1, -12), S(-8, -56),
                        S(-30, -66), S(-27, -28), S(-11, -29), S(-2, -18), S(-7, -16), S(-5, -30), S(-22, -30), S(-11, -55),
                        S(-95, -75), S(-13, -88), S(-46, -47), S(-26, -36), S(-18, -36), S(-15, -43), S(-12, -78), S(-83, -103)};

int BISHOP_TABLE[64] = {S(-120, -97), S(-16, -40), S(-63, 0), S(14, -18), S(31, -19), S(-36, 2), S(19, -33), S(-65, -97),
                        S(-4, -51), S(-1, -29), S(34, -13), S(50, 9), S(47, 6), S(80, -28), S(-1, -26), S(30, -47),
                        S(-19, -36), S(15, -4), S(44, 13), S(50, 18), S(77, 7), S(84, 19), S(42, 0), S(30, -34),
                        S(1, -27), S(11, 2), S(31, 20), S(33, 36), S(22, 40), S(43, 30), S(18, 20), S(27, -13),
                        S(-4, -33), S(3, 0), S(9, 22), S(6, 30), S(12, 28), S(10, 28), S(26, 4), S(3, -23),
                        S(-14, -65), S(-3, -17), S(0, -7), S(6, 9), S(7, 7), S(1, -8), S(1, -12), S(-8, -56),
                        S(-30, -66), S(-27, -28), S(-11, -29), S(-2, -18), S(-7, -16), S(-5, -30), S(-22, -30), S(-11, -55),
                        S(-95, -75), S(-13, -88), S(-46, -47), S(-26, -36), S(-18, -36), S(-15, -43), S(-12, -78), S(-83, -103)};

int ROOK_TABLE[64] = {S(153, 29), S(160, 33), S(157, 42), S(169, 38), S(183, 34), S(179, 40), S(227, 22), S(256, 14),
                      S(64, 45), S(54, 58), S(104, 56), S(131, 58), S(130, 56), S(206, 0), S(106, 27), S(181, 0),
                      S(15, 37), S(86, 21), S(72, 44), S(115, 23), S(158, 6), S(171, 15), S(204, -16), S(127, -2),
                      S(2, 16), S(15, 28), S(36, 38), S(73, 29), S(64, 24), S(74, 16), S(79, 2), S(74, -1),
                      S(-31, -10), S(-37, 21), S(-27, 27), S(-8, 19), S(-15, 11), S(-7, 8), S(26, -1), S(6, -22),
                      S(-50, -49), S(-30, -18), S(-39, -16), S(-27, -18), S(-27, -22), S(-34, -24), S(19, -31), S(-12, -52),
                      S(-91, -53), S(-22, -59), S(-24, -39), S(-17, -43), S(-28, -41), S(-5, -56), S(11, -56), S(-66, -48),
                      S(-17, -55), S(-10, -31), S(-2, -17), S(2, -15), S(-1, -24), S(-4, -22), S(27, -33), S(-1, -77)};

int QUEEN_TABLE[64] = {S(-51, 100), S(-28, 139), S(-20, 142), S(12, 152), S(43, 163), S(135, 135), S(48, 173), S(95, 137),
                       S(-50, 48), S(-97, 108), S(-34, 119), S(-80, 193), S(-54, 243), S(39, 189), S(-102, 257), S(18, 149),
                       S(-52, 10), S(-45, 50), S(-45, 93), S(-15, 121), S(4, 184), S(65, 200), S(78, 161), S(39, 139),
                       S(-41, -15), S(-48, 64), S(-40, 67), S(-45, 139), S(-31, 175), S(-19, 199), S(-3, 170), S(10, 114),
                       S(-30, -53), S(-33, 25), S(-32, 26), S(-47, 110), S(-49, 100), S(-23, 91), S(-17, 57), S(-3, 77),
                       S(-31, -75), S(-22, -45), S(-33, 17), S(-36, -22), S(-36, -19), S(-33, 7), S(-5, -40), S(0, -62),
                       S(-36, -99), S(-31, -51), S(-21, -93), S(-27, -57), S(-24, -81), S(0, -161), S(5, -178), S(-16, -113),
                       S(-30, -71), S(-45, -102), S(-37, -113), S(-24, -107), S(-31, -121), S(-74, -101), S(-68, -133), S(-55, -53)};

int KING_TABLE[64] = {S(306, -329), S(514, -167), S(317, -87), S(314, -53), S(283, -70), S(175, -45), S(254, -65), S(451, -266),
                      S(39, -37), S(211, 57), S(235, 35), S(188, 11), S(211, 16), S(229, 32), S(113, 85), S(69, -5),
                      S(69, 44), S(197, 84), S(225, 72), S(205, 56), S(214, 49), S(250, 85), S(164, 95), S(62, 31),
                      S(62, 25), S(121, 81), S(167, 82), S(71, 99), S(117, 89), S(128, 84), S(115, 72), S(-53, 35),
                      S(55, -19), S(65, 51), S(91, 72), S(38, 97), S(52, 89), S(31, 76), S(74, 41), S(-23, -3),
                      S(-27, -6), S(-27, 33), S(-24, 50), S(-55, 75), S(-26, 68), S(-32, 50), S(14, 14), S(-38, -6),
                      S(21, -9), S(-7, 10), S(-51, 30), S(-135, 45), S(-73, 30), S(-91, 37), S(-1, 3), S(38, -42),
                      S(-8, -85), S(57, -31), S(-7, -12), S(-186, -11), S(-43, -96), S(-151, 0), S(31, -43), S(43, -133)};



Eval::Eval() {
    InitPieceBoards();
    InitKingZoneMask();
    InitPassedPawnsMask();
    InitForwardBackwardMask();
    InitDistanceArray();
    InitIsolatedPawnsMask();

    numPawnHashes = (8 * 0xFFFFF / sizeof(PawnHash));
    pawnHash = new PawnHash [numPawnHashes];
}



Eval::~Eval() {
    delete []pawnHash;
}



void Eval::savePawnHash(uint64_t key, int score) {
    pawnHash[key % numPawnHashes] = PawnHash(key, score);
}



int Eval::probePawnHash(uint64_t key, bool &hit) {
    if (pawnHash[key % numPawnHashes].pawnKey == key && key != 0) {
        hit = true;
        return pawnHash[key % numPawnHashes].score;
    }

    hit = false;
    return 0;
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

            pieceSquare[0][i * 8 + j] = PAWN_TABLE[(7 - i) * 8 + j];
            pieceSquare[1][i * 8 + j] = PAWN_TABLE[j + 8 * i];
            pieceSquare[2][i * 8 + j] = KNIGHT_TABLE[(7 - i) * 8 + j];
            pieceSquare[3][i * 8 + j] = KNIGHT_TABLE[j + 8 * i];
            pieceSquare[4][i * 8 + j] = BISHOP_TABLE[(7 - i) * 8 + j];
            pieceSquare[5][i * 8 + j] = BISHOP_TABLE[j + 8 * i];
            pieceSquare[6][i * 8 + j] = ROOK_TABLE[(7 - i) * 8 + j];
            pieceSquare[7][i * 8 + j] = ROOK_TABLE[j + 8 * i];
            pieceSquare[8][i * 8 + j] = QUEEN_TABLE[(7 - i) * 8 + j];
            pieceSquare[9][i * 8 + j] = QUEEN_TABLE[j + 8 * i];
            pieceSquare[10][i * 8 + j] = KING_TABLE[(7 - i) * 8 + j];
            pieceSquare[11][i * 8 + j] = KING_TABLE[j + 8 * i];

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



// Find all adjacent pawns
uint64_t Eval::adjacentMask(uint64_t pawns) {
    uint64_t ret = (pawns << 1) & ~columnMask[0];
    return ret | ((pawns >> 1) & ~columnMask[7]);
}



// Initialize variables in evaluation
void Eval::InitializeEval(Bitboard &board, ThreadSearch *th) {

    th->pawnAttAll[0] = pawnAttacksAll(board.pieces[0], 0);
    th->pawnAttAll[1] = pawnAttacksAll(board.pieces[1], 1);

    th->knightAttAll[0] = knightAttacks(board.pieces[2]);
    th->knightAttAll[1] = knightAttacks(board.pieces[3]);

    for (int i = 0; i < 2; i++) {

        // King safety
        th->unsafeSquares[i] = 0;
        th->KSAttackersWeight[i] = 0;
        th->KSAttacks[i] = 0;
        th->KSAttackersCount[i] = 0;
        th->attacksKnight[i] = 0;
        th->attacksBishop[i] = 0;
        th->attacksRook[i] = 0;
        th->attacksQueen[i] = 0;

        // Threats
        th->bishopAttacksAll[i] = 0;
        th->rookAttacksAll[i] = 0;

        // Mobility
        th->mobilityUnsafeSquares[i] = th->pawnAttAll[1 - i] | board.pieces[i] | board.pieces[10 + i];

        th->minorUnsafe[i] = th->mobilityUnsafeSquares[i] | board.pieces[8 + i];
        th->queenUnsafe[i] = th->mobilityUnsafeSquares[i] | th->knightAttAll[1 - i];
        th->tempUnsafe[i] = ~(th->pawnAttAll[1 - i] | th->knightAttAll[1 - i] | board.pieces[i]) & kingZoneMask[i][board.kingLoc[1 - i]];

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



#ifdef TUNER
// clears the trace to 0
void Eval::clearTrace() {
    evalTrace = emptyTrace;
}
#endif



// Evaluate the position
int Eval::evaluate(Bitboard &board, ThreadSearch *th) {

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

    InitializeEval(board, th);

    int ret = 0;
    int pawnScore = 0;
    bool hit = false;

    ret += board.material[0] - board.material[1];

    #ifdef TUNER
    evalTrace.tempoCoeff[board.toMove]++;
    for (int i = 0; i < 2; i++) {
        evalTrace.pawnCoeff[i]   = board.pieceCount[i];
        evalTrace.knightCoeff[i] = board.pieceCount[i + 2];
        evalTrace.bishopCoeff[i] = board.pieceCount[i + 4];
        evalTrace.rookCoeff[i]   = board.pieceCount[i + 6];
        evalTrace.queenCoeff[i]  = board.pieceCount[i + 8];
    }
    #endif

    #ifndef TUNER
    ret += probePawnHash(board.getPawnKey(), hit);
    #endif

    ret += evaluatePawns(board, th, false, hit, pawnScore) - evaluatePawns(board, th, true, hit, pawnScore);
    ret += evaluateKnights(board, th, false) - evaluateKnights(board, th, true);
    ret += evaluateBishops(board, th, false) - evaluateBishops(board, th, true);
    ret += evaluateRooks(board, th, false) - evaluateRooks(board, th, true);
    ret += evaluateQueens(board, th, false) - evaluateQueens(board, th, true);
    ret += evaluateKing(board, th, false) - evaluateKing(board, th, true);

    #ifdef TUNER
    return ret;
    #endif

    if (!hit) {
        savePawnHash(board.getPawnKey(), pawnScore);
    }

    int phase = getPhase(board);
    ret = ((MGVAL(ret) * (256 - phase)) + (EGVAL(ret) * phase)) / 256;
    return board.toMove? -ret : ret;
}



int Eval::evaluatePawns(Bitboard &board, ThreadSearch *th, bool col, bool hit, int &pawnScore) {

    int ret = 0;
    uint64_t pawns = board.pieces[col];
    while (pawns) {

        int bscan = bitScan(pawns);

        // PST
        ret += pieceSquare[col][bscan];

        #ifdef TUNER
        evalTrace.pawnPstCoeff[col? bscan : (bscan % 8) + (7 - (bscan / 8)) * 8][col]++;
        #endif

        pawns &= pawns - 1;
    }

    return ret;
}



int Eval::evaluateKnights(Bitboard &board, ThreadSearch *th, bool col) {

    int ret = 0;
    uint64_t knights = board.pieces[col + 2];
    while (knights) {

        int bscan = bitScan(knights);

        // PST
        ret += pieceSquare[col + 2][bscan];

        #ifdef TUNER
        evalTrace.knightPstCoeff[col? bscan : (bscan % 8) + (7 - (bscan / 8)) * 8][col]++;
        #endif

        knights &= knights - 1;
    }

    return ret;
}



int Eval::evaluateBishops(Bitboard &board, ThreadSearch *th, bool col) {

    int ret = 0;
    uint64_t bishops = board.pieces[col + 2];
    while (bishops) {

        int bscan = bitScan(bishops);

        // PST
        ret += pieceSquare[col + 4][bscan];

        #ifdef TUNER
        evalTrace.bishopPstCoeff[col? bscan : (bscan % 8) + (7 - (bscan / 8)) * 8][col]++;
        #endif

        bishops &= bishops - 1;
    }

    return ret;
}



int Eval::evaluateRooks(Bitboard &board, ThreadSearch *th, bool col) {

    int ret = 0;
    uint64_t rooks = board.pieces[col + 6];
    while (rooks) {

        int bscan = bitScan(rooks);

        // PST
        ret += pieceSquare[col + 6][bscan];

        #ifdef TUNER
        evalTrace.rookPstCoeff[col? bscan : (bscan % 8) + (7 - (bscan / 8)) * 8][col]++;
        #endif

        rooks &= rooks - 1;
    }

    return ret;
}



int Eval::evaluateQueens(Bitboard &board, ThreadSearch *th, bool col) {

    int ret = 0;
    uint64_t queens = board.pieces[col + 8];
    while (queens) {

        int bscan = bitScan(queens);

        // PST
        ret += pieceSquare[col + 8][bscan];

        #ifdef TUNER
        evalTrace.queenPstCoeff[col? bscan : (bscan % 8) + (7 - (bscan / 8)) * 8][col]++;
        #endif

        queens &= queens - 1;
    }

    return ret;
}



int Eval::evaluateKing(Bitboard &board, ThreadSearch *th, bool col) {

    int ret = 0;

    // PST
    ret += pieceSquare[col + 10][board.kingLoc[col]];

    #ifdef TUNER
    evalTrace.kingPstCoeff[col? board.kingLoc[col] : (board.kingLoc[col] % 8) + (7 - (board.kingLoc[col] / 8)) * 8][col]++;
    #endif

    return ret;
}













//
