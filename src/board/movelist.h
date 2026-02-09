#pragma once
#include "../defs.h"
#include "legality.h"
#include <cassert>

// A struct which contains a list of all moves.
struct MoveList {
    int scores[MAX_NUM_MOVES] = {0};
    MOVE moves[MAX_NUM_MOVES];
    int count;

    MoveList() : count(0) {}

    inline void append_move(MOVE move) {
        moves[count++] = move;
    }

    inline void remove_move(MOVE move) {
        for (int i = 0; i < count; i++) {
            if (moves[i] == move) {
                moves[i] = moves[count - 1];
                scores[i] = scores[count - 1];
                count--;
                break;
            }
        }   
    }

    inline MOVE get_index_move(int index) {
        assert(index < count && index >= 0);
        return moves[index];
    }

    inline void set_score_index(int index, int score) {
        assert(index < count && index >= 0);
        scores[index] = score;
    }

    void set_score_move(MOVE move, int score) {
        for (int i = 0; i < count; i++) {
            if (moves[i] == move) {
                scores[i] = score;
                break;
            }
        }
    }

    bool get_next_move(MOVE &move) {
        if (count == 0) {
            move = NO_MOVE;
            return false;
        }

        int index = 0;
        for (int i = 1; i < count; i++) {
            if (scores[i] > scores[index]) {
                index = i;
            }
        }

        move = moves[index];
        moves[index] = moves[count - 1];
        scores[index] = scores[count - 1];
        count--;

        return true;
    }

    bool get_next_move_score(MOVE &move, int &score) {
        if (count == 0) {
            move = NO_MOVE;
            score = 0;
            return false;
        }

        int index = 0;
        for (int i = 1; i < count; i++) {
            if (scores[i] > scores[index]) {
                index = i;
            }
        }

        move = moves[index];
        score = scores[index];

        return true;
    }

};