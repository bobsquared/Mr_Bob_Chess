#include "eval.h"

extern Magics *magics;


int pieceValues[6] = {S(85, 120), S(450, 475), S(450, 475), S(650, 700), S(1350, 1510), S(5000, 5000)};


// Pawn Eval
int PAWN_TABLE[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                      S(152, 119), S(112, 123), S(121, 110), S(142, 79), S(116, 80), S(121, 100), S(41, 138), S(111, 120),
                      S(30, 113), S(16, 98), S(32, 63), S(50, 19), S(33, 20), S(83, 57), S(2, 92), S(33, 93),
                      S(37, 50), S(26, 29), S(7, 8), S(24, -23), S(35, -25), S(23, 4), S(30, 28), S(24, 30),
                      S(18, 16), S(1, 12), S(10, -18), S(-8, -36), S(-3, -35), S(5, -11), S(-1, 10), S(-20, 5),
                      S(2, 4), S(-12, 0), S(-19, -17), S(-26, -25), S(-21, -24), S(-19, -12), S(-16, -3), S(-25, -6),
                      S(4, 12), S(0, 3), S(-15, -2), S(-17, -14), S(-35, 7), S(14, -9), S(24, -10), S(-2, -6),
                      S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int supportedPawnWeight[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                               S(134, 21), S(259, -19), S(183, 47), S(243, 64), S(260, 54), S(353, 2), S(359, -4), S(196, -13),
                               S(48, 39), S(61, 56), S(74, 72), S(91, 89), S(106, 87), S(95, 68), S(85, 33), S(26, 40),
                               S(-1, 19), S(18, 23), S(32, 29), S(38, 20), S(53, 21), S(48, 16), S(20, 15), S(12, 7),
                               S(5, 0), S(19, 6), S(19, 9), S(24, 24), S(28, 16), S(16, 5), S(15, 0), S(19, -4),
                               S(18, 1), S(29, 8), S(36, 16), S(32, 21), S(29, 25), S(17, 13), S(27, 6), S(29, 2),
                               S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                               S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int adjacentPawnWeight[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                              S(-151, 189), S(-105, 182), S(126, 198), S(135, 114), S(40, 133), S(45, 269), S(-160, 101), S(-61, 55),
                              S(-52, 58), S(43, 124), S(38, 124), S(62, 133), S(125, 99), S(99, 108), S(93, 99), S(58, 43),
                              S(14, 27), S(5, 43), S(41, 37), S(33, 52), S(37, 49), S(56, 47), S(12, 31), S(26, 32),
                              S(-9, 1), S(22, 10), S(3, 18), S(17, 31), S(21, 27), S(12, 10), S(36, 1), S(-4, 0),
                              S(16, 3), S(10, -3), S(14, 9), S(12, 13), S(18, 9), S(18, 7), S(6, -1), S(15, -4),
                              S(12, -11), S(3, 5), S(16, -6), S(3, 27), S(20, 26), S(-1, -13), S(1, 10), S(0, -11),
                              S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int passedPawnWeight[7] = {S(0, 0), S(-64, -75), S(-65, -61), S(-49, -15), S(-19, 25), S(42, 86), S(128, 111)};
int freePasser[7] = {S(0, 0), S(-6, -12), S(0, -6), S(0, -21), S(2, -35), S(-7, -74), S(-49, -106)};
int opposedPawnValue[8] = {S(-11, -25), S(20, -20), S(4, 7), S(5, 4), S(-3, 17), S(4, 7), S(41, -2), S(-13, -10)};
int doublePawnValue = S(9, 30);
int isolatedPawnValue = S(13, 12);
int blockedPawns5th = S(-15, -24);
int blockedPawns6th = S(-6, -61);

int kingPawnDistFriendly[8] = {S(0, 0), S(26, 20), S(19, 19), S(5, 14), S(-9, 11), S(-20, 11), S(-31, 6), S(-28, -4)};
int kingPawnDistEnemy[8] = {S(0, 0), S(3, -39), S(32, 2), S(4, 6), S(-1, 9), S(-6, 15), S(-7, 15), S(3, 14)};
int kingPassedDistFriendly[8] = {S(0, 0), S(-3, 84), S(-2, 60), S(7, 40), S(15, 26), S(20, 17), S(29, 14), S(9, 7)};
int kingPassedDistEnemy[8] = {S(0, 0), S(-66, -3), S(-5, 13), S(11, 38), S(24, 53), S(40, 61), S(42, 74), S(35, 83)};
int pawnThreat = S(72, 48);
int pawnPushThreat = S(15, 8);


// Knight Eval
int KNIGHT_TABLE[64] = {S(-240, -38), S(-140, -4), S(-154, 25), S(-68, 2), S(-39, 0), S(-157, 20), S(-130, 2), S(-181, -62),
                        S(-46, -9), S(-25, -7), S(9, -14), S(40, 5), S(27, 0), S(35, -27), S(-24, -6), S(-19, -13),
                        S(-51, -12), S(-14, -3), S(25, 19), S(30, 13), S(57, 2), S(70, 0), S(16, -13), S(-9, -12),
                        S(6, -5), S(21, 0), S(47, 16), S(82, 16), S(54, 27), S(80, 7), S(60, 2), S(72, -10),
                        S(-13, -1), S(12, 9), S(41, 22), S(43, 31), S(58, 30), S(40, 26), S(59, 0), S(10, 6),
                        S(-29, -20), S(-7, -8), S(6, 1), S(15, 24), S(27, 24), S(9, 3), S(18, -8), S(1, -6),
                        S(-46, -9), S(-33, 3), S(-21, -5), S(-8, 4), S(-5, 4), S(-4, -4), S(21, 0), S(-12, 6),
                        S(-96, -17), S(-25, -38), S(-50, -11), S(-25, 4), S(-13, 2), S(-33, 3), S(-29, -14), S(-78, -10)};

int KNIGHT_OUTPOST[64] = {S(266, -119), S(110, 51), S(78, -8), S(567, -146), S(81, -2), S(300, 24), S(81, -72), S(66, 157),
                          S(22, 23), S(16, 10), S(16, 24), S(22, -5), S(82, -18), S(17, 24), S(-3, 6), S(32, -6),
                          S(43, 34), S(21, 48), S(54, 26), S(49, 27), S(45, 37), S(43, 44), S(47, 51), S(60, 28),
                          S(10, 20), S(25, 20), S(34, 29), S(32, 41), S(51, 40), S(45, 35), S(38, 32), S(4, 27),
                          S(19, 15), S(20, 14), S(34, 18), S(45, 19), S(52, 24), S(41, 16), S(25, 26), S(24, 8),
                          S(-2, 18), S(3, 4), S(0, 9), S(-3, 10), S(9, 0), S(-6, 9), S(0, 4), S(-11, 0),
                          S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                          S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int KNIGHT_OUTPOST_HOLE[64] = {S(-5, 25), S(99, 1), S(39, 17), S(75, 3), S(-6, 29), S(57, 8), S(-142, 84), S(-54, 106),
                               S(8, 18), S(-2, 20), S(-3, 8), S(-4, 16), S(-11, 23), S(6, 10), S(27, 15), S(-11, 19),
                               S(11, 15), S(3, 6), S(0, 4), S(2, 8), S(-5, 6), S(-10, 13), S(-12, 11), S(-3, 7),
                               S(-9, 21), S(0, 6), S(-1, 4), S(-22, 17), S(-5, 4), S(-5, 8), S(-13, 7), S(-26, 16),
                               S(8, 15), S(5, 2), S(-14, 7), S(-3, 0), S(-16, 3), S(2, 1), S(-10, 10), S(5, 10),
                               S(3, -8), S(1, 6), S(4, -4), S(9, -1), S(3, -2), S(10, -5), S(2, 4), S(-2, -2),
                               S(5, 15), S(0, 14), S(5, 0), S(8, 0), S(9, 4), S(10, -2), S(-20, 16), S(5, 2),
                               S(22, -3), S(2, 30), S(16, 21), S(17, 17), S(14, 12), S(29, 7), S(4, 17), S(14, 4)};

int knightMobilityBonus[9] = {S(-42, -155), S(-20, -64), S(-6, -19), S(2, 5), S(9, 21), S(16, 37), S(23, 41), S(32, 38), S(41, 24)};
int knightWeight[9] = {S(-151, -12), S(-22, -15), S(-12, -20), S(-21, -12), S(-18, -4), S(-12, 7), S(-9, 29), S(-6, 60), S(-14, 102)};
int knightThreatPiece[5] = {S(-2, 13), S(0, 0), S(31, 48), S(68, 20), S(55, 15)};
int knightPair = S(0, -1);


// Bishop Eval
int BISHOP_TABLE[64] = {S(-61, 41), S(-87, 41), S(-117, 44), S(-153, 54), S(-134, 44), S(-150, 36), S(-80, 33), S(-104, 39),
                        S(-48, 26), S(-32, 33), S(-19, 25), S(-41, 31), S(-43, 28), S(-32, 29), S(-67, 41), S(-93, 34),
                        S(1, 21), S(4, 28), S(18, 29), S(13, 19), S(20, 27), S(30, 37), S(-8, 31), S(14, 22),
                        S(-13, 23), S(17, 23), S(5, 29), S(28, 34), S(25, 38), S(18, 29), S(19, 27), S(-8, 25),
                        S(-1, 9), S(7, 13), S(17, 29), S(29, 30), S(25, 25), S(19, 26), S(20, 17), S(15, 4),
                        S(3, 6), S(33, 12), S(18, 20), S(21, 22), S(25, 27), S(26, 20), S(37, 13), S(26, 10),
                        S(20, 1), S(20, -8), S(28, -3), S(6, 14), S(13, 16), S(33, 1), S(47, 1), S(32, -20),
                        S(24, -13), S(27, 0), S(7, 3), S(-4, 14), S(9, 9), S(9, 20), S(15, -1), S(35, -13)};

int BISHOP_OUTPOST[64] = {S(306, -192), S(139, -1), S(210, -49), S(375, -96), S(297, -71), S(328, -62), S(401, -101), S(-429, -16),
                          S(-4, 39), S(87, -11), S(89, -15), S(84, -24), S(161, -40), S(106, -7), S(143, -16), S(0, 15),
                          S(-19, 34), S(46, 7), S(33, 5), S(57, 5), S(67, -10), S(83, -3), S(71, 7), S(-55, 10),
                          S(19, 1), S(38, 12), S(39, 4), S(59, 7), S(65, -3), S(62, 1), S(36, 15), S(1, 9),
                          S(0, 30), S(44, 19), S(56, 10), S(57, 9), S(78, 10), S(34, 16), S(52, 10), S(6, 9),
                          S(19, 31), S(8, 29), S(21, 22), S(13, 35), S(15, 13), S(0, 10), S(1, 19), S(-8, 27),
                          S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                          S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int bishopMobilityBonus[14] = {S(-28, -102), S(-4, -66), S(7, -28), S(16, -6), S(26, 11), S(33, 27), S(36, 36), S(38, 42), S(42, 50), S(48, 49), S(57, 45), S(79, 35), S(83, 54), S(127, 15)};
int bishopWeight[9] = {S(76, -7), S(45, 66), S(14, 89), S(16, 88), S(25, 84), S(24, 89), S(21, 99), S(17, 120), S(7, 151)};
int bishopThreatPiece[5] = {S(0, 10), S(29, 48), S(0, 0), S(62, 25), S(57, 53)};


// Rook Eval
int ROOK_TABLE[64] = {S(14, 87), S(14, 90), S(-6, 98), S(-4, 94), S(2, 87), S(25, 86), S(37, 81), S(46, 83),
                      S(3, 76), S(3, 80), S(34, 75), S(53, 71), S(47, 71), S(58, 55), S(31, 62), S(48, 57),
                      S(-4, 67), S(64, 43), S(41, 55), S(64, 40), S(84, 33), S(73, 44), S(98, 31), S(34, 46),
                      S(4, 57), S(31, 50), S(40, 54), S(65, 44), S(64, 37), S(66, 36), S(52, 38), S(32, 43),
                      S(-12, 50), S(-4, 60), S(0, 60), S(12, 53), S(16, 47), S(14, 50), S(25, 44), S(0, 41),
                      S(-22, 39), S(-2, 42), S(-8, 44), S(4, 38), S(10, 36), S(8, 35), S(27, 27), S(-3, 26),
                      S(-35, 35), S(-6, 29), S(-3, 35), S(5, 30), S(6, 28), S(18, 20), S(29, 14), S(-26, 27),
                      S(-5, 30), S(0, 34), S(5, 37), S(14, 31), S(20, 25), S(14, 37), S(9, 33), S(-2, 24)};

int rookMobilityBonus[15] = {S(-100, -123), S(-33, -36), S(-15, 3), S(-14, 25), S(-13, 35), S(-12, 49), S(-7, 56), S(0, 57), S(7, 63), S(11, 67), S(13, 72), S(20, 75), S(28, 75), S(45, 62), S(103, 32)};
int rookWeight[9] = {S(89, 98), S(55, 79), S(39, 76), S(34, 77), S(30, 79), S(30, 83), S(33, 86), S(38, 90), S(48, 76)};
int rookThreatPiece[5] = {S(-11, 16), S(2, 23), S(8, 24), S(0, 0), S(44, 29)};
int rookPair = S(-23, -5);
int rookOnOpen = S(19, 6);
int rookOnSemiOpen = S(15, 1);
int rookOnQueen = S(-11, 0);
int rookBehindPasser = S(8, 9);
int trappedRook = S(4, -38);


// Queen Eval
int QUEEN_TABLE[64] = {S(-52, 75), S(-43, 102), S(-39, 117), S(-25, 131), S(-15, 134), S(-17, 145), S(-7, 112), S(-21, 111),
                       S(-22, 67), S(-68, 111), S(-17, 106), S(-51, 149), S(-39, 184), S(0, 148), S(-36, 122), S(6, 110),
                       S(-25, 61), S(-11, 61), S(-12, 94), S(7, 107), S(25, 137), S(26, 148), S(38, 115), S(14, 110),
                       S(-15, 49), S(0, 77), S(0, 70), S(-6, 118), S(-7, 144), S(12, 138), S(26, 139), S(17, 94),
                       S(3, 18), S(5, 62), S(2, 65), S(-5, 108), S(-6, 103), S(14, 95), S(15, 77), S(22, 65),
                       S(2, -4), S(7, 33), S(3, 56), S(-1, 45), S(1, 51), S(10, 52), S(21, 27), S(10, 3),
                       S(0, -15), S(3, 0), S(9, -12), S(7, 18), S(7, 14), S(22, -36), S(29, -57), S(11, -31),
                       S(5, -27), S(-13, -7), S(-9, -9), S(-3, 0), S(0, 0), S(-7, -13), S(-8, -33), S(0, -15)};

int queenMobilityBonus[28] = {S(-67, -179), S(-28, -212), S(-16, -68), S(-8, -28), S(-5, 23), S(-1, 44), S(1, 58), S(2, 80),
                              S(7, 91), S(10, 100), S(13, 107), S(15, 112), S(16, 115), S(16, 122), S(17, 121), S(16, 128),
                              S(12, 134), S(12, 131), S(9, 137), S(20, 127), S(15, 130), S(30, 123), S(23, 124), S(24, 127),
                              S(-2, 143), S(76, 92), S(-65, 199), S(17, 117)};


// King Eval
int KING_TABLE[64] = {S(188, -121), S(153, -52), S(147, -33), S(85, -5), S(156, -35), S(102, -8), S(183, -23), S(297, -131),
                      S(-2, 2), S(108, 22), S(120, 14), S(144, 0), S(153, -4), S(154, 15), S(108, 32), S(106, 0),
                      S(19, 16), S(141, 18), S(140, 18), S(102, 14), S(138, 12), S(193, 16), S(142, 21), S(98, -4),
                      S(54, -3), S(59, 11), S(77, 22), S(9, 32), S(53, 22), S(87, 19), S(95, 2), S(14, -2),
                      S(50, -29), S(41, 0), S(52, 13), S(0, 29), S(31, 23), S(76, 6), S(100, -10), S(36, -29),
                      S(26, -25), S(-3, 0), S(1, 8), S(-27, 25), S(4, 18), S(29, 7), S(30, -4), S(51, -29),
                      S(40, -22), S(-27, 4), S(-31, 12), S(-74, 22), S(-44, 19), S(-28, 19), S(24, 4), S(61, -20),
                      S(-7, -45), S(-18, -4), S(-35, 1), S(-95, 7), S(-17, -19), S(-58, 14), S(19, 0), S(45, -49)};

int kingPawnFront = S(24, 2);
int kingPawnFrontN = S(6, -3);

int pawnShield[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                      S(69, 26), S(93, 0), S(37, 24), S(7, 11), S(-17, 15), S(-61, 17), S(0, 21), S(65, 9),
                      S(76, 4), S(70, 11), S(15, 15), S(-48, 22), S(-37, 12), S(-5, -6), S(9, 13), S(60, 7),
                      S(52, -5), S(1, 10), S(18, 2), S(1, 5), S(-13, 8), S(-8, -7), S(0, -3), S(17, -6),
                      S(43, -19), S(1, 2), S(-9, 2), S(12, 0), S(6, -1), S(-1, -9), S(-9, -10), S(25, -13),
                      S(50, -38), S(34, -20), S(-11, -1), S(5, 0), S(11, -2), S(-8, -1), S(22, -24), S(37, -30),
                      S(40, -60), S(33, -27), S(16, 1), S(17, 2), S(4, 0), S(12, 2), S(14, -24), S(30, -56),
                      S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int pawnStorm[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                     S(12, 19), S(23, -4), S(9, 0), S(0, -8), S(13, 0), S(-4, -2), S(1, 2), S(17, 15),
                     S(21, 8), S(33, -3), S(7, 0), S(9, -10), S(14, -4), S(-5, 2), S(16, 1), S(21, 3),
                     S(12, 27), S(11, 13), S(-6, 8), S(-7, -6), S(7, -10), S(-11, 9), S(3, 15), S(8, 14),
                     S(-14, 71), S(-11, 41), S(-24, 29), S(-30, 10), S(-7, 2), S(-24, 30), S(4, 33), S(2, 45),
                     S(0, 140), S(-53, 115), S(-105, 91), S(-34, 52), S(-54, 50), S(-59, 77), S(-51, 98), S(63, 92),
                     S(201, 111), S(149, 59), S(-83, 110), S(-6, 49), S(-80, 57), S(-39, 74), S(72, 93), S(268, 26),
                     S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};
                     
int pawnStormBlockade[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                             S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                             S(21, -36), S(-37, -9), S(-17, -11), S(57, -19), S(0, 26), S(-1, 0), S(41, -24), S(-17, -13),
                             S(17, -25), S(23, -30), S(-7, -22), S(8, -7), S(6, -3), S(3, -9), S(8, -17), S(32, -12),
                             S(32, -34), S(37, -41), S(23, -33), S(13, -17), S(13, -11), S(8, -21), S(14, -26), S(32, -25),
                             S(59, -74), S(47, -62), S(22, -44), S(17, -34), S(10, -28), S(17, -42), S(16, -47), S(50, -65),
                             S(-10, -176), S(18, -124), S(14, -113), S(22, -149), S(-3, -78), S(9, -89), S(38, -130), S(-34, -147),
                             S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};
int tempoBonus = S(22, 21);


// King safety
int pieceAttackValue[5] = {48, 62, 62, 60, 25};
int attacksSafety = 18;
int queenCheckVal = 75;
int rookCheckVal = 60;
int bishopCheckVal = 63;
int knightCheckVal = 94;
int noQueen = -185;
int KSOffset = 122;




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

    // Opposite color bishop
    if (check_bit(board.pieces[4]) && check_bit(board.pieces[5]) && check_bit(bishops & lightSquares) && minorAndMajors == bishops) {
        return 100;
    }

    // KR vs KRM
    if (pawns == 0 && queens == 0 && numMajorMinors == 3 && check_bit(board.pieces[6]) && check_bit(board.pieces[7])) {
        return 0;
    }

    // KR vs KB
    if (pawns == 0 && queens == 0 && numMajorMinors == 2 && check_bit(bishops) && check_bit(board.color[0] & minorAndMajors) ) {
        return 0;
    }

    // KR vs KN
    if (pawns == 0 && queens == 0 && numMajorMinors == 2 && check_bit(knights) && check_bit(board.color[0] & minorAndMajors) ) {
        return 100;
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
