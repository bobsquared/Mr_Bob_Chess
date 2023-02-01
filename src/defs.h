#pragma once
#include <algorithm>
#include <string>
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
#define MAX_PLY 269
#define MATE_VALUE 16383
#define MATE_VALUE_MAX (MATE_VALUE - MAX_PLY)

#define QUIET_MOVES_FLAG 0
#define DOUBLE_PAWN_PUSH_FLAG 1
#define KING_CASTLE_FLAG 2
#define QUEEN_CASTLE_FLAG 3
#define CAPTURES_NORMAL_FLAG 4
#define ENPASSANT_FLAG 5
#define KNIGHT_PROMOTION_FLAG 8
#define BISHOP_PROMOTION_FLAG 9
#define ROOK_PROMOTION_FLAG 10
#define QUEEN_PROMOTION_FLAG 11
#define KNIGHT_PROMOTION_CAPTURE_FLAG 12
#define BISHOP_PROMOTION_CAPTURE_FLAG 13
#define ROOK_PROMOTION_CAPTURE_FLAG 14
#define QUEEN_PROMOTION_CAPTURE_FLAG 15

#define PROMOTION_FLAG 8
#define CAPTURE_FLAG 4
#define MOVE_FLAGS 15

#define KING_CASTLE_WHITE_MASK ((1ULL << 6) | (1ULL << 5) | (1ULL << 4))
#define QUEEN_CASTLE_WHITE_MASK ((1ULL << 2) | (1ULL << 3) | (1ULL << 4))

#define KING_CASTLE_BLACK_MASK ((1ULL << 60) | (1ULL << 61) | (1ULL << 62))
#define QUEEN_CASTLE_BLACK_MASK ((1ULL << 60) | (1ULL << 59) | (1ULL << 58))

#define KING_CASTLE_OCCUPIED_WHITE_MASK ((1ULL << 6) | (1ULL << 5))
#define QUEEN_CASTLE_OCCUPIED_WHITE_MASK ((1ULL << 1) | (1ULL << 2) | (1ULL << 3))

#define KING_CASTLE_OCCUPIED_BLACK_MASK ((1ULL << 61) | (1ULL << 62))
#define QUEEN_CASTLE_OCCUPIED_BLACK_MASK ((1ULL << 57) | (1ULL << 58) | (1ULL << 59))

#define KING_CASTLE_RIGHTS_WHITE 8
#define QUEEN_CASTLE_RIGHTS_WHITE 4

#define KING_CASTLE_RIGHTS_BLACK 2
#define QUEEN_CASTLE_RIGHTS_BLACK 1

#define PROMOTION_MASK 11
#define QUEEN_PROMOTION_MASK 3
#define ROOK_PROMOTION_MASK 2
#define BISHOP_PROMOTION_MASK 1
#define KNIGHT_PROMOTION_MASK 0




// All information about a move
struct Move {
    int score;
    MOVE move;

    Move() : score(0), move(0) {}

    Move(MOVE move, int score) :
        score(score), move(move) {}

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

        int index = 0;
        for (int i = 0; i < count; i++) {
            if (moveList[i].score > moveList[index].score) {
                index = i;
            }
        }

        move = count? moveList[index].move : NO_MOVE;
        moveList[index] = moveList[std::max(0, count - 1)];
        count--;

        return count + 1;
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
    uint64_t posKey;
    MOVE move;
    uint16_t halfMove;
    int16_t score;
    int16_t staticScore;
    int8_t depth;
    uint8_t flag;

    ZobristVal() : posKey(0), move(NO_MOVE), halfMove(0), score(0), staticScore(0), depth(0), flag(0) {}

    ZobristVal(MOVE move, int16_t score, int16_t staticScore, int8_t depth, uint8_t flag, uint64_t posKey, uint16_t halfMove) :
        posKey(posKey), move(move), halfMove(halfMove), score(score), staticScore(staticScore), depth(depth), flag(flag) {}
};



struct MoveInfo {
    uint64_t posKey;
    int captureType;
    int enpassantSq;
    int halfMoves;
    MOVE move;
    uint8_t castleRights;

    bool operator==(const uint64_t& rhs) {
        return posKey == rhs;
    }

    MoveInfo() :
        posKey(0), captureType(-1), enpassantSq(0), halfMoves(0), move(NO_MOVE), castleRights(15) {}

    MoveInfo(int captureType, int enpassantSq, int halfMoves, uint8_t castleRights, uint64_t posKey, MOVE move) :
        posKey(posKey), captureType(captureType), enpassantSq(enpassantSq), halfMoves(halfMoves), move(move), castleRights(castleRights) {}

};


struct MoveInfoStack {
    MoveInfo move[1024] = {};
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


// Information for search
struct SearchStack {
    int eval;
    int extLevel;
    int hashLevel;
    MOVE singMove;

    SearchStack() : eval(0), extLevel(0), hashLevel(0), singMove(NO_MOVE) {};
};



// Info each thread has
struct ThreadSearch {
    uint64_t ttWrites;
    uint64_t nodes;

    SearchStack searchStack[MAX_PLY] = {};
    int seldepth;

    int history[2][64][64] = {};
    int captureHistory[2][64][64] = {};
    int counterHistory[2][6][64][6][64] = {};
    MOVE killers[MAX_PLY][2] = {};
    MOVE counterMove[2][64][64] = {};
    MOVE bestMove;

    bool nullMoveTree;

    ThreadSearch() : ttWrites(0), nodes(0), seldepth(0), bestMove(NO_MOVE), nullMoveTree(false)  {};
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

extern uint64_t columnMask[64];
extern uint64_t rowMask[64];

extern int bitScan(const uint64_t bitboard);
extern void InitHistory(ThreadSearch *th);
extern void InitColumnsMask();
extern void InitRowsMask();
extern void InitKillers(ThreadSearch *th);
extern void InitCounterMoves(ThreadSearch *th);
extern uint64_t pawnAttacksAll(uint64_t bitboard, bool colorFlag);
extern uint64_t knightAttacks(uint64_t knights);
extern int count_population(uint64_t bitboard);
extern int check_bit(uint64_t bitboard);
extern void printBoard(const uint64_t board);
extern bool isCaptureMove(const MOVE move);
extern bool isCaptureOrPromotionMove(const MOVE move);
extern bool isQuietMove(const MOVE move);


extern int get_move_from(MOVE move);
extern int get_move_to(MOVE move);
extern std::string moveToString(MOVE move);
