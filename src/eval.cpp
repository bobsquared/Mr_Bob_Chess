#include "eval.h"

extern Magics *magics;



int pieceValues[6] = {S(114, 94), S(491, 332), S(500, 328), S(694, 559), S(1271, 1259), S(5000, 5000)};

// Queen, Bishop, Knight and rook weights
int knightWeight[9] = {S(-89, -75), S(-28, -35), S(-15, -20), S(-7, -9), S(-5, -1), S(4, 2), S(5, 23) , S(6, 38), S(14, 62)};
int rookWeight[9] = {S(125, 36), S(59, 33), S(38, 30), S(25, 29), S(13, 26), S(1, 26), S(-8, 22), S(-15, 16), S(-24, 9)};
int bishopWeight[9] = {S(81, 78), S(40, 77), S(37, 74), S(35, 72), S(35, 70), S(29, 69), S(26, 69), S(21, 67), S(20, 23)};
// int queenWeight[7] = {S(37, 20), S(45, 10), S(35, 24), S(18, 10), S(-5, 10), S(11, 19), S(54, 48)};

// Supported and adjacent pawn weights
int supportedPawnWeight[7] = {S(0, 0), S(0, 0), S(25, 24), S(21, 13), S(32, 10), S(69, 44), S(354, -53)};
int adjacentPawnWeight[7]  = {S(0, 0), S(11, 7), S(7, 10), S(18, 7), S(31, 29), S(101, 82), S(-69, 451)};

// Passed Pawn weights
int passedPawnWeight[7] = {S(0, 0), S(5, 2), S(-3, 9), S(-7, 29), S(14, 60), S(29, 145), S(62, 213)};

// Doubled pawns and isolated pawns
int doublePawnValue = S(18, 12);
int isolatedPawnValue = S(10, 2);

// Mobility
int knightMobilityBonus[9] =  {S(-42, -69), S(-23, -17), S(-8, 11), S(-8, 17), S(-1, 19), S(6, 20), S(12, 22), S(13, 28), S(33, 17)};
int bishopMobilityBonus[14] = {S(-35, -12), S(-26, -10), S(-4, 5), S(4, 20), S(17, 22), S(22, 30), S(26, 36), S(27, 38), S(30, 41), S(32, 41), S(46, 41), S(64, 42), S(82, 42), S(83, 43)};
int rookMobilityBonus[15] =   {S(-108, -64), S(-18, -44), S(-10, 2), S(-7, 18), S(-5, 25), S(-1, 31), S(1, 39), S(10, 41), S(20, 42), S(32, 42), S(37, 46), S(46, 50), S(52, 51), S(55, 51), S(70, 52)};
int queenMobilityBonus[27] =  {S(-164, -313), S(-29, -107), S(-27, -70), S(-22, -63), S(-15, -60), S(-15, -44), S(-5, -42), S(-2, -27), S(3, -8),
                              S(3, -2), S(2, 4), S(2, 19), S(6, 21), S(7, 29), S(7, 51), S(7, 53), S(7, 56), S(7, 59), S(27, 59),
                              S(44, 60), S(45, 63), S(47, 64), S(63, 64), S(84, 64), S(84, 67), S(90, 85), S(123, 88)};


int pieceAttackValue[5] = {0, 53, 47, 48, 3};
int attacksSafety = 31;
int queenCheckVal = 71;
int rookCheckVal = 63;
int bishopCheckVal = 59;
int knightCheckVal = 121;
int KSOffset = 96;


int kingPawnFront = S(27, -15);
int kingPawnFrontN = S(18, -9);

int kingPawnAdj = S(15, -18);
int kingPawnAdjN = S(10, -2);

int rookOnOpen = S(29, -15);
int rookOnSemiOpen = S(27, 12);
int rookOnQueen = S(-24, 18);

int knightPair = S(9, -2);
int rookPair = S(22, 18);
int noPawns = S(15, 35);

int trappedRook = S(-8, 48);




// -----------------------Pawn attack tables----------------------------------//
const int PAWN_TABLE[64] = {  0,   0,   0,   0,   0,   0,   0,   0,
                             57,  79,  18,  78,  71,  85, -29, -93,
                             17, -11,  30,  26,  81,  83,  45,  25,
                              2,   0,   0,  30,  32,  22,  18,   0,
                            -10, -22,   2,  20,  20,  24,  -2, -16,
                            -10, -22,  -2, -18,  -8, -16,  12, -14,
                              5,   7,   1, -11, -16,  20,  16, -12,
                              0,   0,   0,   0,   0,   0,   0,   0};

const int PAWN_TABLE_EG[64] = {  0,   0,   0,   0,   0,   0,   0,   0,
                                80,  56,  40,   4,  18,  -6,  63, 107,
                                56,  49,  16, -24, -45, -24,  20,  34,
                                33,  20,   6, -16, -12,  -4,  12,  20,
                                18,   0, -10,  -6,  -8,   7,   8,   0,
                                 4,  -8,   3,   0,   2, -12,  -8,  12,
                                 2,   8,   2,  18,   0,  -4,  -8,   0,
                                 0,   0,   0,   0,   0,   0,   0,   0};
// ---------------------------------------------------------------------------//


// -----------------------Knight attack tables----------------------------------//
const int KNIGHT_TABLE[64] = {-125,  -95,  -93, -64, -29,  -98, -82, -168,
                               -99,  -91,   25, -23, -27,   16,  -8,  -62,
                               -97,   26,   12,  32,  54,   72,  42,   14,
                               -29,   -5,    9,  37,  16,   57,   0,   10,
                               -37,   -8,   -1,   2,  16,    9,   3,  -15,
                               -52,  -30,   -5,  -1,  19,    2,  16,  -40,
                               -45,  -52,  -27,  -9,  -5,    0, -13,  -24,
                               -86,  -37,  -69, -56, -10,  -44, -37,  -21};

const int KNIGHT_TABLE_EG[64] = {-32, -20, -13, -30, -33, -21, -56, -88,
                                  -8,  -8, -26,  16,   0, -28, -25, -51,
                                 -12, -10,  22,  10,   8,  -5, -22, -59,
                                  -6,  10,  36,  25,  28,   6,   9, -14,
                                  -6,  -1,  24,  34,  20,  32,  17, -14,
                                 -14,   4,  -4,  20,  12,  -4, -20,  -1,
                                 -25,  -8,   0,  -4,  -1, -12, -11, -43,
                                   5, -43, -14,   4, -18,  -8, -39, -52};
// ---------------------------------------------------------------------------//


// -----------------------Bishop attack tables----------------------------------//
const int BISHOP_TABLE[64] = {-78, -50, -196, -136, -88, -103, -40,   -9,
                              -73, -28,  -60,  -63,   6,   12,  -6, -102,
                              -60,   1,   12,    2,   0,   37,  12,  -40,
                              -35,  -3,  -17,   20,  19,   11,  -2,  -18,
                              -20,  -6,    0,   12,  16,   -8,  -2,    2,
                               -8,   6,   10,    7,   2,   24,  11,   -5,
                                3,  20,   14,   -8,   8,   12,  28,   -6,
                              -38,   4,  -10,  -20,  -8,  -10, -58,  -39};

const int BISHOP_TABLE_EG[64] = {-8, -12,  12,  10,  8,  1, -7, -25,
                                  8,   2,  14,  -4, -6,  0, -2,  -6,
                                 12,   1,  -2,  -2, -4, -4,  1,   5,
                                  5,   9,  18,   8,  9,  6, -3,   4,
                                  0,   0,  16,  16,  5, 14, -4,  -7,
                                 -5,   2,  10,  13, 20,  0,  2,   4,
                                 -7, -16,  -4,   4,  5, -6, -6, -24,
                                 -2,   4,  -8,  8,  -2,  4,  8,   3};
// ---------------------------------------------------------------------------//


// -----------------------Rook attack tables----------------------------------//
const int ROOK_TABLE[64] = { -9,  30, -44,  19,   7, -51,   2, -20,
                             -4,   0,  28,  10,  50,  28, -18,   4,
                            -26,  -4,   2,  14, -14,  28,  62,  -3,
                            -38, -22,   4,   6,  14,  40, -16, -16,
                            -32, -26, -24, -10,  -4,  -9,  20, -14,
                            -33, -12, -10, -20,  -4,   8,  16,  -8,
                            -20,  -7, -16,  -4,   9,  20,   2, -47,
                             -4,  -4,   0,   4,  16,  22, -16,  16};

const int ROOK_TABLE_EG[64] = { 12,   2,  24,   8,  16,  22,  10,  12,
                                12,  16,   6,  10, -12,   6,  20,   8,
                                14,  16,   4,   4,   4,  -6, -11,  -6,
                                16,   8,  16,  -2,   0,  -6,   0,   4,
                                 8,  10,  16,   4,  -4,   1, -10, -10,
                                 0,   0,  -6,   0, -10, -16, -16, -16,
                                -6,  -8,  -4,  -4, -17, -22, -20,  -6,
                                -6,  -2,   0,  -4, -12, -18,  -8, -28};
// ---------------------------------------------------------------------------//


// -----------------------Queen attack tables----------------------------------//
const int QUEEN_TABLE[64] = { -39, -54, -43, -50, 116,  48,  56,  76,
                              -60, -81, -32,  -4, -80,  38,  16,  48,
                              -16, -21,   8, -20,  28,  76,  46,  52,
                              -41, -33, -40, -30, -16,   1,  -9,   2,
                               17, -48,  -8, -14,  -6,   0,  -8,   8,
                               -7,   8,  -8,   2,  -8,   0,  15,  22,
                              -16,   0,  24,  12,  22,  26,   3,  24,
                               17,  -8,  -4,  16, -16, -12, -29, -44};

const int QUEEN_TABLE_EG[64] = {  -6,  72,  68,  68,  -4,  24,   8,  22,
                                  20,  48,  57,  63, 113,  34,  56,  25,
                                   0,  22,   8,  84,  64,  32,  40,  28,
                                  40,  46,  56,  56,  80,  60,  96,  64,
                                 -28,  68,  32,  62,  48,  52,  80,  40,
                                  10, -16,  33,  16,  32,  40,  39,  32,
                                   0,  -8, -24,  -8, -10, -18, -24,  -2,
                                 -31, -20,  -8, -32,  28,   0,  -4, -40};
// ---------------------------------------------------------------------------//


// -----------------------King attack tables----------------------------------//
const int KING_TABLE_EG[64] =  { -60, -50, -40, -25, -25, -40, -50, -60,
                                 -50, -15, -10,   0,   0, -10, -15, -50,
                                 -40, -10,  20,  25,  25,  20, -10, -40,
                                 -30, -10,  25,  35,  35,  25, -10, -30,
                                 -30, -10,  25,  35,  35,  25, -10, -30,
                                 -40, -10,  20,  25,  25,  20, -10, -40,
                                 -50, -15,   0,   0,   0,   0, -15, -50,
                                 -60, -50, -40, -30, -30, -40, -50, -60};



const int KING_TABLE[64] =  { -50, -50, -50, -50, -50, -50, -50, -50,
                              -40, -40, -40, -45, -45, -40, -40, -40,
                              -35, -35, -35, -40, -40, -35, -35, -35,
                              -30, -30, -30, -35, -35, -30, -30, -30,
                              -20, -25, -25, -30, -30, -25, -25, -20,
                              -10, -20, -20, -25, -25, -20, -20, -10,
                               12,  12,   5,   0,   0,   0,  15,  15,
                               18,  22,  25,   0,   0,  15,  30,  20};
// ---------------------------------------------------------------------------//




Eval::Eval() {
    InitPieceBoards();
    InitKingZoneMask();
    InitPassedPawnsMask();
    InitForwardBackwardMask();
    InitDistanceArray();
    InitIsolatedPawnsMask();
    InitOutpostMask();

    numPawnHashes = (4 * 0xFFFFF / sizeof(PawnHash));
    pawnHash = new PawnHash [numPawnHashes];
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

            pieceSquare[0][i * 8 + j] = S(PAWN_TABLE[(7 - i) * 8 + j], PAWN_TABLE_EG[(7 - i) * 8 + j]);
            pieceSquare[1][i * 8 + j] = S(PAWN_TABLE[j + 8 * i], PAWN_TABLE_EG[j + 8 * i]);
            pieceSquare[2][i * 8 + j] = S(KNIGHT_TABLE[(7 - i) * 8 + j], KNIGHT_TABLE_EG[(7 - i) * 8 + j]);
            pieceSquare[3][i * 8 + j] = S(KNIGHT_TABLE[j + 8 * i], KNIGHT_TABLE_EG[j + 8 * i]);
            pieceSquare[4][i * 8 + j] = S(BISHOP_TABLE[(7 - i) * 8 + j], BISHOP_TABLE_EG[(7 - i) * 8 + j]);
            pieceSquare[5][i * 8 + j] = S(BISHOP_TABLE[j + 8 * i], BISHOP_TABLE_EG[j + 8 * i]);
            pieceSquare[6][i * 8 + j] = S(ROOK_TABLE[(7 - i) * 8 + j], ROOK_TABLE_EG[(7 - i) * 8 + j]);
            pieceSquare[7][i * 8 + j] = S(ROOK_TABLE[j + 8 * i], ROOK_TABLE_EG[j + 8 * i]);
            pieceSquare[8][i * 8 + j] = S(QUEEN_TABLE[(7 - i) * 8 + j], QUEEN_TABLE_EG[(7 - i) * 8 + j]);
            pieceSquare[9][i * 8 + j] = S(QUEEN_TABLE[j + 8 * i], QUEEN_TABLE_EG[j + 8 * i]);
            pieceSquare[10][i * 8 + j] = S(KING_TABLE[(7 - i) * 8 + j], KING_TABLE_EG[(7 - i) * 8 + j]);
            pieceSquare[11][i * 8 + j] = S(KING_TABLE[j + 8 * i], KING_TABLE_EG[j + 8 * i]);

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
    pawnScore = 0;

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

    ret += probePawnHash(board.getPawnKey(), hit);
    ret += evaluatePawns(board, false) - evaluatePawns(board, true);
    ret += evaluateKnights(board, false) - evaluateKnights(board, true);
    ret += evaluateBishops(board, false) - evaluateBishops(board, true);
    ret += evaluateRooks(board, false) - evaluateRooks(board, true);
    ret += evaluateQueens(board, false) - evaluateQueens(board, true);
    ret += evaluateKing(board, false) - evaluateKing(board, true);

    if (!hit) {
        savePawnHash(board.getPawnKey(), pawnScore);
    }

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
        ret += bishopWeight[board.pieceCount[0]];
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
    // ret += queenWeight[std::min(board.pieceCount[2 + col] + board.pieceCount[4 + col] + board.pieceCount[6 + col], 6)] * board.pieceCount[8 + col];

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



    if (!hit) {
        uint64_t supportedPawns = board.pieces[col] & pawnAttacksAll(board.pieces[col], col);
        uint64_t adjacentPawns = board.pieces[col] & adjacentMask(board.pieces[col]);
        uint64_t doubledPawns = col? ((board.pieces[col] ^ supportedPawns) << 8) & board.pieces[col] : ((board.pieces[col] ^ supportedPawns) >> 8) & board.pieces[col];
        uint64_t isolatedPawns = ~supportedPawns & ~adjacentPawns & board.pieces[col];

        ret -= doublePawnValue * count_population(doubledPawns);

        while (isolatedPawns) {
            if ((isolatedPawnMask[bitScan(isolatedPawns)] & board.pieces[col]) == 0) {
                ret -= isolatedPawnValue;
            }
            isolatedPawns &= isolatedPawns - 1;
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

        pawnScore += col? -ret : ret;
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

    return ret;

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
