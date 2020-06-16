#include "movegen.h"



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



void MoveGen::generate_pawn_moves(MoveList &moveList, uint64_t *pieces, uint64_t *color, uint64_t pawnAttacks[][2], uint64_t occupied, int enSq, bool col, bool capPro) {

    uint64_t bb = pieces[0 + col];
    uint64_t enpassantSq = enSq? 1ULL << enSq : 0;

    int nPush = 8;
    int dPush = 16;
    uint64_t promotionRank = rowMask[48];
    uint64_t startRank = rowMask[8];

    if (col) {
        nPush = -8;
        dPush = -16;
        promotionRank = rowMask[8];
        startRank = rowMask[48];
    }


    while (bb) {

        uint64_t loc = bb & -bb;
        int locIndex = bitScan(loc);
        bool isPromotion = promotionRank & loc;
        bool canDoubleMove = startRank & loc;


        // Captures
        uint64_t captures = color[!col] & pawnAttacks[locIndex][col];
        while (captures) {

            if (!isPromotion) {
                create_move(moveList, locIndex, bitScan(captures), CAPTURES_NORMAL_FLAG);
            }
            else {
                create_all_promotions_captures(moveList, locIndex, bitScan(captures));
            }
            captures &= captures - 1;
        }


        // Enpassants
        captures = enpassantSq & pawnAttacks[locIndex][col];
        if (captures) {
            create_move(moveList, locIndex, bitScan(captures), ENPASSANT_FLAG);
        }


        // Pawn pushes and normal promotions
        if (!((col? (loc >> 8) : (loc << 8)) & occupied)) {

            if (isPromotion) {
                create_all_promotions(moveList, locIndex, locIndex + nPush);
            }
            else if (!capPro) {
                create_move(moveList, locIndex, locIndex + nPush, QUIET_MOVES_FLAG);

                // Consider double pawn pushes
                if (canDoubleMove && !((col? (loc >> 16) : (loc << 16)) & occupied)) {
                    create_move(moveList, locIndex, locIndex + dPush, DOUBLE_PAWN_PUSH_FLAG);
                }
            }

        }

        bb &= bb - 1;

    }
}



void MoveGen::generate_knight_moves(MoveList &moveList, uint64_t *pieces, uint64_t *color, uint64_t *knightMoves, uint64_t occupied, bool col, bool capPro) {

    uint64_t bb = pieces[2 + col];
    while (bb) {

        int locIndex = bitScan(bb);

        uint64_t captures = color[!col] & knightMoves[locIndex];
        while (captures) {
            create_move(moveList, locIndex, bitScan(captures), CAPTURES_NORMAL_FLAG);
            captures &= captures - 1;
        }


        if (!capPro) {
            uint64_t nonCaptures = (~occupied) & knightMoves[locIndex];
            while (nonCaptures) {
                create_move(moveList, locIndex, bitScan(nonCaptures), QUIET_MOVES_FLAG);
                nonCaptures &= nonCaptures - 1;
            }
        }


        bb &= bb - 1;

    }
}



void MoveGen::generate_bishop_moves(MoveList &moveList, uint64_t *pieces, uint64_t *color, Magics *magics, uint64_t occupied, bool col, bool capPro) {

    uint64_t bb = pieces[4 + col];
    while (bb) {

        int locIndex = bitScan(bb);
        uint64_t bishopAtt = magics->bishopAttacksMask(occupied, locIndex);

        uint64_t captures = color[!col] & bishopAtt;
        while (captures) {
            create_move(moveList, locIndex, bitScan(captures), CAPTURES_NORMAL_FLAG);
            captures &= captures - 1;
        }

        if (!capPro) {
            uint64_t nonCaptures = (~occupied) & bishopAtt;
            while (nonCaptures) {
                create_move(moveList, locIndex, bitScan(nonCaptures), QUIET_MOVES_FLAG);
                nonCaptures &= nonCaptures - 1;
            }
        }

        bb &= bb - 1;

    }
}



void MoveGen::generate_rook_moves(MoveList &moveList, uint64_t *pieces, uint64_t *color, Magics *magics, uint64_t occupied, bool col, bool capPro) {

    uint64_t bb = pieces[6 + col];
    while (bb) {

        int locIndex = bitScan(bb);
        uint64_t rookAtt = magics->rookAttacksMask(occupied, locIndex);

        uint64_t captures = color[!col] & rookAtt;
        while (captures) {
            create_move(moveList, locIndex, bitScan(captures), CAPTURES_NORMAL_FLAG);
            captures &= captures - 1;
        }


        if (!capPro) {
            uint64_t nonCaptures = (~occupied) & rookAtt;
            while (nonCaptures) {
                create_move(moveList, locIndex, bitScan(nonCaptures), QUIET_MOVES_FLAG);
                nonCaptures &= nonCaptures - 1;
            }
        }

        bb &= bb - 1;

    }
}



void MoveGen::generate_queen_moves(MoveList &moveList, uint64_t *pieces, uint64_t *color, Magics *magics, uint64_t occupied, bool col, bool capPro) {

    uint64_t bb = pieces[8 + col];
    while (bb) {

        int locIndex = bitScan(bb);
        uint64_t queenAtt = magics->queenAttacksMask(occupied, locIndex);

        uint64_t captures = color[!col] & queenAtt;
        while (captures) {
            create_move(moveList, locIndex, bitScan(captures), CAPTURES_NORMAL_FLAG);
            captures &= captures - 1;
        }


        if (!capPro) {
            uint64_t nonCaptures = (~occupied) & queenAtt;
            while (nonCaptures) {
                create_move(moveList, locIndex, bitScan(nonCaptures), QUIET_MOVES_FLAG);
                nonCaptures &= nonCaptures - 1;
            }
        }

        bb &= bb - 1;

    }
}



void MoveGen::generate_king_moves(MoveList &moveList, uint64_t *pieces, uint64_t *color, uint64_t *kingMoves, Magics *magics, uint64_t occupied, uint8_t castleRights, bool col, bool capPro) {

    uint64_t bb = pieces[10 + col];
    int locIndex = bitScan(bb);

    assert(bb != 0);
    assert(1ULL << locIndex == bb);

    uint64_t captures = color[!col] & kingMoves[locIndex];
    while (captures) {
        create_move(moveList, locIndex, bitScan(captures), CAPTURES_NORMAL_FLAG);
        captures &= captures - 1;
    }

    if (!capPro) {
        uint64_t nonCaptures = (~occupied) & kingMoves[locIndex];
        while (nonCaptures) {
            create_move(moveList, locIndex, bitScan(nonCaptures), QUIET_MOVES_FLAG);
            nonCaptures &= nonCaptures - 1;
        }

        if (can_castle_king(pieces, magics, kingMoves, castleRights, occupied, col)) {
            create_move(moveList, locIndex, col? 62 : 6, KING_CASTLE_FLAG);
        }

        if (can_castle_queen(pieces, magics, kingMoves, castleRights, occupied, col)) {
            create_move(moveList, locIndex, col? 58 : 2, QUEEN_CASTLE_FLAG);
        }
    }
}



bool MoveGen::can_castle_king(uint64_t *pieces, Magics *magics, uint64_t *kingMoves, uint8_t castleRights, uint64_t occupied, bool col) {

    uint8_t crightsMask;
    uint64_t occupiedMask;
    uint64_t attackedMask;

    if (col) {
        crightsMask = KING_CASTLE_RIGHTS_BLACK;
        occupiedMask = KING_CASTLE_OCCUPIED_BLACK_MASK;
        attackedMask = KING_CASTLE_BLACK_MASK;
    }
    else {
        crightsMask = KING_CASTLE_RIGHTS_WHITE;
        occupiedMask = KING_CASTLE_OCCUPIED_WHITE_MASK;
        attackedMask = KING_CASTLE_WHITE_MASK;
    }

    if ((castleRights & crightsMask) == 0) {
        return false;
    }

    if (occupied & occupiedMask) {
        return false;
    }

    if (isAttackedCastleMask(pieces, magics, kingMoves, attackedMask, occupied, col)) {
        return false;
    }

    return true;
}



// Determine if player can castle queenside
bool MoveGen::can_castle_queen(uint64_t *pieces, Magics *magics, uint64_t *kingMoves, uint8_t castleRights, uint64_t occupied, bool col) {

    uint8_t crightsMask;
    uint64_t occupiedMask;
    uint64_t attackedMask;

    if (col) {
        crightsMask = QUEEN_CASTLE_RIGHTS_BLACK;
        occupiedMask = QUEEN_CASTLE_OCCUPIED_BLACK_MASK;
        attackedMask = QUEEN_CASTLE_BLACK_MASK;
    }
    else {
        crightsMask = QUEEN_CASTLE_RIGHTS_WHITE;
        occupiedMask = QUEEN_CASTLE_OCCUPIED_WHITE_MASK;
        attackedMask = QUEEN_CASTLE_WHITE_MASK;
    }

    if ((castleRights & crightsMask) == 0) {
        return false;
    }

    if (occupied & occupiedMask) {
        return false;
    }

    if (isAttackedCastleMask(pieces, magics, kingMoves, attackedMask, occupied, col)) {
        return false;
    }

    return true;
}



// Determine if player can castle kingside
bool MoveGen::isAttackedCastleMask(uint64_t *pieces, Magics *magics, uint64_t *kingMoves, uint64_t bitboard, uint64_t occupied, bool col) {

    uint64_t ret = 0;

    ret = kingMoves[bitScan(pieces[10 + !col])];
    ret |= knightAttacks(pieces[2 + !col]);
    ret |= pawnAttacksAll(pieces[!col], !col);

    if (ret & bitboard) {
        return true;
    }

    uint64_t piece = pieces[4 + !col];
    while (piece) {
        if (magics->bishopAttacksMask(occupied, bitScan(piece)) & bitboard) {
            return true;
        }
        piece &= piece - 1;
    }

    piece = pieces[6 + !col];
    while (piece) {
        if (magics->rookAttacksMask(occupied, bitScan(piece)) & bitboard) {
            return true;
        }
        piece &= piece - 1;
    }

    piece = pieces[8 + !col];
    while (piece) {
        if (magics->queenAttacksMask(occupied, bitScan(piece)) & bitboard) {
            return true;
        }
        piece &= piece - 1;
    }

    return false;

}



// Generate all pseudo-legal moves
void MoveGen::generate_all_moves(MoveList &moveList, uint64_t *pieces, uint64_t *color, uint64_t pawnAttacks[][2], uint64_t *knightMoves, Magics *magics, uint64_t *kingMoves, uint64_t occupied, int enpassantSq, uint8_t castleRights, bool col) {
    generate_pawn_moves(moveList, pieces, color, pawnAttacks, occupied, enpassantSq, col, false);
    generate_knight_moves(moveList, pieces, color, knightMoves, occupied, col, false);
    generate_bishop_moves(moveList, pieces, color, magics, occupied, col, false);
    generate_rook_moves(moveList, pieces, color, magics, occupied, col, false);
    generate_queen_moves(moveList, pieces, color, magics, occupied, col, false);
    generate_king_moves(moveList, pieces, color, kingMoves, magics, occupied, castleRights, col, false);
}



// Generate all pseudo-legal captures
void MoveGen::generate_captures_promotions(MoveList &moveList, uint64_t *pieces, uint64_t *color, uint64_t pawnAttacks[][2], uint64_t *knightMoves, Magics *magics, uint64_t *kingMoves, uint64_t occupied, int enpassantSq, uint8_t castleRights, bool col) {

    generate_pawn_moves(moveList, pieces, color, pawnAttacks, occupied, enpassantSq, col, true);
    generate_knight_moves(moveList, pieces, color, knightMoves, occupied, col, true);
    generate_bishop_moves(moveList, pieces, color, magics, occupied, col, true);
    generate_rook_moves(moveList, pieces, color, magics, occupied, col, true);
    generate_queen_moves(moveList, pieces, color, magics, occupied, col, true);
    generate_king_moves(moveList, pieces, color, kingMoves, magics, occupied, castleRights, col, true);

}
