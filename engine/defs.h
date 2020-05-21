
#pragma once
#include <unordered_map>
#include <string>


// Debug mode off:
#ifdef DEBUGFLAG
#define NDEBUG
#endif


#define MATE_VALUE 32500
#define INFINITY_VAL 1000000

#define QSEARCH_CHECKS_MAX 16

// Aspiration Window:
// Higher number = less cutoffs
// lower number  = more potential searching
#define ASPIRATION_WINDOW_DELTA 22

// Pruning:
// Higher number = more pruning
// Lower number  = more accurate
#define LMR_REDUCTION_PERCENT 100

// Evaluation -------------------------//
// ------------------------------------//

// King Safety ------------------------//

// King and piece distance:
// Higher number = less aggressive
// Lower number  = more aggressive
#define KING_PIECE_DISTANCE 256

// piece attack value:
// Higher number = more aggressive
// Lower number  = less aggressive
#define KING_KNIGHT_ATTACK 35 // Knight
#define KING_BISHOP_ATTACK 25 // bishop
#define KING_ROOK_ATTACK   55 // rook
#define KING_QUEEN_ATTACK  75 // queen


// King and piece distance:
// Higher number = more aggressive
// Lower number  = less aggressive
#define NUMBER_ATTACKS_WEIGHT 12

// Weight of king safety
#define KING_SAFETY_WEIGHT 1


// Mobility ---------------------------//

// Mobility weight:
// Higher number = plays more positionally
// Lower number  = plays less positionally
#define MOBILITY_WEIGHT 1

// Pawns ------------------------------//

// Pawn structure:
// Higher number = values pawn structure more
// Lower number  = values pawn structure less
#define SUPPORTED_PAWNS 2  // Supported pawns
#define ADJACENT_PAWNS  1  // Adjacent pawns
#define DOUBLED_PAWNS   12 // Doubled pawns
#define ISOLATED_PAWNS  9  // Isolated pawns
#define PASSED_PAWNS    27 // Passed pawns

// Weight of pawns
#define PAWNS_WEIGHT 1

// Threats ----------------------------//

// Pawn structure:
// Higher number = More threats to pieces with pawns
// Lower number  = Less threats to pieces with pawns
#define PAWN_THREAT 8

// Weight of threats
#define THREATS_WEIGHT 1


// Pawn Endgame -----------------------//

// King pawn distance:
// Higher number = King more likely to guard passed pawns and weak pawns
// Lower number  = King less likely to guard passed pawns and weak pawns
#define WEAK_PAWN_DISTANCE   3
#define PASSED_PAWN_DISTANCE 6

// Weight of pawn endgame
#define PAWN_ENDGAME_WEIGHT 1

// Outposts ---------------------------//

// Knight and bishop outposts:
// Higher number = Values outposts more
// Lower number  = Values outposts less
#define BISHOP_OUTPOST 12 // bishop
#define KNIGHT_OUTPOST 25 // knight

// Weight of outposts
#define OUTPOSTS_WEIGHT 1


// Imbalance --------------------------//

// Bishop Pair:
// Higher number = Values both bishops more
// Lower number  = Values both bishops less
#define BISHOP_PAIR 35 // bishop

// Knight Pair:
// Higher number = Values both knight less
// Lower number  = Values both knight more
#define KNIGHT_PAIR 16 // knight

// No Pawn Penalty:
// Higher number = Tries to hold on to one pawn more
// Lower number  = Tries to hold on to one pawn less
#define NO_PAWNS 50

// Weight of imbalance
#define IMBALANCE_WEIGHT 1



#define PAWNPHASE   0
#define KNIGHTPHASE 1
#define BISHOPPHASE 1
#define ROOKPHASE   2
#define QUEENPHASE  4

#define TOTALPHASE (PAWNPHASE * 16 + KNIGHTPHASE * 4 + BISHOPPHASE * 4 + ROOKPHASE * 4 + QUEENPHASE * 2)



// ------------------------------------//


// Number to algebra
const std::string TO_ALG[64] = {
  "a1","b1","c1","d1","e1","f1","g1","h1",
  "a2","b2","c2","d2","e2","f2","g2","h2",
  "a3","b3","c3","d3","e3","f3","g3","h3",
  "a4","b4","c4","d4","e4","f4","g4","h4",
  "a5","b5","c5","d5","e5","f5","g5","h5",
  "a6","b6","c6","d6","e6","f6","g6","h6",
  "a7","b7","c7","d7","e7","f7","g7","h7",
  "a8","b8","c8","d8","e8","f8","g8","h8"
};

// Algebra to number
extern std::unordered_map<std::string, uint8_t> TO_NUM;
