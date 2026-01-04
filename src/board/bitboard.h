
#pragma once

#include "../defs.h"
#include "../zobrist_hashing.h"
#include "../nnue/accumulator.h"


// Castling flags
#define CASTLE_FLAG_KING_WHITE  8
#define CASTLE_FLAG_QUEEN_WHITE 4
#define CASTLE_FLAG_KING_BLACK  2
#define CASTLE_FLAG_QUEEN_BLACK 1

#define CASTLE_FLAG_WHITE (CASTLE_FLAG_KING_WHITE | CASTLE_FLAG_QUEEN_WHITE)
#define CASTLE_FLAG_BLACK (CASTLE_FLAG_KING_BLACK | CASTLE_FLAG_QUEEN_BLACK)

struct MoveInfo {
    uint64_t posKey;
    int captureType;
    int enpassantSq;
    int halfMoves;
    MOVE move;
    uint8_t castleRights;

    bool operator==(const uint64_t& rhs) const {
        return posKey == rhs;
    }

    MoveInfo() :
        posKey(0), captureType(-1), enpassantSq(0), halfMoves(0), move(NO_MOVE), castleRights(15) {}

    MoveInfo(int captureType, int enpassantSq, int halfMoves, uint8_t castleRights, uint64_t posKey, MOVE move) :
        posKey(posKey), captureType(captureType), enpassantSq(enpassantSq), halfMoves(halfMoves), move(move), castleRights(castleRights) {}

};



struct MoveInfoStack {
    MoveInfo moves[512] = {};
    int count;

    MoveInfoStack() : count(0) {}

    inline void insert(int captureType, int enpassantSq, int halfMoves, uint8_t castleRights, uint64_t posKey, MOVE move) {
        MoveInfo& info = moves[count++];
        info.captureType = captureType;
        info.enpassantSq = enpassantSq;
        info.halfMoves = halfMoves;
        info.castleRights = castleRights;
        info.posKey = posKey;
        info.move = move;
    }

    inline MoveInfo& pop() {
        return moves[--count];
    }

    inline void clear() {
        count = 0;
    }
};



struct BoardState {
    uint64_t pieces[12];
    uint64_t color[2];
    uint64_t occupied;
    uint64_t posKey;
    int pieceAt[64];
    int pieceCount[12];
    int kingLoc[2];
    int enpassantSq;
    int fullMoves;
    int halfMoves;
    uint8_t castleRights;
    bool toMove;
};



struct Board {
    BoardState state;
    MoveInfoStack moveHistory;
    Accumulator acc;
};



namespace BITBOARD {

    struct PieceMoves {
        uint64_t kingMoves[64];
        uint64_t knightMoves[64];
        uint64_t pawnAttacks[64][2];
        uint8_t rookCastleFlagMask[64];
    };

    extern PieceMoves pieceMoves;


    // Initialization functions
    void InitBoard();
    void CopyAllBoard(const Board &bOrig, Board &bTarget);
    void reset(Board& b);



     // Return the piece from index
    inline int getPiece(BoardState& bs, int index) {
        if (bs.pieceAt[index] == -1) {
            return -1;
        }
        return bs.pieceAt[index] / 2;
    }



    // Return the rank from index depending on the side to move
    inline int getRankFromSideToMove(BoardState& bs, int index) {
        return bs.toMove? 7 - (index / 8) : index / 8;
    }



    // Returns true if null move pruning is allowed in the position
    inline bool nullMoveable(BoardState& bs) {
        return bs.color[bs.toMove] != (bs.pieces[bs.toMove] | bs.pieces[10 + bs.toMove]);
    }
}






