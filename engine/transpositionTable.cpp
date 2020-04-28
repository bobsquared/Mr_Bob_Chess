
#include "transpositionTable.h"




TranspositionTable::TranspositionTable() {

  numHashes = (double) hashSize / (double) sizeof(ZobristVal) * 0xFFFFF;
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




TranspositionTable::~TranspositionTable() {
  delete [] hashTable;
}



void TranspositionTable::updateHalfMove() {
  halfMove++;
}



void TranspositionTable::saveTT(Bitboard::Move &move, int score, int depth, uint8_t flag, uint64_t key) {

  ttWrites++;
  uint64_t posKey = key % numHashes;
  // If there is a colision
  if (hashTable[posKey].posKey != 0) {

    // if (flag == 0) {
    //   ttOverwrites++;
    //   hashTable[posKey] = ZobristVal(move, (int16_t) score, (int8_t) depth, flag, key, halfMove);
    //   return;
    // }
    // else if (hashTable[posKey].flag != 0 && halfMove >= hashTable[posKey].halfMove + 2){
    //   ttOverwrites++;
    //   hashTable[posKey] = ZobristVal(move, (int16_t) score, (int8_t) depth, flag, key, halfMove);
    //   return;
    // }

    if (halfMove >= hashTable[posKey].halfMove + 2) {
      ttOverwrites++;
      hashTable[posKey] = ZobristVal(move, (int16_t) score, (int8_t) depth, flag, key, halfMove);
      return;
    }

    if (hashTable[posKey].depth > depth) {
      return;
    }

    ttOverwrites++;

  }

  hashTable[posKey] = ZobristVal(move, (int16_t) score, (int8_t) depth, flag, key, halfMove);

}



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



std::string TranspositionTable::getPV(Bitboard &bitboard) {

  std::string pv = "";
  std::vector<uint64_t> loopChecker;
  int numUndo = 0;

  while (true) {
    // Transposition table for duplicate detection:
    // Get the hash key
    uint64_t hashF = bitboard.getPosKey();
    loopChecker.push_back(hashF);

    if (std::count(loopChecker.begin(), loopChecker.end(), loopChecker.back()) >= 3) {
      break;
    }

    ZobristVal hashedBoard = ZobristVal();

    if (hashTable[hashF % numHashes].posKey == hashF) {

      hashedBoard = hashTable[hashF % numHashes];
      pv += " " + TO_ALG[hashedBoard.move.fromLoc] + TO_ALG[hashedBoard.move.toLoc];
      bitboard.movePiece(hashedBoard.move);
      numUndo++;
    }
    else {
      break;
    }

  }

  for (int j = 0; j < numUndo; j++) {
    bitboard.undoMove();
  }

  return pv;

}



void TranspositionTable::getHashStats() {

  std::cout << "ttHitRate: " << (double) ttHits / (double) ttCalls << " " << ttHits << " " << ttCalls << std::endl;
  std::cout << "ttOverwriteRate: " << (double) ttOverwrites / (double) ttWrites << " " << ttOverwrites << " " << ttWrites << std::endl;

  ttHits = 0;
  ttCalls = 0;

  ttOverwrites = 0;
  ttWrites = 0;

}
