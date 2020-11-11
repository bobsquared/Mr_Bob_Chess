#pragma once
#include <iostream>
#include <bitset>
#include <string>
#include <regex>
#include "defs.h"
#include "magic_bitboards.h"
#include "zobrist_hashing.h"
#include "transpositionTable.h"


// Castling flags
#define CASTLE_FLAG_KING_WHITE  8
#define CASTLE_FLAG_QUEEN_WHITE 4
#define CASTLE_FLAG_KING_BLACK  2
#define CASTLE_FLAG_QUEEN_BLACK 1

#define CASTLE_FLAG_WHITE (CASTLE_FLAG_KING_WHITE | CASTLE_FLAG_QUEEN_WHITE)
#define CASTLE_FLAG_BLACK (CASTLE_FLAG_KING_BLACK | CASTLE_FLAG_QUEEN_BLACK)


class Bitboard {


public:

    int pieceAt[64];
    uint64_t pieces[12];
    uint64_t kingMoves[64];
    uint64_t knightMoves[64];
    uint64_t pawnAttacks[64][2];
    uint64_t color[2];
    MoveInfoStack moveHistory;
    int enpassantSq;
    uint64_t occupied;
    bool toMove;
    uint8_t castleRights;
    int fullMoves;
    int halfMoves;

    int material[2];
    int pieceCount[12];

    Bitboard();
    Bitboard(const Bitboard &b);

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
    void insertKiller(ThreadSearch *th, int depth, MOVE move);
    void insertCounterMove(ThreadSearch *th, MOVE move);

    uint64_t getPosKey() const;
    uint64_t getPawnKey() const;
    bool probeTT(uint64_t posKey, ZobristVal &hashedBoard, int depth, bool &ttRet, int &alpha, int &beta, int ply);
    bool probeTTQsearch(uint64_t posKey, ZobristVal &hashedBoard, bool &ttRet, int &alpha, int &beta, int ply);
    void debugZobristHash();
    void clearHashTable();
    int getHashFull(uint64_t writes);
    void setTTAge();

    bool nullMoveable();
    int seeCapture(MOVE capture);
    bool isRepetition();
    bool isKiller(ThreadSearch *th, int depth, MOVE move);
    void removeKiller(ThreadSearch *th, int depth);
    bool isLegal(MOVE move);

    bool can_castle_king();
    bool can_castle_queen();
    bool isPseudoLegal(MOVE move);


private:

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

    // Position info
    uint8_t rookCastleFlagMask[64];

    // Move info
    uint64_t posKey;
    uint64_t pawnKey;

    // Initialization functions
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

    const std::string pieceIndex = "PpNnBbRrQqKk";
    const int pvals[6] = {100, 350, 350, 500, 900, 5000};



};
