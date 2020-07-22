#pragma once
#include <iostream>
#include <bitset>
#include <string>
#include <regex>
#include "movegen.h"
#include "defs.h"
#include "magic_bitboards.h"
#include "eval.h"
#include "movepick.h"
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

    Bitboard();
    ~Bitboard();

    void printPretty();
    std::string getPv();
    std::string posToFEN();
    void setPosFen(std::string fen);

    bool InCheck();
    bool InCheckOther();

    void generate(MoveList &moveList, int depth, MOVE pvMove);
    void generate_captures_promotions(MoveList &moveList, MOVE pvMove);

    bool pickMove(MOVE &move);
    void make_move(MOVE move);
    void undo_move(MOVE move);

    int evaluate();
    int evaluate_debug();
    bool isDraw();
    bool noPotentialWin();

    void reset();
    bool getSideToMove();
    void insertKiller(int depth, MOVE move);
    void insertCounterMove(MOVE move);


    uint64_t getPosKey();
    bool probeTT(uint64_t posKey, ZobristVal &hashedBoard, int depth, bool &ttRet, int &alpha, int &beta);
    void saveTT(MOVE move, int score, int depth, uint8_t flag, uint64_t key);
    void debugZobristHash();
    void clearHashStats();
    void clearHashTable();
    int getHashFull();

    bool nullMoveable();
    int seeCapture(MOVE capture);
    bool isRepetition();
    bool isKiller(int depth, MOVE move);
    void removeKiller(int depth);


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
    uint64_t blackPawnMoves[64];
    uint64_t whitePawnMoves[64];
    uint64_t pawnAttacks[64][2];
    uint64_t knightMoves[64];
    uint64_t bishopMoves[64];
    uint64_t rookMoves[64];
    uint64_t queenMoves[64];
    uint64_t kingMoves[64];

    // Position info
    uint64_t pieces[12];
    uint64_t color[2];
    int pieceAt[64];
    int material[2];
    int pieceCount[12];
    uint8_t rookCastleFlagMask[64];
    uint64_t occupied;
    bool toMove;

    // Move info
    MoveInfoStack moveHistory;
    uint64_t posKey;
    int enpassantSq;
    int fullMoves;
    int halfMoves;
    uint8_t castleRights;

    // Additional objects
    MoveGen *moveGen;
    Magics *magics;
    Eval *eval;
    MovePick *movePick;
    Zobrist *zobrist;
    TranspositionTable *tt;

    // Initialization functions
    void InitWhitePawnMoves();
    void InitBlackPawnMoves();
    void InitBlackPawnAttacks();
    void InitWhitePawnAttacks();
    void InitKnightMoves();
    void InitBishopMoves();
    void InitRookMoves();
    void InitQueenMoves();
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
