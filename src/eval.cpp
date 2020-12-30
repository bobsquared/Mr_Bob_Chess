#include "eval.h"

extern Magics *magics;


int pieceValues[6] = {S(114, 94), S(491, 332), S(500, 328), S(694, 559), S(1271, 1259), S(5000, 5000)};

// Queen, Bishop, Knight and rook weights
int knightWeight[9] = {S(-89, -75), S(-28, -35), S(-15, -20), S(-7, -9), S(-5, -1), S(4, 2), S(5, 23) , S(6, 38), S(14, 62)};
int rookWeight[9] = {S(125, 36), S(59, 33), S(38, 30), S(25, 29), S(13, 26), S(1, 26), S(-8, 22), S(-15, 16), S(-24, 9)};
int bishopWeight[9] = {S(81, 78), S(40, 77), S(37, 74), S(35, 72), S(35, 70), S(29, 69), S(26, 69), S(21, 67), S(20, 23)};
// int queenWeight[7] = {S(37, 20), S(45, 10), S(35, 24), S(18, 10), S(-5, 10), S(11, 19), S(54, 48)};

// Supported and adjacent pawn weights
int supportedPawnWeight[7] = {S(0, 0), S(0, 0), S(17, 24), S(14, 13), S(20, 21), S(53, 54), S(288, -38)};
int adjacentPawnWeight[7]  = {S(0, 0), S(4, 8), S(6, 7), S(7, 19), S(23, 48), S(57, 96), S(123, 74)};
int freePasser[7]  = {S(0, 0), S(-4, -2), S(-4, 1), S(-2, -6), S(-1, -14), S(3, -39), S(53, -116)};

// Passed Pawn weights
int passedPawnWeight[7] = {S(0, 0), S(-2, 18), S(-8, 21), S(-5, 46), S(29, 67), S(57, 127), S(65, 210)};

// Doubled pawns and isolated pawns and backward pawns
int doublePawnValue = S(18, 12);
int isolatedPawnValue = S(10, 2);

// Mobility
int knightMobilityBonus[9] =  {S(-45, -152), S(-30, -67), S(-21, -23), S(-19, 2), S(-18, 12), S(-13, 24), S(-5, 28), S(1, 29), S(9, 22)};
int bishopMobilityBonus[14] = {S(-35, -12), S(-26, -10), S(-4, 5), S(4, 20), S(17, 22), S(22, 30), S(26, 36), S(27, 38), S(30, 41), S(32, 41), S(46, 41), S(64, 42), S(82, 42), S(83, 43)};
int rookMobilityBonus[15] =   {S(-108, -64), S(-18, -44), S(-10, 2), S(-7, 18), S(-5, 25), S(-1, 31), S(1, 39), S(10, 41), S(20, 42), S(32, 42), S(37, 46), S(46, 50), S(52, 51), S(55, 51), S(70, 52)};
int queenMobilityBonus[28] =  {S(-164, -313), S(-29, -107), S(-27, -70), S(-22, -63), S(-15, -60), S(-15, -44), S(-5, -42), S(-2, -27), S(3, -8),
                              S(3, -2), S(2, 4), S(2, 19), S(6, 21), S(7, 29), S(7, 51), S(7, 53), S(7, 56), S(7, 59), S(27, 59),
                              S(44, 60), S(45, 63), S(47, 64), S(63, 64), S(84, 64), S(84, 67), S(90, 85), S(123, 88), S(135, 90)};


int pieceAttackValue[5] = {0, 63, 63, 57, 7};
int attacksSafety = 17;
int queenCheckVal = 72;
int rookCheckVal = 78;
int bishopCheckVal = 72;
int knightCheckVal = 126;
int KSOffset = 123;

int pawnShield = S(18, -1);
int kingPawnFront = S(33, -19);
int kingPawnFrontN = S(23, -13);
int kingPawnAdj = S(20, -15);
int kingPawnAdjN = S(6, -7);

int rookOnOpen = S(21, 5);
int rookOnSemiOpen = S(20, 2);
int rookOnQueen = S(-17, 19);

int knightPair = S(9, -2);
int rookPair = S(22, 18);
int noPawns = S(15, 35);

int trappedRook = S(-8, 48);
int rookBehindPasser = S(6, 12);
int tempoBonus = S(16, 16);

int pawnThreat = S(43, 53);
int pawnPushThreat = S(6, 10);

int knightThreatPiece[5] = {S(1, 13), S(0, 0), S(31, 33), S(54, 15), S(37, 33)};
int bishopThreatPiece[5] = {S(-2, 8), S(27, 40), S(0, 0), S(36, 28), S(29, 30)};
int rookThreatPiece[5] = {S(-4, 17), S(9, 21), S(12, 24), S(0, 0), S(20, 25)};


// -----------------------Pawn attack tables----------------------------------//
int PAWN_TABLE[64] = {  0,   0,   0,   0,   0,   0,   0,   0,
                       68,  63,  44,  71,  45,  17, -40, -72,
                       16,   4,  24,  23,  72, 100,  40,   8,
                        0,   4,   4,  20,  20,  12,   8, -14,
                       -4, -22,   6,  15,  15,  18, -12, -22,
                      -10, -19,  -3, -22, -10, -16,   8, -22,
                      -13,  -3,  -8, -13, -16,  17,  28, -18,
                        0,   0,   0,   0,   0,   0,   0,   0};

int PAWN_TABLE_EG[64] = {  0,   0,   0,   0,   0,   0,   0,   0,
                          73,  64,  33,   6,  21,  23,  60,  86,
                          52,  44,  20, -20, -38, -20,  17,  33,
                          31,  17,   4, -14,  -8,  -3,  13,  18,
                          12,  14,  -6, -12,  -8, -10,   4,   2,
                           2,   4,  -8,   3,   3,   2,  -9,  -7,
                          10,   3,   6,   1,  16,   2, -10,  -9,
                           0,   0,   0,   0,   0,   0,   0,   0};
// ---------------------------------------------------------------------------//


// -----------------------Knight attack tables----------------------------------//
int KNIGHT_TABLE[64] = {-216, -152,  -84, -41,  32, -164, -96, -172,
                        -136,  -82,  -22, -23, -64,   -4, -54,  -56,
                         -87,   -6,  -14,   8,  48,   72,  36,   14,
                         -19,   22,    3,  44,  38,   72,  30,   25,
                         -18,    4,   11,  17,  32,   32,  34,   -5,
                         -28,  -16,   15,  12,  32,   20,  28,  -20,
                         -39,  -51,  -15,   1,   7,   15,   2,  -12,
                        -128,  -25,  -48, -40,   3,  -34, -23,  -17};

int KNIGHT_TABLE_EG[64] = {-20, -18,  14, -25, -21, -12, -40, -58,
                             8,  16,   0,  20,  16, -11,  -4, -37,
                            -3,  -1,  26,  16,  -5, -10, -21, -41,
                             2,  12,  29,  24,  20,   2,   8,  -9,
                             1,   6,  22,  30,  22,  24,  19,   2,
                            -1,  12,  -1,  24,  16,   4, -12,   2,
                            -8,   1,  10,   8,   8,  -7,   0, -24,
                            19, -20,  -6,  18,  -4,   8, -24, -23};
// ---------------------------------------------------------------------------//


// -----------------------Bishop attack tables----------------------------------//
int BISHOP_TABLE[64] = {-48, -26, -140, -104, -104,  -81, -24,   -6,
                        -48, -31,  -57,  -76,    8,  -15, -44,  -96,
                        -22,  13,    1,    2,  -10,   61,  18,    4,
                        -23,   1,   -6,   40,   16,   20,  -2,   -2,
                        -16,   3,    6,   28,   31,    4,   2,   10,
                        -12,  12,   10,    4,   15,   28,  18,    3,
                        -4,  11,    8,   -1,    4,   12,  36,   -2,
                        -37,  -3,   -2,  -13,    2,  -12, -48,  -16};

int BISHOP_TABLE_EG[64] = {-8, -15,   0,   2,  9,  -3, -13, -28,
                           -2,   6,  13,  -4, -9,   0,   2,  -9,
                           -1,  -7,   1,  -5, -7, -11,  -3,  -8,
                            2,   8,  15,   2, 10,   8,  -3,  -4,
                           -8,   1,  10,  11, -2,   8,  -2,  -6,
                           -4,  -4,   6,  12, 12,  -4,  -2,  -8,
                           -7, -16,  -4,   0,  4,  -6, -16, -25,
                           -8,   2,  -8,  0,  -4,   3,  10,  -6};
// ---------------------------------------------------------------------------//


// -----------------------Rook attack tables----------------------------------//
int ROOK_TABLE[64] = {-24,   7, -24,   4, -17, -65,   2, -19,
                       -6,  10,  13,  16,  28,  37, -24,  -4,
                      -19,  10,  14,   2, -16,  44,  80,  -4,
                      -26, -12,   0,  11,   4,  35,  -7, -20,
                      -32, -28, -20,  -8,  -3,  -4,  36, -19,
                      -36, -18, -15, -18,  -5,  12,  10, -16,
                      -35, -16, -25, -13,   0,   8,  10, -50,
                       -8,  -8,  -4,   2,   6,  15, -20,   0};

int ROOK_TABLE_EG[64] = { 10,   0,  12,   4,  10,  17,   1,   4,
                           3,   1,   4,   0, -13, -10,   8,   2,
                           0,  -2,  -9,  -1,  -4, -18, -24, -13,
                           3,  -1,   7,  -8,   3,  -9,  -6,   0,
                           4,   6,   8,   0,  -6,  -9, -18,  -8,
                          -4,  -3, -10,  -7, -15, -21, -14, -19,
                          -4,  -8,  -2,   0, -16, -19, -29,  -5,
                          -8,  -5,  -4,  -8, -12, -18,  -6, -20};
// ---------------------------------------------------------------------------//


// -----------------------Queen attack tables----------------------------------//
int QUEEN_TABLE[64] = { -38, -18, -38, -13,  87,  28,  64,  60,
                        -54, -69, -22,   8, -68,  16,  -7,  36,
                         -9, -12, -10,   0,  27,  80,  51,  55,
                        -34, -17, -20, -32, -15,  18,   4,  12,
                         17, -43,  -8, -12,   2,   8,  -1,   9,
                         -4,   3,  -8,  -1,  -8,   1,   9,  17,
                        -27, -11,  11,   6,  16,  21,   0,  32,
                          2, -13,  -8,   9,  -8, -11, -27, -60};

int QUEEN_TABLE_EG[64] = { -24,  28,  44,  12,  -5,  19, -33,  18,
                            -9,  16,  34,  40,  94,  19,  31,  16,
                           -21,   0,  -6,  54,  42,  16,  18,   8,
                            20,   8,  21,  43,  64,  37,  81,  40,
                           -40,  63,  24,  48,  24,  36,  62,  41,
                           -10, -20,  14,   3,  24,  34,  35,  19,
                             0, -16, -23,  -9, -20, -26, -40, -36,
                           -34, -30, -18, -48,  -3, -32, -16, -32};
// ---------------------------------------------------------------------------//


// -----------------------King attack tables----------------------------------//
int KING_TABLE[64] =  { -97, -22, -18, -33, -57, -21, -13, -29,
                        -15, -17, -24, -16, -21,  -8, -21, -10,
                         -1,   4,  -2, -42, -30,   7,  23, -10,
                         -8,  -5, -12, -25, -35, -24,  -9, -28,
                        -24,  21,   1, -24, -79, -12, -28, -60,
                        -19,   0,  -9, -32, -40, -32,   0,  -4,
                         16,   0, -24, -68, -44, -32,   8,  42,
                        -31,  16,   7, -72,   0, -52,  12,  20};

int KING_TABLE_EG[64] =  { -80, -57, -62, -45,  -2,   3, -22, -31,
                           -56,  -9,  -4, -24,   6,  26,  10,  12,
                           -20,   4,  -4,  -1,   0,  22,  20,  -2,
                           -32,   5,  10,  18,  16,  24,  14,  -7,
                           -26, -20,  17,  26,  37,  19,   9, -14,
                           -20,  -3,  12,  24,  30,  21,  -7, -16,
                           -31,  -3,  15,  24,  24,  20,   2, -32,
                           -56, -32, -16,   6, -12,   9, -16, -56};
// ---------------------------------------------------------------------------//



// ---------------------------Knight Outpost----------------------------------//
int outpostPotential[64] = { 0,  0,  0,  0,  0,  0,  0,  0,
                           -75, 59,-34,-31, 16, 66, 64, 23,
                            75, 51, 24, 25, 87, 34, 21, 64,
                            18, 26, 20, 48, 52, 64, 66, 63,
                            25, 26, 44, 52, 49, 66, 36, 30,
                             6,  0,  8, 16, 16,  0,-56, 32,
                             0,  0,  0,  0,  0,  0,  0,  0,
                             0,  0,  0,  0,  0,  0,  0,  0};

int outpostPotentialEG[64] = { 0,  0,  0,  0,  0,  0,  0,  0,
                             -71,  1,-26,-26, 50,-31, 56, 63,
                               4, 12, 14, 33, 15, 30, 47, 32,
                              15,  0, 14, 22, 18, 21, -6,  6,
                              26, 14, 24, 13, 22, -8,-14,  6,
                             -72,  9, -6, 24, 18,  0, 46, -4,
                               0,  0,  0,  0,  0,  0,  0,  0,
                               0,  0,  0,  0,  0,  0,  0,  0};
// ---------------------------------------------------------------------------//



Eval::Eval() {
    InitPieceBoards();
    InitKingZoneMask();
    InitPassedPawnsMask();
    InitForwardBackwardMask();
    InitDistanceArray();
    InitIsolatedPawnsMask();
    InitOutpostMask();

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

  for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        knightOutpost[0][i * 8 + j] = S(outpostPotential[(7 - i) * 8 + j], outpostPotentialEG[(7 - i) * 8 + j]);
        knightOutpost[1][i * 8 + j] = S(outpostPotential[j + 8 * i], outpostPotentialEG[j + 8 * i]);
      }
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

    // assert(board.kingLoc[0] == bitScan(board.pieces[10]));
    // assert(board.kingLoc[1] == bitScan(board.pieces[11]));
    #endif



    InitializeEval(board, th);

    int ret = 0;
    int pawnScore = 0;
    bool hit = false;

    ret += board.toMove? -tempoBonus : tempoBonus;
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

    ret += evaluateImbalance(board, false) - evaluateImbalance(board, true);
    ret += evaluatePawnShield(board, false) - evaluatePawnShield(board, true);

    #ifndef TUNER
    ret += probePawnHash(board.getPawnKey(), hit);
    #endif

    ret += evaluatePawns(board, th, false, hit, pawnScore) - evaluatePawns(board, th, true, hit, pawnScore);
    ret += evaluateKnights(board, th, false) - evaluateKnights(board, th, true);
    ret += evaluateBishops(board, th, false) - evaluateBishops(board, th, true);
    ret += evaluateRooks(board, th, false) - evaluateRooks(board, th, true);
    ret += evaluateQueens(board, th, false) - evaluateQueens(board, th, true);
    ret += evaluateKing(board, th, false) - evaluateKing(board, th, true);
    ret += evaluateThreats(board, th, false) - evaluateThreats(board, th, true);

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




// Evaluate material imbalance
int Eval::evaluateImbalance(Bitboard &board, bool col) {

    int ret = 0;

    // Bishop pair
    if (board.pieceCount[4 + col] >= 2) {
        ret += bishopWeight[board.pieceCount[col]];

        #ifdef TUNER
        evalTrace.bishopWeightCoeff[board.pieceCount[col]][col]++;
        #endif
    }

    // Knight pair
    if (board.pieceCount[2 + col] >= 2) {
        ret -= knightPair;

        #ifdef TUNER
        evalTrace.knightPairCoeff[!col]++;
        #endif
    }

    // Rook pair
    if (board.pieceCount[6 + col] >= 2) {
        ret -= rookPair;

        #ifdef TUNER
        evalTrace.rookPairCoeff[!col]++;
        #endif
    }

    // Pawn count
    if (board.pieceCount[col] == 0) {
        ret -= noPawns;

        #ifdef TUNER
        evalTrace.noPawnsCoeff[!col]++;
        #endif
    }

    ret += knightWeight[board.pieceCount[col]] * board.pieceCount[2 + col];
    ret += rookWeight[board.pieceCount[col]] * board.pieceCount[6 + col];

    #ifdef TUNER
    evalTrace.knightWeightCoeff[board.pieceCount[col]][col] += board.pieceCount[2 + col];
    evalTrace.rookWeightCoeff[board.pieceCount[col]][col] += board.pieceCount[6 + col];
    #endif

    return ret;
}



int Eval::evaluatePawns(Bitboard &board, ThreadSearch *th, bool col, bool hit, int &pawnScore) {

    int ret = 0;
    int dist = 0;
    int distFinal = 0;
    uint64_t piece = board.pieces[col];
    int ourKing = board.kingLoc[col];
    int theirKing = board.kingLoc[!col];

    th->unsafeSquares[!col] |= th->pawnAttAll[col];



    if (!hit) {
        uint64_t supportedPawns = board.pieces[col] & th->pawnAttAll[col];
        uint64_t adjacentPawns = board.pieces[col] & adjacentMask(board.pieces[col]);
        uint64_t doubledPawns = col? ((board.pieces[col] ^ supportedPawns) << 8) & board.pieces[col] : ((board.pieces[col] ^ supportedPawns) >> 8) & board.pieces[col];
        uint64_t isolatedPawns = ~supportedPawns & ~adjacentPawns & board.pieces[col];

        ret -= doublePawnValue * count_population(doubledPawns);
        #ifdef TUNER
        evalTrace.doubledPawnsCoeff[!col] = count_population(doubledPawns);
        #endif

        while (isolatedPawns) {
            int bscan = bitScan(isolatedPawns);
            if ((isolatedPawnMask[bscan] & board.pieces[col]) == 0) {
                #ifdef TUNER
                evalTrace.isolatedPawnsCoeff[!col]++;
                #endif
                ret -= isolatedPawnValue;
            }
            isolatedPawns &= isolatedPawns - 1;
        }

        while (supportedPawns) {
            int bscan = bitScan(supportedPawns) / 8;
            ret += supportedPawnWeight[col? 7 - bscan : bscan];

            #ifdef TUNER
            evalTrace.supportedPawnsCoeff[col? 7 - bscan : bscan][col]++;
            #endif

            supportedPawns &= supportedPawns - 1;
        }

        while (adjacentPawns) {
            int bscan = bitScan(adjacentPawns) / 8;
            ret += adjacentPawnWeight[col? 7 - bscan : bscan];

            #ifdef TUNER
            evalTrace.adjacentPawnsCoeff[col? 7 - bscan : bscan][col]++;
            #endif

            adjacentPawns &= adjacentPawns - 1;
        }

        pawnScore += col? -ret : ret;
    }


    while (piece) {
        int bscan = bitScan(piece);

        // PST
        ret += pieceSquare[col][bscan];

        #ifdef TUNER
        evalTrace.pawnPstCoeff[col? bscan : (bscan % 8) + (7 - (bscan / 8)) * 8][col]++;
        #endif

        dist = manhattanArray[bscan][ourKing] * 2;
        dist -= manhattanArray[bscan][theirKing] * 2;

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

            if (columnMask[bscan] & board.pieces[6 + col]) {
                ret += rookBehindPasser;

                #ifdef TUNER
                evalTrace.rookBehindPasserCoeff[col]++;
                #endif
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



int Eval::evaluateKnights(Bitboard &board, ThreadSearch *th, bool col) {

    int ret = 0;
    uint64_t piece = board.pieces[2 + col];
    uint64_t holes = th->pawnAttAll[col];
    uint64_t defendedKnight = piece & holes;

    while (piece) {
        int bscan = bitScan(piece);

        // PST
        ret += pieceSquare[2 + col][bscan];

        #ifdef TUNER
        evalTrace.knightPstCoeff[col? bscan : (bscan % 8) + (7 - (bscan / 8)) * 8][col]++;
        #endif

        // Mobility
        ret += knightMobilityBonus[count_population(board.knightMoves[bscan] & ~th->minorUnsafe[col])];

        #ifdef TUNER
        evalTrace.knightMobilityCoeff[count_population(board.knightMoves[bscan] & ~th->minorUnsafe[col])][col]++;
        #endif

        // King safety
        th->unsafeSquares[!col] |= board.knightMoves[bscan];
        th->attacksKnight[col] |= board.knightMoves[bscan];
        int attacks = count_population(board.knightMoves[bscan] & th->tempUnsafe[col]);
        if (attacks) {
            th->KSAttackersWeight[col] += pieceAttackValue[1];
            th->KSAttacks[col] += attacks;
            (th->KSAttackersCount[col])++;
        }

        // Outposts
        if ((defendedKnight & (1ULL << bscan)) && (outpostMask[col][bscan] & board.pieces[!col]) == 0) {
            ret += knightOutpost[col][bscan];
        }

        piece &= piece - 1;
    }

    return ret;

}



int Eval::evaluateBishops(Bitboard &board, ThreadSearch *th, bool col) {

    int ret = 0;
    uint64_t piece = board.pieces[4 + col];

    while (piece) {
        int bscan = bitScan(piece);
        uint64_t bishopAttacks = magics->bishopAttacksMask(board.occupied ^ board.pieces[8 + col], bscan);
        th->bishopAttacksAll[col] |= bishopAttacks;

        // PST
        ret += pieceSquare[4 + col][bscan];

        #ifdef TUNER
        evalTrace.bishopPstCoeff[col? bscan : (bscan % 8) + (7 - (bscan / 8)) * 8][col]++;
        #endif

        // Mobility
        ret += bishopMobilityBonus[count_population(bishopAttacks & ~th->minorUnsafe[col])];

        #ifdef TUNER
        evalTrace.bishopMobilityCoeff[count_population(bishopAttacks & ~th->minorUnsafe[col])][col]++;
        #endif

        // King safety
        th->unsafeSquares[!col] |= bishopAttacks;
        th->attacksBishop[col] |= bishopAttacks;
        int attacks = count_population(bishopAttacks & th->tempUnsafe[col]);
        if (attacks) {
            th->KSAttackersWeight[col] += pieceAttackValue[2];
            th->KSAttacks[col] += attacks;
            (th->KSAttackersCount[col])++;
        }

        piece &= piece - 1;
    }

    return ret;

}



int Eval::evaluateRooks(Bitboard &board, ThreadSearch *th, bool col) {

    int ret = 0;
    uint64_t piece = board.pieces[6 + col];

    while (piece) {
        int bscan = bitScan(piece);
        uint64_t rookAttacks = magics->rookAttacksMask(board.occupied ^ board.pieces[8 + col], bscan);
        th->rookAttacksAll[col] |= rookAttacks;

        // PST
        ret += pieceSquare[6 + col][bscan];

        #ifdef TUNER
        evalTrace.rookPstCoeff[col? bscan : (bscan % 8) + (7 - (bscan / 8)) * 8][col]++;
        #endif

        // Mobility
        ret += rookMobilityBonus[count_population(rookAttacks & ~th->mobilityUnsafeSquares[col])];

        #ifdef TUNER
        evalTrace.rookMobilityCoeff[count_population(rookAttacks & ~th->mobilityUnsafeSquares[col])][col]++;
        #endif

        // King safety
        th->unsafeSquares[!col] |= rookAttacks;
        th->attacksRook[col] |= rookAttacks;
        int attacks = count_population(rookAttacks & th->tempUnsafe[col]);
        if (attacks) {
            th->KSAttackersWeight[col] += pieceAttackValue[3];
            th->KSAttacks[col] += attacks;
            (th->KSAttackersCount[col])++;
        }

        // Rook on open file
        if ((columnMask[bscan] & board.pieces[col]) == 0) {
            ret += rookOnSemiOpen;
            #ifdef TUNER
            evalTrace.rookOnSemiOpenCoeff[col]++;
            #endif
            if ((columnMask[bscan] & board.pieces[!col]) == 0) {
                ret += rookOnOpen;
                #ifdef TUNER
                evalTrace.rookOnOpenCoeff[col]++;
                #endif
            }
        }

        // Rook on enemy queen file
        if ((columnMask[bscan] & board.pieces[8 + !col]) == 0) {
            ret += rookOnQueen;
            #ifdef TUNER
            evalTrace.rookOnQueenCoeff[col]++;
            #endif
        }

        // Trapped rooks
        uint64_t pieceLoc = piece & -piece;
        if (rowMask[col * 56] & pieceLoc) {
            if (board.pieces[col + 10] > 1ULL << (3 + (col * 56)) && pieceLoc > board.pieces[col + 10]) {
                ret -= trappedRook;

                #ifdef TUNER
                evalTrace.trappedRookCoeff[!col]++;
                #endif
            }
            if (board.pieces[col + 10] < 1ULL << (4 + (col * 56)) && pieceLoc < board.pieces[col + 10]) {
                ret -= trappedRook;

                #ifdef TUNER
                evalTrace.trappedRookCoeff[!col]++;
                #endif
            }
        }

        piece &= piece - 1;
    }

    return ret;

}



int Eval::evaluateQueens(Bitboard &board, ThreadSearch *th, bool col) {

    int ret = 0;
    uint64_t piece = board.pieces[8 + col];

    while (piece) {
        int bscan = bitScan(piece);
        uint64_t queenAttacks = magics->queenAttacksMask(board.occupied, bscan);

        // PST
        ret += pieceSquare[8 + col][bscan];

        #ifdef TUNER
        evalTrace.queenPstCoeff[col? bscan : (bscan % 8) + (7 - (bscan / 8)) * 8][col]++;
        #endif

        // Mobility
        ret += queenMobilityBonus[count_population(queenAttacks & ~th->queenUnsafe[col])];

        #ifdef TUNER
        evalTrace.queenMobilityCoeff[count_population(queenAttacks & ~th->queenUnsafe[col])][col]++;
        #endif

        // King safety
        th->unsafeSquares[!col] |= queenAttacks;
        th->attacksQueen[col] |= queenAttacks;
        int attacks = count_population(queenAttacks & th->tempUnsafe[col]);
        if (attacks) {
            th->KSAttackersWeight[col] += pieceAttackValue[4];
            th->KSAttacks[col] += attacks;
            (th->KSAttackersCount[col])++;
        }

        piece &= piece - 1;
    }

    return ret;

}



int Eval::evaluateKing(Bitboard &board, ThreadSearch *th, bool col) {

    int ret = 0;
    int theirKing = board.kingLoc[!col];

    // PST
    ret += pieceSquare[10 + col][board.kingLoc[col]];

    #ifdef TUNER
    evalTrace.kingPstCoeff[col? board.kingLoc[col] : (board.kingLoc[col] % 8) + (7 - (board.kingLoc[col] / 8)) * 8][col]++;
    #endif

    if (th->KSAttackersCount[col] > 1) {

        uint64_t bishopKing = magics->bishopAttacksMask(board.occupied, theirKing);
        uint64_t rookKing = magics->rookAttacksMask(board.occupied, theirKing);
        uint64_t knightChecks = board.knightMoves[theirKing] & th->attacksKnight[col] & ~th->unsafeSquares[col];
        uint64_t bishopChecks = bishopKing & th->attacksBishop[col] & ~th->unsafeSquares[col];
        uint64_t rookChecks = rookKing & th->attacksRook[col] & ~th->unsafeSquares[col];
        uint64_t queenChecks = (bishopKing | rookKing) & th->attacksQueen[col] & ~th->unsafeSquares[col];

        // King safety
        int kingSafe = th->KSAttackersWeight[col];
        kingSafe += queenCheckVal * count_population(queenChecks);
        kingSafe += rookCheckVal * count_population(rookChecks);
        kingSafe += bishopCheckVal * count_population(bishopChecks);
        kingSafe += knightCheckVal * count_population(knightChecks);
        kingSafe += th->KSAttacks[col] * attacksSafety / (count_population(kingZoneMask[!col][theirKing] & board.pieces[!col]) + 1);
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
    int bscan = board.kingLoc[col];
    uint64_t shield = passedPawnMask[col][bscan] & (rowMask[col? std::max(bscan - 8, 0) : std::min(bscan + 8, 63)] | rowMask[col? std::max(bscan - 16, 0) : std::min(bscan + 16, 63)]);
    int shieldCount = count_population(shield & board.pieces[col]);

    ret += pawnShield * shieldCount;

    #ifdef TUNER
    evalTrace.pawnShieldCoeff[col] += shieldCount;
    #endif

    if ((forwardMask[col][bscan] & board.pieces[col]) != 0) {
        ret += kingPawnFront;

        #ifdef TUNER
        evalTrace.kingPawnFrontCoeff[col]++;
        #endif

        if ((forwardMask[col][bscan] & board.pieces[!col]) != 0) {
            ret += kingPawnFrontN;

            #ifdef TUNER
            evalTrace.kingPawnFrontNCoeff[col]++;
            #endif
        }
    }

    if (bscan % 8 > 0 && (forwardMask[col][bscan - 1] & board.pieces[col]) != 0) {
        ret += kingPawnAdj;

        #ifdef TUNER
        evalTrace.kingPawnAdjCoeff[col]++;
        #endif

        if ((forwardMask[col][bscan - 1] & board.pieces[!col]) != 0) {
            ret += kingPawnAdjN;

            #ifdef TUNER
            evalTrace.kingPawnAdjNCoeff[col]++;
            #endif
        }
    }

    if (bscan % 8 < 7 && (forwardMask[col][bscan + 1] & board.pieces[col]) != 0) {
        ret += kingPawnAdj;

        #ifdef TUNER
        evalTrace.kingPawnAdjCoeff[col]++;
        #endif

        if ((forwardMask[col][bscan + 1] & board.pieces[!col]) != 0) {
            ret += kingPawnAdjN;

            #ifdef TUNER
            evalTrace.kingPawnAdjNCoeff[col]++;
            #endif
        }
    }

    return ret;

}




int Eval::evaluateThreats(Bitboard &board, ThreadSearch *th, bool col) {

    int ret = 0;

    // Pawn threats
    uint64_t attacks = pawnAttacksAll((~th->unsafeSquares[col] | th->unsafeSquares[!col]) & board.pieces[col], col);
    int numAttacks = count_population(attacks & (board.pieces[2 + !col] | board.pieces[4 + !col] | board.pieces[6 + !col] | board.pieces[8 + !col]));
    ret += pawnThreat * numAttacks;

    #ifdef TUNER
    evalTrace.pawnThreatCoeff[col] += numAttacks;
    #endif

    attacks = pawnAttacksAll((col? board.pieces[col] >> 8 : board.pieces[col] << 8) & ~board.occupied, col);
    numAttacks = count_population(attacks & (board.pieces[2 + !col] | board.pieces[4 + !col] | board.pieces[6 + !col] | board.pieces[8 + !col]));
    ret += pawnPushThreat * numAttacks;

    #ifdef TUNER
    evalTrace.pawnPushThreatCoeff[col] += numAttacks;
    #endif

    // Knight threats
    attacks = th->knightAttAll[col];
    ret += (knightThreatPiece[0] * count_population(attacks & board.pieces[!col]));
    ret += (knightThreatPiece[2] * count_population(attacks & board.pieces[4 + !col]));
    ret += (knightThreatPiece[3] * count_population(attacks & board.pieces[6 + !col]));
    ret += (knightThreatPiece[4] * count_population(attacks & board.pieces[8 + !col]));

    #ifdef TUNER
    evalTrace.knightThreatCoeff[0][col] += count_population(attacks & board.pieces[!col]);
    evalTrace.knightThreatCoeff[2][col] += count_population(attacks & board.pieces[4 + !col]);
    evalTrace.knightThreatCoeff[3][col] += count_population(attacks & board.pieces[6 + !col]);
    evalTrace.knightThreatCoeff[4][col] += count_population(attacks & board.pieces[8 + !col]);
    #endif

    // Bishop threats
    attacks = th->bishopAttacksAll[col];
    ret += (bishopThreatPiece[0] * count_population(attacks & board.pieces[!col]));
    ret += (bishopThreatPiece[1] * count_population(attacks & board.pieces[2 + !col]));
    ret += (bishopThreatPiece[3] * count_population(attacks & board.pieces[6 + !col]));
    ret += (bishopThreatPiece[4] * count_population(attacks & board.pieces[8 + !col]));

    #ifdef TUNER
    evalTrace.bishopThreatCoeff[0][col] += count_population(attacks & board.pieces[!col]);
    evalTrace.bishopThreatCoeff[1][col] += count_population(attacks & board.pieces[2 + !col]);
    evalTrace.bishopThreatCoeff[3][col] += count_population(attacks & board.pieces[6 + !col]);
    evalTrace.bishopThreatCoeff[4][col] += count_population(attacks & board.pieces[8 + !col]);
    #endif

    // Rook threats
    attacks = th->rookAttacksAll[col];
    ret += (rookThreatPiece[0] * count_population(attacks & board.pieces[!col]));
    ret += (rookThreatPiece[1] * count_population(attacks & board.pieces[2 + !col]));
    ret += (rookThreatPiece[2] * count_population(attacks & board.pieces[4 + !col]));
    ret += (rookThreatPiece[4] * count_population(attacks & board.pieces[8 + !col]));

    #ifdef TUNER
    evalTrace.rookThreatCoeff[0][col] += count_population(attacks & board.pieces[!col]);
    evalTrace.rookThreatCoeff[1][col] += count_population(attacks & board.pieces[2 + !col]);
    evalTrace.rookThreatCoeff[2][col] += count_population(attacks & board.pieces[4 + !col]);
    evalTrace.rookThreatCoeff[4][col] += count_population(attacks & board.pieces[8 + !col]);
    #endif

    return ret;

}












//
