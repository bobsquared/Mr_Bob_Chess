#include "eval.h"

extern Magics *magics;


int pieceValues[6] = {S(114, 94), S(491, 332), S(500, 328), S(694, 559), S(1271, 1259), S(5000, 5000)};

// Queen, Bishop, Knight and rook weights
int knightWeight[9] = {S(-107, -73), S(-16, -10), S(-52, 0), S(-53, 5), S(-49, 8), S(-40, 10), S(-32, 21), S(-25, 46), S(-22, 66)};
int rookWeight[9] = {S(46, 1), S(-38, 53), S(-27, 40), S(-18, 30), S(-18, 25), S(-14, 20), S(-7, 18), S(1, 16), S(16, 0)};
int bishopWeight[9] = {S(-64, 8), S(-9, 76), S(3, 97), S(-4, 99), S(12, 83), S(17, 75), S(18, 79), S(22, 84), S(22, 89)};

// Supported and adjacent pawn weights
int supportedPawnWeight[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                               S(146, 88), S(186, -3), S(114, 79), S(163, 66), S(91, 112), S(210, 57), S(172, 58), S(149, -28),
                               S(8, 50), S(17, 61), S(71, 43), S(78, 68), S(72, 86), S(65, 61), S(49, 45), S(0, 44),
                               S(-4, 18), S(5, 22), S(22, 23), S(24, 30), S(43, 26), S(40, 17), S(5, 19), S(1, 15),
                               S(-1, 2), S(10, 10), S(13, 16), S(19, 26), S(17, 18), S(12, 9), S(11, 5), S(10, -2),
                               S(10, 9), S(16, 17), S(27, 16), S(18, 24), S(17, 24), S(9, 13), S(37, 1), S(31, 1),
                               S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                               S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int adjacentPawnWeight[64]  = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                               S(-89, 135), S(31, 186), S(166, 205), S(72, 159), S(91, 212), S(129, 234), S(-141, 135), S(-220, 31),
                               S(-39, 95), S(53, 136), S(68, 131), S(76, 123), S(117, 72), S(118, 88), S(93, 95), S(49, 35),
                               S(2, 30), S(7, 24), S(33, 35), S(33, 39), S(45, 39), S(59, 37), S(20, 15), S(6, 24),
                               S(-9, 2), S(18, 10), S(7, 14), S(21, 28), S(17, 25), S(26, 5), S(14, 11), S(8, -4),
                               S(5, 0), S(5, 6), S(9, 8), S(12, 18), S(18, 9), S(8, 10), S(7, -2), S(7, -3),
                               S(8, 1), S(3, 0), S(4, -4), S(11, 21), S(31, 5), S(-5, -11), S(5, 2), S(4, -6),
                               S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int freePasser[7]  = {S(0, 0), S(0, -11), S(0, 0), S(8, -14), S(1, -29), S(-9, -77), S(-57, -83)};

// Passed Pawn weights
int passedPawnWeight[7] = {S(0, 0), S(-33, -55), S(-33, -42), S(-36, 0), S(-8, 37), S(26, 86), S(69, 108)};
int opposedPawnValue[8] = {S(-14, -12), S(17, -6), S(1, 15), S(9, 2), S(-4, 28), S(9, 6), S(57, -14), S(7, -14)};

// Doubled pawns and isolated pawns and backward pawns
int doublePawnValue = S(8, 37);
int isolatedPawnValue = S(13, 7);

// Blocked pawns
int blockedPawns5th = S(-18, -23);
int blockedPawns6th = S(-16, -76);

int pawnBlockedByMinors = S(-12, 2);
int pawnBlockedByMajors = S(2, 1);

// Mobility
int knightMobilityBonus[9] = {S(-76, -89), S(-41, -25), S(-22, 6), S(-11, 22), S(-1, 34), S(8, 49), S(17, 53), S(24, 56), S(28, 51)};
int bishopMobilityBonus[14] = {S(-92, -39), S(-54, -23), S(-36, 3), S(-27, 22), S(-15, 34), S(-5, 47), S(0, 53), S(7, 57), S(11, 64), S(15, 65), S(30, 61), S(40, 63), S(50, 73), S(58, 58)};
int rookMobilityBonus[15] = {S(-101, -92), S(-33, -39), S(-11, -13), S(-11, 4), S(-6, 11), S(-7, 22), S(-1, 27), S(4, 31), S(10, 35), S(17, 39), S(21, 41), S(28, 45), S(39, 44), S(45, 40), S(46, 38)};
int queenMobilityBonus[28] = {S(-67, -134), S(-51, -178), S(-26, -104), S(-9, -69), S(-3, -20), S(2, -7), S(6, 12), S(7, 44),
                              S(11, 55), S(15, 61), S(17, 72), S(19, 85), S(21, 85), S(24, 94), S(30, 92), S(29, 103),
                              S(33, 107), S(30, 112), S(40, 110), S(63, 95), S(62, 101), S(73, 100), S(106, 81), S(123, 70),
                              S(121, 80), S(239, 5), S(79, 77), S(153, 35)};

int pieceAttackValue[5] = {48, 62, 62, 60, 25};
int attacksSafety = 18;
int queenCheckVal = 75;
int rookCheckVal = 60;
int bishopCheckVal = 63;
int knightCheckVal = 94;
int noQueen = -185;
int KSOffset = 122;

int kingPawnFront = S(37, -5);
int kingPawnFrontN = S(9, -4);

int pawnShield[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                      S(40, 68), S(103, -2), S(34, 60), S(31, 12), S(-13, 21), S(-17, 8), S(-4, 29), S(62, 24),
                      S(36, 28), S(66, 34), S(0, 24), S(-29, 11), S(-57, 15), S(10, -2), S(35, 14), S(54, 29),
                      S(74, 5), S(28, 21), S(46, 0), S(7, 6), S(3, 1), S(6, -1), S(14, 5), S(42, -8),
                      S(41, -7), S(10, 7), S(4, 3), S(24, -1), S(4, 6), S(15, -8), S(4, -10), S(31, -6),
                      S(47, -34), S(35, -19), S(-6, -3), S(-3, -2), S(15, -4), S(0, -3), S(42, -27), S(43, -21),
                      S(21, -44), S(24, -15), S(10, -9), S(-14, 3), S(-15, 6), S(9, -9), S(16, -22), S(14, -34),
                      S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int pawnStorm[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                     S(30, 10), S(37, -12), S(9, 2), S(8, -8), S(17, 7), S(7, -2), S(2, 1), S(34, 1),
                     S(40, 0), S(50, -9), S(10, 0), S(15, -12), S(22, -5), S(5, -2), S(18, -4), S(34, -1),
                     S(31, 21), S(22, 15), S(0, 10), S(-2, -2), S(20, -11), S(-2, 7), S(7, 7), S(21, 14),
                     S(8, 70), S(-2, 43), S(1, 33), S(-28, 23), S(6, 7), S(-20, 36), S(11, 41), S(20, 42),
                     S(84, 148), S(-25, 133), S(-93, 116), S(-13, 63), S(-55, 72), S(-23, 83), S(-27, 112), S(94, 103),
                     S(211, 159), S(127, 121), S(-88, 157), S(-56, 96), S(-107, 100), S(14, 51), S(51, 151), S(256, 86),
                     S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int pawnStormBlockade[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                             S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                             S(103, -36), S(10, -46), S(4, -18), S(39, 18), S(24, -9), S(2, 5), S(57, -19), S(38, -36),
                             S(-4, -25), S(32, -42), S(-5, -20), S(22, -16), S(-1, 5), S(12, -10), S(0, -18), S(26, -13),
                             S(46, -45), S(34, -47), S(16, -41), S(11, -18), S(18, -16), S(10, -28), S(6, -18), S(44, -33),
                             S(58, -77), S(10, -57), S(3, -43), S(17, -36), S(0, -32), S(13, -50), S(1, -57), S(47, -58),
                             S(-61, -187), S(22, -139), S(-32, -94), S(-7, -133), S(3, -112), S(-40, -75), S(27, -149), S(-58, -161),
                             S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int rookOnOpen = S(21, 7);
int rookOnSemiOpen = S(20, 0);
int rookOnQueen = S(-41, 95);

int knightPair = S(10, -7);
int rookPair = S(56, 10);
int noPawns = S(-74, -10);

int trappedRook = S(-7, 27);
int rookBehindPasser = S(15, 12);
int tempoBonus = S(43, 29);

int pawnThreat = S(94, 25);
int pawnPushThreat = S(20, 2);

int knightThreatPiece[5] = {S(-6, 12), S(0, 0), S(40, 38), S(91, 10), S(75, -39)};
int bishopThreatPiece[5] = {S(-2, 15), S(27, 35), S(0, 0), S(71, 25), S(88, 66)};
int rookThreatPiece[5] = {S(-7, 17), S(9, 22), S(20, 20), S(0, 0), S(76, 47)};

int kingPawnDistFriendly[8] = {S(0, 0), S(27, 16), S(1, 20), S(-18, 18), S(-35, 16), S(-48, 16), S(-49, 7), S(-34, -5)};
int kingPawnDistEnemy[8] = {S(0, 0), S(-15, -27), S(14, 8), S(-12, 13), S(-23, 19), S(-29, 24), S(-35, 26), S(-35, 25)};
int kingPassedDistFriendly[8] = {S(0, 0), S(26, 59), S(3, 40), S(7, 18), S(15, 7), S(26, 1), S(39, 4), S(23, 4)};
int kingPassedDistEnemy[8] = {S(0, 0), S(-33, 6), S(6, 0), S(2, 27), S(14, 36), S(12, 48), S(7, 60), S(-20, 69)};


// -----------------------Pawn attack tables----------------------------------//
int PAWN_TABLE[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                      S(94, 115), S(86, 99), S(42, 113), S(77, 82), S(46, 87), S(51, 103), S(31, 131), S(72, 125),
                      S(29, 92), S(12, 90), S(7, 62), S(15, 28), S(0, 32), S(41, 58), S(0, 90), S(22, 82),
                      S(19, 37), S(11, 18), S(0, 4), S(-8, -14), S(8, -17), S(-6, 3), S(12, 18), S(20, 18),
                      S(4, 8), S(-5, 6), S(-3, -15), S(-12, -28), S(-7, -29), S(-5, -13), S(-9, 1), S(-4, -5),
                      S(-5, 0), S(-4, -8), S(-12, -18), S(-16, -27), S(-16, -22), S(-7, -17), S(-26, -3), S(-18, -13),
                      S(-1, 5), S(5, -2), S(5, -10), S(-2, -21), S(-21, 7), S(17, -8), S(15, -8), S(3, -14),
                      S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};
// ---------------------------------------------------------------------------//


// -----------------------Knight attack tables----------------------------------//
int KNIGHT_TABLE[64] = {S(-202, -14), S(-181, 22), S(-132, 32), S(-60, 14), S(-30, 11), S(-113, 0), S(-129, 10), S(-146, -47),
                        S(-63, 17), S(-44, 21), S(-20, 2), S(-12, 9), S(0, -5), S(36, -15), S(-22, 17), S(-16, -2),
                        S(-40, 14), S(-21, 13), S(2, 22), S(9, 16), S(37, 14), S(64, -4), S(14, 5), S(39, -14),
                        S(-3, 21), S(15, 11), S(27, 18), S(39, 31), S(47, 25), S(60, 26), S(47, 14), S(63, 7),
                        S(-31, 32), S(-6, 29), S(14, 32), S(19, 35), S(26, 48), S(21, 26), S(52, 8), S(0, 29),
                        S(-32, 10), S(-18, 7), S(-5, 10), S(3, 35), S(18, 32), S(3, 15), S(12, 5), S(-2, 21),
                        S(-47, 13), S(-27, 17), S(-28, 14), S(-14, 15), S(-7, 21), S(-17, 17), S(9, 11), S(-4, 29),
                        S(-86, 22), S(-36, -6), S(-49, 14), S(-32, 19), S(-20, 18), S(-22, 18), S(-34, 10), S(-38, 38)};
// ---------------------------------------------------------------------------//


// -----------------------Bishop attack tables----------------------------------//
int BISHOP_TABLE[64] = {S(-69, 56), S(-94, 53), S(-99, 51), S(-142, 59), S(-119, 50), S(-117, 42), S(-66, 42), S(-96, 39),
                        S(-40, 32), S(-42, 39), S(-39, 34), S(-62, 45), S(-46, 29), S(-23, 30), S(-61, 41), S(-57, 38),
                        S(-32, 48), S(-12, 37), S(-31, 43), S(-13, 27), S(-18, 36), S(16, 36), S(14, 37), S(11, 37),
                        S(-35, 44), S(-11, 39), S(-2, 37), S(11, 42), S(7, 38), S(8, 42), S(-4, 35), S(-13, 38),
                        S(-19, 32), S(-26, 40), S(-1, 38), S(14, 38), S(11, 30), S(0, 34), S(-12, 39), S(4, 20),
                        S(-16, 29), S(22, 32), S(2, 35), S(-1, 38), S(7, 43), S(12, 35), S(24, 28), S(9, 22),
                        S(0, 27), S(-5, 13), S(8, 17), S(-15, 33), S(-7, 31), S(10, 25), S(23, 18), S(12, 11),
                        S(-11, 18), S(16, 28), S(-8, 28), S(-21, 36), S(-4, 31), S(-17, 44), S(-1, 25), S(19, -1)};
// ---------------------------------------------------------------------------//


// -----------------------Rook attack tables----------------------------------//
int ROOK_TABLE[64] = {S(-5, 45), S(-10, 48), S(-10, 53), S(-10, 47), S(7, 37), S(36, 33), S(19, 42), S(50, 29),
                      S(-7, 38), S(-1, 44), S(14, 45), S(34, 30), S(22, 31), S(58, 22), S(64, 17), S(76, 11),
                      S(-18, 36), S(17, 27), S(6, 31), S(12, 22), S(43, 15), S(73, 3), S(110, -1), S(64, 0),
                      S(-14, 37), S(0, 30), S(3, 35), S(4, 30), S(16, 16), S(40, 7), S(46, 10), S(28, 9),
                      S(-25, 32), S(-21, 33), S(-12, 31), S(-6, 30), S(-1, 26), S(5, 20), S(34, 6), S(7, 9),
                      S(-27, 26), S(-19, 21), S(-16, 22), S(-13, 25), S(4, 16), S(14, 6), S(51, -16), S(14, -5),
                      S(-27, 17), S(-24, 21), S(-10, 22), S(-6, 19), S(3, 12), S(17, 2), S(39, -10), S(-9, 1),
                      S(-19, 19), S(-14, 17), S(-13, 25), S(-4, 17), S(10, 6), S(4, 12), S(10, 4), S(-12, 8)};
// ---------------------------------------------------------------------------//


// -----------------------Queen attack tables----------------------------------//
int QUEEN_TABLE[64] = {S(-36, 33), S(-29, 43), S(-7, 60), S(32, 45), S(30, 49), S(45, 43), S(98, -25), S(27, 19),
                       S(-6, 31), S(-28, 63), S(-19, 89), S(-36, 113), S(-25, 130), S(28, 79), S(32, 53), S(77, 43),
                       S(-3, 34), S(0, 40), S(-5, 78), S(8, 86), S(21, 95), S(81, 68), S(87, 34), S(76, 38),
                       S(-7, 34), S(7, 40), S(8, 55), S(-1, 86), S(6, 100), S(35, 76), S(40, 75), S(38, 57),
                       S(9, -5), S(-9, 52), S(-1, 51), S(1, 76), S(6, 68), S(18, 56), S(18, 55), S(33, 45),
                       S(8, -19), S(-1, 25), S(0, 34), S(-6, 38), S(2, 44), S(18, 27), S(24, 27), S(24, 8),
                       S(1, -23), S(-4, -7), S(0, -1), S(7, 3), S(7, 1), S(20, -24), S(21, -27), S(32, -61),
                       S(-8, -23), S(-16, -12), S(-13, -7), S(-10, -14), S(-5, -11), S(-3, -23), S(-11, -20), S(8, -49)};
// ---------------------------------------------------------------------------//


// -----------------------King attack tables----------------------------------//
int KING_TABLE[64] =  {S(48, -42), S(82, -21), S(110, -19), S(-23, 22), S(50, 0), S(52, 13), S(129, 10), S(201, -79),
                       S(-46, 32), S(38, 35), S(33, 30), S(143, 7), S(81, 26), S(75, 44), S(97, 41), S(50, 35),
                       S(-80, 36), S(57, 29), S(20, 33), S(0, 39), S(64, 41), S(120, 33), S(50, 43), S(2, 26),
                       S(-40, 13), S(-35, 23), S(-45, 31), S(-64, 37), S(-57, 37), S(-33, 37), S(-51, 35), S(-111, 34),
                       S(-70, 0), S(-70, 13), S(-81, 24), S(-97, 31), S(-89, 29), S(-50, 18), S(-78, 21), S(-121, 18),
                       S(-40, -7), S(-40, 3), S(-72, 14), S(-78, 20), S(-70, 19), S(-56, 15), S(-40, 9), S(-36, 0),
                       S(25, -18), S(-27, 2), S(-20, 3), S(-46, 9), S(-51, 16), S(-27, 12), S(1, 3), S(25, -9),
                       S(9, -42), S(7, -20), S(0, -12), S(-59, 3), S(0, -12), S(-28, 8), S(17, -3), S(37, -34)};
// ---------------------------------------------------------------------------//



// ---------------------------Knight Outpost----------------------------------//
int KNIGHT_OUTPOST[64] = {S(-181, -133), S(25, 137), S(255, -64), S(-82, 33), S(75, -16), S(129, 29), S(-25, -71), S(-584, -40),
                          S(44, -14), S(-1, 28), S(25, 33), S(1, 34), S(85, 0), S(37, 27), S(3, 17), S(-179, 97),
                          S(29, 28), S(21, 15), S(47, 23), S(56, 24), S(62, 27), S(55, 42), S(62, 17), S(13, 41),
                          S(-2, 13), S(9, 24), S(26, 26), S(27, 32), S(42, 31), S(41, 15), S(47, 13), S(-1, 26),
                          S(26, 1), S(18, 1), S(19, 13), S(18, 20), S(42, 22), S(36, 13), S(32, 22), S(26, 4),
                          S(-1, 16), S(-8, 31), S(2, 13), S(-8, 18), S(-3, 7), S(2, 0), S(-5, 6), S(4, 4),
                          S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                          S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int KNIGHT_OUTPOST_HOLE[64] = {S(-86, 58), S(90, 10), S(63, 13), S(9, 0), S(42, 29), S(104, 1), S(-110, 90), S(-129, 123),
                               S(14, 29), S(5, 29), S(-18, 42), S(16, 25), S(-16, 34), S(-1, 28), S(-6, 23), S(22, 21),
                               S(9, 24), S(0, 27), S(-15, 17), S(-7, 16), S(-18, 5), S(-5, 0), S(-15, 12), S(-31, 31),
                               S(-5, 21), S(-13, 15), S(-12, 15), S(-2, 1), S(-13, 0), S(-3, -2), S(-17, 2), S(-18, 7),
                               S(17, 2), S(10, -7), S(-8, 5), S(-3, 0), S(-5, -6), S(4, 0), S(-16, 18), S(8, 0),
                               S(-4, 1), S(-6, 12), S(-1, 4), S(-10, 0), S(-6, 0), S(3, -3), S(-1, 6), S(-3, 3),
                               S(-3, 14), S(-14, 14), S(-2, 8), S(-3, 2), S(-2, -5), S(9, 2), S(-19, 7), S(-13, -6),
                               S(24, -10), S(-1, 25), S(8, 26), S(11, 16), S(13, 8), S(4, 1), S(-8, 13), S(0, -20)};

int BISHOP_OUTPOST[64] = {S(444, -255), S(123, -23), S(114, -83), S(153, -1), S(257, -18), S(201, -82), S(147, -83), S(-245, -54),
                          S(63, 13), S(87, -2), S(91, 0), S(113, -34), S(181, -15), S(90, 12), S(103, -10), S(-44, 55),
                          S(5, 22), S(35, 4), S(53, 4), S(58, 9), S(92, -11), S(75, 3), S(33, -7), S(-15, -2),
                          S(-6, 2), S(28, 10), S(25, 12), S(48, 13), S(61, -1), S(57, -5), S(40, 13), S(40, -12),
                          S(-7, 39), S(39, 13), S(44, 15), S(36, 22), S(76, 19), S(31, 20), S(35, 6), S(-3, 25),
                          S(42, 10), S(5, 37), S(35, 16), S(21, 39), S(12, 24), S(1, 15), S(10, 22), S(12, 10),
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
    uint64_t piece = board.pieces[col];
    uint64_t leveredPawns = board.pieces[!col] & th->pawnAttAll[col];
    uint64_t ourMinors = board.pieces[2 + col] | board.pieces[4 + col];
    uint64_t ourMajors = board.pieces[6 + col] | board.pieces[8 + col];
    uint64_t minorsBlockingPawn = board.pieces[col] & (col? (ourMinors << 8) : (ourMinors >> 8));
    uint64_t majorsBlockingPawn = board.pieces[col] & (col? (ourMajors << 8) : (ourMajors >> 8));
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

        ret += pawnBlockedByMinors * count_population(minorsBlockingPawn);
        ret += pawnBlockedByMajors * count_population(majorsBlockingPawn);

        #ifdef TUNER
        evalTrace.pawnBlockedByMinorsCoeff[col] += count_population(minorsBlockingPawn);
        evalTrace.pawnBlockedByMajorsCoeff[col] += count_population(majorsBlockingPawn);
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
