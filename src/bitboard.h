#pragma once
#include <iostream>
#include <bitset>
#include <string>
#include <regex>
#include "defs.h"
#include "magic_bitboards.h"
#include "eval.h"
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
    Magics *magics;

    Bitboard();
    ~Bitboard();

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

    int evaluate();
    int evaluate_debug();
    bool isDraw();
    bool noPotentialWin();

    void reset();
    bool getSideToMove();
    void insertKiller(int depth, MOVE move);
    void insertCounterMove(MOVE move);


    uint64_t getPosKey();
    bool probeTT(uint64_t posKey, ZobristVal &hashedBoard, int depth, bool &ttRet, int &alpha, int &beta, int ply);
    void saveTT(MOVE move, int score, int depth, uint8_t flag, uint64_t key, int ply);
    void debugZobristHash();
    void clearHashStats();
    void clearHashTable();
    int getHashFull();
    void replaceHash(int hashSize);

    bool nullMoveable();
    int seeCapture(MOVE capture);
    bool isRepetition();
    bool isKiller(int depth, MOVE move);
    void removeKiller(int depth);
    bool isLegal(MOVE move);


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

    // Piece movement
    uint64_t bishopMoves[64];
    uint64_t rookMoves[64];

    // Position info
    int material[2];
    int pieceCount[12];
    uint8_t rookCastleFlagMask[64];

    // Move info
    uint64_t posKey;
    int fullMoves;
    int halfMoves;

    // Additional objects
    Eval *eval;
    Zobrist *zobrist;
    TranspositionTable *tt;

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



};
