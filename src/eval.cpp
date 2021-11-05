#include "eval.h"

extern Magics *magics;


int pieceValues[6] = {S(114, 94), S(491, 332), S(500, 328), S(694, 559), S(1271, 1259), S(5000, 5000)};

// Queen, Bishop, Knight and rook weights
int knightWeight[9] = {S(-122, -73), S(-17, -10), S(-55, 1), S(-57, 6), S(-54, 10), S(-45, 13), S(-38, 27), S(-31, 52), S(-28, 73)};
int rookWeight[9] = {S(46, -1), S(-51, 57), S(-35, 43), S(-24, 33), S(-23, 28), S(-18, 24), S(-9, 22), S(0, 22), S(15, 7)};
int bishopWeight[9] = {S(-87, 10), S(-9, 74), S(4, 95), S(-5, 100), S(10, 86), S(15, 81), S(15, 86), S(19, 94), S(19, 105)};

// Supported and adjacent pawn weights
int supportedPawnWeight[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                               S(153, 87), S(217, -18), S(121, 76), S(182, 58), S(88, 115), S(239, 46), S(193, 50), S(169, -38),
                               S(7, 50), S(18, 61), S(70, 43), S(77, 68), S(73, 85), S(64, 61), S(52, 43), S(-2, 45),
                               S(-5, 18), S(5, 21), S(23, 23), S(26, 28), S(43, 26), S(41, 16), S(7, 18), S(2, 15),
                               S(0, 2), S(11, 9), S(15, 15), S(19, 26), S(18, 17), S(14, 9), S(14, 5), S(11, -1),
                               S(12, 8), S(18, 15), S(28, 15), S(19, 24), S(18, 25), S(9, 14), S(35, 3), S(31, 1),
                               S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                               S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int adjacentPawnWeight[64]  = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                               S(-144, 140), S(-3, 202), S(180, 226), S(55, 171), S(84, 236), S(116, 252), S(-180, 140), S(-245, 37),
                               S(-56, 95), S(50, 146), S(56, 133), S(69, 130), S(124, 69), S(120, 87), S(100, 103), S(48, 26),
                               S(1, 31), S(6, 24), S(33, 35), S(33, 39), S(44, 39), S(59, 37), S(20, 14), S(5, 26),
                               S(-9, 2), S(17, 10), S(7, 13), S(20, 28), S(18, 24), S(25, 5), S(17, 10), S(7, -4),
                               S(7, 1), S(6, 4), S(9, 9), S(13, 16), S(19, 9), S(9, 9), S(8, -1), S(7, -2),
                               S(9, 0), S(4, 0), S(4, -4), S(11, 21), S(30, 10), S(-7, -10), S(4, 4), S(3, -8),
                               S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int freePasser[7]  = {S(0, 0), S(0, -11), S(4, -3), S(11, -16), S(4, -30), S(-5, -79), S(-53, -89)};

// Passed Pawn weights
int passedPawnWeight[7] = {S(0, 0), S(-32, -57), S(-34, -41), S(-36, 0), S(-11, 38), S(24, 88), S(68, 111)};
int opposedPawnValue[8] = {S(-12, -14), S(16, -7), S(1, 15), S(9, 1), S(-5, 30), S(6, 8), S(51, -10), S(9, -16)};

// Doubled pawns and isolated pawns and backward pawns
int doublePawnValue = S(7, 35);
int isolatedPawnValue = S(11, 8);

// Blocked pawns
int blockedPawns5th = S(-19, -24);
int blockedPawns6th = S(-11, -80);

int pawnBlockedByMinors = S(-12, 2);
int pawnBlockedByMajors = S(2, 2);

// Mobility
int knightMobilityBonus[9] = {S(-79, -87), S(-45, -22), S(-26, 8), S(-16, 25), S(-6, 38), S(3, 53), S(11, 57), S(19, 60), S(22, 56)};
int bishopMobilityBonus[14] = {S(-101, -33), S(-63, -16), S(-45, 10), S(-35, 29), S(-24, 41), S(-14, 54), S(-8, 60), S(-1, 64), S(2, 70), S(7, 71), S(22, 66), S(32, 68), S(42, 78), S(50, 62)};
int rookMobilityBonus[15] = {S(-104, -89), S(-35, -37), S(-13, -9), S(-13, 8), S(-9, 14), S(-9, 25), S(-3, 31), S(1, 34), S(7, 38), S(14, 42), S(18, 45), S(25, 48), S(36, 48), S(42, 44), S(44, 42)};
int queenMobilityBonus[28] = {S(-65, -144), S(-45, -200), S(-24, -110), S(-8, -69), S(-3, -17), S(2, -4), S(7, 15), S(7, 46),
                              S(11, 58), S(14, 64), S(16, 75), S(17, 88), S(20, 88), S(22, 98), S(29, 96), S(28, 107),
                              S(31, 112), S(28, 116), S(38, 115), S(61, 100), S(61, 106), S(70, 106), S(104, 86), S(124, 75),
                              S(124, 82), S(263, -4), S(77, 81), S(182, 24)};

int pieceAttackValue[5] = {48, 62, 62, 60, 25};
int attacksSafety = 18;
int queenCheckVal = 75;
int rookCheckVal = 60;
int bishopCheckVal = 63;
int knightCheckVal = 94;
int noQueen = -185;
int KSOffset = 122;

int kingPawnFront = S(36, -3);
int kingPawnFrontN = S(6, -3);

int pawnShield[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                      S(36, 66), S(119, -10), S(36, 57), S(33, 10), S(-9, 18), S(-15, 4), S(4, 24), S(74, 15),
                      S(38, 24), S(72, 30), S(4, 22), S(-29, 10), S(-55, 14), S(12, -6), S(38, 11), S(65, 25),
                      S(77, 2), S(30, 20), S(48, -1), S(9, 3), S(0, 2), S(6, -2), S(16, 3), S(44, -9),
                      S(44, -10), S(13, 6), S(4, 2), S(24, -2), S(6, 5), S(15, -8), S(3, -8), S(34, -10),
                      S(48, -36), S(36, -21), S(-7, -4), S(-5, -1), S(17, -5), S(-1, -3), S(42, -27), S(45, -26),
                      S(22, -49), S(27, -23), S(8, -11), S(-17, 5), S(-14, 5), S(6, -10), S(16, -27), S(16, -41),
                      S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int pawnStorm[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                     S(30, 10), S(36, -11), S(10, 1), S(8, -8), S(18, 4), S(8, -3), S(4, 1), S(36, 2),
                     S(39, 0), S(50, -9), S(12, -1), S(12, -10), S(24, -7), S(6, -3), S(18, -2), S(37, -2),
                     S(30, 22), S(21, 16), S(0, 11), S(-3, -2), S(21, -12), S(-2, 7), S(7, 10), S(24, 14),
                     S(4, 75), S(-4, 46), S(0, 34), S(-30, 26), S(7, 7), S(-21, 39), S(11, 43), S(18, 49),
                     S(87, 154), S(-33, 140), S(-104, 121), S(-16, 64), S(-59, 75), S(-25, 90), S(-38, 122), S(106, 111),
                     S(207, 160), S(129, 113), S(-105, 163), S(-66, 97), S(-119, 102), S(13, 49), S(39, 152), S(271, 74),
                     S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int pawnStormBlockade[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                             S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                             S(102, -33), S(5, -41), S(0, -13), S(40, 17), S(28, -10), S(0, 8), S(59, -19), S(30, -38),
                             S(-5, -24), S(32, -41), S(-6, -18), S(21, -14), S(4, 5), S(13, -10), S(0, -18), S(25, -13),
                             S(43, -43), S(35, -48), S(18, -42), S(11, -17), S(18, -17), S(10, -28), S(8, -22), S(38, -30),
                             S(60, -81), S(10, -60), S(3, -45), S(21, -42), S(0, -35), S(15, -53), S(0, -57), S(48, -64),
                             S(-65, -197), S(32, -150), S(-14, -105), S(2, -152), S(17, -125), S(-36, -85), S(36, -158), S(-62, -174),
                             S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int rookOnOpen = S(20, 8);
int rookOnSemiOpen = S(20, -1);
int rookOnQueen = S(-42, 97);

int knightPair = S(-10, 9);
int rookPair = S(-67, -4);
int noPawns = S(81, 11);

int trappedRook = S(10, -33);
int rookBehindPasser = S(14, 11);
int tempoBonus = S(42, 28);

int pawnThreat = S(93, 26);
int pawnPushThreat = S(19, 2);

int knightThreatPiece[5] = {S(-6, 12), S(0, 0), S(40, 38), S(91, 11), S(74, -36)};
int bishopThreatPiece[5] = {S(-1, 14), S(27, 34), S(0, 0), S(71, 25), S(87, 68)};
int rookThreatPiece[5] = {S(-7, 17), S(9, 22), S(19, 20), S(0, 0), S(75, 49)};

int kingPawnDistFriendly[8] = {S(0, 0), S(28, 16), S(1, 20), S(-19, 18), S(-38, 16), S(-52, 17), S(-55, 8), S(-42, -4)};
int kingPawnDistEnemy[8] = {S(0, 0), S(-14, -26), S(9, 10), S(-16, 14), S(-25, 19), S(-31, 24), S(-35, 25), S(-39, 25)};
int kingPassedDistFriendly[8] = {S(0, 0), S(23, 61), S(2, 41), S(6, 19), S(15, 7), S(25, 1), S(38, 4), S(24, 4)};
int kingPassedDistEnemy[8] = {S(0, 0), S(-42, 9), S(5, 0), S(1, 27), S(13, 37), S(12, 49), S(7, 61), S(-14, 68)};


// -----------------------Pawn attack tables----------------------------------//
int PAWN_TABLE[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                      S(92, 118), S(83, 103), S(38, 117), S(79, 82), S(47, 88), S(52, 105), S(24, 136), S(80, 125),
                      S(27, 95), S(10, 92), S(7, 62), S(17, 27), S(1, 32), S(42, 61), S(-6, 94), S(25, 85),
                      S(21, 38), S(13, 18), S(0, 4), S(-7, -13), S(11, -19), S(-6, 4), S(12, 19), S(20, 20),
                      S(5, 9), S(-4, 5), S(-4, -15), S(-14, -29), S(-8, -29), S(-7, -13), S(-9, 0), S(-5, -4),
                      S(-4, 0), S(-5, -8), S(-13, -19), S(-20, -26), S(-20, -23), S(-10, -18), S(-27, -5), S(-19, -12),
                      S(-1, 6), S(4, -1), S(3, -9), S(-4, -21), S(-22, 4), S(16, -9), S(16, -9), S(1, -11),
                      S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};
// ---------------------------------------------------------------------------//


// -----------------------Knight attack tables----------------------------------//
int KNIGHT_TABLE[64] = {S(-209, -6), S(-191, 27), S(-136, 36), S(-65, 17), S(-35, 14), S(-116, 3), S(-134, 13), S(-148, -42),
                        S(-68, 22), S(-46, 24), S(-24, 5), S(-16, 12), S(-1, -2), S(33, -13), S(-25, 20), S(-18, 0),
                        S(-45, 18), S(-26, 17), S(0, 25), S(5, 19), S(37, 15), S(67, -4), S(11, 8), S(37, -13),
                        S(-7, 25), S(10, 16), S(24, 20), S(41, 32), S(49, 24), S(63, 26), S(47, 15), S(66, 7),
                        S(-35, 37), S(-10, 32), S(12, 33), S(17, 37), S(27, 51), S(18, 29), S(50, 10), S(-4, 33),
                        S(-35, 13), S(-20, 9), S(-9, 13), S(0, 39), S(13, 36), S(0, 16), S(9, 6), S(-5, 24),
                        S(-52, 18), S(-31, 20), S(-33, 18), S(-18, 19), S(-11, 26), S(-21, 21), S(9, 12), S(-6, 33),
                        S(-92, 27), S(-40, -2), S(-53, 18), S(-37, 23), S(-24, 22), S(-27, 23), S(-38, 14), S(-43, 43)};
// ---------------------------------------------------------------------------//


// -----------------------Bishop attack tables----------------------------------//
int BISHOP_TABLE[64] = {S(-73, 58), S(-98, 56), S(-103, 53), S(-146, 61), S(-123, 53), S(-121, 45), S(-69, 45), S(-100, 42),
                        S(-45, 35), S(-45, 42), S(-43, 38), S(-66, 48), S(-51, 32), S(-27, 33), S(-63, 45), S(-60, 40),
                        S(-36, 52), S(-17, 40), S(-35, 47), S(-17, 31), S(-23, 40), S(10, 41), S(8, 42), S(6, 41),
                        S(-39, 47), S(-16, 43), S(-6, 40), S(6, 47), S(2, 43), S(3, 46), S(-9, 40), S(-18, 41),
                        S(-23, 36), S(-30, 44), S(-6, 43), S(9, 43), S(6, 35), S(-5, 39), S(-17, 43), S(0, 24),
                        S(-20, 32), S(18, 36), S(-1, 40), S(-6, 42), S(2, 48), S(8, 39), S(20, 31), S(5, 24),
                        S(-3, 31), S(-9, 17), S(4, 21), S(-19, 36), S(-11, 35), S(6, 29), S(18, 22), S(8, 14),
                        S(-14, 20), S(11, 32), S(-12, 31), S(-25, 38), S(-8, 34), S(-21, 47), S(-5, 28), S(15, 1)};
// ---------------------------------------------------------------------------//


// -----------------------Rook attack tables----------------------------------//
int ROOK_TABLE[64] = {S(-10, 49), S(-14, 51), S(-14, 57), S(-14, 51), S(3, 40), S(32, 37), S(14, 46), S(46, 32),
                      S(-9, 41), S(-3, 47), S(12, 47), S(32, 33), S(21, 33), S(56, 25), S(63, 20), S(73, 14),
                      S(-20, 39), S(15, 30), S(5, 34), S(10, 25), S(41, 17), S(72, 5), S(107, 1), S(61, 3),
                      S(-16, 40), S(-2, 33), S(2, 38), S(3, 32), S(14, 19), S(38, 10), S(43, 14), S(26, 13),
                      S(-27, 35), S(-23, 36), S(-14, 34), S(-8, 33), S(-3, 29), S(2, 23), S(32, 9), S(5, 12),
                      S(-29, 29), S(-21, 24), S(-17, 26), S(-15, 28), S(1, 19), S(11, 9), S(49, -13), S(12, -2),
                      S(-30, 20), S(-26, 24), S(-12, 25), S(-8, 22), S(0, 14), S(15, 5), S(37, -8), S(-10, 4),
                      S(-21, 22), S(-16, 20), S(-15, 27), S(-6, 20), S(7, 9), S(1, 15), S(8, 7), S(-13, 11)};
// ---------------------------------------------------------------------------//


// -----------------------Queen attack tables----------------------------------//
int QUEEN_TABLE[64] = {S(-38, 37), S(-33, 48), S(-9, 63), S(29, 49), S(26, 54), S(42, 46), S(95, -21), S(23, 24),
                       S(-8, 36), S(-27, 65), S(-19, 91), S(-36, 116), S(-25, 133), S(26, 83), S(30, 57), S(76, 46),
                       S(-4, 38), S(-1, 43), S(-4, 81), S(8, 89), S(19, 99), S(80, 71), S(84, 38), S(74, 41),
                       S(-7, 37), S(7, 43), S(8, 57), S(-1, 88), S(6, 102), S(34, 79), S(39, 79), S(36, 60),
                       S(10, -4), S(-9, 54), S(-2, 53), S(2, 78), S(6, 70), S(18, 58), S(17, 57), S(32, 48),
                       S(8, -18), S(-1, 26), S(0, 35), S(-6, 39), S(2, 45), S(18, 28), S(24, 28), S(23, 11),
                       S(1, -22), S(-4, -6), S(0, -1), S(7, 4), S(7, 2), S(20, -23), S(23, -29), S(31, -59),
                       S(-8, -22), S(-16, -10), S(-13, -6), S(-10, -14), S(-5, -10), S(-2, -22), S(-11, -19), S(10, -49)};
// ---------------------------------------------------------------------------//


// -----------------------King attack tables----------------------------------//
int KING_TABLE[64] =  {S(54, -46), S(87, -24), S(112, -21), S(-30, 24), S(43, 0), S(45, 15), S(131, 8), S(208, -82),
                       S(-45, 30), S(37, 34), S(26, 31), S(138, 7), S(73, 28), S(67, 46), S(96, 41), S(52, 33),
                       S(-79, 35), S(57, 28), S(10, 35), S(-15, 42), S(53, 44), S(116, 34), S(50, 43), S(4, 25),
                       S(-40, 12), S(-37, 22), S(-58, 34), S(-84, 41), S(-76, 42), S(-45, 40), S(-56, 35), S(-112, 33),
                       S(-72, -1), S(-75, 13), S(-94, 26), S(-117, 35), S(-110, 34), S(-64, 20), S(-85, 21), S(-125, 17),
                       S(-41, -9), S(-48, 3), S(-85, 16), S(-94, 24), S(-87, 24), S(-69, 16), S(-48, 8), S(-38, -1),
                       S(27, -21), S(-31, 0), S(-29, 4), S(-57, 12), S(-62, 19), S(-35, 12), S(-2, 1), S(26, -12),
                       S(16, -45), S(9, -19), S(0, -13), S(-60, 2), S(-1, -12), S(-27, 6), S(20, -4), S(44, -37)};
// ---------------------------------------------------------------------------//



// ---------------------------Knight Outpost----------------------------------//
int KNIGHT_OUTPOST[64] = {S(-167, -138), S(5, 143), S(294, -78), S(-106, 39), S(85, -20), S(142, 25), S(-20, -73), S(-639, -20),
                          S(46, -15), S(0, 25), S(26, 32), S(1, 34), S(87, -1), S(40, 25), S(2, 16), S(-177, 97),
                          S(30, 28), S(22, 14), S(47, 23), S(56, 24), S(62, 27), S(50, 44), S(63, 16), S(10, 44),
                          S(-2, 12), S(10, 24), S(24, 26), S(26, 33), S(41, 32), S(40, 17), S(45, 14), S(-4, 28),
                          S(27, 0), S(20, 0), S(19, 13), S(19, 21), S(43, 21), S(36, 13), S(31, 22), S(26, 5),
                          S(-1, 16), S(-6, 31), S(2, 13), S(-7, 18), S(-3, 6), S(1, 3), S(-4, 7), S(3, 5),
                          S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                          S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int KNIGHT_OUTPOST_HOLE[64] = {S(-79, 51), S(102, 6), S(68, 11), S(13, -1), S(46, 28), S(112, -1), S(-105, 89), S(-125, 121),
                               S(17, 27), S(6, 29), S(-17, 42), S(17, 25), S(-18, 35), S(-3, 29), S(-5, 23), S(23, 20),
                               S(11, 23), S(1, 26), S(-15, 17), S(-6, 15), S(-21, 7), S(-11, 4), S(-15, 12), S(-33, 33),
                               S(-4, 20), S(-10, 12), S(-12, 15), S(-7, 3), S(-19, 2), S(-10, 0), S(-21, 6), S(-24, 11),
                               S(17, 1), S(10, -7), S(-10, 6), S(-5, 0), S(-9, -8), S(4, -1), S(-18, 20), S(8, 0),
                               S(-4, 1), S(-7, 13), S(-1, 4), S(-9, 0), S(-4, 0), S(2, 0), S(-1, 7), S(-3, 3),
                               S(-1, 13), S(-13, 14), S(-1, 7), S(-3, 2), S(-1, -6), S(10, 1), S(-22, 10), S(-14, -6),
                               S(29, -16), S(-1, 27), S(9, 25), S(12, 15), S(13, 8), S(5, 0), S(-7, 13), S(2, -22)};

int BISHOP_OUTPOST[64] = {S(504, -267), S(153, -35), S(142, -95), S(186, -15), S(306, -33), S(213, -90), S(168, -91), S(-257, -51),
                          S(62, 12), S(87, -4), S(92, -1), S(113, -35), S(192, -21), S(92, 10), S(103, -11), S(-40, 51),
                          S(6, 20), S(36, 3), S(53, 3), S(58, 8), S(93, -13), S(78, 0), S(35, -9), S(-14, -3),
                          S(-6, 1), S(29, 9), S(25, 11), S(48, 12), S(61, -3), S(58, -6), S(41, 12), S(41, -13),
                          S(-6, 38), S(41, 11), S(45, 14), S(37, 22), S(77, 19), S(32, 20), S(36, 5), S(-2, 25),
                          S(42, 10), S(6, 37), S(36, 16), S(21, 39), S(12, 23), S(0, 16), S(12, 20), S(13, 11),
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

    // Pawn count
    if (board.pieceCount[col] == 0) {
        ret += noPawns;

        #ifdef TUNER
        evalTrace.noPawnsCoeff[col]++;
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
