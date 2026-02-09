#include "legality.h"

namespace BITBOARD {

    /************************************************************************************************
    **  Checks and legal moves section
    **  Used for determining whether a player is in check.
    *************************************************************************************************/

    // Determine if playr can castle
    bool isAttackedCastleMask(BoardState& bs, uint64_t bitboard) {

        uint64_t ret = 0;
        BITBOARD::PieceMoves& pm = BITBOARD::pieceMoves;

        ret = pm.kingMoves[bitScan(bs.pieces[10 + !bs.toMove])];
        ret |= knightAttacks(bs.pieces[2 + !bs.toMove]);
        ret |= pawnAttacksAll(bs.pieces[!bs.toMove], !bs.toMove);

        if (ret & bitboard) {
            return true;
        }

        uint64_t piece = bs.pieces[4 + !bs.toMove];
        while (piece) {
            if (MAGIC_BITBOARDS::bishopAttacksMask(bs.occupied, bitScan(piece)) & bitboard) {
                return true;
            }
            piece &= piece - 1;
        }

        piece = bs.pieces[6 + !bs.toMove];
        while (piece) {
            if (MAGIC_BITBOARDS::rookAttacksMask(bs.occupied, bitScan(piece)) & bitboard) {
                return true;
            }
            piece &= piece - 1;
        }

        piece = bs.pieces[8 + !bs.toMove];
        while (piece) {
            if (MAGIC_BITBOARDS::queenAttacksMask(bs.occupied, bitScan(piece)) & bitboard) {
                return true;
            }
            piece &= piece - 1;
        }

        return false;

    }


    // Determines whether the other player is in check.
    bool InCheckOther(BoardState& bs) {

        assert(bs.pieces[10 + !bs.toMove] != 0);
        BITBOARD::PieceMoves& pm = BITBOARD::pieceMoves;
        uint64_t ret = 0;
        int index = bs.kingLoc[!bs.toMove];

        ret = bs.pieces[bs.toMove] & pm.pawnAttacks[index][!bs.toMove];
        ret |= bs.pieces[2 + bs.toMove] & pm.knightMoves[index];
        ret |= bs.pieces[10 + bs.toMove] & pm.kingMoves[index];
        ret |= (bs.pieces[4 + bs.toMove] | bs.pieces[8 + bs.toMove]) & MAGIC_BITBOARDS::bishopAttacksMask(bs.occupied, index);
        ret |= (bs.pieces[6 + bs.toMove] | bs.pieces[8 + bs.toMove]) & MAGIC_BITBOARDS::rookAttacksMask(bs.occupied, index);

        return ret != 0;

    }



    // Determines whether the current player is in check.
    bool InCheck(BoardState& bs) {

        assert(bs.pieces[10 + bs.toMove] != 0);
        BITBOARD::PieceMoves& pm = BITBOARD::pieceMoves;
        uint64_t ret = 0;
        int index = bs.kingLoc[bs.toMove];

        ret = bs.pieces[!bs.toMove] & pm.pawnAttacks[index][bs.toMove];
        ret |= bs.pieces[2 + !bs.toMove] & pm.knightMoves[index];
        ret |= bs.pieces[10 + !bs.toMove] & pm.kingMoves[index];
        ret |= (bs.pieces[4 + !bs.toMove] | bs.pieces[8 + !bs.toMove]) & MAGIC_BITBOARDS::bishopAttacksMask(bs.occupied, index);
        ret |= (bs.pieces[6 + !bs.toMove] | bs.pieces[8 + !bs.toMove]) & MAGIC_BITBOARDS::rookAttacksMask(bs.occupied, index);

        return ret != 0;

    }



    // Determines if a move is legal
    bool isLegal(Board& b, MOVE move) {

        // Special cases
        if ((MOVE_FLAGS & move) == ENPASSANT_FLAG) {
            BITBOARD::make_move(b, move);
            bool legal = InCheckOther(b.state);
            BITBOARD::undo_move(b, move);
            return !legal;
        }

        // Move to and from square, and if its a capture
        int from = get_move_from(move);
        int to = get_move_to(move);
        int kingSide = 10 + b.state.toMove;
        uint64_t to64 = (1ULL << to);
        uint64_t from64 = (1ULL << from);
        uint64_t tofrom = to64 | from64;

        bool attacked = false;
        bool kingMove = b.state.pieceAt[from] / 2 == 5;

        // If the moving piece is a king
        b.state.pieces[kingSide] ^= kingMove * tofrom;

        if (kingMove) {
            b.state.kingLoc[b.state.toMove] = to;
        }

        // If it is a capture
        if (move & CAPTURE_FLAG) {
            b.state.occupied ^= from64;
            b.state.pieces[b.state.pieceAt[to]] ^= to64;
            attacked = InCheck(b.state);
            b.state.pieces[b.state.pieceAt[to]] ^= to64;
            b.state.occupied ^= from64;
        }
        else {
            b.state.occupied ^= tofrom;
            attacked = InCheck(b.state);
            b.state.occupied ^= tofrom;
        }

        // If the moving piece is a king
        b.state.pieces[kingSide] ^= kingMove * tofrom;

        if (kingMove) {
            b.state.kingLoc[b.state.toMove] = from;
        }

        return !attacked;
    }



    // Determines if a move is pseudo legal
    bool isPseudoLegal(BoardState& bs, MOVE move) {

        int from = get_move_from(move);
        int to = get_move_to(move);
        int piece = bs.pieceAt[from] >> 1;
        uint64_t to64 = 1ULL << to;
        uint64_t from64 = 1ULL << from;
        uint64_t valid = to64 & ~bs.color[bs.toMove] & ~bs.pieces[10 + !bs.toMove];
        int moveFlags = move & MOVE_FLAGS;

        if (!valid || move == NO_MOVE || move == NULL_MOVE || bs.pieceAt[from] == -1 || (bs.pieceAt[from] % 2) != bs.toMove || from == to) {
            return false;
        }

        if ((move & PROMOTION_FLAG) || (moveFlags == ENPASSANT_FLAG) || (moveFlags == DOUBLE_PAWN_PUSH_FLAG)) {
            if (piece != 0) {
                return false;
            }
        }   

        if (move & CAPTURE_FLAG) {
            if (!((bs.color[!bs.toMove] | (1ULL << bs.enpassantSq)) & to64)) {
                return false;
            }
        }

        if (moveFlags == KING_CASTLE_FLAG || moveFlags == QUEEN_CASTLE_FLAG) {
            if (piece != 5) {
                return false;
            }
        }

        if (moveFlags == ENPASSANT_FLAG) {
            if (bs.enpassantSq == 0 || to != bs.enpassantSq)
                return false;
        }

        
        switch (piece) {
            // PAWN
            case 0: {
                // En passant
                if (moveFlags == ENPASSANT_FLAG) {
                    uint64_t enpassantPawns = bs.enpassantSq? BITBOARD::pieceMoves.pawnAttacks[bs.enpassantSq][!bs.toMove] & bs.pieces[bs.toMove] : 0;
                    if ((enpassantPawns & from64) && to == bs.enpassantSq) {
                        return true;
                    }
                }

                if (move & CAPTURE_FLAG) {
                    uint64_t attacks = BITBOARD::pieceMoves.pawnAttacks[from][bs.toMove] & bs.color[!bs.toMove] & valid;
                    uint64_t pawnAtts = pawnAttacksAll(valid, !bs.toMove);
                    uint64_t promotionCapturePawns = (rowMask[48 - bs.toMove * 40] & bs.pieces[bs.toMove] & pawnAtts);
                    // Capture promotions
                    if (move & PROMOTION_FLAG) {
                        if (promotionCapturePawns & from64) {
                            return true;
                        }
                    }
                    // Regular captures
                    else if (attacks){
                        return true;
                    }
                    
                } else {
                    const int normalPush = (bs.toMove << 4) - 8;
                
                    // Quiet promotions
                    if (move & PROMOTION_FLAG) {
                        uint64_t promotionPawns = rowMask[48 - bs.toMove * 40] & (bs.toMove? (~bs.occupied << 8) : (~bs.occupied >> 8)) & bs.pieces[bs.toMove];
                        if ((promotionPawns & from64) && to == (from - normalPush) ) {
                            return true;
                        }
                    }
                    // Regular pushes
                    else if (isQuietMove(move)) {
                        const int doublePush = (bs.toMove << 5) - 16;
                        uint64_t normalPawns = (bs.toMove? (~bs.occupied << 8) & ~rowMask[8] : (~bs.occupied >> 8) & ~rowMask[48]) & bs.pieces[bs.toMove];
                        uint64_t doublePushablePawns = normalPawns & (bs.toMove? (~bs.occupied << 16) & rowMask[48] : (~bs.occupied >> 16) & rowMask[8]);

                        if (moveFlags == DOUBLE_PAWN_PUSH_FLAG && (doublePushablePawns & from64) && (to == (from - doublePush))) {
                            return true;
                        }

                        if ((normalPawns & from64) && (to == (from - normalPush)) && (~bs.occupied & valid)) {
                            return true;
                        }
                    }
                }

                break;
            }
            // KNIGHT
            case 1: {
                uint64_t knightMoves = BITBOARD::pieceMoves.knightMoves[from];
                if (knightMoves & valid) {
                    if (move & CAPTURE_FLAG) {
                        if (bs.color[!bs.toMove] & valid) {
                            return true;
                        }
                    }
                    else if (isQuietMove(move)){
                        if (~bs.occupied & valid) {
                            return true;
                        }
                    }
                }
                break;
            }
            // BISHOP
            case 2: {
                uint64_t bishopMoves = MAGIC_BITBOARDS::bishopAttacksMask(bs.occupied, from);
                if (bishopMoves & valid) {
                    if (move & CAPTURE_FLAG) {
                        if (bs.color[!bs.toMove] & valid) {
                            return true;
                        }
                    }
                    else if (isQuietMove(move)){
                        if (~bs.occupied & valid) {
                            return true;
                        }
                    }
                }
                break;
            }
            // ROOK
            case 3: {
                uint64_t rookMoves = MAGIC_BITBOARDS::rookAttacksMask(bs.occupied, from);
                if (rookMoves & valid) {
                    if (move & CAPTURE_FLAG) {
                        if (bs.color[!bs.toMove] & valid) {
                            return true;
                        }
                    }
                    else if (isQuietMove(move)){
                        if (~bs.occupied & valid) {
                            return true;
                        }
                    }
                }
                break;
            }
            // QUEEN
            case 4: {
                uint64_t queenMoves = MAGIC_BITBOARDS::queenAttacksMask(bs.occupied, from);
                if (queenMoves & valid) {
                    if (move & CAPTURE_FLAG) {
                        if (bs.color[!bs.toMove] & valid) {
                            return true;
                        }
                    }
                    else if (isQuietMove(move)){
                        if (~bs.occupied & valid) {
                            return true;
                        }
                    }
                }
                break;
            }
            // KING
            case 5: {
                uint64_t kingMoves = BITBOARD::pieceMoves.kingMoves[from];
                if (kingMoves & valid) {
                    if (move & CAPTURE_FLAG) {
                        if (bs.color[!bs.toMove] & valid) {
                            return true;
                        }
                    }
                    else if (isQuietMove(move)){
                        if (~bs.occupied & valid) {
                            return true;
                        }
                    }
                }

                if (moveFlags == KING_CASTLE_FLAG && BITBOARD::can_castle_king(bs)) {
                    if (to == (bs.toMove? 62 : 6)) {
                        return true;
                    }
                }

                if (moveFlags == QUEEN_CASTLE_FLAG && BITBOARD::can_castle_queen(bs)) {
                    if (to == (bs.toMove? 58 : 2)) {
                        return true;
                    }
                }
                break;
            }
        }

        return false;
    }



    // Determine if player can castle kingside
    bool can_castle_king(BoardState& bs) {

        if (!(bs.castleRights & (bs.toMove? KING_CASTLE_RIGHTS_BLACK : KING_CASTLE_RIGHTS_WHITE))) {
            return false;
        }

        if (bs.occupied & (bs.toMove? KING_CASTLE_OCCUPIED_BLACK_MASK : KING_CASTLE_OCCUPIED_WHITE_MASK)) {
            return false;
        }

        if (isAttackedCastleMask(bs, bs.toMove? KING_CASTLE_BLACK_MASK : KING_CASTLE_WHITE_MASK)) {
            return false;
        }

        return true;
    }



    // Determine if player can castle queenside
    bool can_castle_queen(BoardState& bs) {

        if (!(bs.castleRights & (bs.toMove? QUEEN_CASTLE_RIGHTS_BLACK : QUEEN_CASTLE_RIGHTS_WHITE))) {
            return false;
        }

        if (bs.occupied & (bs.toMove? QUEEN_CASTLE_OCCUPIED_BLACK_MASK : QUEEN_CASTLE_OCCUPIED_WHITE_MASK)) {
            return false;
        }

        if (isAttackedCastleMask(bs, bs.toMove? QUEEN_CASTLE_BLACK_MASK : QUEEN_CASTLE_WHITE_MASK)) {
            return false;
        }

        return true;
    }


}