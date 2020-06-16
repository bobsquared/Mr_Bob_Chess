#pragma once
#include <iostream>
#include "magic_bitboards.h"
#include "defs.h"


#define QUIET_MOVES_FLAG 0
#define DOUBLE_PAWN_PUSH_FLAG 1
#define KING_CASTLE_FLAG 2
#define QUEEN_CASTLE_FLAG 3
#define CAPTURES_NORMAL_FLAG 4
#define ENPASSANT_FLAG 5
#define KNIGHT_PROMOTION_FLAG 8
#define BISHOP_PROMOTION_FLAG 9
#define ROOK_PROMOTION_FLAG 10
#define QUEEN_PROMOTION_FLAG 11
#define KNIGHT_PROMOTION_CAPTURE_FLAG 12
#define BISHOP_PROMOTION_CAPTURE_FLAG 13
#define ROOK_PROMOTION_CAPTURE_FLAG 14
#define QUEEN_PROMOTION_CAPTURE_FLAG 15

#define PROMOTION_FLAG 8
#define CAPTURE_FLAG 4
#define MOVE_FLAGS 15

#define KING_CASTLE_WHITE_MASK ((1ULL << 6) | (1ULL << 5) | (1ULL << 4))
#define QUEEN_CASTLE_WHITE_MASK ((1ULL << 2) | (1ULL << 3) | (1ULL << 4))

#define KING_CASTLE_BLACK_MASK ((1ULL << 60) | (1ULL << 61) | (1ULL << 62))
#define QUEEN_CASTLE_BLACK_MASK ((1ULL << 60) | (1ULL << 59) | (1ULL << 58))

#define KING_CASTLE_OCCUPIED_WHITE_MASK ((1ULL << 6) | (1ULL << 5))
#define QUEEN_CASTLE_OCCUPIED_WHITE_MASK ((1ULL << 1) | (1ULL << 2) | (1ULL << 3))

#define KING_CASTLE_OCCUPIED_BLACK_MASK ((1ULL << 61) | (1ULL << 62))
#define QUEEN_CASTLE_OCCUPIED_BLACK_MASK ((1ULL << 57) | (1ULL << 58) | (1ULL << 59))

#define KING_CASTLE_RIGHTS_WHITE 8
#define QUEEN_CASTLE_RIGHTS_WHITE 4

#define KING_CASTLE_RIGHTS_BLACK 2
#define QUEEN_CASTLE_RIGHTS_BLACK 1

#define PROMOTION_MASK 11
#define QUEEN_PROMOTION_MASK 3
#define ROOK_PROMOTION_MASK 2
#define BISHOP_PROMOTION_MASK 1
#define KNIGHT_PROMOTION_MASK 0




class MoveGen {

public:

    MoveGen();
    void generate_all_moves(MoveList &moveList, uint64_t *pieces, uint64_t *color, uint64_t pawnAttacks[][2], uint64_t *knightMoves, Magics *magics, uint64_t *kingMoves, uint64_t occupied, int enpassantSq, uint8_t castleRights, bool col);
    void generate_captures_promotions(MoveList &moveList, uint64_t *pieces, uint64_t *color, uint64_t pawnAttacks[][2], uint64_t *knightMoves, Magics *magics, uint64_t *kingMoves, uint64_t occupied, int enpassantSq, uint8_t castleRights, bool col);

private:

    void create_move(MoveList &moveList, int from, int to, MOVE flags);
    void generate_pawn_moves(MoveList &moveList, uint64_t *pieces, uint64_t *color, uint64_t pawnAttacks[][2], uint64_t occupied, int enpassantSq, bool col, bool capPro);
    void generate_knight_moves(MoveList &moveList, uint64_t *pieces, uint64_t *color, uint64_t *knightMoves, uint64_t occupied, bool col, bool capPro);
    void generate_bishop_moves(MoveList &moveList, uint64_t *pieces, uint64_t *color, Magics *magics, uint64_t occupied, bool col, bool capPro);
    void generate_rook_moves(MoveList &moveList, uint64_t *pieces, uint64_t *color, Magics *magics, uint64_t occupied, bool col, bool capPro);
    void generate_queen_moves(MoveList &moveList, uint64_t *pieces, uint64_t *color, Magics *magics, uint64_t occupied, bool col, bool capPro);
    void generate_king_moves(MoveList &moveList, uint64_t *pieces, uint64_t *color, uint64_t *kingMoves, Magics *magics, uint64_t occupied, uint8_t castleRights, bool col, bool capPro);
    void create_all_promotions_captures(MoveList &moveList, int from, int to);
    void create_all_promotions(MoveList &moveList, int from, int to);

    bool can_castle_king(uint64_t *pieces, Magics *magics, uint64_t *knightMov, uint8_t castleRights, uint64_t occupied, bool col);
    bool can_castle_queen(uint64_t *pieces, Magics *magics, uint64_t *knightMov, uint8_t castleRights, uint64_t occupied, bool col);
    bool isAttackedCastleMask(uint64_t *pieces, Magics *magics, uint64_t *kingMoves, uint64_t bitboard, uint64_t occupied, bool col);


};
