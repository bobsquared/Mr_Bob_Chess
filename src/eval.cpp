#include "eval.h"

extern Magics *magics;


int pieceValues[6] = {S(114, 94), S(491, 332), S(500, 328), S(694, 559), S(1271, 1259), S(5000, 5000)};

// Queen, Bishop, Knight and rook weights
int knightWeight[9] = {S(-131, 14), S(-22, 13), S(-19, 11), S(-32, 21), S(-33, 30), S(-30, 42), S(-28, 63), S(-28, 92), S(-37, 133)};
int rookWeight[9] = {S(183, 43), S(62, 75), S(36, 78), S(26, 81), S(16, 85), S(10, 91), S(5, 98), S(1, 105), S(3, 95)};
int bishopWeight[9] = {S(70, -8), S(62, 55), S(27, 80), S(23, 82), S(28, 80), S(24, 86), S(20, 96), S(13, 116), S(2, 145)};

// Supported and adjacent pawn weights
int supportedPawnWeight[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                               S(136, 26), S(265, -14), S(187, 46), S(247, 57), S(260, 48), S(347, 0), S(369, -9), S(210, -16),
                               S(46, 35), S(56, 51), S(71, 64), S(93, 75), S(103, 75), S(93, 58), S(76, 30), S(25, 35),
                               S(-1, 15), S(16, 19), S(27, 24), S(32, 18), S(45, 18), S(41, 13), S(16, 12), S(11, 5),
                               S(4, 0), S(16, 5), S(16, 8), S(21, 21), S(24, 14), S(13, 5), S(13, 0), S(17, -3),
                               S(15, 2), S(24, 6), S(31, 15), S(28, 18), S(24, 22), S(13, 12), S(22, 6), S(25, 2),
                               S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                               S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int adjacentPawnWeight[64]  = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                               S(-118, 178), S(-82, 166), S(130, 186), S(140, 106), S(68, 113), S(72, 252), S(-117, 86), S(0, 47),
                               S(-39, 53), S(49, 113), S(48, 110), S(70, 120), S(128, 85), S(112, 96), S(100, 88), S(69, 37),
                               S(12, 27), S(7, 38), S(35, 34), S(32, 43), S(35, 41), S(49, 43), S(10, 28), S(22, 30),
                               S(-8, 0), S(19, 8), S(2, 15), S(14, 28), S(18, 23), S(9, 9), S(31, 0), S(-4, 0),
                               S(14, 3), S(9, -2), S(12, 9), S(11, 12), S(16, 8), S(14, 8), S(6, -1), S(13, -2),
                               S(10, -10), S(2, 6), S(14, -6), S(3, 23), S(19, 22), S(-2, -11), S(0, 10), S(-1, -10),
                               S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int freePasser[7]  = {S(0, 0), S(-3, -11), S(1, -5), S(1, -18), S(1, -30), S(-10, -66), S(-58, -94)};

// Passed Pawn weights
int passedPawnWeight[7] = {S(0, 0), S(-47, -60), S(-47, -48), S(-31, -7), S(-2, 28), S(44, 79), S(124, 100)};
int opposedPawnValue[8] = {S(-12, -21), S(17, -16), S(4, 7), S(4, 3), S(-2, 14), S(3, 6), S(34, -1), S(-14, -7)};

// Doubled pawns and isolated pawns and backward pawns
int doublePawnValue = S(10, 29);
int isolatedPawnValue = S(13, 12);

// Blocked pawns
int blockedPawns5th = S(-17, -23);
int blockedPawns6th = S(-25, -63);

// Mobility
int knightMobilityBonus[9] = {S(-60, -104), S(-38, -19), S(-24, 22), S(-16, 45), S(-9, 60), S(-3, 74), S(3, 77), S(11, 75), S(19, 62)};
int bishopMobilityBonus[14] = {S(-70, -37), S(-46, -4), S(-33, 30), S(-26, 50), S(-15, 66), S(-8, 82), S(-4, 89), S(-2, 95), S(0, 101), S(6, 101), S(14, 97), S(36, 87), S(39, 107), S(83, 69)};
int rookMobilityBonus[15] = {S(-100, -94), S(-35, -14), S(-18, 22), S(-17, 42), S(-15, 52), S(-15, 65), S(-9, 71), S(-2, 73), S(4, 77), S(8, 81), S(11, 86), S(17, 88), S(24, 89), S(40, 76), S(94, 48)};
int queenMobilityBonus[28] = {S(-44, -94), S(-4, -132), S(7, 6), S(15, 42), S(19, 92), S(22, 112), S(26, 125), S(27, 147),
                              S(31, 158), S(34, 167), S(37, 173), S(39, 178), S(40, 181), S(41, 187), S(42, 186), S(40, 193),
                              S(37, 198), S(37, 195), S(33, 201), S(44, 190), S(40, 193), S(55, 185), S(47, 187), S(50, 188),
                              S(22, 204), S(99, 155), S(-30, 253), S(29, 186)};

int pieceAttackValue[5] = {48, 62, 62, 60, 25};
int attacksSafety = 18;
int queenCheckVal = 75;
int rookCheckVal = 60;
int bishopCheckVal = 63;
int knightCheckVal = 94;
int noQueen = -185;
int KSOffset = 122;

int kingPawnFront = S(26, 2);
int kingPawnFrontN = S(7, -4);

int pawnShield[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                      S(81, 25), S(103, -1), S(41, 26), S(12, 11), S(-12, 16), S(-67, 18), S(-2, 21), S(70, 8),
                      S(88, 3), S(83, 10), S(19, 15), S(-55, 24), S(-40, 13), S(-2, -8), S(15, 9), S(71, 4),
                      S(60, -6), S(1, 11), S(20, 1), S(2, 5), S(-14, 9), S(-10, -8), S(-2, -3), S(20, -8),
                      S(49, -23), S(-1, 3), S(-11, 1), S(15, -1), S(8, -2), S(-3, -10), S(-15, -10), S(28, -14),
                      S(56, -45), S(38, -24), S(-14, -1), S(6, 0), S(14, -3), S(-10, -1), S(25, -28), S(43, -36),
                      S(44, -68), S(37, -32), S(19, -1), S(24, 0), S(6, 0), S(15, 1), S(16, -28), S(34, -62),
                      S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int pawnStorm[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                     S(15, 19), S(27, -7), S(9, 0), S(0, -9), S(16, 0), S(-5, -3), S(1, 2), S(17, 17),
                     S(26, 6), S(40, -5), S(8, -1), S(12, -12), S(17, -5), S(-5, 1), S(18, 0), S(22, 4),
                     S(14, 28), S(15, 13), S(-7, 9), S(-9, -6), S(7, -11), S(-11, 9), S(3, 16), S(5, 16),
                     S(-14, 73), S(-11, 43), S(-27, 31), S(-35, 11), S(-8, 2), S(-26, 32), S(5, 35), S(1, 49),
                     S(0, 140), S(-54, 114), S(-109, 91), S(-35, 53), S(-56, 51), S(-59, 78), S(-52, 101), S(73, 94),
                     S(215, 106), S(161, 59), S(-88, 111), S(-4, 48), S(-82, 57), S(-37, 75), S(84, 94), S(294, 18),
                     S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int pawnStormBlockade[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                             S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                             S(25, -38), S(-47, -7), S(-23, -11), S(67, -19), S(0, 28), S(-7, 2), S(44, -24), S(-22, -13),
                             S(18, -27), S(25, -32), S(-8, -23), S(10, -7), S(6, -3), S(3, -10), S(8, -19), S(37, -14),
                             S(35, -39), S(40, -44), S(25, -36), S(16, -20), S(16, -14), S(9, -22), S(17, -28), S(39, -29),
                             S(63, -77), S(53, -67), S(24, -47), S(19, -36), S(12, -31), S(20, -45), S(17, -48), S(57, -71),
                             S(-15, -172), S(10, -116), S(-1, -108), S(20, -160), S(-10, -81), S(3, -88), S(32, -128), S(-44, -149),
                             S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int rookOnOpen = S(24, 7);
int rookOnSemiOpen = S(20, 1);
int rookOnQueen = S(-18, 21);

int knightPair = S(-3, 0);
int rookPair = S(-55, 2);

int trappedRook = S(1, -36);
int rookBehindPasser = S(9, 10);
int tempoBonus = S(26, 20);

int pawnThreat = S(72, 44);
int pawnPushThreat = S(15, 7);

int knightThreatPiece[5] = {S(-2, 12), S(0, 0), S(29, 44), S(64, 17), S(52, 15)};
int bishopThreatPiece[5] = {S(0, 10), S(28, 45), S(0, 0), S(59, 21), S(54, 52)};
int rookThreatPiece[5] = {S(-10, 15), S(3, 21), S(8, 21), S(0, 0), S(42, 33)};

int kingPawnDistFriendly[8] = {S(0, 0), S(13, 14), S(6, 13), S(-6, 8), S(-22, 5), S(-32, 4), S(-42, 0), S(-40, -9)};
int kingPawnDistEnemy[8] = {S(0, 0), S(0, -24), S(31, 12), S(4, 16), S(-1, 19), S(-5, 24), S(-6, 24), S(5, 22)};
int kingPassedDistFriendly[8] = {S(0, 0), S(-7, 71), S(-7, 49), S(2, 30), S(8, 17), S(12, 9), S(19, 7), S(0, 1)};
int kingPassedDistEnemy[8] = {S(0, 0), S(-76, -3), S(-16, 14), S(1, 37), S(15, 51), S(32, 58), S(36, 69), S(29, 79)};


// -----------------------Pawn attack tables----------------------------------//
int PAWN_TABLE[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                      S(142, 106), S(110, 109), S(119, 97), S(136, 71), S(114, 72), S(122, 89), S(50, 125), S(109, 107),
                      S(28, 101), S(17, 87), S(30, 57), S(43, 21), S(32, 21), S(73, 54), S(8, 85), S(30, 85),
                      S(19, 44), S(12, 27), S(-1, 10), S(12, -13), S(20, -15), S(13, 8), S(18, 26), S(10, 29),
                      S(6, 18), S(-5, 14), S(1, -9), S(-11, -24), S(-7, -23), S(0, -5), S(-4, 12), S(-21, 9),
                      S(-4, 8), S(-15, 2), S(-18, -9), S(-25, -16), S(-21, -15), S(-18, -6), S(-15, 0), S(-26, 0),
                      S(-2, 15), S(-5, 7), S(-17, 1), S(-18, -8), S(-31, 9), S(7, -3), S(15, -4), S(-8, 0),
                      S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};
// ---------------------------------------------------------------------------//


// -----------------------Knight attack tables----------------------------------//
int KNIGHT_TABLE[64] = {S(-235, -8), S(-139, 17), S(-149, 44), S(-70, 22), S(-46, 20), S(-154, 39), S(-130, 23), S(-179, -31),
                        S(-49, 12), S(-32, 14), S(-1, 8), S(24, 25), S(13, 20), S(18, -2), S(-31, 14), S(-28, 9),
                        S(-55, 9), S(-22, 17), S(12, 36), S(16, 31), S(40, 21), S(50, 19), S(4, 8), S(-20, 9),
                        S(-3, 15), S(9, 19), S(31, 33), S(62, 33), S(36, 43), S(60, 26), S(41, 22), S(52, 11),
                        S(-20, 17), S(1, 27), S(27, 38), S(27, 46), S(42, 45), S(25, 42), S(40, 21), S(0, 24),
                        S(-32, 2), S(-14, 12), S(-3, 20), S(4, 41), S(14, 41), S(0, 22), S(6, 12), S(-7, 13),
                        S(-47, 9), S(-36, 22), S(-26, 14), S(-15, 22), S(-12, 22), S(-11, 15), S(10, 19), S(-19, 25),
                        S(-94, 3), S(-29, -14), S(-51, 9), S(-30, 22), S(-19, 21), S(-38, 22), S(-33, 5), S(-79, 12)};
// ---------------------------------------------------------------------------//


// -----------------------Bishop attack tables----------------------------------//
int BISHOP_TABLE[64] = {S(-72, 77), S(-94, 77), S(-120, 78), S(-149, 86), S(-135, 77), S(-152, 71), S(-88, 69), S(-110, 76),
                        S(-58, 63), S(-44, 69), S(-32, 63), S(-51, 67), S(-53, 65), S(-45, 66), S(-75, 76), S(-101, 72),
                        S(-14, 59), S(-11, 65), S(0, 67), S(-3, 58), S(2, 64), S(12, 73), S(-23, 68), S(-2, 60),
                        S(-27, 60), S(0, 61), S(-10, 65), S(9, 70), S(6, 74), S(0, 66), S(1, 64), S(-22, 62),
                        S(-16, 49), S(-8, 52), S(0, 65), S(10, 67), S(7, 63), S(1, 64), S(2, 55), S(-1, 44),
                        S(-11, 45), S(13, 52), S(0, 58), S(3, 60), S(7, 64), S(8, 58), S(17, 53), S(8, 49),
                        S(2, 43), S(2, 32), S(9, 38), S(-9, 52), S(-3, 54), S(14, 42), S(26, 42), S(12, 23),
                        S(6, 29), S(8, 42), S(-8, 44), S(-19, 53), S(-6, 49), S(-7, 57), S(0, 39), S(16, 29)};
// ---------------------------------------------------------------------------//


// -----------------------Rook attack tables----------------------------------//
int ROOK_TABLE[64] = {S(10, 90), S(9, 93), S(-9, 100), S(-8, 97), S(-3, 91), S(18, 90), S(28, 86), S(36, 86),
                      S(2, 80), S(1, 84), S(29, 79), S(45, 75), S(40, 75), S(49, 62), S(23, 70), S(38, 64),
                      S(-5, 72), S(55, 51), S(35, 62), S(56, 48), S(73, 42), S(61, 53), S(84, 40), S(25, 54),
                      S(3, 62), S(26, 57), S(34, 60), S(56, 51), S(55, 45), S(56, 44), S(43, 45), S(25, 51),
                      S(-11, 56), S(-3, 65), S(0, 65), S(10, 58), S(14, 54), S(11, 56), S(20, 51), S(-2, 48),
                      S(-19, 46), S(-2, 49), S(-7, 50), S(4, 45), S(9, 44), S(6, 42), S(22, 35), S(-3, 34),
                      S(-31, 42), S(-5, 37), S(-3, 42), S(4, 38), S(5, 36), S(15, 29), S(25, 24), S(-25, 36),
                      S(-5, 38), S(0, 41), S(4, 45), S(12, 39), S(18, 34), S(11, 44), S(8, 41), S(-3, 33)};
// ---------------------------------------------------------------------------//


// -----------------------Queen attack tables----------------------------------//
int QUEEN_TABLE[64] = {S(-31, 114), S(-22, 138), S(-19, 152), S(-6, 165), S(3, 167), S(1, 177), S(10, 147), S(-3, 146),
                       S(-4, 108), S(-47, 150), S(0, 144), S(-30, 182), S(-19, 215), S(16, 181), S(-17, 157), S(21, 146),
                       S(-7, 103), S(5, 103), S(5, 133), S(22, 144), S(39, 172), S(39, 181), S(52, 149), S(28, 146),
                       S(2, 92), S(16, 117), S(15, 111), S(10, 155), S(9, 178), S(27, 172), S(40, 172), S(32, 131),
                       S(20, 63), S(20, 105), S(18, 107), S(11, 146), S(9, 141), S(30, 133), S(30, 117), S(36, 107),
                       S(19, 42), S(23, 78), S(18, 99), S(14, 88), S(18, 94), S(26, 94), S(35, 73), S(25, 50),
                       S(15, 32), S(19, 47), S(24, 37), S(23, 63), S(23, 60), S(36, 13), S(43, -5), S(25, 18),
                       S(21, 20), S(4, 40), S(7, 38), S(13, 47), S(16, 46), S(9, 33), S(8, 15), S(16, 33)};
// ---------------------------------------------------------------------------//


// -----------------------King attack tables----------------------------------//
int KING_TABLE[64] =  {S(165, -105), S(142, -45), S(136, -28), S(87, -3), S(150, -30), S(100, -7), S(170, -16), S(261, -114),
                       S(4, 2), S(111, 20), S(118, 14), S(141, 0), S(145, -3), S(152, 13), S(110, 30), S(98, 1),
                       S(32, 13), S(141, 16), S(140, 16), S(104, 13), S(136, 10), S(184, 15), S(138, 19), S(92, -3),
                       S(54, -3), S(59, 10), S(80, 19), S(18, 28), S(56, 20), S(86, 18), S(92, 2), S(17, -2),
                       S(52, -27), S(41, 0), S(54, 11), S(7, 25), S(34, 21), S(76, 5), S(96, -10), S(39, -28),
                       S(27, -23), S(-1, 0), S(4, 7), S(-21, 22), S(6, 16), S(30, 6), S(32, -4), S(53, -27),
                       S(39, -20), S(-23, 4), S(-27, 11), S(-69, 21), S(-43, 19), S(-26, 18), S(25, 4), S(59, -19),
                       S(-3, -41), S(-13, -5), S(-33, 2), S(-91, 7), S(-21, -15), S(-55, 14), S(20, 0), S(45, -45)};
// ---------------------------------------------------------------------------//



// ---------------------------Knight Outpost----------------------------------//
int KNIGHT_OUTPOST[64] = {S(227, -104), S(93, 44), S(69, -6), S(527, -131), S(62, 0), S(270, 22), S(57, -59), S(71, 138),
                          S(16, 21), S(12, 9), S(13, 20), S(18, -3), S(70, -15), S(15, 19), S(-1, 3), S(27, -4),
                          S(35, 29), S(18, 40), S(44, 22), S(40, 24), S(38, 32), S(38, 38), S(37, 45), S(49, 24),
                          S(8, 16), S(19, 18), S(28, 25), S(27, 35), S(41, 34), S(40, 30), S(32, 27), S(4, 23),
                          S(15, 13), S(16, 12), S(26, 16), S(36, 17), S(42, 22), S(34, 14), S(20, 22), S(18, 8),
                          S(-1, 15), S(2, 5), S(0, 7), S(-3, 8), S(7, 0), S(-4, 6), S(0, 3), S(-10, 0),
                          S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                          S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int KNIGHT_OUTPOST_HOLE[64] = {S(0, 19), S(91, 0), S(38, 15), S(67, 1), S(-3, 25), S(52, 7), S(-129, 73), S(-48, 90),
                               S(8, 15), S(-2, 17), S(-2, 7), S(-3, 13), S(-8, 20), S(7, 8), S(24, 13), S(-8, 15),
                               S(10, 13), S(2, 5), S(-1, 3), S(0, 7), S(-5, 6), S(-9, 11), S(-9, 10), S(-2, 6),
                               S(-7, 18), S(0, 6), S(-2, 4), S(-20, 15), S(-5, 4), S(-6, 8), S(-11, 6), S(-21, 14),
                               S(6, 12), S(3, 2), S(-12, 7), S(-4, 1), S(-15, 4), S(1, 1), S(-8, 9), S(4, 9),
                               S(3, -6), S(1, 6), S(2, -3), S(7, -1), S(2, -2), S(8, -3), S(1, 3), S(-1, -2),
                               S(4, 12), S(0, 12), S(5, 0), S(6, 1), S(6, 3), S(7, -1), S(-15, 13), S(4, 1),
                               S(19, -1), S(2, 26), S(13, 18), S(13, 15), S(11, 11), S(24, 6), S(3, 15), S(14, 0)};

int BISHOP_OUTPOST[64] = {S(260, -159), S(125, -2), S(201, -48), S(348, -90), S(278, -68), S(302, -62), S(388, -97), S(-400, -13),
                          S(-4, 35), S(76, -11), S(79, -16), S(77, -25), S(144, -36), S(97, -11), S(129, -18), S(0, 11),
                          S(-15, 28), S(40, 6), S(28, 3), S(49, 4), S(58, -9), S(71, -2), S(62, 5), S(-48, 9),
                          S(16, 2), S(32, 11), S(32, 3), S(49, 6), S(54, -2), S(54, 1), S(31, 13), S(2, 7),
                          S(0, 25), S(36, 16), S(46, 8), S(47, 8), S(66, 8), S(29, 13), S(45, 7), S(4, 8),
                          S(15, 27), S(7, 24), S(17, 19), S(11, 29), S(13, 10), S(1, 8), S(2, 14), S(-6, 23),
                          S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                          S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};
// ---------------------------------------------------------------------------//



Eval::Eval() {
    InitPieceBoards();
    InitKingZoneMask();
    InitPassedPawnsMask();
    InitForwardBackwardMask();
    InitDistanceArray();
    InitIsolatedPawnsMask();
    InitOutpostMask();
    InitLightSquares();

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
        kingZoneMask[0][i] |= kingZoneMask[0][i] << 8;
        kingZoneMask[0][i] |= kingZoneMask[0][i] >> 8;

        kingZoneMask[1][i] = tempBitBoard | (1ULL << i);
        kingZoneMask[1][i] |= kingZoneMask[1][i] << 8;
        kingZoneMask[1][i] |= kingZoneMask[1][i] >> 8;
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

            pawnShieldVal[0][i * 8 + j] = pawnShield[(7 - i) * 8 + j];
            pawnShieldVal[1][i * 8 + j] = pawnShield[j + 8 * i];
            pawnStormVal[0][i * 8 + j] = pawnStorm[(7 - i) * 8 + j];
            pawnStormVal[1][i * 8 + j] = pawnStorm[j + 8 * i];
            pawnStormBlockadeVal[0][i * 8 + j] = pawnStormBlockade[(7 - i) * 8 + j];
            pawnStormBlockadeVal[1][i * 8 + j] = pawnStormBlockade[j + 8 * i];

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
        knightOutpost[0][i * 8 + j] = KNIGHT_OUTPOST[(7 - i) * 8 + j];
        knightOutpost[1][i * 8 + j] = KNIGHT_OUTPOST[j + 8 * i];

        knightOutpostHole[0][i * 8 + j] = KNIGHT_OUTPOST_HOLE[(7 - i) * 8 + j];
        knightOutpostHole[1][i * 8 + j] = KNIGHT_OUTPOST_HOLE[j + 8 * i];

        bishopOutpost[0][i * 8 + j] = BISHOP_OUTPOST[(7 - i) * 8 + j];
        bishopOutpost[1][i * 8 + j] = BISHOP_OUTPOST[j + 8 * i];
      }
  }
}



void Eval::InitLightSquares() {
    lightSquares = 0;
    for (int i = 0; i < 64; i++) {
        if (((i / 8) + (i % 8)) % 2) {
            lightSquares |= 1ULL << i;
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



int Eval::getFile(int index) {
    return index % 8;
}



int Eval::getRank(int index) {
    return index / 8;
}



int Eval::flipFile(int index) {
    return 7 - getFile(index);
}



int Eval::flipRank(int index) {
    return 7 - getRank(index);
}



int Eval::flipIndex64(int index) {
    return getFile(index) + flipRank(index) * 8;
}



#ifdef TUNER
// clears the trace to 0
void Eval::clearTrace() {
    evalTrace = emptyTrace;
}
#endif



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

    if (check_bit(board.pieces[4]) && check_bit(board.pieces[5]) && check_bit(bishops & lightSquares) && minorAndMajors == bishops) {
        return 122;
    }

    if (rooks == minorAndMajors && check_bit(board.pieces[6]) && check_bit(board.pieces[7])) {
        return std::min(140 + 50 * board.pieceCount[attackingColor] - board.pieceCount[!attackingColor], 256);
    }

    // K vs KNN
    if (pawns == 0 && numMajorMinors == 2 && minorAndMajors == board.pieces[2 + attackingColor]) {
        return 0;
    }

    // KB vs KNN or KN vs KNN
    if (pawns == 0 && numMajorMinors == 3 && check_bit(defending & minors) && (defending & majors) == 0 && (attacking & minorAndMajors) == board.pieces[2 + attackingColor]) {
        return 0;
    }

    return 256;

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
    ret = ((MGVAL(ret) * (256 - phase)) + (EGVAL(ret) * phase * scaleEndgame(board, EGVAL(ret)) / 256)) / 256;
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
        ret += knightPair;

        #ifdef TUNER
        evalTrace.knightPairCoeff[col]++;
        #endif
    }

    // Rook pair
    if (board.pieceCount[6 + col] >= 2) {
        ret += rookPair;

        #ifdef TUNER
        evalTrace.rookPairCoeff[col]++;
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
    uint64_t piece = board.pieces[col];
    uint64_t leveredPawns = board.pieces[!col] & th->pawnAttAll[col];
    th->unsafeSquares[!col] |= th->pawnAttAll[col];
    th->KSAttackersWeight[col] += pieceAttackValue[0] * count_population(th->pawnAttAll[col] & th->tempUnsafe[col]);

    if (!hit) {
        uint64_t adjacentPawns = board.pieces[col] & adjacentMask(board.pieces[col]);
        uint64_t supportedPawns = board.pieces[col] & th->pawnAttAll[col];
        uint64_t isolatedPawns = ~supportedPawns & ~adjacentPawns & board.pieces[col];
        uint64_t doubledPawns = col? ((board.pieces[col] ^ supportedPawns) << 8) & board.pieces[col] : ((board.pieces[col] ^ supportedPawns) >> 8) & board.pieces[col];
        uint64_t blockedPawns = col? ((board.pieces[!col] << 8) & board.pieces[col]) : ((board.pieces[!col] >> 8) & board.pieces[col]);

        ret -= doublePawnValue * count_population(doubledPawns);
        #ifdef TUNER
        evalTrace.doubledPawnsCoeff[!col] = count_population(doubledPawns);
        #endif

        ret += blockedPawns5th * count_population(blockedPawns & (col? rowMask[24] : rowMask[32]));
        ret += blockedPawns6th * count_population(blockedPawns & (col? rowMask[16] : rowMask[40]));

        #ifdef TUNER
        evalTrace.blockedPawns5thCoeff[col] += count_population(blockedPawns & (col? rowMask[24] : rowMask[32]));
        evalTrace.blockedPawns6thCoeff[col] += count_population(blockedPawns & (col? rowMask[16] : rowMask[40]));
        #endif

        while (isolatedPawns) {
            int bscan = bitScan(isolatedPawns);
            if ((isolatedPawnMask[bscan] & board.pieces[col]) == 0) {
                #ifdef TUNER
                evalTrace.isolatedPawnsCoeff[!col]++;
                #endif
                ret -= isolatedPawnValue;

                // Opposed Pawns
                if (((1ULL << bscan) & doubledPawns) && (forwardMask[col][bscan] & board.pieces[!col])) {
                    ret += opposedPawnValue[bscan % 8];

                    #ifdef TUNER
                    evalTrace.opposedPawnCoeff[bscan % 8][col]++;
                    #endif
                }
            }
            isolatedPawns &= isolatedPawns - 1;
        }

        while (supportedPawns) {
            int bscan = bitScan(supportedPawns);
            ret += supportedPawnsVal[col][bscan];

            #ifdef TUNER
            evalTrace.supportedPawnsCoeff[col? bscan : flipIndex64(bscan)][col]++;
            #endif

            supportedPawns &= supportedPawns - 1;
        }

        while (adjacentPawns) {
            int bscan = bitScan(adjacentPawns);
            ret += adjacentPawnsVal[col][bscan];

            #ifdef TUNER
            evalTrace.adjacentPawnsCoeff[col? bscan : flipIndex64(bscan)][col]++;
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
        evalTrace.pawnPstCoeff[col? bscan : flipIndex64(bscan)][col]++;
        #endif

        ret += kingPawnDistFriendly[chebyshevArray[board.kingLoc[col]][bscan]];
        ret += kingPawnDistEnemy[chebyshevArray[board.kingLoc[!col]][bscan]];

        #ifdef TUNER
        evalTrace.kingPawnDistFriendlyCoeff[chebyshevArray[board.kingLoc[col]][bscan]][col]++;
        evalTrace.kingPawnDistEnemyCoeff[chebyshevArray[board.kingLoc[!col]][bscan]][col]++;
        #endif

        // Passed pawns
        if ((passedPawnMask[col][bscan] & (board.pieces[!col] ^ leveredPawns)) == 0 && (forwardMask[col][bscan] & board.pieces[col]) == 0) {
            ret += col? passedPawnWeight[flipRank(bscan)] : passedPawnWeight[getRank(bscan)];

            #ifdef TUNER
            evalTrace.passedPawnCoeff[col? flipRank(bscan) : getRank(bscan)][col]++;
            #endif

            if ((forwardMask[col][bscan] & board.occupied) != 0) {
                ret += freePasser[col? flipRank(bscan) : getRank(bscan)];

                #ifdef TUNER
                evalTrace.freePasserCoeff[col? flipRank(bscan) : getRank(bscan)][col]++;
                #endif
            }

            if (columnMask[bscan] & board.pieces[6 + col]) {
                ret += rookBehindPasser;

                #ifdef TUNER
                evalTrace.rookBehindPasserCoeff[col]++;
                #endif
            }

            ret += kingPassedDistFriendly[chebyshevArray[board.kingLoc[col]][bscan]];
            ret += kingPassedDistEnemy[chebyshevArray[board.kingLoc[!col]][bscan]];

            #ifdef TUNER
            evalTrace.kingPassedDistFriendlyCoeff[chebyshevArray[board.kingLoc[col]][bscan]][col]++;
            evalTrace.kingPassedDistEnemyCoeff[chebyshevArray[board.kingLoc[!col]][bscan]][col]++;
            #endif

        }

        piece &= piece - 1;
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
        evalTrace.knightPstCoeff[col? bscan : flipIndex64(bscan)][col]++;
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
            evalTrace.knightOutpostCoeff[col? bscan : flipIndex64(bscan)][col]++;
            #endif
        }

        // Jump to outpost
        if (board.knightMoves[bscan] & holes) {
            ret += knightOutpostHole[col][bscan];

            #ifdef TUNER
            evalTrace.knightOutpostHoleCoeff[col? bscan : flipIndex64(bscan)][col]++;
            #endif
        }

        piece &= piece - 1;
    }

    return ret;

}



int Eval::evaluateBishops(Bitboard &board, ThreadSearch *th, bool col) {

    int ret = 0;
    uint64_t piece = board.pieces[4 + col];
    uint64_t holes = th->pawnAttAll[col];
    uint64_t defendedBishop = piece & holes;

    while (piece) {
        int bscan = bitScan(piece);
        uint64_t bishopAttacks = magics->bishopAttacksMask(board.occupied ^ board.pieces[8 + col], bscan);

        // PST
        ret += pieceSquare[4 + col][bscan];

        #ifdef TUNER
        evalTrace.bishopPstCoeff[col? bscan : flipIndex64(bscan)][col]++;
        #endif

        // Mobility
        ret += bishopMobilityBonus[count_population(bishopAttacks & ~th->minorUnsafe[col])];

        #ifdef TUNER
        evalTrace.bishopMobilityCoeff[count_population(bishopAttacks & ~th->minorUnsafe[col])][col]++;
        #endif

        // Outposts
        if ((defendedBishop & (1ULL << bscan)) && (outpostMask[col][bscan] & board.pieces[!col]) == 0) {
            ret += bishopOutpost[col][bscan];

            #ifdef TUNER
            evalTrace.bishopOutpostCoeff[col? bscan : flipIndex64(bscan)][col]++;
            #endif
        }

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

        // PST
        ret += pieceSquare[6 + col][bscan];

        #ifdef TUNER
        evalTrace.rookPstCoeff[col? bscan : flipIndex64(bscan)][col]++;
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
                ret += trappedRook;

                #ifdef TUNER
                evalTrace.trappedRookCoeff[col]++;
                #endif
            }
            if (board.pieces[col + 10] < 1ULL << (4 + (col * 56)) && pieceLoc < board.pieces[col + 10]) {
                ret += trappedRook;

                #ifdef TUNER
                evalTrace.trappedRookCoeff[col]++;
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
        evalTrace.queenPstCoeff[col? bscan : flipIndex64(bscan)][col]++;
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
    evalTrace.kingPstCoeff[col? board.kingLoc[col] : flipIndex64(board.kingLoc[col])][col]++;
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
        kingSafe += (th->KSAttacks[col] * attacksSafety) / (count_population(kingZoneMask[!col][theirKing] & board.pieces[!col]) + 1);
        kingSafe += (board.pieces[8 + col] == 0) * noQueen;
        kingSafe -= KSOffset;



        #if TUNER
        kingSafetyTrace.queenChecksCount[col] = count_population(queenChecks);
        kingSafetyTrace.rookChecksCount[col] = count_population(rookChecks);
        kingSafetyTrace.bishopChecksCount[col] = count_population(bishopChecks);
        kingSafetyTrace.knightChecksCount[col] = count_population(knightChecks);
        kingSafetyTrace.attackScaleCount[col] = count_population(kingZoneMask[!col][theirKing] & board.pieces[!col]) + 1;
        kingSafetyTrace.noQueen[col] = board.pieces[8 + col] == 0;
        #endif

        if (kingSafe > 0) {
            ret += S(kingSafe, 0);
        }
    }

    return ret;

}



int Eval::evaluatePawnShield(Bitboard &board, bool col) {

    int ret = 0;
    int bscanKing = board.kingLoc[col];
    uint64_t pawnShields = passedPawnMask[col][bscanKing] & board.pieces[col];
    uint64_t pawnStormers = passedPawnMask[col][bscanKing] & board.pieces[!col];
    uint64_t pawnStormBlockade = pawnShields & (col? (pawnStormers << 8) : (pawnStormers >> 8));

    if ((forwardMask[col][bscanKing] & pawnShields) != 0) {
        ret += kingPawnFront;

        #ifdef TUNER
        evalTrace.kingPawnFrontCoeff[col]++;
        #endif

        if ((forwardMask[col][bscanKing] & board.pieces[!col]) != 0) {
            ret += kingPawnFrontN;

            #ifdef TUNER
            evalTrace.kingPawnFrontNCoeff[col]++;
            #endif
        }
    }

    while (pawnShields) {
        int bscan = bitScan(pawnShields);

        ret += pawnShieldVal[col][bscan];
        #ifdef TUNER
        evalTrace.pawnShieldCoeff[col? bscan : flipIndex64(bscan)][col]++;
        #endif

        if (1ULL << bscan & pawnStormBlockade) {
            ret += pawnStormBlockadeVal[col][bscan];
            #ifdef TUNER
            evalTrace.pawnStormBlockadeCoeff[col? bscan : flipIndex64(bscan)][col]++;
            #endif
        }

        pawnShields &= pawnShields - 1;
    }

    while (pawnStormers) {
        int bscan = bitScan(pawnStormers);

        ret += pawnStormVal[col][bscan];
        #ifdef TUNER
        evalTrace.pawnStormCoeff[col? bscan : flipIndex64(bscan)][col]++;
        #endif

        pawnStormers &= pawnStormers - 1;
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
    attacks = th->attacksBishop[col];
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
    attacks = th->attacksRook[col];
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
