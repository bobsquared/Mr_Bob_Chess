#include "move.h"

namespace BITBOARD::detail {
    /************************************************************************************************
    **  Moves section
    **  Changing the position of the board.
    *************************************************************************************************/



    void move_quiet(BoardState& bs, Accumulator<768, 768>& acc, int from, int to, int piece, uint64_t i1i2) {
        bs.color[bs.toMove] ^= i1i2;
        bs.pieces[piece] ^= i1i2;
        bs.occupied      ^= i1i2;
        bs.pieceAt[to]    = piece;
        bs.pieceAt[from]  = -1;
        acc.Add(piece * 64 + to);
        acc.Remove(piece * 64 + from);
    }



    void make_null_move_impl(BoardState& bs, MoveInfoStack& moveHistory) {
        bs.toMove = !bs.toMove;
        moveHistory.insert(0, bs.enpassantSq, bs.halfMoves, bs.castleRights, bs.posKey, NULL_MOVE);

        if (bs.enpassantSq) {
            ZOBRIST::hashBoard_enpassant(bs.posKey, bs.enpassantSq);
            bs.enpassantSq = 0;
        }

        ZOBRIST::hashBoard_turn(bs.posKey);
        return;
    }

    

    void undo_null_move_impl(BoardState& bs, MoveInfoStack& moveHistory) {
        bs.toMove = !bs.toMove;
        MoveInfo moveInfo = moveHistory.pop();
        bs.halfMoves = moveInfo.halfMoves;
        bs.castleRights = moveInfo.castleRights;
        bs.enpassantSq = moveInfo.enpassantSq;
        bs.posKey = moveInfo.posKey;
        return;
    }



    void make_move_impl(BoardState& bs, MoveInfoStack& moveHistory, Accumulator<768, 768>& acc, MOVE move) {
        
        PieceMoves& pm = BITBOARD::pieceMoves;
        int from = get_move_from(move);
        int to = get_move_to(move);

        int fromPiece = bs.pieceAt[from];
        int toPiece = bs.pieceAt[to];

        uint64_t i1 = 1ULL << from;
        uint64_t i2 = 1ULL << to;
        uint64_t i1i2 = i1 | i2;

        uint8_t crights = bs.castleRights;
        int hmoves = bs.halfMoves;
        int enSq = bs.enpassantSq;
        uint64_t prevPosKey = bs.posKey;
        int moveFlags = move & MOVE_FLAGS;
        uint8_t cflag = bs.toMove? CASTLE_FLAG_BLACK : CASTLE_FLAG_WHITE;

        bs.halfMoves++;
        bs.fullMoves += bs.toMove;

        if (bs.enpassantSq) {
            ZOBRIST::hashBoard_enpassant(bs.posKey, bs.enpassantSq);
            bs.enpassantSq = 0;
        }

        // Update half moves
        if (fromPiece == bs.toMove) {
            bs.halfMoves = 0;
        }

        // Update castling rights
        if (fromPiece == 6 + bs.toMove) {
            ZOBRIST::hashBoard_castle(bs.posKey, bs.castleRights & ~pm.rookCastleFlagMask[from]);
            bs.castleRights &= pm.rookCastleFlagMask[from];
        }
        else if (fromPiece == 10 + bs.toMove) {
            if (cflag) {
                ZOBRIST::hashBoard_castle(bs.posKey, bs.castleRights & cflag);
                bs.castleRights &= ~cflag;
            }
            bs.kingLoc[bs.toMove] = to;
        }

        assert(fromPiece != -1);
        assert(from != 0 || to != 0);

        if (moveFlags == QUIET_MOVES_FLAG) {
            assert(toPiece == -1);
            move_quiet(bs, acc, from, to, fromPiece, i1i2);
            ZOBRIST::hashBoard_quiet(bs.posKey, from, to, fromPiece);
        }
        else if (moveFlags == ENPASSANT_FLAG) {
            assert(toPiece == -1);
            assert(fromPiece == bs.toMove);
            move_quiet(bs, acc, from, to, fromPiece, i1i2);

            uint64_t toCap = to + (bs.toMove * 2 - 1) * 8;
            ZOBRIST::hashBoard_quiet(bs.posKey, from, to, fromPiece);
            ZOBRIST::hashBoard_square(bs.posKey, toCap, !bs.toMove);
            bs.color[!bs.toMove] ^= 1ULL << toCap;
            bs.pieces[!bs.toMove] ^= 1ULL << toCap;
            bs.occupied ^= 1ULL << toCap;
            bs.pieceAt[toCap] = -1;
            bs.pieceCount[!bs.toMove]--;
            acc.Remove(!bs.toMove * 64 + toCap);
        }
        else if (move & CAPTURE_FLAG) {
            assert(toPiece != -1);

            if (toPiece == 6 + !bs.toMove) {
                ZOBRIST::hashBoard_castle(bs.posKey, bs.castleRights & ~pm.rookCastleFlagMask[to]);
                bs.castleRights &= pm.rookCastleFlagMask[to];
            }

            bs.color[bs.toMove] ^= i1i2;
            bs.color[!bs.toMove] ^= i2;
            bs.pieceAt[from] = -1;
            bs.pieces[toPiece] ^= i2;
            bs.occupied ^= i1;
            bs.pieceCount[toPiece]--;
            acc.Remove(fromPiece * 64 + from);
            acc.Remove(toPiece * 64 + to);

            if (move & PROMOTION_FLAG) {
                assert(fromPiece == bs.toMove);
                int pieceVal = (moveFlags - 11);
                int promotePiece = pieceVal * 2 + bs.toMove;
                bs.pieces[fromPiece] ^= i1;
                bs.pieces[promotePiece] ^= i2;
                bs.pieceAt[to] = promotePiece;
                bs.pieceCount[bs.toMove]--;
                bs.pieceCount[promotePiece]++;
                ZOBRIST::hashBoard_capture_promotion(bs.posKey, from, to, fromPiece, toPiece, promotePiece);
                acc.Add(promotePiece * 64 + to);
            }
            else {
                bs.pieceAt[to] = fromPiece;
                bs.pieces[fromPiece] ^= i1i2;
                ZOBRIST::hashBoard_capture(bs.posKey, from, to, fromPiece, toPiece);
                acc.Add(fromPiece * 64 + to);
            }

            bs.halfMoves = 0;
        }
        else if (moveFlags == DOUBLE_PAWN_PUSH_FLAG) {
            assert(toPiece == -1);
            move_quiet(bs, acc, from, to, fromPiece, i1i2);
            bs.enpassantSq = to + (bs.toMove * 2 - 1) * 8;
            ZOBRIST::hashBoard_quiet(bs.posKey, from, to, fromPiece);
            ZOBRIST::hashBoard_enpassant(bs.posKey, bs.enpassantSq);
        }
        else if (move & PROMOTION_FLAG) {
            assert(toPiece == -1);
            assert(fromPiece == bs.toMove);
            bs.color[bs.toMove] ^= i1i2;
            bs.pieceAt[from] = -1;
            bs.pieces[fromPiece] ^= i1;
            bs.occupied ^= i1i2;

            int pieceVal = (moveFlags - 7);
            int promotePiece = pieceVal * 2 + bs.toMove;
            bs.pieces[promotePiece] ^= i2;
            bs.pieceAt[to] = promotePiece;
            bs.pieceCount[bs.toMove]--;
            bs.pieceCount[promotePiece]++;
            ZOBRIST::hashBoard_promotion(bs.posKey, from, to, fromPiece, promotePiece);
            acc.Add(promotePiece * 64 + to);
            acc.Remove(fromPiece * 64 + from);
        }
        else if (moveFlags == KING_CASTLE_FLAG) {
            assert(toPiece == -1);
            assert(fromPiece == 10 + bs.toMove);
            move_quiet(bs, acc, from, to, fromPiece, i1i2);
            move_quiet(bs, acc, to + 1, to - 1, 6 + bs.toMove, 1ULL << (to - 1) | 1ULL << (to + 1));
            ZOBRIST::hashBoard_quiet(bs.posKey, from, to, fromPiece);
            ZOBRIST::hashBoard_quiet(bs.posKey, to + 1, to - 1, 6 + bs.toMove);
        }
        else if (moveFlags == QUEEN_CASTLE_FLAG) {
            assert(toPiece == -1);
            assert(fromPiece == 10 + bs.toMove);
            move_quiet(bs, acc, from, to, fromPiece, i1i2);
            move_quiet(bs, acc, to - 2, to + 1, 6 + bs.toMove, 1ULL << (to - 2) | 1ULL << (to + 1));
            ZOBRIST::hashBoard_quiet(bs.posKey, from, to, fromPiece);
            ZOBRIST::hashBoard_quiet(bs.posKey, to - 2, to + 1, 6 + bs.toMove);
        }

        bs.toMove = !bs.toMove;
        ZOBRIST::hashBoard_turn(bs.posKey);
        moveHistory.insert(toPiece, enSq, hmoves, crights, prevPosKey, move);
    }



    void undo_move_impl(BoardState& bs, MoveInfoStack& moveHistory, Accumulator<768, 768>& acc, MOVE move) {

        bs.toMove = !bs.toMove;
        ZOBRIST::hashBoard_turn(bs.posKey);
        int from = get_move_from(move);
        int to = get_move_to(move);
        int toPiece = bs.pieceAt[to];

        uint64_t i1 = 1ULL << from;
        uint64_t i2 = 1ULL << to;
        uint64_t i1i2 = i1 | i2;
        int moveFlags = move & MOVE_FLAGS;

        MoveInfo moveInfo = moveHistory.pop();
        bs.halfMoves = moveInfo.halfMoves;
        bs.castleRights = moveInfo.castleRights;
        bs.enpassantSq = moveInfo.enpassantSq;
        bs.posKey = moveInfo.posKey;

        bs.fullMoves -= bs.toMove;

        if (toPiece == 10 + bs.toMove) {
            bs.kingLoc[bs.toMove] = from;
        }

        if (moveFlags == QUIET_MOVES_FLAG) {
            move_quiet(bs, acc, to, from, toPiece, i1i2);
        }
        else if (moveFlags == ENPASSANT_FLAG) {
            assert(toPiece == bs.toMove);
            move_quiet(bs, acc, to, from, toPiece, i1i2);

            uint64_t toCap = to + (bs.toMove * 2 - 1) * 8;
            bs.color[!bs.toMove] ^= 1ULL << toCap;
            bs.pieces[!bs.toMove] ^= 1ULL << toCap;
            bs.occupied ^= 1ULL << toCap;
            bs.pieceAt[toCap] = !bs.toMove;
            bs.pieceCount[!bs.toMove]++;
            acc.Add(!bs.toMove * 64 + toCap);
        }
        else if (move & CAPTURE_FLAG) {

            bs.color[bs.toMove] ^= i1i2;
            bs.color[!bs.toMove] ^= i2;
            bs.pieces[moveInfo.captureType] ^= i2;
            bs.pieceAt[to] = moveInfo.captureType;
            bs.occupied ^= i1;
            bs.pieceCount[moveInfo.captureType]++;
            acc.Add(moveInfo.captureType * 64 + to);

            if (move & PROMOTION_FLAG) {
                int pieceVal = (moveFlags - 11);
                int promotePiece = pieceVal * 2 + bs.toMove;
                bs.pieceAt[from] = bs.toMove;
                bs.pieces[bs.toMove] ^= i1;
                bs.pieces[promotePiece] ^= i2;
                bs.pieceCount[bs.toMove]++;
                bs.pieceCount[promotePiece]--;
                acc.Remove(promotePiece * 64 + to);
                acc.Add(bs.toMove * 64 + from);
            }
            else {
                bs.pieces[toPiece] ^= i1i2;
                bs.pieceAt[from] = toPiece;
                acc.Remove(toPiece * 64 + to);
                acc.Add(toPiece * 64 + from);
            }

        }
        else if (moveFlags == DOUBLE_PAWN_PUSH_FLAG) {
            assert(toPiece == bs.toMove);
            move_quiet(bs, acc, to, from, toPiece, i1i2);
        }
        else if (move & PROMOTION_FLAG) {
            bs.color[bs.toMove] ^= i1i2;
            bs.pieceAt[from] = 0 + bs.toMove;
            bs.pieceAt[to] = -1;
            bs.pieces[0 + bs.toMove] ^= i1;
            bs.occupied ^= i1i2;

            int pieceVal = (moveFlags - 7);
            int promotePiece = pieceVal * 2 + bs.toMove;
            bs.pieces[promotePiece] ^= i2;
            bs.pieceCount[bs.toMove]++;
            bs.pieceCount[promotePiece]--;
            acc.Remove(promotePiece * 64 + to);
            acc.Add(bs.toMove * 64 + from);

        }
        else if (moveFlags == KING_CASTLE_FLAG) {
            assert(toPiece == 10 + bs.toMove);
            move_quiet(bs, acc, to, from, toPiece, i1i2);
            move_quiet(bs, acc, to - 1, to + 1, 6 + bs.toMove, 1ULL << (to - 1) | 1ULL << (to + 1));
        }
        else if (moveFlags == QUEEN_CASTLE_FLAG) {
            assert(toPiece == 10 + bs.toMove);
            move_quiet(bs, acc, to, from, toPiece, i1i2);
            move_quiet(bs, acc, to + 1, to - 2, 6 + bs.toMove, 1ULL << (to - 2) | 1ULL << (to + 1));
        }
    }
}