#include "eval.h"

extern Magics *magics;

// 556 - 861862
int pieceValues[6] = {S(85, 120), S(450, 475), S(450, 475), S(650, 700), S(1350, 1510), S(5000, 5000)};


// Pawns
int PAWN_TABLE[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                      S(115, 95), S(78, 102), S(86, 85), S(102, 57), S(78, 54), S(78, 75), S(24, 112), S(60, 99),
                      S(20, 94), S(12, 79), S(21, 44), S(39, -1), S(20, 1), S(73, 37), S(6, 74), S(29, 72),
                      S(31, 49), S(25, 26), S(5, 6), S(20, -25), S(31, -28), S(18, 3), S(31, 25), S(23, 30),
                      S(13, 15), S(1, 9), S(7, -19), S(-8, -37), S(-5, -36), S(2, -12), S(0, 8), S(-18, 3),
                      S(-2, 3), S(-8, -3), S(-17, -19), S(-25, -27), S(-21, -27), S(-18, -17), S(-9, -7), S(-28, -8),
                      S(0, 11), S(3, 1), S(-14, -4), S(-17, -16), S(-35, 4), S(14, -12), S(29, -14), S(-6, -7),
                      S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int supportedPawnWeight[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                               S(88, 52), S(137, 28), S(127, 71), S(175, 83), S(175, 92), S(196, 65), S(212, 59), S(108, 18),
                               S(42, 41), S(58, 56), S(74, 70), S(91, 86), S(99, 93), S(92, 71), S(78, 40), S(17, 44),
                               S(-3, 19), S(18, 22), S(31, 28), S(36, 22), S(54, 22), S(49, 16), S(20, 16), S(11, 6),
                               S(4, 1), S(17, 6), S(19, 9), S(22, 24), S(28, 17), S(15, 7), S(15, 1), S(17, -3),
                               S(18, 4), S(26, 10), S(35, 17), S(30, 22), S(26, 26), S(14, 14), S(26, 7), S(32, 2),
                               S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                               S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int adjacentPawnWeight[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                              S(5, 96), S(36, 144), S(138, 178), S(113, 128), S(81, 116), S(132, 186), S(37, 95), S(4, 23),
                              S(-17, 60), S(41, 119), S(57, 121), S(74, 127), S(121, 99), S(110, 101), S(108, 99), S(69, 50),
                              S(9, 30), S(8, 35), S(38, 40), S(33, 50), S(39, 49), S(59, 45), S(16, 31), S(22, 29),
                              S(-10, 0), S(20, 10), S(4, 15), S(17, 31), S(26, 24), S(13, 11), S(38, 0), S(-5, 1),
                              S(15, 5), S(9, -4), S(12, 11), S(12, 11), S(17, 10), S(15, 7), S(5, -1), S(15, -4),
                              S(13, -9), S(3, 6), S(14, -6), S(4, 26), S(23, 28), S(-4, -14), S(1, 12), S(1, -10),
                              S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int passedPawnWeight[7] = {S(0, 0), S(-46, -69), S(-46, -55), S(-30, -9), S(0, 31), S(65, 110), S(159, 148)};
int freePasser[7] = {S(0, 0), S(-6, -13), S(1, -7), S(1, -21), S(3, -35), S(-6, -76), S(-44, -107)};
int opposedPawnValue[8] = {S(-10, -24), S(19, -17), S(2, 8), S(4, 3), S(-3, 19), S(0, 7), S(40, -3), S(-9, -13)};

int doublePawnValue = S(-10, -30);
int isolatedPawnValue = S(-12, -11);
int backwardPawnValue = S(-4, -5);
int blockedPawns5th = S(-17, -23);
int blockedPawns6th = S(-2, -44);

int kingPawnDistFriendly[8] = {S(0, 0), S(30, 16), S(19, 16), S(4, 11), S(-11, 8), S(-23, 8), S(-29, 1), S(-19, -8)};
int kingPawnDistEnemy[8] = {S(0, 0), S(13, -38), S(38, 5), S(10, 8), S(5, 11), S(0, 16), S(0, 16), S(-12, 21)};
int kingPassedDistFriendly[8] = {S(0, 0), S(-6, 83), S(-6, 59), S(3, 39), S(10, 26), S(16, 18), S(22, 16), S(7, 10)};
int kingPassedDistEnemy[8] = {S(0, 0), S(-104, 5), S(-16, 7), S(-2, 33), S(9, 47), S(24, 54), S(27, 66), S(29, 69)};
int pawnThreat = S(76, 45);
int pawnPushThreat = S(16, 8);


// Knights
int KNIGHT_TABLE[64] = {S(-242, -46), S(-120, -16), S(-135, 19), S(-66, -4), S(-51, 0), S(-156, 12), S(-134, 0), S(-183, -64),
                        S(-57, -7), S(-40, -4), S(-5, -14), S(17, 7), S(4, 1), S(18, -26), S(-31, -5), S(-43, -12),
                        S(-52, -9), S(-19, -5), S(14, 15), S(15, 13), S(34, 2), S(48, 1), S(-7, -10), S(-24, -13),
                        S(-12, 1), S(11, 1), S(36, 13), S(49, 25), S(37, 24), S(65, 9), S(35, 4), S(37, -2),
                        S(-20, 1), S(3, 6), S(15, 24), S(26, 25), S(27, 28), S(29, 20), S(36, 2), S(2, 5),
                        S(-38, -26), S(-18, -9), S(-2, -5), S(9, 18), S(18, 17), S(6, -6), S(9, -12), S(-10, -11),
                        S(-52, -9), S(-40, 3), S(-26, -8), S(-12, 0), S(-7, 1), S(-5, -9), S(-5, 4), S(-19, 2),
                        S(-97, -21), S(-35, -30), S(-51, -10), S(-24, 7), S(-14, 2), S(-20, -3), S(-38, -12), S(-73, -18)};

int KNIGHT_OUTPOST[64] = {S(17, -20), S(67, 65), S(32, 1), S(76, 10), S(42, 24), S(45, 74), S(-3, -42), S(32, 90),
                          S(19, 16), S(20, 10), S(24, 23), S(19, -1), S(82, -12), S(16, 23), S(-11, 4), S(-9, 13),
                          S(48, 27), S(23, 44), S(55, 25), S(52, 26), S(53, 34), S(51, 40), S(50, 48), S(63, 29),
                          S(13, 13), S(26, 19), S(35, 27), S(31, 39), S(51, 37), S(43, 32), S(41, 27), S(11, 21),
                          S(18, 9), S(21, 10), S(35, 17), S(42, 18), S(52, 25), S(40, 15), S(26, 23), S(21, 5),
                          S(-3, 20), S(5, 5), S(1, 10), S(-4, 11), S(6, -1), S(-6, 10), S(-1, 3), S(-8, 0),
                          S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                          S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int KNIGHT_OUTPOST_JUMP[64] = {S(49, 5), S(26, -7), S(40, -6), S(31, -15), S(35, -8), S(71, -5), S(25, 5), S(-15, -27),
                               S(0, 8), S(6, 3), S(17, -7), S(7, 0), S(23, -5), S(12, -5), S(-7, 0), S(3, -15),
                               S(7, 0), S(10, 0), S(11, 3), S(8, 3), S(3, 2), S(14, 0), S(14, 2), S(0, 1),
                               S(7, 0), S(6, 2), S(7, 4), S(6, 6), S(5, 6), S(17, 3), S(6, 5), S(9, 2),
                               S(1, 2), S(-2, 0), S(1, 4), S(2, 3), S(0, 3), S(-1, 0), S(1, -2), S(3, 0),
                               S(-3, -2), S(-7, 3), S(0, -2), S(0, -3), S(2, -5), S(0, -7), S(-6, -3), S(0, -3),
                               S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                               S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int knightMobilityBonus[9] = {S(-51, -144), S(-27, -59), S(-12, -17), S(-3, 3), S(3, 18), S(11, 33), S(18, 37), S(26, 34), S(34, 22)};
int knightWeight[9] = {S(-94, 28), S(11, -21), S(-1, -18), S(-9, -11), S(-6, -4), S(-1, 5), S(2, 26), S(5, 56), S(0, 93)};
int knightThreatPiece[5] = {S(-3, 13), S(0, 0), S(33, 46), S(71, 20), S(60, 2)};
int knightPair = S(-1, -1);



// Bishops
int BISHOP_TABLE[64] = {S(-57, 30), S(-79, 31), S(-104, 30), S(-145, 41), S(-127, 31), S(-144, 22), S(-72, 21), S(-107, 31),
                        S(-51, 17), S(-31, 21), S(-21, 15), S(-43, 20), S(-45, 16), S(-32, 16), S(-73, 31), S(-98, 23),
                        S(-5, 14), S(2, 19), S(15, 18), S(7, 8), S(11, 16), S(21, 30), S(-11, 22), S(10, 15),
                        S(-19, 15), S(12, 16), S(5, 19), S(23, 25), S(20, 28), S(14, 21), S(13, 18), S(-14, 18),
                        S(-5, 2), S(0, 5), S(12, 19), S(26, 22), S(21, 16), S(15, 17), S(15, 9), S(12, -5),
                        S(-1, -3), S(30, 7), S(15, 12), S(17, 15), S(22, 20), S(24, 12), S(33, 5), S(22, 0),
                        S(16, -4), S(14, -16), S(26, -11), S(2, 7), S(9, 9), S(30, -3), S(44, -5), S(28, -28),
                        S(19, -21), S(26, -10), S(4, -2), S(-7, 6), S(7, 3), S(4, 13), S(13, -6), S(32, -22)};

int BISHOP_OUTPOST[64] = {S(24, -83), S(55, 31), S(38, 13), S(79, 13), S(40, 28), S(97, 43), S(52, 6), S(-103, -67),
                          S(15, 31), S(76, -1), S(69, 1), S(55, -6), S(130, -13), S(74, 13), S(111, 0), S(5, 30),
                          S(-15, 32), S(47, 6), S(35, 7), S(56, 9), S(65, -4), S(80, -3), S(58, 11), S(-56, 12),
                          S(17, 0), S(39, 10), S(41, 2), S(61, 6), S(65, -4), S(61, -1), S(40, 10), S(5, 7),
                          S(2, 20), S(47, 17), S(55, 11), S(56, 10), S(79, 9), S(31, 16), S(55, 4), S(2, 5),
                          S(27, 21), S(10, 28), S(24, 22), S(16, 35), S(18, 12), S(4, 8), S(5, 15), S(-5, 19),
                          S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                          S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int BISHOP_OUTPOST_JUMP[64] = {S(49, 5), S(26, -7), S(40, -6), S(31, -15), S(35, -8), S(71, -5), S(25, 5), S(-15, -27),
                               S(0, 8), S(6, 3), S(17, -7), S(7, 0), S(23, -5), S(12, -5), S(-7, 0), S(3, -15),
                               S(7, 0), S(10, 0), S(11, 3), S(8, 3), S(3, 2), S(14, 0), S(14, 2), S(0, 1),
                               S(7, 0), S(6, 2), S(7, 4), S(6, 6), S(5, 6), S(17, 3), S(6, 5), S(9, 2),
                               S(1, 2), S(-2, 0), S(1, 4), S(2, 3), S(0, 3), S(-1, 0), S(1, -2), S(3, 0),
                               S(-3, -2), S(-7, 3), S(0, -2), S(0, -3), S(2, -5), S(0, -7), S(-6, -3), S(0, -3),
                               S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                               S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int bishopMobilityBonus[14] = {S(-35, -93), S(-8, -60), S(6, -24), S(14, -3), S(25, 13), S(32, 30), S(36, 38), S(39, 44),
                               S(42, 51), S(47, 51), S(56, 48), S(76, 40), S(80, 58), S(114, 26)};
int bishopWeight[9] = {S(83, -3), S(54, 64), S(19, 88), S(20, 87), S(29, 81), S(26, 86), S(24, 93), S(21, 112), S(12, 139)};
int bishopThreatPiece[5] = {S(0, 12), S(29, 46), S(0, 0), S(64, 24), S(63, 55)};




// Rooks
int ROOK_TABLE[64] = {S(17, 61), S(11, 66), S(-3, 72), S(-6, 69), S(1, 62), S(23, 61), S(16, 62), S(35, 60),
                      S(5, 51), S(0, 57), S(30, 53), S(47, 48), S(37, 48), S(49, 34), S(26, 40), S(39, 36),
                      S(-3, 43), S(56, 23), S(34, 35), S(52, 20), S(71, 14), S(59, 24), S(82, 13), S(24, 26),
                      S(3, 35), S(23, 30), S(32, 33), S(52, 24), S(54, 17), S(55, 17), S(45, 18), S(22, 24),
                      S(-15, 30), S(-11, 39), S(-7, 39), S(5, 32), S(8, 28), S(5, 30), S(19, 22), S(-8, 21),
                      S(-26, 20), S(-10, 22), S(-15, 24), S(-4, 20), S(3, 17), S(2, 15), S(25, 3), S(-7, 6),
                      S(-37, 16), S(-15, 11), S(-10, 17), S(-2, 13), S(0, 9), S(11, 1), S(25, -6), S(-30, 8),
                      S(-12, 14), S(-7, 15), S(-3, 20), S(6, 13), S(13, 7), S(6, 18), S(4, 13), S(-9, 6)};

int rookMobilityBonus[15] = {S(-89, -111), S(-20, -27), S(-2, 7), S(-2, 28), S(0, 38), S(0, 50), S(5, 57), S(12, 59),
                             S(19, 64), S(23, 68), S(25, 73), S(32, 76), S(39, 76), S(54, 65), S(100, 42)};
int rookWeight[9] = {S(-94, 161), S(27, 69), S(26, 60), S(20, 60), S(13, 62), S(11, 64), S(12, 66), S(15, 71), S(22, 58)};
int rookThreatPiece[5] = {S(-12, 16), S(3, 23), S(9, 23), S(0, 0), S(50, 37)};
int rookPair = S(-8, -11);
int rookOnOpen = S(19, 7);
int rookOnSemiOpen = S(16, 1);
int rookOnQueen = S(-16, 27);
int rookBehindPasser = S(9, 10);
int trappedRook = S(6, -40);




// Queens
int QUEEN_TABLE[64] = {S(-41, 55), S(-21, 71), S(-16, 87), S(2, 95), S(10, 99), S(31, 96), S(4, 81), S(-21, 86),
                       S(-16, 52), S(-59, 94), S(-12, 92), S(-42, 130), S(-33, 163), S(7, 130), S(-25, 96), S(11, 91),
                       S(-17, 45), S(-5, 45), S(-4, 78), S(11, 90), S(24, 122), S(29, 129), S(40, 94), S(19, 87),
                       S(-8, 36), S(5, 60), S(5, 55), S(-2, 102), S(-3, 126), S(16, 115), S(30, 120), S(19, 75),
                       S(9, 3), S(7, 47), S(5, 50), S(-1, 93), S(-1, 86), S(18, 75), S(19, 60), S(25, 48),
                       S(8, -20), S(10, 18), S(6, 39), S(1, 30), S(6, 37), S(16, 35), S(27, 13), S(15, -12),
                       S(2, -27), S(6, -17), S(12, -25), S(12, 1), S(12, -1), S(26, -51), S(33, -67), S(19, -53),
                       S(7, -41), S(-10, -23), S(-6, -24), S(0, -14), S(4, -16), S(-2, -28), S(-4, -45), S(6, -34)};

int queenMobilityBonus[28] = {S(-77, -117), S(-41, -191), S(-15, -110), S(-6, -73), S(-3, -17), S(0, 5), S(3, 22), S(4, 45),
                              S(8, 56), S(10, 65), S(13, 72), S(14, 78), S(16, 82), S(16, 90), S(18, 89), S(16, 98),
                              S(14, 103), S(13, 101), S(12, 106), S(23, 96), S(21, 98), S(36, 90), S(38, 85), S(47, 82),
                              S(46, 87), S(71, 64), S(59, 90), S(58, 58)};





// Kings
int KING_TABLE[64] = {S(0, -84), S(74, -42), S(56, -25), S(23, 0), S(62, -18), S(32, 1), S(93, -6), S(73, -92),
                      S(-45, 4), S(65, 22), S(54, 21), S(83, 9), S(66, 12), S(86, 24), S(49, 40), S(21, 11),
                      S(-55, 21), S(86, 22), S(78, 24), S(52, 20), S(70, 23), S(118, 26), S(80, 28), S(23, 0),
                      S(-32, 2), S(29, 9), S(43, 19), S(-16, 30), S(16, 23), S(44, 23), S(54, 5), S(-55, 2),
                      S(-21, -25), S(21, -6), S(28, 9), S(-16, 24), S(11, 19), S(52, 3), S(73, -13), S(-49, -18),
                      S(-39, -23), S(0, -13), S(-7, 0), S(-33, 17), S(-4, 12), S(19, 0), S(22, -12), S(-23, -19),
                      S(0, -24), S(-20, -8), S(-29, 1), S(-73, 14), S(-48, 12), S(-28, 10), S(21, -4), S(12, -18),
                      S(-23, -51), S(-4, -19), S(-26, -10), S(-90, -1), S(-16, -25), S(-51, 4), S(21, -10), S(16, -51)};

int kingPawnFront = S(25, 1);
int kingPawnFrontN = S(6, -2);

int pawnShield[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                      S(41, 37), S(17, 24), S(28, 32), S(2, 14), S(0, 11), S(-28, 4), S(-3, 19), S(56, 16),
                      S(65, 9), S(52, 20), S(17, 17), S(-41, 20), S(-36, 12), S(8, -9), S(16, 10), S(46, 14),
                      S(45, 0), S(0, 14), S(19, 3), S(1, 5), S(-11, 9), S(-7, -6), S(-2, 0), S(16, -5),
                      S(35, -15), S(-4, 4), S(-10, 2), S(12, 0), S(7, -1), S(-2, -8), S(-13, -9), S(23, -12),
                      S(42, -34), S(29, -20), S(-12, -2), S(3, 0), S(12, -2), S(-9, -1), S(19, -23), S(37, -30),
                      S(31, -56), S(28, -28), S(14, -1), S(13, 3), S(1, 1), S(9, 1), S(9, -24), S(26, -52),
                      S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int pawnStorm[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                     S(12, 18), S(26, -5), S(9, 0), S(0, -10), S(12, 0), S(-4, -2), S(6, 0), S(18, 13),
                     S(20, 8), S(34, -3), S(7, -1), S(8, -9), S(14, -5), S(-5, 0), S(17, 0), S(21, 4),
                     S(10, 29), S(11, 15), S(-6, 8), S(-8, -6), S(8, -11), S(-12, 9), S(4, 14), S(8, 15),
                     S(-14, 71), S(-11, 43), S(-23, 32), S(-31, 13), S(-7, 3), S(-25, 30), S(4, 36), S(0, 48),
                     S(14, 131), S(-30, 113), S(-86, 90), S(-29, 53), S(-54, 53), S(-43, 76), S(-27, 93), S(52, 101),
                     S(149, 129), S(99, 86), S(-39, 96), S(11, 48), S(-85, 62), S(-26, 69), S(53, 108), S(201, 67),
                     S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int pawnStormBlockade[64] = {S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                             S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0),
                             S(30, -32), S(-9, -22), S(-13, -9), S(42, -4), S(4, 18), S(-8, 5), S(27, -16), S(-2, -19),
                             S(13, -26), S(23, -32), S(-6, -20), S(11, -8), S(7, -3), S(4, -7), S(8, -20), S(31, -13),
                             S(33, -35), S(38, -42), S(24, -35), S(13, -17), S(14, -11), S(9, -22), S(15, -28), S(34, -27),
                             S(57, -70), S(47, -63), S(21, -45), S(19, -36), S(10, -31), S(17, -43), S(17, -50), S(50, -67),
                             S(-29, -158), S(-12, -109), S(-22, -96), S(2, -128), S(-11, -75), S(-8, -87), S(8, -117), S(-27, -153),
                             S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0)};

int tempoBonus = S(25, 23);


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

        knightOutpostJump[0][i * 8 + j] = KNIGHT_OUTPOST_JUMP[(7 - i) * 8 + j];
        knightOutpostJump[1][i * 8 + j] = KNIGHT_OUTPOST_JUMP[j + 8 * i];

        bishopOutpostJump[0][i * 8 + j] = BISHOP_OUTPOST_JUMP[(7 - i) * 8 + j];
        bishopOutpostJump[1][i * 8 + j] = BISHOP_OUTPOST_JUMP[j + 8 * i];

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
    int ret = 256;

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

    if ((defending & pawns) == 0 && check_bit(defending)) {
        ret += 16 * (7 - chebyshevArray[board.kingLoc[0]][board.kingLoc[1]]);
        if (check_bit(board.pieces[4 + attackingColor])) {
            if (lightSquares & board.pieces[4 + attackingColor]) {
                int brDistance = manhattanArray[board.kingLoc[!attackingColor]][7];
                int tlDistance = manhattanArray[board.kingLoc[!attackingColor]][56];
                ret += 16 * (7 - std::min(brDistance, tlDistance));
            }
            if (~lightSquares & board.pieces[4 + attackingColor]) {
                int brDistance = manhattanArray[board.kingLoc[!attackingColor]][0];
                int tlDistance = manhattanArray[board.kingLoc[!attackingColor]][63];
                ret += 16 * (7 - std::min(brDistance, tlDistance));
            }
        }
    }
    

    return ret;

}



// Evaluate the position
int Eval::evaluate(Bitboard &board, KPNNUE *model, ThreadSearch *th) {

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

    int retm = (board.toMove? -model->evaluate(board) : model->evaluate(board))  + (board.toMove? -15 : 15);
    return retm;

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



// Evaluate the position for kings and pawns
int Eval::evaluateKP(Bitboard &board, ThreadSearch *th) {

    InitializeEval(board, th);

    int ret = 0;
    int pawnScore = 0;
    bool hit = false;

    ret += board.toMove? -tempoBonus : tempoBonus;
    ret += board.material[0] - board.material[1];


    ret += evaluatePawns(board, th, false, hit, pawnScore) - evaluatePawns(board, th, true, hit, pawnScore);
    ret += evaluateKingForKP(board, false) - evaluateKingForKP(board, true);

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
        uint64_t coveredPawn = col? ((th->pawnAttAll[!col] << 8) & board.pieces[col]) : ((th->pawnAttAll[!col] >> 8) & board.pieces[col]);
        uint64_t backwardPawns = coveredPawn & ~adjacentPawns;

        ret += doublePawnValue * count_population(doubledPawns);
        #ifdef TUNER
        evalTrace.doubledPawnsCoeff[col] = count_population(doubledPawns);
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
                evalTrace.isolatedPawnsCoeff[col]++;
                #endif
                ret += isolatedPawnValue;

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

        while (backwardPawns) {
            int bscan = bitScan(backwardPawns);

            if ((isolatedPawnMask[bscan] & passedPawnMask[!col][bscan] & board.pieces[col]) == 0) {
                #ifdef TUNER
                evalTrace.backwardPawnCoeff[col]++;
                #endif
                ret += backwardPawnValue;
            }

            backwardPawns &= backwardPawns - 1;
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
    uint64_t pawnAtk = th->pawnAttAll[col];
    uint64_t defendedKnight = piece & pawnAtk;

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
        int outpostJumps = board.knightMoves[bscan] & pawnAtk & ~board.color[col];
        while (outpostJumps) {
            int bscan2 = bitScan(outpostJumps);
            if ((outpostMask[col][bscan2] & board.pieces[!col]) == 0) {
                ret += knightOutpostJump[col][bscan];

                #ifdef TUNER
                evalTrace.knightOutpostJumpCoeff[col? bscan2 : flipIndex64(bscan2)][col]++;
                #endif
            }

            outpostJumps &= outpostJumps - 1;
        }
        

        piece &= piece - 1;
    }

    return ret;

}



int Eval::evaluateBishops(Bitboard &board, ThreadSearch *th, bool col) {

    int ret = 0;
    uint64_t piece = board.pieces[4 + col];
    uint64_t pawnAtk = th->pawnAttAll[col];
    uint64_t defendedBishop = piece & pawnAtk;

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

        // Jump to outpost
        int outpostJumps = bishopAttacks & pawnAtk & ~board.color[col];
        while (outpostJumps) {
            int bscan2 = bitScan(outpostJumps);
            if ((outpostMask[col][bscan2] & board.pieces[!col]) == 0) {
                ret += bishopOutpostJump[col][bscan];

                #ifdef TUNER
                evalTrace.bishopOutpostJumpCoeff[col? bscan2 : flipIndex64(bscan2)][col]++;
                #endif
            }

            outpostJumps &= outpostJumps - 1;
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



int Eval::evaluateKingForKP(Bitboard &board, bool col) {
    int ret = 0;
    ret += pieceSquare[10 + col][board.kingLoc[col]];
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
