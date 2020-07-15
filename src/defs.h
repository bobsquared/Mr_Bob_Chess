#pragma once
#include <algorithm>
#include <iostream>
#include <cstdint>
#include <bitset>
#include <unordered_map>
#include <cassert>

#define HASH_SIZE 256
#define MAX_NUM_MOVES 256

#define FROM_LOC_MOVE ((1ULL << 15) | (1ULL << 14) | (1ULL << 13) | (1ULL << 12) | (1ULL << 11) | (1ULL << 10))
#define TO_LOC_MOVE ((1ULL << 9) | (1ULL << 8) | (1ULL << 7) | (1ULL << 6) | (1ULL << 5) | (1ULL << 4))
#define MOVE_BITS (FROM_LOC_MOVE | TO_LOC_MOVE)

typedef uint16_t MOVE;
#define NULL_MOVE ((1ULL << 16) - 1)
#define NO_MOVE 0
#define INFINITY_VAL 99999


// All information about a move
struct Move {
    MOVE move;
    int score;

    Move() : move(0), score(0) {}

    Move(MOVE move, int score) :
    move(move), score(score) {}

    bool operator<(const Move& a) const { return score > a.score; }
    bool operator>(const Move& a) const { return score < a.score; }
};



// A struct which contains a list of all moves.
// Used in move generation
struct MoveList {
    Move moveList[MAX_NUM_MOVES];
    int count;

    MoveList() : count(0) {}

    void append_move(Move move) {
        moveList[count] = move;
        count++;
    }

    bool get_next_move(MOVE &move) {
        if (count == 0) {
            move = 0;
            return false;
        }

        int index = 0;
        for (int i = 0; i < count; i++) {
            if (moveList[i].score >= moveList[index].score) {
                index = i;
            }
        }

        move = moveList[index].move;
        moveList[index] = moveList[count - 1];
        count--;

        return true;
    }

    void get_index_move(int index, MOVE &move) {
        assert(index < count && index >= 0);
        move = moveList[index].move;
    }

    void set_score_index(int index, int score) {
        assert(index < count && index >= 0);
        moveList[index].score = score;
    }

    void set_score_move(MOVE move, int score) {
        for (int i = 0; i < count; i++) {
            if (moveList[i].move == move) {
                moveList[i].score = score;
            }
        }
    }

};



// This is what is stored in the transposition table
struct ZobristVal {
    MOVE move;
    int16_t score;
    int8_t depth;
    uint8_t flag;
    uint64_t posKey;
    uint16_t halfMove;

    ZobristVal() : move(NO_MOVE), score(0), depth(0), flag(0), posKey(0), halfMove(0) {}

    ZobristVal(MOVE move, int16_t score, int8_t depth, uint8_t flag, uint64_t posKey, uint16_t halfMove) :
        move(move), score(score), depth(depth), flag(flag), posKey(posKey), halfMove(halfMove) {}
};



struct MoveInfo {
    int captureType;
    int enpassantSq;
    int halfMoves;
    uint8_t castleRights;
    uint64_t posKey;
    MOVE move;

    bool operator==(const uint64_t& rhs) {
        return posKey == rhs;
    }

    MoveInfo() :
        captureType(-1), enpassantSq(0), halfMoves(0), castleRights(15), posKey(0), move(NO_MOVE) {}

    MoveInfo(int captureType, int enpassantSq, int halfMoves, uint8_t castleRights, uint64_t posKey, MOVE move) :
        captureType(captureType), enpassantSq(enpassantSq), halfMoves(halfMoves), castleRights(castleRights), posKey(posKey), move(move) {}
};


struct MoveInfoStack {
    MoveInfo move[1024];
    int count;

    MoveInfoStack() : count(0) {}

    void insert(MoveInfo moveInfo) {
        move[count] = moveInfo;
        count++;
    }

    MoveInfo pop() {
        count--;
        return move[count];
    }

    void clear() {
        count = 0;
    }
};



// For Bitscanning
const int MSB_TABLE[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
    57, 49, 41, 37, 28, 16,  3, 61,
    54, 58, 35, 52, 50, 42, 21, 44,
    38, 32, 29, 23, 17, 11,  4, 62,
    46, 55, 26, 59, 40, 36, 15, 53,
    34, 51, 20, 43, 31, 22, 10, 45,
    25, 39, 14, 33, 19, 30,  9, 24,
    13, 18,  8, 12,  7,  6,  5, 63
};



// Number to algebra
const std::string TO_ALG[64] = {
    "a1","b1","c1","d1","e1","f1","g1","h1",
    "a2","b2","c2","d2","e2","f2","g2","h2",
    "a3","b3","c3","d3","e3","f3","g3","h3",
    "a4","b4","c4","d4","e4","f4","g4","h4",
    "a5","b5","c5","d5","e5","f5","g5","h5",
    "a6","b6","c6","d6","e6","f6","g6","h6",
    "a7","b7","c7","d7","e7","f7","g7","h7",
    "a8","b8","c8","d8","e8","f8","g8","h8"
};



// Algebra to number
extern std::unordered_map<std::string, uint8_t> TO_NUM;

extern int evalStack[1024];
extern int history[2][64][64];
extern MOVE killers[2][1024][2];
extern MOVE counterMove[2][64][64];
extern uint64_t columnMask[64];
extern uint64_t rowMask[64];
extern uint64_t forwardMask[64];
extern uint64_t backwardMask[64];

extern int bitScan(const uint64_t bitboard);
extern void InitHistory();
extern void InitColumnsMask();
extern void InitRowsMask();
extern void InitKillers();
extern void InitCounterMoves();
extern uint64_t pawnAttacksAll(uint64_t bitboard, bool colorFlag);
extern uint64_t knightAttacks(uint64_t knights);
extern int count_population(uint64_t bitboard);
extern void printBoard(const uint64_t board);

extern int get_move_from(MOVE move);
extern int get_move_to(MOVE move);