
#include "transpositionTable.h"




// Initialize transposition table
TranspositionTable::TranspositionTable() {
    numHashes = ((uint64_t) HASH_SIZE * 0xFFFFFULL) / sizeof(ZobristVal);
    hashTable = new ZobristVal [numHashes];
    age = 1;

    clearHashTable();
}



// Initialize transposition table
TranspositionTable::TranspositionTable(int hashSize) {
    numHashes = ((uint64_t) HASH_SIZE * 0xFFFFFULL) / sizeof(ZobristVal);
    hashTable = new ZobristVal [numHashes];
    age = 1;

    clearHashTable();
}



// set transposition table size
void TranspositionTable::setSize(uint64_t hashSize) {
    delete [] hashTable;

    numHashes = ((uint64_t) hashSize * 0xFFFFFULL) / sizeof(ZobristVal);
    hashTable = new ZobristVal [numHashes];

    clearHashTable();
}



// Delete the hash table
TranspositionTable::~TranspositionTable() {
    delete [] hashTable;
}



// Set TT age
void TranspositionTable::incrementTTAge() {
    age++;
    age = age % 64;
}



// Save the position into the transposition table
void TranspositionTable::saveTT(ThreadSearch *th, MOVE move, int score, int staticScore, int depth, uint8_t flag, uint64_t key, int ply) {
    uint64_t posKey = key % numHashes;

    score += score > MATE_VALUE_MAX? ply : (score < -MATE_VALUE_MAX? -ply : 0);
    ZobristVal tt = hashTable[posKey];
    uint8_t ttAge = getAgeFromTT(tt.flagsAndAge);
    uint8_t ttFlag = getFlagsFromTT(tt.flagsAndAge);

    if (tt.posKey == 0) {
        th->ttWrites++;
        hashTable[posKey] = ZobristVal(move, (int16_t) score, (int16_t) staticScore, (int8_t) depth, setFlagsAndAgeInTT(age, flag), key);
    }
    else if (age != ttAge || flag == EXACT || (ttFlag == EXACT && depth >= tt.depth) || (ttFlag != EXACT && depth >= tt.depth - 3)) {
        hashTable[posKey] = ZobristVal(move, (int16_t) score, (int16_t) staticScore, (int8_t) depth, setFlagsAndAgeInTT(age, flag), key);
    }


}


// Probe the transposition table
bool TranspositionTable::probeTT(uint64_t key, ZobristVal &hashedBoard, int depth, bool &ttRet, MOVE &ttMove, int alpha, int beta, int ply) {

    bool ret = false;

    // Store the hash table value
    hashedBoard = hashTable[key % numHashes];

    if (hashTable[key % numHashes].posKey == key) {

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
    }

    return ret;

}



// Probe the transposition table
// Currently using: Always Replace
bool TranspositionTable::probeTTQsearch(uint64_t key, ZobristVal &hashedBoard, bool &ttRet, MOVE &ttMove, int alpha, int beta, int ply) {

    bool ret = false;

    // Store the hash table value
    hashedBoard = hashTable[key % numHashes];

    if (hashTable[key % numHashes].posKey == key) {

        ret = true;
        ttMove = hashedBoard.move;
        hashedBoard.score += hashedBoard.score < -MATE_VALUE_MAX? ply : (hashedBoard.score > MATE_VALUE_MAX? -ply : 0);
        uint8_t ttFlag = getFlagsFromTT(hashedBoard.flagsAndAge);

        alpha = ttFlag == LOWER_BOUND? hashedBoard.score : alpha; // Low bound
        beta = ttFlag == UPPER_BOUND? hashedBoard.score : beta; // upper bound

        if (ttFlag == EXACT || alpha >= beta) { // exact or alpha >= beta
            ttRet = true;
        }

    }

    return ret;

}



// Return the principal variation as a string.
// It returns the string as a list of moves, (ex. 'e2e4 e7e5 d2d4 e5d4')
std::string TranspositionTable::getPv(Board &b) {

    std::string pv = "";
    std::vector<uint64_t> loopChecker;
    std::stack<MOVE> movesToUndo;

    while (true) {
        uint64_t posKey = b.state.posKey;
        loopChecker.push_back(posKey);

        if (std::count(loopChecker.begin(), loopChecker.end(), loopChecker.back()) >= 3) {
            break;
        }

        ZobristVal hashedBoard = hashTable[posKey % numHashes];
        if (hashedBoard.posKey == posKey) {
            if (hashedBoard.move == NULL_MOVE) {
                break;
            }
            movesToUndo.push(hashedBoard.move);
            pv += " " + moveToString(hashedBoard.move);
            BITBOARD::make_move(b, hashedBoard.move);
        }
        else {
            break;
        }
    }

    while (!movesToUndo.empty()) {
        BITBOARD::undo_move(b, movesToUndo.top());
        movesToUndo.pop();
    }

    return pv;
}


// Print hash table statistics
int TranspositionTable::getHashFull(uint64_t writes) {
    return (1000 * writes) / numHashes;
}



// Clear hash table
void TranspositionTable::clearHashTable() {
    for (uint64_t i = 0; i < numHashes; i++) {
        hashTable[i] = ZobristVal();
    }
}
