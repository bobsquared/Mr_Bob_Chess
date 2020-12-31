#include "eval.h"

extern Magics *magics;


int pieceValues[6] = {S(114, 94), S(491, 332), S(500, 328), S(694, 559), S(1271, 1259), S(5000, 5000)};

// Queen, Bishop, Knight and rook weights
int knightWeight[9] = {S(-89, -75), S(-28, -35), S(-15, -20), S(-7, -9), S(-5, -1), S(4, 2), S(5, 23) , S(6, 38), S(14, 62)};
int rookWeight[9] = {S(125, 36), S(59, 33), S(38, 30), S(25, 29), S(13, 26), S(1, 26), S(-8, 22), S(-15, 16), S(-24, 9)};
int bishopWeight[9] = {S(81, 78), S(40, 77), S(37, 74), S(35, 72), S(35, 70), S(29, 69), S(26, 69), S(21, 67), S(20, 23)};
// int queenWeight[7] = {S(37, 20), S(45, 10), S(35, 24), S(18, 10), S(-5, 10), S(11, 19), S(54, 48)};

// Supported and adjacent pawn weights
int supportedPawnWeight[7] = {S(0, 0), S(0, 0), S(18, 17), S(16, 10), S(19, 15), S(50, 29), S(212, -41)};
int adjacentPawnWeight[7]  = {S(0, 0), S(4, 3), S(9, 3), S(8, 12), S(20, 31), S(90, 69), S(-105, 359)};
int freePasser[7]  = {S(0, 0), S(-7, -4), S(24, -6), S(13, -12), S(13, -34), S(17, -106), S(60, -177)};

// Passed Pawn weights
int passedPawnWeight[7] = {S(0, 0), S(10, 12), S(-14, 21), S(-20, 50), S(6, 83), S(37, 154), S(66, 221)};

// Doubled pawns and isolated pawns and backward pawns
int doublePawnValue = S(11, 10);
int isolatedPawnValue = S(11, 8);

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
int PAWN_TABLE[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                      S(41, 108), S(83, 86), S(2, 71), S(64, 33), S(33, 53), S(133, 17), S(-86, 95), S(-113, 124),
                      S(5, 68), S(-24, 65), S(6, 38), S(-5, 2), S(45, -15), S(90, -6), S(29, 33), S(-2, 49),
                      S(-5, 28), S(2, 8), S(0, -4), S(20, -25), S(20, -15), S(14, -11), S(10, 3), S(-13, 11),
                      S(-13, 8), S(-17, 2), S(9, -16), S(18, -22), S(21, -18), S(25, -20), S(-1, -8), S(-20, -6),
                      S(-17, -3), S(-21, -4), S(-3, -14), S(-21, -5), S(-5, -6), S(-6, -8), S(14, -21), S(-19, -16),
                      S(-17, 3), S(3, -11), S(-4, -1), S(-9, -5), S(-8, 5), S(30, -11), S(42, -25), S(-11, -18),
                      S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};
// ---------------------------------------------------------------------------//


// -----------------------Knight attack tables----------------------------------//
int KNIGHT_TABLE[64] = {S(-248, 18), S(-161, -3), S(-111, 20), S(-75, -13), S(11, -16), S(-164, -4), S(-116, -29), S(-191, -37),
                        S(-130, 14), S(-99, 21), S(22, -21), S(-26, 14), S(-13, -3), S(-3, -20), S(-36, -6), S(-80, -22),
                        S(-93, 0), S(-2, -9), S(-9, 19), S(15, 6), S(30, -3), S(99, -23), S(38, -25), S(20, -36),
                        S(-21, 5), S(14, 11), S(-2, 25), S(38, 20), S(29, 17), S(58, 2), S(17, 6), S(12, 0),
                        S(-22, 6), S(4, -3), S(7, 16), S(11, 26), S(28, 19), S(24, 18), S(27, 19), S(-10, 14),
                        S(-29, 1), S(-19, 4), S(12, -6), S(10, 17), S(28, 11), S(19, -5), S(26, -19), S(-21, 9),
                        S(-41, -2), S(-56, 7), S(-15, 0), S(1, 0), S(7, 4), S(15, -11), S(0, 4), S(-17, -13),
                        S(-139, 35), S(-27, -12), S(-54, -2), S(-40, 21), S(1, -2), S(-32, 10), S(-25, -11), S(-18, -7)};
// ---------------------------------------------------------------------------//


// -----------------------Bishop attack tables----------------------------------//
int BISHOP_TABLE[64] = {S(-72, -5), S(-45, -12), S(-196, 11), S(-142, 5), S(-111, 9), S(-112, 2), S(-43, -8), S(-45, -17),
                        S(-61, 4), S(-47, 6), S(-70, 10), S(-79, -4), S(-21, -2), S(6, -8), S(-51, 3), S(-104, -3),
                        S(-33, 1), S(0, -4), S(6, -3), S(-5, -4), S(-6, -5), S(32, -4), S(6, -1), S(-18, -2),
                        S(-28, 0), S(-9, 8), S(-12, 12), S(23, 5), S(3, 12), S(9, 6), S(-10, -1), S(-15, 0),
                        S(-24, -4), S(-9, 2), S(-1, 8), S(11, 15), S(14, 1), S(-5, 10), S(-2, -5), S(0, -7),
                        S(-19, -7), S(4, -5), S(2, 7), S(0, 8), S(7, 12), S(20, -4), S(10, -1), S(-4, -9),
                        S(-7, -9), S(4, -18), S(2, -9), S(-7, -2), S(0, 0), S(7, -7), S(29, -19), S(-6, -28),
                        S(-45, -12), S(-6, -5), S(-6, -13), S(-19, -1), S(-10, -3), S(-15, -2), S(-50, 4), S(-27, -12)};
// ---------------------------------------------------------------------------//


// -----------------------Rook attack tables----------------------------------//
int ROOK_TABLE[64] = {S(-41, 11), S(0, -3), S(-42, 10), S(13, -6), S(11, -3), S(-79, 15), S(-24, 1), S(-40, 4),
                      S(-17, -1), S(-5, -4), S(25, -14), S(21, -15), S(48, -35), S(35, -17), S(-28, 2), S(-5, -5),
                      S(-29, -7), S(-4, -11), S(-1, -17), S(8, -19), S(-10, -19), S(33, -27), S(72, -33), S(-10, -21),
                      S(-39, -6), S(-16, -14), S(3, -6), S(11, -22), S(9, -18), S(43, -21), S(-7, -16), S(-18, -8),
                      S(-39, -4), S(-29, -3), S(-20, -4), S(-8, -11), S(-1, -18), S(-2, -17), S(28, -27), S(-20, -19),
                      S(-39, -12), S(-18, -12), S(-11, -21), S(-20, -15), S(-2, -23), S(10, -29), S(11, -25), S(-17, -31),
                      S(-39, -13), S(-14, -19), S(-22, -13), S(-6, -15), S(5, -28), S(13, -30), S(10, -34), S(-49, -13),
                      S(-13, -17), S(-8, -14), S(-2, -15), S(6, -23), S(9, -27), S(10, -26), S(-18, -14), S(4, -33)};
// ---------------------------------------------------------------------------//


// -----------------------Queen attack tables----------------------------------//
int QUEEN_TABLE[64] = {S(1, -44), S(-1, 24), S(9, 21), S(4, 25), S(164, -50), S(105, -25), S(87, -36), S(102, -6),
                       S(-5, -29), S(-33, 2), S(15, 13), S(32, 30), S(-33, 73), S(72, 2), S(44, 19), S(69, -6),
                       S(28, -42), S(16, -16), S(49, -34), S(23, 38), S(72, 20), S(113, -5), S(92, -2), S(86, -5),
                       S(0, -1), S(14, -2), S(8, 6), S(11, 16), S(26, 36), S(49, 20), S(32, 66), S(40, 33),
                       S(45, -60), S(-7, 31), S(28, -2), S(24, 22), S(35, 1), S(40, 13), S(32, 34), S(41, 16),
                       S(27, -31), S(42, -58), S(27, -11), S(36, -23), S(30, -9), S(39, 0), S(46, 6), S(47, 2),
                       S(11, -36), S(27, -49), S(49, -57), S(44, -42), S(56, -52), S(59, -56), S(40, -69), S(60, -45),
                       S(42, -65), S(23, -56), S(29, -47), S(48, -81), S(22, -24), S(25, -57), S(12, -39), S(-15, -61)};
// ---------------------------------------------------------------------------//


// -----------------------King attack tables----------------------------------//
int KING_TABLE[64] =  {S(-193, -46), S(209, -63), S(238, -59), S(151, -42), S(-123, 20), S(-9, 30), S(122, -5), S(83, -20),
                       S(220, -44), S(151, 1), S(102, 7), S(204, -10), S(101, 11), S(89, 34), S(16, 31), S(-112, 39),
                       S(160, -13), S(111, 12), S(176, 3), S(65, 11), S(136, 1), S(180, 32), S(229, 20), S(56, 3),
                       S(72, -22), S(67, 17), S(65, 23), S(10, 33), S(11, 29), S(-5, 42), S(45, 28), S(-47, 11),
                       S(-22, -15), S(66, -8), S(-24, 34), S(-64, 45), S(-89, 51), S(-44, 38), S(-49, 25), S(-67, 0),
                       S(12, -16), S(8, 7), S(-20, 28), S(-42, 39), S(-60, 46), S(-38, 35), S(-3, 21), S(-3, -1),
                       S(26, -23), S(8, 7), S(-21, 27), S(-73, 38), S(-45, 36), S(-33, 33), S(14, 15), S(40, -14),
                       S(-24, -44), S(19, -17), S(3, 0), S(-71, 16), S(-3, 0), S(-53, 24), S(20, -5), S(30, -46)};
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

            #ifdef TUNER
            evalTrace.knightOutpostCoeff[col? bscan : (bscan % 8) + (7 - (bscan / 8)) * 8][col]++;
            #endif
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
