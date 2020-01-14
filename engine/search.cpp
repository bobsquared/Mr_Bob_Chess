#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <atomic>
#include "bitboard.h"
#include "zobrist_hashing.h"

uint64_t traversedNodes = 0;
uint64_t pruning = 0;
uint64_t pruningTotal = 0;
const int MATE_VALUE = 100000;

std::atomic<bool> exit_thread_flag{false};

const std::string TO_ALG[64] = {
  "a1",
  "b1",
  "c1",
  "d1",
  "e1",
  "f1",
  "g1",
  "h1",
  "a2",
  "b2",
  "c2",
  "d2",
  "e2",
  "f2",
  "g2",
  "h2",
  "a3",
  "b3",
  "c3",
  "d3",
  "e3",
  "f3",
  "g3",
  "h3",
  "a4",
  "b4",
  "c4",
  "d4",
  "e4",
  "f4",
  "g4",
  "h4",
  "a5",
  "b5",
  "c5",
  "d5",
  "e5",
  "f5",
  "g5",
  "h5",
  "a6",
  "b6",
  "c6",
  "d6",
  "e6",
  "f6",
  "g6",
  "h6",
  "a7",
  "b7",
  "c7",
  "d7",
  "e7",
  "f7",
  "g7",
  "h7",
  "a8",
  "b8",
  "c8",
  "d8",
  "e8",
  "f8",
  "g8",
  "h8"

};


std::unordered_map<std::string, uint8_t> TO_NUM = {
  {"a1", 0},
  {"b1", 1},
  {"c1", 2},
  {"d1", 3},
  {"e1", 4},
  {"f1", 5},
  {"g1", 6},
  {"h1", 7},
  {"a2", 8},
  {"b2", 9},
  {"c2", 10},
  {"d2", 11},
  {"e2", 12},
  {"f2", 13},
  {"g2", 14},
  {"h2", 15},
  {"a3", 16},
  {"b3", 17},
  {"c3", 18},
  {"d3", 19},
  {"e3", 20},
  {"f3", 21},
  {"g3", 22},
  {"h3", 23},
  {"a4", 24},
  {"b4", 25},
  {"c4", 26},
  {"d4", 27},
  {"e4", 28},
  {"f4", 29},
  {"g4", 30},
  {"h4", 31},
  {"a5", 32},
  {"b5", 33},
  {"c5", 34},
  {"d5", 35},
  {"e5", 36},
  {"f5", 37},
  {"g5", 38},
  {"h5", 39},
  {"a6", 40},
  {"b6", 41},
  {"c6", 42},
  {"d6", 43},
  {"e6", 44},
  {"f6", 45},
  {"g6", 46},
  {"h6", 47},
  {"a7", 48},
  {"b7", 49},
  {"c7", 50},
  {"d7", 51},
  {"e7", 52},
  {"f7", 53},
  {"g7", 54},
  {"h7", 55},
  {"a8", 56},
  {"b8", 57},
  {"c8", 58},
  {"d8", 59},
  {"e8", 60},
  {"f8", 61},
  {"g8", 62},
  {"h8", 63}

};


int quiesceSearch(bool useMax, Bitboard &bitboard, int alpha, int beta, int depth=2) {
  traversedNodes++;

  if (depth <= 0) {
    return bitboard.evaluate();
  }

  int stand_pat = bitboard.evaluate();
  if (stand_pat >= beta) {
    return stand_pat;
  }

  if (alpha < stand_pat) {
    alpha = stand_pat;
  }

  std::vector<Bitboard::Move> vMoves = bitboard.allValidMoves(!useMax);
  int ret = stand_pat;
  for (Bitboard::Move move : vMoves) {

    if (move.quiet) {
      continue;
    }

    bitboard.movePiece(move.fromLoc, move.toLoc);

    if (!bitboard.filterCheck(!useMax)) {
      bitboard.undoMove();
      continue;
    }

    ret = -quiesceSearch(!useMax, bitboard, -beta, -alpha, depth - 1);
    bitboard.undoMove();

    alpha = std::max(ret, alpha);
    if (ret >= beta) {
      return beta;
    }

    if (ret > alpha) {
      alpha = ret;
    }


  }

  return ret;

}




int minimaxR(bool useMax, Bitboard &bitboard, int depth) {
  traversedNodes++;
  // std::cout << "depth: " << depth << std::endl;

  if (depth <= 0) {
    return bitboard.evaluate();
  }


  if (useMax) {
    int res = -1000000;


    std::vector<Bitboard::Move> vMoves = bitboard.allValidMoves(0);

    for (Bitboard::Move loc : vMoves) {

      bitboard.movePiece(loc.fromLoc, loc.toLoc);

      if (!bitboard.filterCheck(0)) {
        bitboard.undoMove();
        continue;
      }

      int ret = minimaxR(false, bitboard, depth - 1);
      bitboard.undoMove();

      if (ret > res) {
        res = ret;
      }

    }

    return res;
  }
  else {
    int res = 1000000;
    std::vector<Bitboard::Move> vMoves = bitboard.allValidMoves(true);

    for (Bitboard::Move loc : vMoves) {

      bitboard.movePiece(loc.fromLoc, loc.toLoc);

      if (!bitboard.filterCheck(1)) {
        bitboard.undoMove();
        continue;
      }

      int ret = minimaxR(true, bitboard, depth - 1);
      bitboard.undoMove();

      if (ret < res) {
        res = ret;
      }

    }

    return res;
  }

}


std::string minimaxRoot(bool useMax, Bitboard &bitboard, int depth) {

  std::string bestMove = "";
  if (useMax) {
    int res = -1000000;
    std::vector<Bitboard::Move> vMoves = bitboard.allValidMoves(0);

    for (Bitboard::Move loc : vMoves) {

      bitboard.movePiece(loc.fromLoc, loc.toLoc);

      if (!bitboard.filterCheck(0)) {
        bitboard.undoMove();
        continue;
      }

      int ret = minimaxR(false, bitboard, depth - 1);
      bitboard.undoMove();

      if (ret > res) {
        res = ret;
        bestMove = TO_ALG[loc.fromLoc] + TO_ALG[loc.toLoc];
      }


    }
    return bestMove;
  }
  else {
    int res = 1000000;
    std::vector<Bitboard::Move> vMoves = bitboard.allValidMoves(1);

    for (Bitboard::Move loc : vMoves) {

      bitboard.movePiece(loc.fromLoc, loc.toLoc);

      if (!bitboard.filterCheck(1)) {
        bitboard.undoMove();
        continue;
      }

      int ret = minimaxR(true, bitboard, depth - 1);
      bitboard.undoMove();

      if (ret < res) {
        res = ret;
        bestMove = TO_ALG[loc.fromLoc] + TO_ALG[loc.toLoc];
      }

    }

    return bestMove;
  }

}





int alphabetaR(bool useMax, Bitboard &bitboard, int depth, int alpha, int beta, int maxDepth) {
  traversedNodes++;
  int origAlpha = alpha;
  int origBeta = beta;
  uint8_t R = 3 + (depth / 8); // Null move reduction.

  if (depth <= 0 || exit_thread_flag) {
    // return quiesceSearch(useMax, bitboard, alpha, beta);
    return bitboard.evaluate();
  }

  uint64_t hashF = bitboard.hashBoard(useMax);
  Bitboard::ZobristVal hashedBoard = Bitboard::ZobristVal();
  if (bitboard.lookup.find(hashF) != bitboard.lookup.end()) {
    hashedBoard = bitboard.lookup[hashF];

    if (hashedBoard.depth >= depth) {
      if (hashedBoard.flag == 0) { //Exact
        return hashedBoard.score;
      }
      else if (hashedBoard.flag == 1) { // Low bound
        alpha = std::max(alpha, hashedBoard.score);
      }
      else if (hashedBoard.flag == 2) { // Upper bound
        beta = std::min(beta, hashedBoard.score);
      }

      if (alpha >= beta) {
        return hashedBoard.score;
      }
    }
  }

  Bitboard::Move bestMove = Bitboard::Move{};

  if (useMax) {
    bool nullMoves = true;
    int ret = -10000000;
    bool isCheck = !bitboard.filterCheck(0);

    std::vector<Bitboard::Move> vMoves = bitboard.allValidMoves(0);

    bitboard.sortMoves(vMoves, hashedBoard.move, depth);

    uint8_t iteration = 0;
    for (Bitboard::Move move : vMoves) {

      if (move.fromLoc == 65 && move.toLoc == 65 && isCheck && !bitboard.canNullMove()) { // If It's null move and in check
        continue;
      }

      bitboard.movePiece(move.fromLoc, move.toLoc);

      if (!bitboard.filterCheck(0)) {
        bitboard.undoMove();
        continue;
      }



      if (move.fromLoc == 65 && move.toLoc == 65) {
        ret = std::max(ret, alphabetaR(false, bitboard, depth - R - 1, alpha, beta, maxDepth));
      }
      else if (maxDepth >= 3 && iteration > 1 && move.quiet && !isCheck){
        ret = std::max(ret, alphabetaR(false, bitboard, depth - 2, alpha, beta, maxDepth));

        if (ret > alpha) {
          ret = std::max(ret, alphabetaR(false, bitboard, depth - 1, alpha, beta, maxDepth));
        }
      }
      else {
        ret = std::max(ret, alphabetaR(false, bitboard, depth - 1, alpha, beta, maxDepth));
      }

      // Moves
      iteration++;
      bitboard.undoMove();

      if (ret >= beta) { //Cut off

        if (move.quiet) {
          bitboard.InsertKiller(move, depth);
        }
        if (move.fromLoc != 65 && move.toLoc != 65) { //lower bound
          bitboard.InsertLookup(move, ret, alpha, beta, depth, 1, hashF);
        }
        if (iteration == 1) {
          pruning++;
          pruningTotal++;
        }

        return beta;
      }

      if (iteration == 1) {
        pruningTotal++;
      }

      if (move.fromLoc == 65 && move.toLoc == 65) {
        continue;
      }

      nullMoves = false;

      if (ret > alpha) {
        alpha = ret;
        bestMove = move;
      }

    }

    if (nullMoves) {
      if (isCheck) {
        return -MATE_VALUE - depth;
      }
      else {
        return 0;
      }
    }

    if (alpha == origAlpha) {
      //upper bound
      bitboard.InsertLookup(bestMove, ret, alpha, beta, depth, 2, hashF);

    }
    else {
      //exact
      bitboard.InsertLookup(bestMove, ret, alpha, beta, depth, 0, hashF);
    }

    return alpha;
  }
  else {
    bool nullMoves = true;
    int ret = 10000000;
    bool isCheck = !bitboard.filterCheck(1);

    std::vector<Bitboard::Move> vMoves = bitboard.allValidMoves(1);
    bitboard.sortMoves(vMoves, hashedBoard.move, depth);
    int iteration = 0;

    for (Bitboard::Move move : vMoves) {

      if (move.fromLoc == 65 && move.toLoc == 65 && isCheck && !bitboard.canNullMove()) {
        continue;
      }

      bitboard.movePiece(move.fromLoc, move.toLoc);

      if (!bitboard.filterCheck(1)) {
        bitboard.undoMove();
        continue;
      }


      // Null Move Pruning
      if (move.fromLoc == 65 && move.toLoc == 65) {
        ret = std::min(ret, alphabetaR(true, bitboard, depth - R - 1, alpha, beta, maxDepth));
      }
      else if (maxDepth >= 3 && iteration > 1 && move.quiet && !isCheck) {
        ret = std::min(ret, alphabetaR(true, bitboard, depth - 2, alpha, beta, maxDepth));

        if (ret < beta) {
          ret = std::min(ret, alphabetaR(true, bitboard, depth - 1, alpha, beta, maxDepth));
        }
      }
      else {
        ret = std::min(ret, alphabetaR(true, bitboard, depth - 1, alpha, beta, maxDepth));
      }


      iteration++;
      bitboard.undoMove();


      if (ret <= alpha) {
        if (move.quiet) {
          bitboard.InsertKiller(move, depth);
        }

        if (move.fromLoc != 65 && move.toLoc != 65) {
          bitboard.InsertLookup(move, ret, alpha, beta, depth, 2, hashF);
        }
        if (iteration == 1) {
          pruning++;
          pruningTotal++;
        }

        return alpha;
      }

      if (iteration == 1) {
        pruningTotal++;
      }



      if (move.fromLoc == 65 && move.toLoc == 65) {
        continue;
      }

      nullMoves = false;

      if (ret < beta) {
        beta = ret;
        bestMove = move;
      }

    }

    if (nullMoves) {

      if (isCheck) {
        return MATE_VALUE + depth;
      }
      else {
        return 0;
      }
    }

    if (beta == origBeta) {
      //upper bound
      bitboard.InsertLookup(bestMove, ret, alpha, beta, depth, 1, hashF);

    }
    else {
      //exact
      bitboard.InsertLookup(bestMove, ret, alpha, beta, depth, 0, hashF);
      // bitboard.PVMoves.push_back(bestMove);

    }
    return beta;

  }

}

struct ReturnInfo {
  std::string bestMove;
  int score;
  int mateIn;
};



ReturnInfo alphabetaRoot(bool useMax, Bitboard &bitboard, int depth, int maxDepth, int alpha=-1000000, int beta=1000000) {
  int origBeta = beta;
  int origAlpha = alpha;

  std::string bestMove = "HAHA";
  int mateIn = 0;


  uint64_t hashF = bitboard.hashBoard(useMax);
  Bitboard::ZobristVal hashedBoard = Bitboard::ZobristVal();
  if (bitboard.lookup.find(hashF) != bitboard.lookup.end()) {
    hashedBoard = bitboard.lookup[hashF];

    if (hashedBoard.depth >= depth) {
      if (hashedBoard.flag == 0) { //Exact
        if (useMax && hashedBoard.score >= MATE_VALUE) {
          return ReturnInfo{TO_ALG[hashedBoard.move.fromLoc] + TO_ALG[hashedBoard.move.toLoc], hashedBoard.score, hashedBoard.score - MATE_VALUE};
        }
        else if (useMax && hashedBoard.score <= -MATE_VALUE) {
          return ReturnInfo{TO_ALG[hashedBoard.move.fromLoc] + TO_ALG[hashedBoard.move.toLoc], hashedBoard.score, -(hashedBoard.score + MATE_VALUE)};
        }
        else if (!useMax && hashedBoard.score <= -MATE_VALUE) {
          return ReturnInfo{TO_ALG[hashedBoard.move.fromLoc] + TO_ALG[hashedBoard.move.toLoc], hashedBoard.score, (hashedBoard.score + MATE_VALUE)};
        }
        else if (!useMax && hashedBoard.score >= -MATE_VALUE) {
          return ReturnInfo{TO_ALG[hashedBoard.move.fromLoc] + TO_ALG[hashedBoard.move.toLoc], hashedBoard.score, -(hashedBoard.score + MATE_VALUE)};
        }
        else {
          return ReturnInfo{TO_ALG[hashedBoard.move.fromLoc] + TO_ALG[hashedBoard.move.toLoc], hashedBoard.score, 0};
        }

      }
      else if (hashedBoard.flag == 1) { // Low bound
        alpha = std::max(alpha, hashedBoard.score);
      }
      else if (hashedBoard.flag == 2) { // Upper bound
        beta = std::min(beta, hashedBoard.score);
      }
    }
  }



  Bitboard::Move bestMoveM = Bitboard::Move{};
  if (useMax) {
    int ret = -10000000;

    std::vector<Bitboard::Move> vMoves = bitboard.allValidMoves(0);
    bitboard.sortMoves(vMoves, hashedBoard.move, depth);

    for (Bitboard::Move move : vMoves) {

      if (move.fromLoc == 65 && move.toLoc == 65) {
        continue;
      }

      bitboard.movePiece(move.fromLoc, move.toLoc);


      if (!bitboard.filterCheck(0)) {
        bitboard.undoMove();
        continue;
      }

      ret = std::max(ret, alphabetaR(false, bitboard, depth - 1, alpha, beta, maxDepth));


      bitboard.undoMove();

      if (ret > alpha) {
        alpha = ret;
        bestMove = TO_ALG[move.fromLoc] + TO_ALG[move.toLoc];
        bestMoveM = move;
      }



      if (ret >= beta) {
        if (move.quiet) {
          bitboard.InsertKiller(move, depth);
        }
        bitboard.InsertLookup(move, ret, alpha, beta, depth, 1, hashF);

        if (ret <= -MATE_VALUE) {
          mateIn = (ret + MATE_VALUE);
        }
        else if (ret >= MATE_VALUE) {
          mateIn = (ret - MATE_VALUE);
        }

        return ReturnInfo{bestMove, ret, mateIn};
      }

    }

    if (alpha == origAlpha) {
      //upper bound
      bitboard.InsertLookup(bestMoveM, ret, alpha, beta, depth, 2, hashF);
    }
    else {
      //exact
      bitboard.InsertLookup(bestMoveM, ret, alpha, beta, depth, 0, hashF);
    }

    if (ret <= -MATE_VALUE) {
      mateIn = (ret + MATE_VALUE);
    }
    else if (ret >= MATE_VALUE) {
      mateIn = (ret - MATE_VALUE);
    }

    return ReturnInfo{bestMove, ret, mateIn};
  }
  else {

    int ret = 10000000;
    std::vector<Bitboard::Move> vMoves = bitboard.allValidMoves(1);
    bitboard.sortMoves(vMoves, hashedBoard.move, depth);

    for (Bitboard::Move move : vMoves) {

      if (move.fromLoc == 65 && move.toLoc == 65) {
        continue;
      }

      bitboard.movePiece(move.fromLoc, move.toLoc);

      if (!bitboard.filterCheck(1)) {
        bitboard.undoMove();
        continue;
      }

      // ret = alphabetaR(true, bitboard, depth - 1, alpha, beta, maxDepth);
      ret = std::min(ret, alphabetaR(true, bitboard, depth - 1, alpha, beta, maxDepth));
      bitboard.undoMove();

      if (ret < beta) {
        beta = ret;
        bestMove = TO_ALG[move.fromLoc] + TO_ALG[move.toLoc];
        bestMoveM = move;
      }

      if (ret <= alpha) {
        if (move.quiet) {
          bitboard.InsertKiller(move, depth);
        }

        bitboard.InsertLookup(move, ret, alpha, beta, depth, 2, hashF);

        if (ret <= -MATE_VALUE) {
          mateIn = -(ret + MATE_VALUE);
        }
        else if (ret >= MATE_VALUE) {
          mateIn = -(ret - MATE_VALUE);
        }

        return ReturnInfo{bestMove, ret, mateIn};
      }

    }

    if (beta == origBeta) {
      //upper bound
      bitboard.InsertLookup(bestMoveM, ret, alpha, beta, depth, 1, hashF);
    }
    else {
      //exact
      bitboard.InsertLookup(bestMoveM, ret, alpha, beta, depth, 0, hashF);
      // bitboard.PVMoves.push_back(bestMove);

    }

    if (ret <= -MATE_VALUE) {
      mateIn = -(ret + MATE_VALUE);
    }
    else if (ret >= MATE_VALUE) {
      mateIn = -(ret - MATE_VALUE);
    }

    return ReturnInfo{bestMove, ret, mateIn};
  }

}
