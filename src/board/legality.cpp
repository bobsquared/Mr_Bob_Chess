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

        BITBOARD::PieceMoves& pm = BITBOARD::pieceMoves;
        int from = get_move_from(move);
        int to = get_move_to(move);
        int pieceMoved = bs.pieceAt[from];

        // who to move and is there piece
        if (pieceMoved == -1 || pieceMoved % 2 != bs.toMove || move == NULL_MOVE || move == NO_MOVE) {
            return false;
        }

        // Enpassant
        if ((MOVE_FLAGS & move) == ENPASSANT_FLAG && pieceMoved / 2 == 0 && (pm.pawnAttacks[from][bs.toMove] & (1ULL << bs.enpassantSq))) {
            return true;
        }

        if (move & CAPTURE_FLAG) {
            if (((1ULL << to) & bs.color[!bs.toMove]) == 0 || ((1ULL << to) & bs.color[bs.toMove])) {
                return false;
            }

            switch (pieceMoved / 2) {
                case 0:
                    return (pm.pawnAttacks[from][bs.toMove] & (1ULL << to)) != 0;
                case 1:
                    return (pm.knightMoves[from] & (1ULL << to)) != 0;
                case 2:
                    return (MAGIC_BITBOARDS::bishopAttacksMask(bs.occupied, from) & (1ULL << to)) != 0;
                case 3:
                    return (MAGIC_BITBOARDS::rookAttacksMask(bs.occupied, from) & (1ULL << to)) != 0;
                case 4:
                    return (MAGIC_BITBOARDS::queenAttacksMask(bs.occupied, from) & (1ULL << to)) != 0;
                case 5:
                    return (pm.kingMoves[from] & (1ULL << to)) != 0;
            }
        }

        if (pieceMoved / 2 == 0) {
            if (((bs.toMove? (1ULL << (from - 8)) : (1ULL << (from + 8))) & bs.occupied)) {
                return false;
            }

            if ((move & MOVE_FLAGS) == DOUBLE_PAWN_PUSH_FLAG) {
                if ((rowMask[8 + bs.toMove * 40] & (1ULL << from)) && ((bs.toMove? (1ULL << (from - 16)) : (1ULL << (from + 16))) & bs.occupied)) {
                    return false;
                }
            }

            return true;
        }



        if (pieceMoved / 2 == 5) {
            if ((MOVE_FLAGS & move) == KING_CASTLE_FLAG || (MOVE_FLAGS & move) == QUEEN_CASTLE_FLAG) {
                if (from != 4 && from != 60) {
                    return false;
                }

                if ((MOVE_FLAGS & move) == KING_CASTLE_FLAG && can_castle_king(bs)) {
                    return true;
                }

                if ((MOVE_FLAGS & move) == QUEEN_CASTLE_FLAG && can_castle_queen(bs)) {
                    return true;
                }
            }
        }


        if ((move & MOVE_FLAGS) == QUIET_MOVES_FLAG) {

            if (((1ULL << to) & bs.occupied) != 0) {
                return false;
            }

            assert (pieceMoved / 2 != 0);
            switch (pieceMoved / 2) {
                case 1:
                    return (pm.knightMoves[from] & (1ULL << to)) != 0;
                case 2:
                    return (MAGIC_BITBOARDS::bishopAttacksMask(bs.occupied, from) & (1ULL << to)) != 0;
                case 3:
                    return (MAGIC_BITBOARDS::rookAttacksMask(bs.occupied, from) & (1ULL << to)) != 0;
                case 4:
                    return (MAGIC_BITBOARDS::queenAttacksMask(bs.occupied, from) & (1ULL << to)) != 0;
                case 5:
                    return (pm.kingMoves[from] & (1ULL << to)) != 0;
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