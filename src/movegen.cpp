#include "movegen.h"

extern Magics *magics;



// Move is 16 bit:
// from location: 16 - 11 MSB
// to location: 10 - 5 MSB
// flags: 4 - 1 MSB
MoveGen::MoveGen() {}



// Create the move and store it in an array
void MoveGen::create_move(MoveList &moveList, int from, int to, MOVE flags) {
    assert(from >= 0 && from < 64);
    assert(to >= 0 && to < 64);

    flags |= from << 10;
    flags |= to << 4;

    moveList.append_move(Move(flags, 0));
}



// Create promotion moves and store it in an array
void MoveGen::create_all_promotions(MoveList &moveList, int from, int to) {
    create_move(moveList, from, to, KNIGHT_PROMOTION_FLAG);
    create_move(moveList, from, to, BISHOP_PROMOTION_FLAG);
    create_move(moveList, from, to, ROOK_PROMOTION_FLAG);
    create_move(moveList, from, to, QUEEN_PROMOTION_FLAG);
}



// Create capture-promotion moves and store it in an array
void MoveGen::create_all_promotions_captures(MoveList &moveList, int from, int to) {
    create_move(moveList, from, to, BISHOP_PROMOTION_CAPTURE_FLAG);
    create_move(moveList, from, to, KNIGHT_PROMOTION_CAPTURE_FLAG);
    create_move(moveList, from, to, ROOK_PROMOTION_CAPTURE_FLAG);
    create_move(moveList, from, to, QUEEN_PROMOTION_CAPTURE_FLAG);
}



void MoveGen::generate_pawn_moves_quiet(MoveList &moveList, Bitboard &b) {

    uint64_t bb = (b.toMove? (~b.occupied << 8) & ~rowMask[8] : (~b.occupied >> 8) & ~rowMask[48]) & b.pieces[b.toMove];

    while (bb) {

        uint64_t loc = bb & -bb;

        // Pawn pushes and normal promotions
        int locIndex = bitScan(loc);
        create_move(moveList, locIndex, locIndex + (b.toMove? -8 : 8), QUIET_MOVES_FLAG);

        // Consider double pawn pushes
        if ((rowMask[8 + b.toMove * 40] & loc) && !((b.toMove? (loc >> 16) : (loc << 16)) & b.occupied)) {
            create_move(moveList, locIndex, locIndex + (b.toMove? -16 : 16), DOUBLE_PAWN_PUSH_FLAG);
        }

        bb &= bb - 1;

    }
}



void MoveGen::generate_pawn_moves_noisy(MoveList &moveList, Bitboard &b) {


    uint64_t enpassantSq = b.enpassantSq? 1ULL << b.enpassantSq : 0;
    uint64_t bb = (rowMask[48 - b.toMove * 40] | pawnAttacksAll(b.color[!b.toMove] | enpassantSq, !b.toMove)) & b.pieces[b.toMove];

    while (bb) {

        uint64_t loc = bb & -bb;
        int locIndex = bitScan(loc);

        // Captures
        uint64_t captures = b.color[!b.toMove] & b.pawnAttacks[locIndex][b.toMove];
        while (captures) {

            if (!(rowMask[48 - b.toMove * 40] & loc)) {
                create_move(moveList, locIndex, bitScan(captures), CAPTURES_NORMAL_FLAG);
            }
            else {
                create_all_promotions_captures(moveList, locIndex, bitScan(captures));
            }
            captures &= captures - 1;
        }


        // Enpassants
        captures = enpassantSq & b.pawnAttacks[locIndex][b.toMove];
        if (captures) {
            create_move(moveList, locIndex, b.enpassantSq, ENPASSANT_FLAG);
        }


        // Pawn pushes and normal promotions
        if ((rowMask[48 - b.toMove * 40] & loc) && !((b.toMove? (loc >> 8) : (loc << 8)) & b.occupied)) {
            create_all_promotions(moveList, locIndex, locIndex + (b.toMove? -8 : 8));
        }

        bb &= bb - 1;

    }
}



void MoveGen::generate_knight_moves_quiet(MoveList &moveList, Bitboard &b) {

    uint64_t bb = b.pieces[2 + b.toMove];
    while (bb) {

        int locIndex = bitScan(bb);
        uint64_t nonCaptures = (~b.occupied) & b.knightMoves[locIndex];
        while (nonCaptures) {
            create_move(moveList, locIndex, bitScan(nonCaptures), QUIET_MOVES_FLAG);
            nonCaptures &= nonCaptures - 1;
        }

        bb &= bb - 1;

    }
}



void MoveGen::generate_knight_moves_noisy(MoveList &moveList, Bitboard &b) {

    uint64_t bb = b.pieces[2 + b.toMove];
    while (bb) {

        int locIndex = bitScan(bb);

        uint64_t captures = b.color[!b.toMove] & b.knightMoves[locIndex];
        while (captures) {
            create_move(moveList, locIndex, bitScan(captures), CAPTURES_NORMAL_FLAG);
            captures &= captures - 1;
        }

        bb &= bb - 1;

    }
}



void MoveGen::generate_bishop_moves_quiet(MoveList &moveList, Bitboard &b) {

    uint64_t bb = b.pieces[4 + b.toMove];
    while (bb) {

        int locIndex = bitScan(bb);
        uint64_t bishopAtt = magics->bishopAttacksMask(b.occupied, locIndex);

        uint64_t nonCaptures = (~b.occupied) & bishopAtt;
        while (nonCaptures) {
            create_move(moveList, locIndex, bitScan(nonCaptures), QUIET_MOVES_FLAG);
            nonCaptures &= nonCaptures - 1;
        }

        bb &= bb - 1;

    }
}



void MoveGen::generate_bishop_moves_noisy(MoveList &moveList, Bitboard &b) {

    uint64_t bb = b.pieces[4 + b.toMove];
    while (bb) {

        int locIndex = bitScan(bb);
        uint64_t bishopAtt = magics->bishopAttacksMask(b.occupied, locIndex);

        uint64_t captures = b.color[!b.toMove] & bishopAtt;
        while (captures) {
            create_move(moveList, locIndex, bitScan(captures), CAPTURES_NORMAL_FLAG);
            captures &= captures - 1;
        }

        bb &= bb - 1;

    }
}



void MoveGen::generate_rook_moves_quiet(MoveList &moveList, Bitboard &b) {

    uint64_t bb = b.pieces[6 + b.toMove];
    while (bb) {

        int locIndex = bitScan(bb);
        uint64_t rookAtt = magics->rookAttacksMask(b.occupied, locIndex);

        uint64_t nonCaptures = (~b.occupied) & rookAtt;
        while (nonCaptures) {
            create_move(moveList, locIndex, bitScan(nonCaptures), QUIET_MOVES_FLAG);
            nonCaptures &= nonCaptures - 1;
        }

        bb &= bb - 1;

    }
}



void MoveGen::generate_rook_moves_noisy(MoveList &moveList, Bitboard &b) {

    uint64_t bb = b.pieces[6 + b.toMove];
    while (bb) {

        int locIndex = bitScan(bb);
        uint64_t rookAtt = magics->rookAttacksMask(b.occupied, locIndex);

        uint64_t captures = b.color[!b.toMove] & rookAtt;
        while (captures) {
            create_move(moveList, locIndex, bitScan(captures), CAPTURES_NORMAL_FLAG);
            captures &= captures - 1;
        }

        bb &= bb - 1;

    }
}



void MoveGen::generate_queen_moves_quiet(MoveList &moveList, Bitboard &b) {

    uint64_t bb = b.pieces[8 + b.toMove];
    while (bb) {

        int locIndex = bitScan(bb);
        uint64_t queenAtt = magics->queenAttacksMask(b.occupied, locIndex);

        uint64_t nonCaptures = (~b.occupied) & queenAtt;
        while (nonCaptures) {
            create_move(moveList, locIndex, bitScan(nonCaptures), QUIET_MOVES_FLAG);
            nonCaptures &= nonCaptures - 1;
        }

        bb &= bb - 1;

    }
}



void MoveGen::generate_queen_moves_noisy(MoveList &moveList, Bitboard &b) {

    uint64_t bb = b.pieces[8 + b.toMove];
    while (bb) {

        int locIndex = bitScan(bb);
        uint64_t queenAtt = magics->queenAttacksMask(b.occupied, locIndex);

        uint64_t captures = b.color[!b.toMove] & queenAtt;
        while (captures) {
            create_move(moveList, locIndex, bitScan(captures), CAPTURES_NORMAL_FLAG);
            captures &= captures - 1;
        }

        bb &= bb - 1;

    }
}



void MoveGen::generate_king_moves_quiet(MoveList &moveList, Bitboard &b) {

    int locIndex = bitScan(b.pieces[10 + b.toMove]);
    assert(1ULL << locIndex == b.pieces[10 + b.toMove]);

    uint64_t nonCaptures = (~b.occupied) & b.kingMoves[locIndex];
    while (nonCaptures) {
        create_move(moveList, locIndex, bitScan(nonCaptures), QUIET_MOVES_FLAG);
        nonCaptures &= nonCaptures - 1;
    }

    if (b.can_castle_king()) {
        create_move(moveList, locIndex, b.toMove? 62 : 6, KING_CASTLE_FLAG);
    }

    if (b.can_castle_queen()) {
        create_move(moveList, locIndex, b.toMove? 58 : 2, QUEEN_CASTLE_FLAG);
    }

}



void MoveGen::generate_king_moves_noisy(MoveList &moveList, Bitboard &b) {

    int locIndex = bitScan(b.pieces[10 + b.toMove]);
    assert(1ULL << locIndex == b.pieces[10 + b.toMove]);

    uint64_t captures = b.color[!b.toMove] & b.kingMoves[locIndex];
    while (captures) {
        create_move(moveList, locIndex, bitScan(captures), CAPTURES_NORMAL_FLAG);
        captures &= captures - 1;
    }

}



// Generate all pseudo-legal moves
void MoveGen::generate_all_moves(MoveList &moveList, Bitboard &b) {

    generate_pawn_moves_quiet(moveList, b);
    generate_pawn_moves_noisy(moveList, b);

    generate_knight_moves_quiet(moveList, b);
    generate_knight_moves_noisy(moveList, b);

    generate_bishop_moves_quiet(moveList, b);
    generate_bishop_moves_noisy(moveList, b);

    generate_rook_moves_quiet(moveList, b);
    generate_rook_moves_noisy(moveList, b);

    generate_queen_moves_quiet(moveList, b);
    generate_queen_moves_noisy(moveList, b);

    generate_king_moves_quiet(moveList, b);
    generate_king_moves_noisy(moveList, b);
}



// Generate all pseudo-legal captures
void MoveGen::generate_captures_promotions(MoveList &moveList, Bitboard &b) {

    generate_pawn_moves_noisy(moveList, b);
    generate_knight_moves_noisy(moveList, b);
    generate_bishop_moves_noisy(moveList, b);
    generate_rook_moves_noisy(moveList, b);
    generate_queen_moves_noisy(moveList, b);
    generate_king_moves_noisy(moveList, b);

}
