#include "eval.h"

extern Magics *magics;


int pieceValues[6] = {S(62, 267), S(315, 790), S(347, 830), S(434, 1371), S(1866, 1993), S(5000, 5000)};

int PAWN_TABLE[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                      S(294, 294), S(290, 288), S(300, 242), S(300, 189), S(312, 169), S(222, 190), S(25, 281), S(55, 289),
                      S(-9, 219), S(79, 204), S(100, 172), S(121, 136), S(133, 113), S(175, 87), S(92, 152), S(7, 159),
                      S(-5, 75), S(34, 60), S(23, 37), S(46, 3), S(69, -7), S(65, -1), S(57, 30), S(12, 21),
                      S(-16, 28), S(14, 27), S(8, -11), S(10, -12), S(27, -17), S(18, -12), S(43, 2), S(9, -14),
                      S(-17, 10), S(0, 17), S(-5, -5), S(-8, 4), S(13, 3), S(4, -1), S(51, 2), S(21, -28),
                      S(-23, 28), S(-4, 29), S(-21, 19), S(-18, 20), S(-20, 30), S(30, 24), S(64, 13), S(15, -33),
                      S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int KNIGHT_TABLE[64] = {S(-256, -184), S(-42, -62), S(-128, 21), S(20, -15), S(48, -18), S(-79, 24), S(29, -48), S(-144, -182),
                        S(-14, -82), S(0, -39), S(61, -6), S(99, 37), S(93, 30), S(149, -35), S(-6, -31), S(53, -75),
                        S(-41, -52), S(27, 10), S(84, 48), S(97, 56), S(152, 36), S(160, 60), S(85, 18), S(56, -45),
                        S(-1, -35), S(19, 26), S(56, 59), S(64, 92), S(41, 100), S(80, 80), S(33, 59), S(49, -8),
                        S(-17, -46), S(3, 17), S(13, 65), S(7, 78), S(21, 75), S(15, 74), S(50, 27), S(4, -29),
                        S(-41, -108), S(-15, -17), S(-7, 2), S(11, 38), S(13, 33), S(-1, 2), S(-4, -9), S(-30, -90),
                        S(-68, -112), S(-65, -37), S(-31, -39), S(-15, -17), S(-25, -11), S(-16, -43), S(-37, -48), S(-29, -92),
                        S(-187, -138), S(-42, -155), S(-101, -74), S(-59, -52), S(-44, -53), S(-37, -70), S(-38, -136), S(-172, -190)};

int BISHOP_TABLE[64] = {S(-85, 31), S(-82, 40), S(-130, 43), S(-127, 56), S(-66, 35), S(-121, 28), S(41, 6), S(-71, 38),
                        S(-55, -4), S(17, 32), S(0, 39), S(-31, 46), S(1, 41), S(44, 28), S(-13, 46), S(22, -15),
                        S(4, 4), S(30, 36), S(65, 41), S(64, 37), S(104, 35), S(122, 52), S(92, 37), S(38, 19),
                        S(-17, 10), S(25, 45), S(36, 40), S(84, 52), S(50, 72), S(72, 44), S(17, 65), S(4, 19),
                        S(-26, -4), S(7, 27), S(14, 51), S(24, 56), S(33, 51), S(4, 48), S(11, 28), S(4, -12),
                        S(-12, -14), S(6, 9), S(1, 32), S(8, 35), S(2, 35), S(-1, 26), S(4, -6), S(14, -10),
                        S(-1, -29), S(1, -29), S(10, -12), S(-17, 5), S(-12, 6), S(1, -23), S(20, -20), S(11, -82),
                        S(0, -56), S(-9, -32), S(-32, -69), S(-54, -14), S(-48, -24), S(-28, -38), S(-26, -45), S(-6, -44)};

int ROOK_TABLE[64] = {S(137, 55), S(145, 60), S(152, 69), S(158, 65), S(174, 61), S(176, 66), S(211, 51), S(239, 43),
                      S(66, 68), S(60, 81), S(107, 79), S(137, 80), S(138, 78), S(208, 23), S(112, 49), S(179, 24),
                      S(23, 60), S(89, 43), S(77, 66), S(118, 46), S(163, 28), S(171, 39), S(210, 6), S(130, 21),
                      S(5, 37), S(21, 51), S(39, 59), S(76, 50), S(64, 47), S(79, 38), S(82, 25), S(79, 19),
                      S(-27, 12), S(-34, 43), S(-26, 49), S(-8, 41), S(-10, 33), S(-8, 32), S(31, 20), S(11, 0),
                      S(-48, -25), S(-28, 2), S(-39, 5), S(-24, 2), S(-26, 0), S(-32, -2), S(20, -9), S(-9, -30),
                      S(-86, -29), S(-22, -36), S(-22, -15), S(-16, -18), S(-26, -17), S(-2, -34), S(17, -36), S(-61, -25),
                      S(-16, -31), S(-8, -10), S(-1, 3), S(3, 4), S(-1, -2), S(-6, -1), S(27, -11), S(-3, -52)};

int QUEEN_TABLE[64] = {S(-50, 117), S(-33, 162), S(-14, 163), S(14, 175), S(45, 185), S(150, 152), S(45, 199), S(92, 163),
                       S(-33, 57), S(-78, 117), S(-18, 132), S(-61, 206), S(-34, 255), S(54, 206), S(-86, 268), S(31, 163),
                       S(-34, 23), S(-31, 62), S(-25, 103), S(0, 136), S(23, 195), S(78, 215), S(97, 173), S(43, 166),
                       S(-28, -4), S(-31, 77), S(-26, 77), S(-32, 153), S(-22, 192), S(-1, 211), S(8, 186), S(19, 133),
                       S(-17, -40), S(-20, 38), S(-21, 40), S(-39, 128), S(-33, 112), S(-13, 106), S(-6, 73), S(9, 89),
                       S(-19, -62), S(-13, -30), S(-23, 30), S(-23, -6), S(-23, -5), S(-22, 25), S(4, -28), S(12, -48),
                       S(-25, -86), S(-20, -44), S(-10, -82), S(-18, -42), S(-15, -63), S(9, -146), S(25, -174), S(-4, -101),
                       S(-23, -62), S(-36, -86), S(-31, -96), S(-19, -85), S(-21, -106), S(-63, -84), S(-54, -123), S(-40, -43)};

int KING_TABLE[64] = {S(298, -334), S(508, -168), S(318, -90), S(311, -55), S(281, -72), S(173, -45), S(250, -65), S(448, -269),
                      S(31, -37), S(198, 59), S(221, 36), S(176, 11), S(208, 14), S(217, 34), S(96, 88), S(63, -6),
                      S(61, 44), S(187, 86), S(211, 73), S(190, 56), S(200, 50), S(235, 87), S(155, 97), S(57, 32),
                      S(61, 24), S(116, 80), S(150, 83), S(55, 100), S(100, 90), S(115, 84), S(108, 73), S(-56, 34),
                      S(53, -19), S(60, 51), S(79, 72), S(18, 97), S(36, 88), S(17, 75), S(70, 41), S(-26, -4),
                      S(-29, -6), S(-28, 32), S(-30, 50), S(-62, 76), S(-35, 67), S(-36, 49), S(10, 14), S(-37, -6),
                      S(23, -8), S(-7, 8), S(-49, 29), S(-139, 45), S(-79, 31), S(-93, 37), S(6, 0), S(37, -40),
                      S(-4, -83), S(62, -32), S(-6, -14), S(-168, -13), S(-40, -89), S(-149, 0), S(30, -40), S(46, -129)};



// Mobility
int knightMobilityBonus[9] = {S(12, -179), S(24, -60), S(31, -2), S(29, 18), S(30, 17), S(33, 24), S(41, 25), S(50, 14), S(70, -18)};
int bishopMobilityBonus[14] = {S(29, -181), S(34, -106), S(33, -60), S(35, -34), S(45, -11), S(51, 15), S(53, 26), S(48, 34), S(50, 37), S(57, 32), S(66, 21), S(103, 0), S(113, 18), S(191, -44)};
int rookMobilityBonus[15] = {S(62, -234), S(50, -67), S(51, -59), S(50, -32), S(48, -18), S(43, -1), S(47, 13), S(51, 17), S(61, 24), S(68, 32), S(79, 37), S(86, 43), S(91, 49), S(113, 30), S(193, -11)};
int queenMobilityBonus[28] = {S(-115, -574), S(-74, -344), S(-69, -150), S(-68, -77), S(-67, -15), S(-68, 20), S(-70, 59), S(-72, 92),
                              S(-75, 125), S(-75, 146), S(-75, 162), S(-76, 174), S(-73, 186), S(-72, 193), S(-78, 206), S(-72, 208),
                              S(-84, 225), S(-76, 219), S(-86, 229), S(-83, 227), S(-75, 214), S(-47, 193), S(-75, 203), S(-24, 181),
                              S(-41, 161), S(-94, 199), S(17, 194), S(-93, 197)};

int passedPawnWeight[7] = {S(0, 0), S(1, -11), S(-7, -6), S(4, 35), S(32, 57), S(37, 50), S(-20, 32)};
int freePasser[7]  = {S(0, 0), S(-3, -9), S(-1, -9), S(6, -26), S(17, -48), S(42, -109), S(59, -140)};
int isolatedPawnValue = S(-4, -10);
int adjacentPawnWeight[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                              S(-18, 314), S(-124, 302), S(-23, 354), S(134, 321), S(13, 187), S(208, 372), S(229, 298), S(130, 182),
                              S(85, 153), S(67, 161), S(12, 182), S(30, 163), S(7, 192), S(47, 189), S(46, 160), S(26, 154),
                              S(31, 30), S(25, 79), S(34, 43), S(29, 90), S(15, 87), S(47, 64), S(7, 63), S(46, 48),
                              S(-4, 0), S(23, 26), S(6, 31), S(20, 39), S(24, 43), S(14, 23), S(42, 10), S(-7, 21),
                              S(16, 7), S(-3, 9), S(8, 11), S(8, 13), S(13, 21), S(20, 3), S(-3, 22), S(8, -11),
                              S(13, -7), S(1, 10), S(27, 15), S(7, 23), S(17, 37), S(16, -2), S(-1, 16), S(28, -26),
                              S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int supportedPawnWeight[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                               S(106, 106), S(272, 75), S(205, 75), S(267, 82), S(410, -3), S(272, 6), S(310, 89), S(44, 183),
                               S(45, -25), S(32, 0), S(46, 26), S(67, 59), S(126, 22), S(108, 28), S(84, -26), S(49, -4),
                               S(-2, 10), S(19, 14), S(24, 12), S(35, -3), S(41, 17), S(39, 4), S(27, 14), S(30, 7),
                               S(11, 11), S(20, 26), S(26, 19), S(23, 28), S(29, 25), S(18, 10), S(12, 18), S(16, 4),
                               S(14, 27), S(25, 28), S(36, 44), S(32, 42), S(30, 41), S(22, 33), S(34, 20), S(33, 21),
                               S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                               S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int tempoBonus = S(14, 22);



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

            adjacentPawnsVal[0][i * 8 + j] = adjacentPawnWeight[(7 - i) * 8 + j];
            adjacentPawnsVal[1][i * 8 + j] = adjacentPawnWeight[j + 8 * i];

            supportedPawnsVal[0][i * 8 + j] = supportedPawnWeight[(7 - i) * 8 + j];
            supportedPawnsVal[1][i * 8 + j] = supportedPawnWeight[j + 8 * i];

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
        th->queenUnsafe[i] = th->mobilityUnsafeSquares[i] | board.pieces[2 + i] | th->knightAttAll[1 - i];
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



int Eval::flipSide64(int index) {
    return (index % 8) + (7 - (index / 8)) * 8;
}



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

    ret += board.toMove == 0? tempoBonus : -tempoBonus;
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
    uint64_t adjacentPawns = board.pieces[col] & adjacentMask(board.pieces[col]);
    uint64_t supportedPawns = board.pieces[col] & th->pawnAttAll[col];

    while (pawns) {

        int bscan = bitScan(pawns);

        // PST
        ret += pieceSquare[col][bscan];

        #ifdef TUNER
        evalTrace.pawnPstCoeff[col? bscan : flipSide64(bscan)][col]++;
        #endif

        // Passed pawns
        if ((passedPawnMask[col][bscan] & board.pieces[!col]) == 0 && (forwardMask[col][bscan] & board.pieces[col]) == 0) {
           ret += col? passedPawnWeight[(7 - (bscan / 8))] : passedPawnWeight[(bscan / 8)];

           #ifdef TUNER
           evalTrace.passedPawnCoeff[col? (7 - (bscan / 8)) : (bscan / 8)][col]++;
           #endif

           if ((forwardMask[col][bscan] & board.occupied) != 0) {
                ret += freePasser[col? (7 - (bscan / 8)) : (bscan / 8)];

                #ifdef TUNER
                evalTrace.freePasserCoeff[col? (7 - (bscan / 8)) : (bscan / 8)][col]++;
                #endif
            }
        }

        if (adjacentPawns & (1ULL << bscan)) {
            ret += adjacentPawnsVal[col][bscan];

            #ifdef TUNER
            evalTrace.adjacentPawnsCoeff[col? bscan : flipSide64(bscan)][col]++;
            #endif
        }

        if (supportedPawns & (1ULL << bscan)) {
            ret += supportedPawnsVal[col][bscan];

            #ifdef TUNER
            evalTrace.supportedPawnsCoeff[col? bscan : flipSide64(bscan)][col]++;
            #endif
        }

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
        evalTrace.knightPstCoeff[col? bscan : flipSide64(bscan)][col]++;
        #endif

        // Mobility
        ret += knightMobilityBonus[count_population(board.knightMoves[bscan] & ~th->minorUnsafe[col])];

        #ifdef TUNER
        evalTrace.knightMobilityCoeff[count_population(board.knightMoves[bscan] & ~th->minorUnsafe[col])][col]++;
        #endif

        knights &= knights - 1;
    }

    return ret;
}



int Eval::evaluateBishops(Bitboard &board, ThreadSearch *th, bool col) {

    int ret = 0;
    uint64_t bishops = board.pieces[col + 4];
    while (bishops) {

        int bscan = bitScan(bishops);
        uint64_t bishopAttacks = magics->bishopAttacksMask(board.occupied, bscan);

        // PST
        ret += pieceSquare[col + 4][bscan];

        #ifdef TUNER
        evalTrace.bishopPstCoeff[col? bscan : flipSide64(bscan)][col]++;
        #endif

        // Mobility
        ret += bishopMobilityBonus[count_population(bishopAttacks & ~th->minorUnsafe[col])];

        #ifdef TUNER
        evalTrace.bishopMobilityCoeff[count_population(bishopAttacks & ~th->minorUnsafe[col])][col]++;
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
        uint64_t rookAttacks = magics->rookAttacksMask(board.occupied, bscan);

        // PST
        ret += pieceSquare[col + 6][bscan];

        #ifdef TUNER
        evalTrace.rookPstCoeff[col? bscan : flipSide64(bscan)][col]++;
        #endif

        // Mobility
        ret += rookMobilityBonus[count_population(rookAttacks & ~th->minorUnsafe[col])];

        #ifdef TUNER
        evalTrace.rookMobilityCoeff[count_population(rookAttacks & ~th->minorUnsafe[col])][col]++;
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
        uint64_t queenAttacks = magics->queenAttacksMask(board.occupied, bscan);

        // PST
        ret += pieceSquare[col + 8][bscan];

        #ifdef TUNER
        evalTrace.queenPstCoeff[col? bscan : flipSide64(bscan)][col]++;
        #endif

        // Mobility
        ret += queenMobilityBonus[count_population(queenAttacks & ~th->queenUnsafe[col])];

        #ifdef TUNER
        evalTrace.queenMobilityCoeff[count_population(queenAttacks & ~th->queenUnsafe[col])][col]++;
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
    evalTrace.kingPstCoeff[col? board.kingLoc[col] : flipSide64(board.kingLoc[col])][col]++;
    #endif

    return ret;
}













//
