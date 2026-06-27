
#include "transpositionTable.h"
#include "board/move.h"
#include "board/legality.h"



namespace TT {

    TranspositionTable tt;



    void clearHashTable() {
        for (uint64_t i = 0; i < tt.numHashes; i++) {
            tt.hashTable[i] = TTBucket();
        }
    }



    void InitTT(uint64_t hashSize) {
        tt.numHashes = ((uint64_t) hashSize * 1024 * 1024) / sizeof(TTBucket);
        tt.numHashes = 1ULL << (63 - __builtin_clzll(tt.numHashes));
        tt.mask = tt.numHashes - 1;

        tt.hashTable = new TTBucket [tt.numHashes];
        tt.age = 1;

        clearHashTable();
    }



    void setSize(uint64_t hashSize) {
        delete [] tt.hashTable;

        tt.numHashes = ((uint64_t) hashSize * 1024 * 1024) / sizeof(TTBucket);
        tt.numHashes = 1ULL << (63 - __builtin_clzll(tt.numHashes));
        tt.mask = tt.numHashes - 1;

        tt.hashTable = new TTBucket [tt.numHashes];

        clearHashTable();
    }



    void DestroyTT() {
        delete [] tt.hashTable;
    }



    int getHashFull(uint64_t writes) {
        return (1000 * writes) / (tt.numHashes * 4);
    }



    void incrementTTAge() {
        tt.age++;
        tt.age = tt.age % 64;
    }



    void saveTT(ThreadData &td, MOVE move, int score, int staticScore, int depth, uint8_t flag, uint64_t key, int ply) {
        score += score > MATE_VALUE_MAX? ply : (score < -MATE_VALUE_MAX? -ply : 0);
        uint32_t lowerKey = key & 0xFFFFFFFFULL;
        uint32_t upperKey = key >> 32;

        uint64_t posKey = lowerKey & tt.mask;
        TTBucket& bucket = tt.hashTable[posKey];

        for (TTEntry& entry : bucket.entries) {
            uint8_t ttFlag = getFlagsFromTT(entry.flagsAndAge);

            if (entry.posKey == upperKey) {
                TTEntry newEntry = entry;
                if (flag == EXACT || (depth >= newEntry.depth - 2 + (ttFlag == EXACT))) {
                    // shift tt moves to have most recent one first.
                    if (move != NULL_MOVE) {
                        if (move != newEntry.move && newEntry.move != NULL_MOVE) {
                            if (move == newEntry.move2) {
                                newEntry.move2 = newEntry.move;
                            }
                            else {
                                newEntry.move3 = newEntry.move2;
                                newEntry.move2 = newEntry.move;
                            }
                        }
                        newEntry.move = move;
                    }
                    newEntry.score = static_cast<int16_t>(score);
                    newEntry.staticScore =  static_cast<int16_t>(staticScore);
                    newEntry.flagsAndAge = setFlagsAndAgeInTT(tt.age, flag);
                    newEntry.depth = static_cast<int8_t>(depth);
                    entry = newEntry;
                }
                return;
            }
        }

        for (TTEntry& entry : bucket.entries) {
            if (entry.posKey == 0) {
                td.ttWrites++;
                entry = TTEntry(
                    upperKey, move,
                    static_cast<int16_t>(score),
                    static_cast<int16_t>(staticScore),
                    setFlagsAndAgeInTT(tt.age, flag),
                    static_cast<int8_t>(depth)
                );
                return;
            }
        }

        
        int replaceIndex = 0;
        int replaceScore = INT32_MAX;
        for (int i = 0; i < 4; i++) {
            TTEntry& entry = bucket.entries[i];
            int ageDiff = (tt.age - getAgeFromTT(entry.flagsAndAge)) & 63;
            int score = static_cast<int>(entry.depth) - (ageDiff << (2 + (getFlagsFromTT(entry.flagsAndAge) != EXACT))); 

            if (score < replaceScore) {
                replaceIndex = i;
                replaceScore = score;
            }
        }
        
        bucket.entries[replaceIndex] = TTEntry(
            upperKey, move,
            static_cast<int16_t>(score),
            static_cast<int16_t>(staticScore),
            setFlagsAndAgeInTT(tt.age, flag),
            static_cast<int8_t>(depth)
        );
    }



    bool probeTT(uint64_t key, TTEntry &hashedBoard, int depth, bool &ttRet, MOVE &ttMove, int alpha, int beta, int ply) {
        bool ret = false;

        uint32_t lowerKey = key & 0xFFFFFFFFULL;
        uint32_t upperKey = key >> 32;
        uint64_t posKey = lowerKey & tt.mask;
        TTBucket& bucket = tt.hashTable[posKey];

        for (TTEntry& entry : bucket.entries) {
            if (entry.posKey == upperKey) {
                hashedBoard = entry;
                ret = true;
                ttMove = hashedBoard.move;
                hashedBoard.score += hashedBoard.score < -MATE_VALUE_MAX? ply : (hashedBoard.score > MATE_VALUE_MAX? -ply : 0);

                // Ensure hashedBoard depth >= current depth
                if (hashedBoard.depth >= depth) {
                    uint8_t ttFlag = getFlagsFromTT(hashedBoard.flagsAndAge);
                    alpha = ttFlag == LOWER_BOUND? hashedBoard.score : alpha; // Low bound
                    beta = ttFlag == UPPER_BOUND? hashedBoard.score : beta; // upper bound

                    if (ttFlag == EXACT || alpha >= beta) { // exact or alpha >= beta
                        ttRet = true;
                    }

                }
                break;
            }
        }

        return ret;
    }



    bool probeTTQsearch(uint64_t key, TTEntry &hashedBoard, bool &ttRet, MOVE &ttMove, int alpha, int beta, int ply) {
        bool ret = false;

        uint32_t lowerKey = key & 0xFFFFFFFFULL;
        uint32_t upperKey = key >> 32;
        uint64_t posKey = lowerKey & tt.mask;
        TTBucket& bucket = tt.hashTable[posKey];

        for (TTEntry& entry : bucket.entries) {
            if (entry.posKey == upperKey) {
                hashedBoard = entry;
                ret = true;
                ttMove = hashedBoard.move;
                hashedBoard.score += hashedBoard.score < -MATE_VALUE_MAX? ply : (hashedBoard.score > MATE_VALUE_MAX? -ply : 0);
                uint8_t ttFlag = getFlagsFromTT(hashedBoard.flagsAndAge);
                alpha = ttFlag == LOWER_BOUND? hashedBoard.score : alpha; // Low bound
                beta = ttFlag == UPPER_BOUND? hashedBoard.score : beta; // upper bound

                if (ttFlag == EXACT || alpha >= beta) { // exact or alpha >= beta
                    ttRet = true;
                }
                break;
            }
        }

        return ret;
    }



    // Return the principal variation as a string.
    // It returns the string as a list of moves, (ex. 'e2e4 e7e5 d2d4 e5d4')
    std::string getPv(Board &b) {

        std::string pv = "";
        std::vector<uint64_t> loopChecker;
        std::stack<MOVE> movesToUndo;

        while (true) {
            uint64_t posKey = b.state.posKey;
            loopChecker.push_back(posKey);

            if (std::count(loopChecker.begin(), loopChecker.end(), loopChecker.back()) >= 3) {
                break;
            }

            uint32_t lowerKey = posKey & 0xFFFFFFFFULL;
            uint32_t upperKey = posKey >> 32;
            TTBucket& bucket = tt.hashTable[lowerKey & tt.mask];

            bool foundMove = false;
            for (TTEntry entry : bucket.entries) {
                if (entry.posKey == upperKey) {
                    uint8_t ttFlag = getFlagsFromTT(entry.flagsAndAge);
                    if (ttFlag == UPPER_BOUND || entry.move == NULL_MOVE || !BITBOARD::isPseudoLegal(b.state, entry.move) || !BITBOARD::isLegal(b, entry.move)) {
                        continue;
                    }
                    movesToUndo.push(entry.move);
                    pv += " " + moveToString(entry.move);
                    BITBOARD::make_move(b, entry.move);
                    foundMove = true;
                    break;
                }
            }

            if (!foundMove) {
                break;
            }
        }

        while (!movesToUndo.empty()) {
            BITBOARD::undo_move(b, movesToUndo.top());
            movesToUndo.pop();
        }

        return pv;
    }

}


