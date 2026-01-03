#include "movegen.h"
#include "magic_bitboards.h"


// Move is 16 bit:
// from location: 16 - 11 MSB
// to location: 10 - 5 MSB
// flags: 4 - 1 MSB


namespace MOVE_GEN {
// Create the move and store it in an array
    static inline void create_move(MoveList &moveList, int from, int to, MOVE flags) {
        assert(from >= 0 && from < 64);
        assert(to >= 0 && to < 64);

        flags |= from << 10;
        flags |= to << 4;

        moveList.append_move(flags);
    }



    // Create promotion moves and store it in an array
    static void create_all_promotions(MoveList &moveList, int from, int to) {
        create_move(moveList, from, to, KNIGHT_PROMOTION_FLAG);
        create_move(moveList, from, to, BISHOP_PROMOTION_FLAG);
        create_move(moveList, from, to, ROOK_PROMOTION_FLAG);
        create_move(moveList, from, to, QUEEN_PROMOTION_FLAG);
    }



    // Create capture-promotion moves and store it in an array
    static void create_all_promotions_captures(MoveList &moveList, int from, int to) {
        create_move(moveList, from, to, BISHOP_PROMOTION_CAPTURE_FLAG);
        create_move(moveList, from, to, KNIGHT_PROMOTION_CAPTURE_FLAG);
        create_move(moveList, from, to, ROOK_PROMOTION_CAPTURE_FLAG);
        create_move(moveList, from, to, QUEEN_PROMOTION_CAPTURE_FLAG);
    }



    static void generate_pawn_moves_quiet(MoveList &moveList, const BoardState &b) {
        const int normalPush = (b.toMove << 4) - 8;
        const int doublePush = (b.toMove << 5) - 16;
        uint64_t normalPawns = (b.toMove? (~b.occupied << 8) & ~rowMask[8] : (~b.occupied >> 8) & ~rowMask[48]) & b.pieces[b.toMove];
        uint64_t doublePushablePawns = normalPawns & (b.toMove? (~b.occupied << 16) & rowMask[48] : (~b.occupied >> 16) & rowMask[8]);

        // Pawn pushes
        while (normalPawns) {
            uint64_t loc = normalPawns & -normalPawns;
            int locIndex = bitScan(loc);
            create_move(moveList, locIndex, locIndex - normalPush, QUIET_MOVES_FLAG);
            normalPawns &= normalPawns - 1;
        }

        // double pushes
        while (doublePushablePawns) {
            uint64_t loc = doublePushablePawns & -doublePushablePawns;
            int locIndex = bitScan(loc);
            create_move(moveList, locIndex, locIndex - doublePush, DOUBLE_PAWN_PUSH_FLAG);
            doublePushablePawns &= doublePushablePawns - 1;
        }
    }



    static void generate_pawn_moves_noisy(MoveList &moveList, const BoardState &b) {
        uint64_t pawnAtts = pawnAttacksAll(b.color[!b.toMove], !b.toMove);
        uint64_t promotionPawns = rowMask[48 - b.toMove * 40] & (b.toMove? (~b.occupied << 8) : (~b.occupied >> 8)) & b.pieces[b.toMove];
        uint64_t enpassantPawns = b.enpassantSq? BITBOARD::pieceMoves.pawnAttacks[b.enpassantSq][!b.toMove] & b.pieces[b.toMove] : 0;
        uint64_t promotionCapturePawns = (rowMask[48 - b.toMove * 40] & b.pieces[b.toMove] & pawnAtts);
        uint64_t capturePawns = pawnAtts & b.pieces[b.toMove] & ~promotionCapturePawns;

        while (capturePawns) {

            uint64_t loc = capturePawns & -capturePawns;
            int locIndex = bitScan(loc);

            // Captures
            uint64_t captures = b.color[!b.toMove] & BITBOARD::pieceMoves.pawnAttacks[locIndex][b.toMove];
            while (captures) {
                create_move(moveList, locIndex, bitScan(captures), CAPTURES_NORMAL_FLAG);
                captures &= captures - 1;
            }

            capturePawns &= capturePawns - 1;

        }

        // capture promotions
        while (promotionCapturePawns) {
            int locIndex = bitScan(promotionCapturePawns);

            uint64_t captures = b.color[!b.toMove] & BITBOARD::pieceMoves.pawnAttacks[locIndex][b.toMove];
            while (captures) {
                create_all_promotions_captures(moveList, locIndex, bitScan(captures));
                captures &= captures - 1;
            }

            promotionCapturePawns &= promotionCapturePawns - 1;
        }

        // normal promotions
        while (promotionPawns) {
            int locIndex = bitScan(promotionPawns);
            create_all_promotions(moveList, locIndex, locIndex + (b.toMove? -8 : 8));
            promotionPawns &= promotionPawns - 1;
        }

        // enpassants
        while (enpassantPawns) {
            int locIndex = bitScan(enpassantPawns);
            create_move(moveList, locIndex, b.enpassantSq, ENPASSANT_FLAG);
            enpassantPawns &= enpassantPawns - 1;
        }
    }



    static void generate_knight_moves_quiet(MoveList &moveList, const BoardState &b) {
        uint64_t bb = b.pieces[2 + b.toMove];
        while (bb) {

            int locIndex = bitScan(bb);
            uint64_t nonCaptures = (~b.occupied) & BITBOARD::pieceMoves.knightMoves[locIndex];
            while (nonCaptures) {
                create_move(moveList, locIndex, bitScan(nonCaptures), QUIET_MOVES_FLAG);
                nonCaptures &= nonCaptures - 1;
            }

            bb &= bb - 1;
        }
    }



    static void generate_knight_moves_noisy(MoveList &moveList, const BoardState &b) {
        uint64_t bb = b.pieces[2 + b.toMove];
        while (bb) {
            int locIndex = bitScan(bb);

            uint64_t captures = b.color[!b.toMove] & BITBOARD::pieceMoves.knightMoves[locIndex];
            while (captures) {
                create_move(moveList, locIndex, bitScan(captures), CAPTURES_NORMAL_FLAG);
                captures &= captures - 1;
            }

            bb &= bb - 1;
        }
    }



    static void generate_bishop_moves_quiet(MoveList &moveList, const BoardState &b) {
        uint64_t bb = b.pieces[4 + b.toMove];
        while (bb) {
            int locIndex = bitScan(bb);
            uint64_t bishopAtt = MAGIC_BITBOARDS::bishopAttacksMask(b.occupied, locIndex);

            uint64_t nonCaptures = (~b.occupied) & bishopAtt;
            while (nonCaptures) {
                create_move(moveList, locIndex, bitScan(nonCaptures), QUIET_MOVES_FLAG);
                nonCaptures &= nonCaptures - 1;
            }

            bb &= bb - 1;
        }
    }



    static void generate_bishop_moves_noisy(MoveList &moveList, const BoardState &b) {
        uint64_t bb = b.pieces[4 + b.toMove];
        while (bb) {
            int locIndex = bitScan(bb);
            uint64_t bishopAtt = MAGIC_BITBOARDS::bishopAttacksMask(b.occupied, locIndex);

            uint64_t captures = b.color[!b.toMove] & bishopAtt;
            while (captures) {
                create_move(moveList, locIndex, bitScan(captures), CAPTURES_NORMAL_FLAG);
                captures &= captures - 1;
            }

            bb &= bb - 1;
        }
    }



    static void generate_rook_moves_quiet(MoveList &moveList, const BoardState &b) {
        uint64_t bb = b.pieces[6 + b.toMove];
        while (bb) {
            int locIndex = bitScan(bb);
            uint64_t rookAtt = MAGIC_BITBOARDS::rookAttacksMask(b.occupied, locIndex);

            uint64_t nonCaptures = (~b.occupied) & rookAtt;
            while (nonCaptures) {
                create_move(moveList, locIndex, bitScan(nonCaptures), QUIET_MOVES_FLAG);
                nonCaptures &= nonCaptures - 1;
            }

            bb &= bb - 1;
        }
    }



    static void generate_rook_moves_noisy(MoveList &moveList, const BoardState &b) {
        uint64_t bb = b.pieces[6 + b.toMove];
        while (bb) {

            int locIndex = bitScan(bb);
            uint64_t rookAtt = MAGIC_BITBOARDS::rookAttacksMask(b.occupied, locIndex);

            uint64_t captures = b.color[!b.toMove] & rookAtt;
            while (captures) {
                create_move(moveList, locIndex, bitScan(captures), CAPTURES_NORMAL_FLAG);
                captures &= captures - 1;
            }

            bb &= bb - 1;

        }
    }



    static void generate_queen_moves_quiet(MoveList &moveList, const BoardState &b) {
        uint64_t bb = b.pieces[8 + b.toMove];
        while (bb) {
            int locIndex = bitScan(bb);
            uint64_t queenAtt = MAGIC_BITBOARDS::queenAttacksMask(b.occupied, locIndex);

            uint64_t nonCaptures = (~b.occupied) & queenAtt;
            while (nonCaptures) {
                create_move(moveList, locIndex, bitScan(nonCaptures), QUIET_MOVES_FLAG);
                nonCaptures &= nonCaptures - 1;
            }

            bb &= bb - 1;
        }
    }



    static void generate_queen_moves_noisy(MoveList &moveList, const BoardState &b) {
        uint64_t bb = b.pieces[8 + b.toMove];
        while (bb) {
            int locIndex = bitScan(bb);
            uint64_t queenAtt = MAGIC_BITBOARDS::queenAttacksMask(b.occupied, locIndex);

            uint64_t captures = b.color[!b.toMove] & queenAtt;
            while (captures) {
                create_move(moveList, locIndex, bitScan(captures), CAPTURES_NORMAL_FLAG);
                captures &= captures - 1;
            }

            bb &= bb - 1;
        }
    }



    static void generate_king_moves_quiet(MoveList &moveList, BoardState &b) {
        int locIndex = bitScan(b.pieces[10 + b.toMove]);
        assert(1ULL << locIndex == b.pieces[10 + b.toMove]);

        uint64_t nonCaptures = (~b.occupied) & BITBOARD::pieceMoves.kingMoves[locIndex];
        while (nonCaptures) {
            create_move(moveList, locIndex, bitScan(nonCaptures), QUIET_MOVES_FLAG);
            nonCaptures &= nonCaptures - 1;
        }

        if (BITBOARD::can_castle_king(b)) {
            create_move(moveList, locIndex, b.toMove? 62 : 6, KING_CASTLE_FLAG);
        }

        if (BITBOARD::can_castle_queen(b)) {
            create_move(moveList, locIndex, b.toMove? 58 : 2, QUEEN_CASTLE_FLAG);
        }
    }



    static void generate_king_moves_noisy(MoveList &moveList, const BoardState &b) {
        int locIndex = bitScan(b.pieces[10 + b.toMove]);
        assert(1ULL << locIndex == b.pieces[10 + b.toMove]);

        uint64_t captures = b.color[!b.toMove] & BITBOARD::pieceMoves.kingMoves[locIndex];
        while (captures) {
            create_move(moveList, locIndex, bitScan(captures), CAPTURES_NORMAL_FLAG);
            captures &= captures - 1;
        }
    }



    // Generate all pseudo-legal moves
    void generate_all_moves(MoveList &moveList, BoardState &b) {
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
    void generate_captures_promotions(MoveList &moveList, BoardState &b) {
        generate_pawn_moves_noisy(moveList, b);
        generate_knight_moves_noisy(moveList, b);
        generate_bishop_moves_noisy(moveList, b);
        generate_rook_moves_noisy(moveList, b);
        generate_queen_moves_noisy(moveList, b);
        generate_king_moves_noisy(moveList, b);
    }

}