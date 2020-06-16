
#include "transpositionTable.h"




// Initialize transposition table
TranspositionTable::TranspositionTable() {

    numHashes = (double) HASH_SIZE / (double) sizeof(ZobristVal) * 0xFFFFF;
    hashTable = new ZobristVal [numHashes];
    halfMove = 1;

    for (uint64_t i = 0; i < numHashes; i++) {
        hashTable[i] = ZobristVal();
    }

    ttHits = 0;
    ttCalls = 0;

    ttOverwrites = 0;
    ttWrites = 0;

}



// Delete the hash table
TranspositionTable::~TranspositionTable() {
    delete [] hashTable;
}



// Save the position into the transposition table
// Currently using: Always Replace
void TranspositionTable::saveTT(MOVE move, int score, int depth, uint8_t flag, uint64_t key) {
    ttWrites++;
    uint64_t posKey = key % numHashes;
    hashTable[posKey] = ZobristVal(move, (int16_t) score, (int8_t) depth, flag, key, halfMove);
}



// Probe the transposition table
// Currently using: Always Replace
bool TranspositionTable::probeTT(uint64_t key, ZobristVal &hashedBoard, int depth, bool &ttRet, int &alpha, int &beta) {

    ttCalls++;
    bool ret = false;
    if (hashTable[key % numHashes].posKey == key) {

        ret = true;
        ttHits++;
        // Store the hash table value
        hashedBoard = hashTable[key % numHashes];

        // Ensure hashedBoard depth >= current depth
        if (hashedBoard.depth >= depth) {

            if (hashedBoard.flag == 1) { // Low bound
                alpha = std::max(alpha, (int) hashedBoard.score);
            }
            else if (hashedBoard.flag == 2) { // Upper bound
                beta = std::min(beta, (int) hashedBoard.score);
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



// Return the hash value of the position key
ZobristVal TranspositionTable::getHashValue(uint64_t posKey) {
    return hashTable[posKey % numHashes];
}



// Print hash table statistics
void TranspositionTable::getHashStats() {

    std::cout << "ttHitRate: " << (double) ttHits / (double) ttCalls << " " << ttHits << " " << ttCalls << std::endl;
    std::cout << "ttOverwriteRate: " << (double) ttOverwrites / (double) ttWrites << " " << ttOverwrites << " " << ttWrites << std::endl;

    ttHits = 0;
    ttCalls = 0;

    ttOverwrites = 0;
    ttWrites = 0;

}
