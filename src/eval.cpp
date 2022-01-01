#include "eval.h"

extern Magics *magics;

// 863332
int pieceValues[6] = {S(85, 120), S(450, 475), S(450, 475), S(650, 700), S(1350, 1510), S(5000, 5000)};


// Pawn Eval
int PAWN_TABLE[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                      S(149, 123), S(110, 127), S(119, 114), S(139, 84), S(113, 84), S(118, 104), S(37, 144), S(107, 124),
                      S(29, 117), S(15, 102), S(31, 67), S(49, 23), S(31, 25), S(82, 61), S(1, 97), S(32, 98),
                      S(37, 52), S(26, 31), S(6, 11), S(23, -20), S(34, -22), S(23, 7), S(30, 31), S(23, 33),
                      S(18, 19), S(0, 15), S(9, -15), S(-8, -34), S(-3, -32), S(5, -8), S(-1, 13), S(-20, 8),
                      S(2, 6), S(-12, 1), S(-18, -14), S(-25, -22), S(-21, -22), S(-18, -10), S(-14, -1), S(-25, -3),
                      S(4, 14), S(0, 5), S(-15, 0), S(-16, -13), S(-34, 9), S(15, -7), S(26, -8), S(-3, -4),
                      S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int supportedPawnWeight[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                               S(131, 31), S(257, -16), S(180, 54), S(243, 65), S(259, 56), S(352, 10), S(354, 6), S(195, 8),
                               S(47, 40), S(60, 57), S(73, 73), S(91, 89), S(106, 88), S(94, 70), S(85, 36), S(24, 43),
                               S(-1, 18), S(18, 22), S(31, 29), S(37, 19), S(53, 21), S(48, 16), S(19, 15), S(12, 7),
                               S(5, 0), S(19, 5), S(19, 9), S(24, 24), S(28, 16), S(15, 5), S(15, 0), S(19, -4),
                               S(18, 0), S(28, 7), S(36, 16), S(32, 21), S(28, 25), S(16, 12), S(27, 6), S(29, 1),
                               S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                               S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int adjacentPawnWeight[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                              S(-175, 180), S(-87, 213), S(148, 232), S(119, 113), S(44, 147), S(44, 264), S(-104, 116), S(3, 74),
                              S(-54, 64), S(41, 130), S(38, 128), S(60, 131), S(126, 104), S(97, 111), S(94, 108), S(59, 57),
                              S(13, 28), S(6, 43), S(40, 37), S(33, 52), S(37, 48), S(56, 48), S(11, 32), S(25, 34),
                              S(-9, 0), S(22, 10), S(3, 17), S(17, 31), S(21, 27), S(12, 10), S(36, 1), S(-4, 0),
                              S(16, 3), S(10, -3), S(15, 9), S(12, 13), S(18, 9), S(17, 7), S(7, -1), S(14, -3),
                              S(12, -11), S(3, 6), S(16, -6), S(3, 27), S(20, 26), S(-1, -13), S(1, 10), S(0, -12),
                              S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int passedPawnWeight[7] = {S(0, 0), S(-66, -71), S(-67, -57), S(-52, -10), S(-22, 30), S(39, 90), S(125, 116)};
int freePasser[7] = {S(0, 0), S(-6, -12), S(0, -6), S(0, -21), S(2, -35), S(-6, -74), S(-48, -107)};
int opposedPawnValue[8] = {S(-11, -25), S(20, -19), S(4, 8), S(5, 4), S(-3, 17), S(4, 7), S(40, -2), S(-12, -10)};
int doublePawnValue = S(9, 30);
int isolatedPawnValue = S(12, 12);
int blockedPawns5th = S(-15, -24);
int blockedPawns6th = S(-5, -63);

int kingPawnDistFriendly[8] = {S(0, 0), S(26, 11), S(19, 10), S(5, 5), S(-9, 2), S(-20, 2), S(-30, -2), S(-28, -12)};
int kingPawnDistEnemy[8] = {S(0, 0), S(1, -31), S(29, 10), S(1, 15), S(-4, 18), S(-9, 24), S(-10, 24), S(0, 23)};
int kingPassedDistFriendly[8] = {S(0, 0), S(3, 80), S(3, 56), S(14, 35), S(22, 21), S(27, 12), S(36, 8), S(16, 1)};
int kingPassedDistEnemy[8] = {S(0, 0), S(-68, -3), S(-8, 13), S(7, 39), S(20, 54), S(36, 61), S(37, 74), S(31, 83)};
int pawnThreat = S(71, 48);
int pawnPushThreat = S(15, 8);


// Knight Eval
int KNIGHT_TABLE[64] = {S(-239, -41), S(-140, -5), S(-154, 26), S(-69, 2), S(-41, 1), S(-156, 22), S(-130, 3), S(-181, -63),
                        S(-46, -9), S(-26, -6), S(9, -13), S(40, 6), S(26, 0), S(34, -27), S(-24, -6), S(-20, -13),
                        S(-52, -12), S(-15, -2), S(24, 19), S(29, 13), S(56, 3), S(69, 0), S(16, -11), S(-10, -11),
                        S(5, -5), S(21, 0), S(47, 16), S(82, 16), S(53, 28), S(79, 8), S(59, 3), S(71, -9),
                        S(-14, -1), S(11, 9), S(41, 22), S(42, 31), S(56, 31), S(39, 27), S(58, 1), S(9, 7),
                        S(-29, -19), S(-8, -7), S(5, 1), S(14, 25), S(27, 25), S(9, 3), S(17, -7), S(0, -5),
                        S(-46, -9), S(-34, 3), S(-22, -4), S(-9, 4), S(-6, 4), S(-5, -4), S(21, 0), S(-12, 6),
                        S(-96, -18), S(-25, -37), S(-50, -11), S(-26, 4), S(-13, 3), S(-34, 3), S(-29, -13), S(-78, -13)};

int KNIGHT_OUTPOST[64] = {S(263, -113), S(107, 53), S(77, -8), S(570, -147), S(80, -1), S(296, 27), S(81, -73), S(65, 155),
                          S(22, 23), S(16, 9), S(16, 24), S(22, -4), S(82, -18), S(17, 24), S(-2, 6), S(33, -9),
                          S(42, 34), S(21, 48), S(53, 26), S(49, 27), S(45, 37), S(43, 45), S(47, 51), S(59, 28),
                          S(11, 20), S(25, 21), S(33, 29), S(32, 41), S(51, 39), S(45, 35), S(38, 31), S(4, 27),
                          S(19, 15), S(20, 14), S(34, 18), S(45, 19), S(52, 24), S(41, 16), S(25, 26), S(24, 8),
                          S(-2, 18), S(4, 5), S(0, 9), S(-3, 10), S(9, 0), S(-6, 9), S(0, 4), S(-12, -1),
                          S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                          S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int KNIGHT_OUTPOST_HOLE[64] = {S(-6, 27), S(97, 2), S(39, 17), S(75, 2), S(-6, 28), S(58, 6), S(-141, 85), S(-55, 108),
                               S(8, 18), S(-2, 20), S(-3, 8), S(-4, 16), S(-11, 24), S(6, 9), S(27, 16), S(-11, 19),
                               S(11, 15), S(3, 6), S(0, 4), S(1, 7), S(-5, 6), S(-10, 14), S(-11, 11), S(-3, 7),
                               S(-9, 21), S(0, 6), S(-1, 4), S(-22, 17), S(-5, 4), S(-4, 8), S(-13, 7), S(-25, 16),
                               S(7, 15), S(5, 3), S(-14, 8), S(-3, 0), S(-15, 3), S(2, 0), S(-10, 10), S(5, 10),
                               S(3, -8), S(2, 7), S(3, -4), S(9, -1), S(3, -2), S(10, -4), S(2, 3), S(-2, -3),
                               S(5, 16), S(0, 14), S(6, 0), S(8, 0), S(8, 3), S(10, -2), S(-20, 16), S(5, 3),
                               S(22, -2), S(2, 31), S(16, 22), S(17, 16), S(14, 13), S(29, 7), S(3, 16), S(13, 8)};

int knightMobilityBonus[9] = {S(-44, -153), S(-21, -62), S(-7, -18), S(1, 7), S(8, 23), S(15, 39), S(22, 43), S(31, 40), S(40, 27)};
int knightWeight[9] = {S(-127, 23), S(-21, -12), S(-16, -17), S(-24, -10), S(-21, -3), S(-14, 8), S(-10, 29), S(-8, 60), S(-16, 102)};
int knightThreatPiece[5] = {S(-2, 13), S(0, 0), S(31, 48), S(67, 20), S(55, 16)};
int knightPair = S(0, -1);


// Bishop Eval
int BISHOP_TABLE[64] = {S(-63, 42), S(-89, 43), S(-119, 45), S(-154, 56), S(-135, 46), S(-152, 38), S(-81, 35), S(-105, 43),
                        S(-50, 27), S(-34, 34), S(-20, 27), S(-43, 33), S(-44, 30), S(-34, 30), S(-69, 43), S(-94, 36),
                        S(0, 22), S(2, 29), S(16, 30), S(12, 20), S(18, 28), S(28, 38), S(-10, 32), S(13, 24),
                        S(-15, 24), S(16, 24), S(3, 30), S(26, 35), S(23, 39), S(16, 30), S(17, 28), S(-9, 27),
                        S(-3, 10), S(4, 14), S(15, 29), S(27, 31), S(24, 26), S(17, 27), S(19, 18), S(13, 5),
                        S(1, 7), S(31, 14), S(16, 21), S(20, 24), S(24, 28), S(25, 22), S(34, 14), S(24, 11),
                        S(18, 3), S(18, -7), S(27, -2), S(5, 15), S(11, 17), S(31, 3), S(46, 3), S(30, -19),
                        S(23, -11), S(25, 1), S(6, 5), S(-6, 15), S(8, 11), S(7, 20), S(14, 0), S(34, -11)};

int BISHOP_OUTPOST[64] = {S(316, -204), S(139, -1), S(213, -51), S(383, -102), S(296, -71), S(330, -66), S(403, -103), S(-406, -41),
                          S(-4, 39), S(86, -11), S(89, -15), S(85, -25), S(160, -39), S(107, -8), S(143, -17), S(0, 13),
                          S(-19, 33), S(46, 7), S(32, 4), S(57, 6), S(67, -11), S(83, -4), S(71, 6), S(-55, 11),
                          S(19, 1), S(38, 13), S(39, 3), S(59, 7), S(65, -2), S(62, 0), S(36, 15), S(1, 10),
                          S(0, 29), S(44, 18), S(55, 9), S(56, 9), S(77, 10), S(33, 16), S(52, 10), S(6, 9),
                          S(18, 31), S(8, 29), S(20, 22), S(13, 35), S(15, 13), S(0, 10), S(1, 18), S(-8, 26),
                          S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                          S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int bishopMobilityBonus[14] = {S(-30, -100), S(-6, -64), S(6, -25), S(14, -4), S(24, 12), S(31, 30), S(35, 39), S(37, 45), S(40, 52), S(46, 52), S(54, 48), S(76, 38), S(80, 59), S(126, 19)};
int bishopWeight[9] = {S(68, -5), S(45, 68), S(12, 91), S(15, 88), S(24, 83), S(23, 88), S(21, 98), S(17, 120), S(7, 151)};
int bishopThreatPiece[5] = {S(0, 10), S(29, 48), S(0, 0), S(61, 24), S(57, 53)};


// Rook Eval
int ROOK_TABLE[64] = {S(14, 90), S(13, 93), S(-6, 101), S(-4, 97), S(1, 90), S(24, 89), S(36, 84), S(45, 85),
                      S(2, 79), S(2, 83), S(34, 77), S(53, 73), S(46, 73), S(57, 58), S(31, 66), S(48, 59),
                      S(-5, 69), S(63, 46), S(41, 59), S(64, 43), S(83, 35), S(72, 47), S(98, 33), S(33, 49),
                      S(3, 60), S(30, 53), S(40, 56), S(64, 46), S(64, 40), S(64, 39), S(51, 41), S(32, 46),
                      S(-12, 53), S(-4, 63), S(0, 62), S(12, 55), S(16, 50), S(13, 53), S(24, 47), S(-2, 44),
                      S(-22, 42), S(-2, 45), S(-8, 46), S(4, 41), S(9, 38), S(8, 37), S(26, 29), S(-3, 29),
                      S(-35, 38), S(-7, 32), S(-3, 38), S(5, 32), S(6, 31), S(18, 22), S(29, 16), S(-27, 30),
                      S(-5, 32), S(0, 37), S(4, 40), S(14, 33), S(20, 28), S(13, 40), S(9, 35), S(-2, 26)};

int rookMobilityBonus[15] = {S(-99, -121), S(-32, -33), S(-16, 5), S(-15, 28), S(-13, 38), S(-13, 51), S(-7, 58), S(0, 60), S(6, 65), S(10, 69), S(13, 75), S(19, 78), S(27, 78), S(44, 65), S(101, 35)};
int rookWeight[9] = {S(13, 139), S(41, 82), S(33, 76), S(29, 76), S(26, 76), S(27, 79), S(31, 82), S(37, 85), S(49, 70)};
int rookThreatPiece[5] = {S(-11, 16), S(3, 22), S(8, 23), S(0, 0), S(44, 30)};
int rookPair = S(-24, -6);
int rookOnOpen = S(19, 6);
int rookOnSemiOpen = S(15, 2);
int rookOnQueen = S(-11, 2);
int rookBehindPasser = S(8, 9);
int trappedRook = S(3, -38);


// Queen Eval
int QUEEN_TABLE[64] = {S(-55, 76), S(-46, 103), S(-42, 117), S(-28, 132), S(-19, 135), S(-21, 146), S(-11, 114), S(-24, 113),
                       S(-25, 68), S(-71, 112), S(-20, 106), S(-54, 149), S(-42, 185), S(-3, 149), S(-39, 123), S(3, 111),
                       S(-27, 62), S(-13, 61), S(-14, 94), S(4, 107), S(22, 138), S(23, 149), S(35, 116), S(11, 111),
                       S(-17, 50), S(-2, 77), S(-2, 70), S(-8, 118), S(-9, 144), S(9, 139), S(23, 140), S(14, 95),
                       S(1, 19), S(2, 63), S(0, 65), S(-8, 108), S(-9, 103), S(11, 96), S(12, 77), S(19, 66),
                       S(0, -4), S(5, 33), S(0, 56), S(-4, 45), S(0, 51), S(8, 53), S(19, 27), S(7, 4),
                       S(-3, -14), S(0, 0), S(6, -11), S(5, 18), S(5, 14), S(19, -35), S(27, -56), S(8, -30),
                       S(3, -27), S(-15, -6), S(-12, -8), S(-5, 0), S(-1, 0), S(-8, -13), S(-10, -32), S(-1, -15)};

int queenMobilityBonus[28] = {S(-67, -175), S(-29, -209), S(-16, -65), S(-9, -25), S(-5, 25), S(-2, 47), S(0, 61), S(2, 82),
                              S(5, 94), S(9, 103), S(11, 110), S(14, 115), S(15, 118), S(15, 125), S(16, 125), S(14, 132),
                              S(11, 138), S(11, 135), S(7, 141), S(18, 131), S(13, 134), S(28, 127), S(21, 129), S(20, 132),
                              S(-7, 150), S(73, 97), S(-68, 205), S(27, 117)};



// King Eval
int KING_TABLE[64] = {S(188, -124), S(149, -54), S(143, -36), S(85, -7), S(155, -36), S(98, -10), S(176, -24), S(286, -135),
                      S(-2, 2), S(103, 22), S(116, 13), S(141, 0), S(149, -6), S(146, 13), S(102, 32), S(100, 0),
                      S(20, 15), S(138, 18), S(140, 16), S(100, 12), S(135, 10), S(186, 15), S(138, 20), S(93, -5),
                      S(54, -3), S(59, 10), S(78, 20), S(12, 30), S(54, 20), S(88, 19), S(93, 2), S(16, -3),
                      S(51, -30), S(41, 0), S(53, 12), S(1, 28), S(32, 22), S(76, 5), S(99, -11), S(37, -31),
                      S(26, -26), S(-3, -1), S(1, 7), S(-26, 24), S(5, 17), S(29, 6), S(30, -4), S(52, -30),
                      S(40, -22), S(-27, 4), S(-31, 12), S(-74, 22), S(-45, 20), S(-29, 19), S(24, 4), S(61, -21),
                      S(-7, -45), S(-18, -4), S(-36, 2), S(-96, 7), S(-18, -19), S(-59, 14), S(19, 0), S(46, -49)};

int kingPawnFront = S(24, 1);
int kingPawnFrontN = S(6, -3);

int pawnShield[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                      S(65, 29), S(91, 0), S(35, 26), S(7, 11), S(-18, 16), S(-62, 17), S(0, 24), S(63, 13),
                      S(75, 5), S(69, 12), S(16, 15), S(-48, 22), S(-36, 11), S(-5, -7), S(9, 14), S(62, 7),
                      S(51, -5), S(0, 10), S(18, 1), S(1, 4), S(-13, 8), S(-8, -7), S(0, -3), S(16, -6),
                      S(43, -19), S(0, 2), S(-9, 1), S(11, -1), S(6, -2), S(-1, -9), S(-9, -10), S(25, -13),
                      S(49, -38), S(33, -20), S(-11, -1), S(5, 0), S(10, -3), S(-8, -1), S(21, -24), S(37, -30),
                      S(39, -59), S(33, -27), S(16, 1), S(17, 1), S(4, 0), S(12, 2), S(13, -24), S(30, -55),
                      S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int pawnStorm[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                     S(12, 19), S(23, -4), S(9, 0), S(0, -8), S(13, 0), S(-4, -2), S(2, 2), S(16, 15),
                     S(21, 8), S(32, -2), S(7, 0), S(8, -10), S(14, -4), S(-4, 1), S(16, 1), S(20, 4),
                     S(11, 27), S(11, 13), S(-6, 8), S(-7, -6), S(7, -10), S(-10, 9), S(3, 15), S(7, 14),
                     S(-14, 71), S(-11, 41), S(-23, 29), S(-29, 9), S(-7, 2), S(-24, 30), S(4, 34), S(1, 47),
                     S(-1, 141), S(-54, 116), S(-105, 90), S(-34, 51), S(-54, 50), S(-58, 77), S(-51, 100), S(63, 95),
                     S(201, 110), S(150, 57), S(-83, 111), S(-5, 47), S(-81, 57), S(-42, 82), S(70, 96), S(266, 26),
                     S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int pawnStormBlockade[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                             S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                             S(21, -37), S(-38, -10), S(-18, -11), S(57, -18), S(0, 26), S(-1, 1), S(42, -25), S(-20, -13),
                             S(17, -24), S(22, -30), S(-7, -22), S(8, -7), S(6, -3), S(3, -9), S(8, -17), S(33, -12),
                             S(31, -34), S(38, -41), S(23, -33), S(13, -17), S(12, -11), S(9, -20), S(14, -26), S(32, -25),
                             S(58, -74), S(47, -63), S(22, -44), S(17, -33), S(10, -28), S(17, -42), S(16, -47), S(51, -66),
                             S(-8, -177), S(20, -125), S(15, -113), S(21, -150), S(-3, -77), S(8, -90), S(38, -132), S(-35, -148),
                             S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int tempoBonus = S(21, 22);


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

    if ((attacking & pawns) == 0) {
        // KR vs KRM
        if (queens == 0 && numMajorMinors == 3 && check_bit(board.pieces[6]) && check_bit(board.pieces[7])) {
            return 0;
        }

        // KR vs KB
        if (queens == 0 && numMajorMinors == 2 && check_bit(bishops) && check_bit(board.color[0] & minorAndMajors) ) {
            return 0;
        }

        // KR vs KN
        if (queens == 0 && numMajorMinors == 2 && check_bit(knights) && check_bit(board.color[0] & minorAndMajors) ) {
            return 100;
        }

        // K vs KNN
        if (numMajorMinors == 2 && minorAndMajors == board.pieces[2 + attackingColor]) {
            return 0;
        }

        // KB vs KNN or KN vs KNN
        if (numMajorMinors == 3 && check_bit(defending & minors) && (defending & majors) == 0 && (attacking & minorAndMajors) == board.pieces[2 + attackingColor]) {
            return 0;
        }
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
