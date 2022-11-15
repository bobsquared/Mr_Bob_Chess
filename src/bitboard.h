
#pragma once
#include <vector>
#include <regex>
#include "defs.h"
#include "magic_bitboards.h"
#include "zobrist_hashing.h"
#include "nnue/accumulator.h"


// Castling flags
#define CASTLE_FLAG_KING_WHITE  8
#define CASTLE_FLAG_QUEEN_WHITE 4
#define CASTLE_FLAG_KING_BLACK  2
#define CASTLE_FLAG_QUEEN_BLACK 1

#define CASTLE_FLAG_WHITE (CASTLE_FLAG_KING_WHITE | CASTLE_FLAG_QUEEN_WHITE)
#define CASTLE_FLAG_BLACK (CASTLE_FLAG_KING_BLACK | CASTLE_FLAG_QUEEN_BLACK)


class Bitboard {


public:

    MoveInfoStack moveHistory;
    uint64_t pieces[12];
    uint64_t kingMoves[64];
    uint64_t knightMoves[64];
    uint64_t pawnAttacks[64][2];
    uint64_t color[2];
    uint64_t occupied;
    int pieceAt[64];
    int enpassantSq;
    int fullMoves;
    int halfMoves;
    int kingLoc[2];
    int material[2];
    int pieceCount[12];
    uint8_t castleRights;
    bool toMove;

    Bitboard();
    Bitboard(const Bitboard &b);
    std::vector<Accumulator::Features>* getAddFeatures();
    std::vector<Accumulator::Features>* getRemoveFeatures();
    bool getResetFlag();
    void setResetFlag(bool f);

    void printPretty();
    std::string getPv();
    std::string posToFEN();
    void setPosFen(std::string fen);

    bool InCheck();
    bool InCheckOther();

    bool pickMove(MOVE &move);
    void make_move(MOVE move);
    void undo_move(MOVE move);
    void make_null_move();
    void undo_null_move();

    bool isDraw(int ply);
    bool noPotentialWin();

    void reset();
    bool getSideToMove();
    void insertCounterMove(ThreadSearch *th, MOVE move);

    uint64_t getPosKey() const;
    uint64_t getPawnKey() const;
    void debugZobristHash();

    bool nullMoveable();
    int seeCapture(MOVE capture);
    bool isRepetition();
    bool isLegal(MOVE move);

    bool can_castle_king();
    bool can_castle_queen();
    bool isPseudoLegal(MOVE move);

    int getPiece(int index);
    int getRankFromSideToMove(int index);

private:

    Accumulator acc;

    // Material location
    uint64_t whitePawns;
    uint64_t blackPawns;
    uint64_t whiteKnights;
    uint64_t blackKnights;
    uint64_t whiteBishops;
    uint64_t blackBishops;
    uint64_t whiteRooks;
    uint64_t blackRooks;
    uint64_t whiteQueens;
    uint64_t blackQueens;
    uint64_t whiteKings;
    uint64_t blackKings;

    // Move info
    uint64_t posKey;
    uint64_t pawnKey;

    const std::string pieceIndex = "PpNnBbRrQqKk";
    const int pvals[6] = {100, 450, 450, 650, 1250, 5000};

    // Position info
    uint8_t rookCastleFlagMask[64];

    // Initialization functions
    void InitFeatures();
    void InitBlackPawnAttacks();
    void InitWhitePawnAttacks();
    void InitKnightMoves();
    void InitBishopMoves();
    void InitRookMoves();
    void InitKingMoves();
    void InitPieceAt();
    void InitRookCastleFlags(uint64_t whiteRooks, uint64_t blackRooks);
    void InitMaterial();
    void InitPieceCount();

    // Move helpers
    void move_quiet(int from, int to, int piece, uint64_t i1i2);

    // SEE helpers
    uint64_t isAttackedSee(int index);
    uint64_t getLeastValuablePiece(uint64_t attadef, bool col, int &piece);

    bool isAttackedCastleMask(uint64_t bitboard);

};
