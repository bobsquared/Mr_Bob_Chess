#pragma once
#include <iostream>
#include "magic_bitboards.h"
#include "defs.h"
#include "bitboard.h"



class MoveGen {

public:

    MoveGen();
    void generate_all_moves(MoveList &moveList, Bitboard &b);
    void generate_captures_promotions(MoveList &moveList, Bitboard &b);

private:

    void create_move(MoveList &moveList, int from, int to, MOVE flags);
    void generate_pawn_moves_quiet(MoveList &moveList, Bitboard &b);
    void generate_pawn_moves_noisy(MoveList &moveList, Bitboard &b);

    void generate_knight_moves(MoveList &moveList, Bitboard &b, bool capPro);
    void generate_bishop_moves(MoveList &moveList, Bitboard &b, bool capPro);
    void generate_rook_moves(MoveList &moveList, Bitboard &b, bool capPro);
    void generate_queen_moves(MoveList &moveList, Bitboard &b, bool capPro);
    void generate_king_moves(MoveList &moveList, Bitboard &b, bool capPro);
    void create_all_promotions_captures(MoveList &moveList, int from, int to);
    void create_all_promotions(MoveList &moveList, int from, int to);

    bool can_castle_king(Bitboard &b);
    bool can_castle_queen(Bitboard &b);
    bool isAttackedCastleMask(Bitboard &b, uint64_t attackedMask);


};
