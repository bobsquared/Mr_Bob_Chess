
#include "transpositionTable.h"




// Initialize transposition table
TranspositionTable::TranspositionTable() {

    numHashes = (double) HASH_SIZE / (double) sizeof(ZobristVal) * 0xFFFFF;
    hashTable = new ZobristVal [numHashes];
    halfMove = 1;

    clearHashTable();

}



// Initialize transposition table
TranspositionTable::TranspositionTable(int hashSize) {

    numHashes = (double) hashSize / (double) sizeof(ZobristVal) * 0xFFFFF;
    hashTable = new ZobristVal [numHashes];
    halfMove = 1;

    clearHashTable();


}



// Delete the hash table
TranspositionTable::~TranspositionTable() {
    delete [] hashTable;
}



// Set TT age
void TranspositionTable::setTTAge(int age) {
    halfMove = age;
}



// Save the position into the transposition table
// Currently using: Always Replace
void TranspositionTable::saveTT(ThreadSearch *th, MOVE move, int score, int staticScore, int depth, uint8_t flag, uint64_t key, int ply) {
    uint64_t posKey = key % numHashes;

    if (std::abs(score) > MATE_VALUE_MAX) {
        if (score > MATE_VALUE_MAX) {
            score += ply;
        }
        else if (score < -MATE_VALUE_MAX) {
            score -= ply;
        }
    }

    if (hashTable[posKey].posKey == 0) {
        th->ttWrites++;
        hashTable[posKey] = ZobristVal(move, (int16_t) score, (int16_t) staticScore, (int8_t) depth, flag, key, halfMove);
    }
    else if (halfMove != hashTable[posKey].halfMove || flag == 0 || depth >= hashTable[posKey].depth - 2) {
        hashTable[posKey] = ZobristVal(move, (int16_t) score, (int16_t) staticScore, (int8_t) depth, flag, key, halfMove);
    }


}



// Probe the transposition table
// Currently using: Always Replace
bool TranspositionTable::probeTT(uint64_t key, ZobristVal &hashedBoard, int depth, bool &ttRet, int alpha, int beta, int ply) {

    bool ret = false;
    if (hashTable[key % numHashes].posKey == key) {

        ret = true;
        // Store the hash table value
        hashedBoard = hashTable[key % numHashes];

        if (std::abs(hashedBoard.score) > MATE_VALUE_MAX) {
            if (hashedBoard.score > MATE_VALUE_MAX) {
                hashedBoard.score -= ply;
            }
        	else if (hashedBoard.score < -MATE_VALUE_MAX) {
                hashedBoard.score += ply;
            }
        }

        // Ensure hashedBoard depth >= current depth
        if (hashedBoard.depth >= depth) {

            if (hashedBoard.flag == LOWER_BOUND) { // Low bound
                alpha = hashedBoard.score;
            }
            else if (hashedBoard.flag == UPPER_BOUND) { // Upper bound
                beta = hashedBoard.score;
            }
            else { //Exact
                ttRet = true;
            }

            if (alpha >= beta) {
                ttRet = true;
            }

        }
    }

    return ret;

}



// Probe the transposition table
// Currently using: Always Replace
bool TranspositionTable::probeTTQsearch(uint64_t key, ZobristVal &hashedBoard, bool &ttRet, int alpha, int beta, int ply) {

    bool ret = false;
    if (hashTable[key % numHashes].posKey == key) {

        ret = true;
        // Store the hash table value
        hashedBoard = hashTable[key % numHashes];

        if (std::abs(hashedBoard.score) > MATE_VALUE_MAX) {
            if (hashedBoard.score > MATE_VALUE_MAX) {
                hashedBoard.score -= ply;
            }
        	else if (hashedBoard.score < -MATE_VALUE_MAX) {
                hashedBoard.score += ply;
            }
        }

        if (hashedBoard.flag == LOWER_BOUND) { // Low bound
            alpha = hashedBoard.score;
        }
        else if (hashedBoard.flag == UPPER_BOUND) { // Upper bound
            beta = hashedBoard.score;
        }
        else { //Exact
            ttRet = true;
        }

        if (alpha >= beta) {
            ttRet = true;
        }

    }

    return ret;

}



// Return the hash value of the position key
ZobristVal TranspositionTable::getHashValue(uint64_t posKey) {
    return hashTable[posKey % numHashes];
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
