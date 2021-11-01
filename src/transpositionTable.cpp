
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



// set transposition table size
void TranspositionTable::setSize(int hashSize) {
    delete [] hashTable;

    numHashes = (double) hashSize / (double) sizeof(ZobristVal) * 0xFFFFF;
    hashTable = new ZobristVal [numHashes];

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

    score += score > MATE_VALUE_MAX? ply : (score < -MATE_VALUE_MAX? -ply : 0);

    if (hashTable[posKey].posKey == 0) {
        th->ttWrites++;
        hashTable[posKey] = ZobristVal(move, (int16_t) score, (int16_t) staticScore, (int8_t) depth, flag, key, halfMove);
    }
    else if (halfMove != hashTable[posKey].halfMove || flag == 0 || depth >= hashTable[posKey].depth - 2 || (flag != 0 && depth >= hashTable[posKey].depth)) {
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

        hashedBoard.score += hashedBoard.score < -MATE_VALUE_MAX? ply : (hashedBoard.score > MATE_VALUE_MAX? -ply : 0);

        // Ensure hashedBoard depth >= current depth
        if (hashedBoard.depth >= depth) {

            alpha = hashedBoard.flag == LOWER_BOUND? hashedBoard.score : alpha; // Low bound
            beta = hashedBoard.flag == UPPER_BOUND? hashedBoard.score : beta; // upper bound

            if (hashedBoard.flag == EXACT || alpha >= beta) { // exact or alpha >= beta
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

        hashedBoard.score += hashedBoard.score < -MATE_VALUE_MAX? ply : (hashedBoard.score > MATE_VALUE_MAX? -ply : 0);

        alpha = hashedBoard.flag == LOWER_BOUND? hashedBoard.score : alpha; // Low bound
        beta = hashedBoard.flag == UPPER_BOUND? hashedBoard.score : beta; // upper bound

        if (hashedBoard.flag == EXACT || alpha >= beta) { // exact or alpha >= beta
            ttRet = true;
        }

    }

    return ret;

}



// Return the principal variation as a string.
// It returns the string as a list of moves, (ex. 'e2e4 e7e5 d2d4 e5d4')
std::string TranspositionTable::getPv(Bitboard &b) {

    std::string pv = "";
    std::vector<uint64_t> loopChecker;
    std::stack<MOVE> movesToUndo;

    while (true) {
        uint64_t posKey = b.getPosKey();
        loopChecker.push_back(posKey);

        if (std::count(loopChecker.begin(), loopChecker.end(), loopChecker.back()) >= 3) {
            break;
        }

        ZobristVal hashedBoard = hashTable[posKey % numHashes];
        if (hashedBoard.posKey == posKey) {
            movesToUndo.push(hashedBoard.move);
            pv += " " + moveToString(hashedBoard.move);
            b.make_move(hashedBoard.move);
        }
        else {
            break;
        }
    }

    while (!movesToUndo.empty()) {
        b.undo_move(movesToUndo.top());
        movesToUndo.pop();
    }

    return pv;
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
