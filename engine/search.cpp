#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <atomic>
#include "bitboard.h"
#include "zobrist_hashing.h"
#include <cassert>


uint64_t traversedNodes = 0;
const int MATE_VALUE = 100000;

// Debugging variables
uint64_t pruning = 0;
uint64_t pruningTotal = 0;
//---------------------

// Atomic boolean for multithreading stop command
std::atomic<bool> exit_thread_flag{false};


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
std::unordered_map<std::string, uint8_t> TO_NUM = {
  {"a1", 0},{"b1", 1},{"c1", 2},{"d1", 3},{"e1", 4},{"f1", 5},{"g1", 6},{"h1", 7},
  {"a2", 8},{"b2", 9},{"c2", 10},{"d2", 11},{"e2", 12},{"f2", 13},{"g2", 14},{"h2", 15},
  {"a3", 16},{"b3", 17},{"c3", 18},{"d3", 19},{"e3", 20},{"f3", 21},{"g3", 22},{"h3", 23},
  {"a4", 24},{"b4", 25},{"c4", 26},{"d4", 27},{"e4", 28},{"f4", 29},{"g4", 30},{"h4", 31},
  {"a5", 32},{"b5", 33},{"c5", 34},{"d5", 35},{"e5", 36},{"f5", 37},{"g5", 38},{"h5", 39},
  {"a6", 40},{"b6", 41},{"c6", 42},{"d6", 43},{"e6", 44},{"f6", 45},{"g6", 46},{"h6", 47},
  {"a7", 48},{"b7", 49},{"c7", 50},{"d7", 51},{"e7", 52},{"f7", 53},{"g7", 54},{"h7", 55},
  {"a8", 56},{"b8", 57},{"c8", 58},{"d8", 59},{"e8", 60},{"f8", 61},{"g8", 62},{"h8", 63}
};

struct ReturnInfo {
  std::string bestMove;
  int score;
  int mateIn;
};






// Quiesce Search is needed to minimize the horizon effect.
// Adapted from https://www.chessprogramming.org/Quiescence_Search
int quiesceSearch(bool useMax, Bitboard &bitboard, int alpha, int beta, int depth=0) {
  int origAlpha = alpha;
  int origBeta = beta;
  traversedNodes++;

  if (exit_thread_flag) {
    return 0;
  }

  // Transposition table:
  // Get the hash key
  uint64_t hashF = bitboard.getPosKey();
  Bitboard::ZobristVal hashedBoard = Bitboard::ZobristVal();
  if (bitboard.lookup.find(hashF) != bitboard.lookup.end()) {
    // Store the hash table value
    hashedBoard = bitboard.lookup[hashF];

    // Ensure hashedBoard depth >= current depth
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


  int stand_pat = bitboard.evaluate();
  if (useMax) {

    if (stand_pat >= beta) {
      return beta;
    }

    if (alpha < stand_pat) {
      alpha = stand_pat;
    }

    std::vector<Bitboard::Move> vMoves = bitboard.allValidCaptures(0);
    std::sort(vMoves.begin(), vMoves.end());
    Bitboard::Move bestMove;
    int ret = stand_pat;
    for (Bitboard::Move move : vMoves) {


      bitboard.movePiece(move.fromLoc, move.toLoc);

      if (!bitboard.filterCheck(0)) {
        bitboard.undoMove();
        continue;
      }

      ret = std::max(ret, quiesceSearch(false, bitboard, alpha, beta, depth - 1));
      bitboard.undoMove();

      if (ret >= beta) {
        bitboard.InsertLookup(move, ret, alpha, beta, depth, 1, hashF);
        return beta;
      }

      if (ret > alpha) {
        alpha = ret;
        bestMove = move;
      }

    }

    //All Nodes
    if (alpha == origAlpha) {
      //upper bound
      bitboard.InsertLookup(bestMove, ret, alpha, beta, depth, 2, hashF);

    }
    // PV Node
    else {
      //exact

      bitboard.InsertLookup(bestMove, ret, alpha, beta, depth, 0, hashF);
    }

    return alpha;

  }
  else {

    if (stand_pat <= alpha) {
      return alpha;
    }

    if (beta > stand_pat) {
      beta = stand_pat;
    }

    std::vector<Bitboard::Move> vMoves = bitboard.allValidCaptures(1);
    std::sort(vMoves.begin(), vMoves.end());
    Bitboard::Move bestMove;
    int ret = stand_pat;
    for (Bitboard::Move move : vMoves) {


      bitboard.movePiece(move.fromLoc, move.toLoc);

      if (!bitboard.filterCheck(1)) {
        bitboard.undoMove();
        continue;
      }

      ret = std::min(ret, quiesceSearch(true, bitboard, alpha, beta, depth - 1));
      bitboard.undoMove();

      if (ret <= alpha) {
        bitboard.InsertLookup(move, ret, alpha, beta, depth, 2, hashF);
        return alpha;
      }

      if (ret < beta) {
        beta = ret;
        bestMove = move;
      }

    }

    //All Nodes
    if (beta == origBeta) {
      //upper bound
      bitboard.InsertLookup(bestMove, ret, alpha, beta, depth, 1, hashF);

    }
    // PV node
    else {
      //exact
      bitboard.InsertLookup(bestMove, ret, alpha, beta, depth, 0, hashF);
      // bitboard.PVMoves.push_back(bestMove);

    }

    return beta;

  }



}






// Minimax algorithm
int minimaxR(bool useMax, Bitboard &bitboard, int depth) {
  traversedNodes++;

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






// Minimax root node
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






// Alpha beta pruning to find the best move
// Return only the score, and return more info in the root node
int alphabetaR(bool useMax, Bitboard &bitboard, int depth, int alpha, int beta, int maxDepth) {



  if (exit_thread_flag) {
    return 0;
  }

  // If depth <= 0 or stop command is called, then evaluate and return
  if (depth <= 0) {
    return quiesceSearch(useMax, bitboard, alpha, beta);
    // return bitboard.evaluate();
  }

  traversedNodes++;

  // Original alpha and beta for transposition table
  int origAlpha = alpha;
  int origBeta = beta;
  bool hashed = false;

  // How much to reduce depth by.
  uint8_t R = 3 + (depth / 8);

  // Store the best move
  Bitboard::Move bestMove;


  // Transposition table:
  // Get the hash key
  uint64_t hashF = bitboard.getPosKey();
  Bitboard::ZobristVal hashedBoard = Bitboard::ZobristVal();
  if (bitboard.lookup.find(hashF) != bitboard.lookup.end()) {
    hashed = true;
    // Store the hash table value
    hashedBoard = bitboard.lookup[hashF];

    // Ensure hashedBoard depth >= current depth
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



  // If it is white to move
  if (useMax) {

    // Flag to check if a legal move is found (true = at least one legal move)
    bool nullMoves = true;
    int ret = -10000000;
    uint8_t iteration = 0;

    if (hashed && bitboard.IsMoveWhite(hashedBoard.move.fromLoc, hashedBoard.move.toLoc)) {
      bitboard.movePiece(hashedBoard.move.fromLoc, hashedBoard.move.toLoc);
      if (!bitboard.filterCheck(0)) {
        bitboard.undoMove();
      }
      else {

        // If three fold repetition, score a zero
        if (bitboard.isThreeFold()) {
          ret = std::max(ret, 0);
        }
        else {
          ret = std::max(ret, alphabetaR(false, bitboard, depth - 1, alpha, beta, maxDepth));
        }

        iteration++;
        bitboard.undoMove();

        if (ret >= beta) { //Cut off

          // Insert killer moves is move is a quiet one
          if (hashedBoard.move.quiet) {
            bitboard.InsertKiller(hashedBoard.move, depth);
            if (hashedBoard.move.quiet && hashedBoard.move.fromLoc < 64 && hashedBoard.move.toLoc < 64) {
              bitboard.history[hashedBoard.move.fromLoc][hashedBoard.move.toLoc] += depth * depth;
            }
          }

          pruning++;
          pruningTotal++;

          bitboard.InsertLookup(hashedBoard.move, ret, alpha, beta, depth, 1, hashF);

          return beta;
        }

        pruningTotal++;
        nullMoves = false;

        // Update the best move and alpha
        if (ret > alpha) {
          alpha = ret;
          bestMove = hashedBoard.move;
        }
      }
    }

    // Is player currently in check
    bool isCheck = !bitboard.filterCheck(0);

    // Generate and sort all moves
    std::vector<Bitboard::Move> vMoves = bitboard.allValidMoves(0);
    bitboard.sortMoves(vMoves, hashedBoard.move, depth);

    // Loop through all moves
    for (Bitboard::Move move : vMoves) {

      if (exit_thread_flag) {
        return 0;
      }

      if (move.fromLoc == 65 && move.toLoc == 65 && isCheck && !bitboard.canNullMove()) { // If It's null move and in check
        continue;
      }

      // Move piece
      bitboard.movePiece(move.fromLoc, move.toLoc);

      // If player is in check, undo move and continue
      bool giveCheck = !bitboard.filterCheck(1);
      if (!bitboard.filterCheck(0)) {
        bitboard.undoMove();
        continue;
      }


      // If three fold repetition, score a zero
      if (bitboard.isThreeFold()) {
        ret = std::max(ret, 0);
      }
      // extend if there's a check
      else if (giveCheck) {
        ret = std::max(ret, alphabetaR(false, bitboard, depth, alpha, beta, maxDepth));
      }
      // If null move is found, reduce the depth of the search
      else if (move.fromLoc == 65 && move.toLoc == 65) {
        ret = std::max(ret, alphabetaR(false, bitboard, depth - R - 1, alpha, beta, maxDepth));
      }
      // Late move reduction
      else if (maxDepth >= 3 && iteration > 0 && move.quiet && !isCheck && !giveCheck){

        if (iteration > 25) {
          ret = std::max(ret, alphabetaR(false, bitboard, depth - R - 1, alpha, beta, maxDepth));
        }
        else if (iteration > 15) {
          ret = std::max(ret, alphabetaR(false, bitboard, depth - 2 - 1, alpha, beta, maxDepth));
        }
        else {
          ret = std::max(ret, alphabetaR(false, bitboard, depth - 1 - 1, alpha, beta, maxDepth));
        }


        if (ret > alpha) {
          ret = std::max(ret, alphabetaR(false, bitboard, depth - 1, alpha, beta, maxDepth));
        }
      }
      // Else do a normal search
      else {
        ret = std::max(ret, alphabetaR(false, bitboard, depth - 1, alpha, beta, maxDepth));
      }

      // undo move
      iteration++;
      bitboard.undoMove();


      if (ret >= beta) { //Cut off

        if (move.fromLoc != 65 && move.toLoc != 65) {
          // Insert killer moves is move is a quiet one
          if (move.quiet) {
            bitboard.InsertKiller(move, depth);
            if (move.quiet && move.fromLoc < 64 && move.toLoc < 64) {
              bitboard.history[move.fromLoc][move.toLoc] += depth * depth;
            }
          }
          // If it's not a null move, insert into transposition table
          //lower bound
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


      // If it's a null move, continue as we do not want to save this move as the best move
      if (move.fromLoc == 65 && move.toLoc == 65) {
        continue;
      }

      nullMoves = false;

      // Update the best move and alpha
      if (ret > alpha) {
        alpha = ret;
        bestMove = move;
      }

    }

    // If no legal move is found then a check should be done to see if king is in check
    // If king is in check, then this is check mate.
    // Else it is stalemate
    if (nullMoves) {
      if (isCheck) {
        return -MATE_VALUE - depth;
      }
      else {
        return 0;
      }
    }

    //All Nodes
    if (alpha == origAlpha) {
      //upper bound
      bitboard.InsertLookup(bestMove, ret, alpha, beta, depth, 2, hashF);

    }
    // PV Node
    else {
      //exact
      if (bestMove.quiet && bestMove.fromLoc < 64 && bestMove.toLoc < 64) {
        bitboard.history[bestMove.fromLoc][bestMove.toLoc] += depth * depth;
      }

      bitboard.InsertLookup(bestMove, ret, alpha, beta, depth, 0, hashF);
    }

    return alpha;
  }
  // Else player is black
  else {

    // Flag to check if a legal move is found (true = at least one legal move)
    bool nullMoves = true;
    int ret = 10000000;
    int iteration = 0;

    if (hashed && bitboard.IsMoveBlack(hashedBoard.move.fromLoc, hashedBoard.move.toLoc)) {
      bitboard.movePiece(hashedBoard.move.fromLoc, hashedBoard.move.toLoc);
      if (!bitboard.filterCheck(1)) {
        bitboard.undoMove();
      }
      else {

        // If three fold repetition, score a zero
        if (bitboard.isThreeFold()) {
          ret = std::min(ret, 0);
        }
        else {
          ret = std::min(ret, alphabetaR(true, bitboard, depth - 1, alpha, beta, maxDepth));
        }

        iteration++;
        bitboard.undoMove();

        if (ret <= alpha) { //Cut off

          // Insert killer moves is move is a quiet one
          if (hashedBoard.move.quiet) {
            bitboard.InsertKiller(hashedBoard.move, depth);
            if (hashedBoard.move.quiet && hashedBoard.move.fromLoc < 64 && hashedBoard.move.toLoc < 64) {
              bitboard.history[hashedBoard.move.fromLoc][hashedBoard.move.toLoc] += depth * depth;
            }
          }

          pruning++;
          pruningTotal++;

          bitboard.InsertLookup(hashedBoard.move, ret, alpha, beta, depth, 2, hashF);

          return alpha;
        }
        pruningTotal++;
        nullMoves = false;

        // Update the best move and alpha
        if (ret < beta) {
          beta = ret;
          bestMove = hashedBoard.move;
        }
      }
    }



    // Is player currently in check
    bool isCheck = !bitboard.filterCheck(1);

    // Generate and sort all moves
    std::vector<Bitboard::Move> vMoves = bitboard.allValidMoves(1);
    bitboard.sortMoves(vMoves, hashedBoard.move, depth);

    // Loop through all moves
    for (Bitboard::Move move : vMoves) {

      if (exit_thread_flag) {
        return 0;
      }

      if (move.fromLoc == 65 && move.toLoc == 65 && isCheck && !bitboard.canNullMove()) {
        continue;
      }

      // Move piece
      bitboard.movePiece(move.fromLoc, move.toLoc);

      // If player is in check, undo move and continue
      bool giveCheck = !bitboard.filterCheck(0);
      if (!bitboard.filterCheck(1)) {
        bitboard.undoMove();
        continue;
      }


      // If three fold repetition, score a zero
      if (bitboard.isThreeFold()) {
        ret = std::min(ret, 0);
      }
      else if (giveCheck) {
        ret = std::min(ret, alphabetaR(true, bitboard, depth, alpha, beta, maxDepth));
      }
      // If null move is found, reduce the depth of the search
      else if (move.fromLoc == 65 && move.toLoc == 65) {
        ret = std::min(ret, alphabetaR(true, bitboard, depth - R - 1, alpha, beta, maxDepth));
      }
      // Late move reduction
      else if (maxDepth >= 3 && iteration > 0 && move.quiet && !isCheck && !giveCheck) {

        if (iteration > 25) {
          ret = std::min(ret, alphabetaR(true, bitboard, depth - R - 1, alpha, beta, maxDepth));
        }
        else if (iteration > 15) {
          ret = std::min(ret, alphabetaR(true, bitboard, depth - 2 - 1, alpha, beta, maxDepth));
        }
        else {
          ret = std::min(ret, alphabetaR(true, bitboard, depth - 1 - 1, alpha, beta, maxDepth));
        }


        if (ret < beta) {
          ret = std::min(ret, alphabetaR(true, bitboard, depth - 1, alpha, beta, maxDepth));
        }
      }
      // Else do a normal search
      else {
        ret = std::min(ret, alphabetaR(true, bitboard, depth - 1, alpha, beta, maxDepth));
      }

      // undo move
      iteration++;
      bitboard.undoMove();


      // Cutoff
      if (ret <= alpha) {

        if (move.fromLoc != 65 && move.toLoc != 65) {
          // Insert killer moves is move is a quiet one
          if (move.quiet) {
            bitboard.InsertKiller(move, depth);
            if (move.quiet && move.fromLoc < 64 && move.toLoc < 64) {
              bitboard.history[move.fromLoc][move.toLoc] += depth * depth;
            }
          }

          // If it's not a null move, insert into transposition table

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

      // If it's a null move, continue as we do not want to save this move as the best move
      if (move.fromLoc == 65 && move.toLoc == 65) {
        continue;
      }

      nullMoves = false;

      // Update the best move and beta
      if (ret < beta) {
        beta = ret;
        bestMove = move;
      }

    }


    // If no legal move is found then a check should be done to see if king is in check
    // If king is in check, then this is check mate.
    // Else it is stalemate
    if (nullMoves) {

      if (isCheck) {
        return MATE_VALUE + depth;
      }
      else {
        return 0;
      }
    }



    //All Nodes
    if (beta == origBeta) {
      //upper bound
      bitboard.InsertLookup(bestMove, ret, alpha, beta, depth, 1, hashF);

    }
    // PV node
    else {
      //exact
      if (bestMove.quiet && bestMove.fromLoc < 64 && bestMove.toLoc < 64) {
        bitboard.history[bestMove.fromLoc][bestMove.toLoc] += depth * depth;
      }
      bitboard.InsertLookup(bestMove, ret, alpha, beta, depth, 0, hashF);
      // bitboard.PVMoves.push_back(bestMove);

    }
    return beta;

  }

}





ReturnInfo alphabetaRoot(bool useMax, Bitboard &bitboard, int depth, int maxDepth, int alpha=-1000000, int beta=1000000) {

  // Keep track of origina beta and alpha
  int origBeta = beta;
  int origAlpha = alpha;
  uint8_t numMoves = 0;
  bool hashed = false;

  // Store best move and mate in somewhere
  std::string bestMove = "";
  int mateIn = 0;


  // Transposition table
  uint64_t hashF = bitboard.getPosKey();
  Bitboard::ZobristVal hashedBoard = Bitboard::ZobristVal();
  if (bitboard.lookup.find(hashF) != bitboard.lookup.end()) {
    hashed = true;
    hashedBoard = bitboard.lookup[hashF];

    if (hashedBoard.depth >= depth) {
      if (hashedBoard.flag == 0) { //Exact

        if (hashedBoard.score <= MATE_VALUE - 100) {
          mateIn = (hashedBoard.score - MATE_VALUE);
          bestMove = TO_ALG[hashedBoard.move.fromLoc] + TO_ALG[hashedBoard.move.toLoc];
          if (hashedBoard.move.pieceFrom == 0 && hashedBoard.move.toLoc >= 56) {
            bestMove += "q";
          }
          return ReturnInfo{bestMove, hashedBoard.score, mateIn};
        }
        else if (hashedBoard.score >= -MATE_VALUE + 100) {
          mateIn = (hashedBoard.score + MATE_VALUE);
          bestMove = TO_ALG[hashedBoard.move.fromLoc] + TO_ALG[hashedBoard.move.toLoc];
          if (hashedBoard.move.pieceFrom == 0 && hashedBoard.move.toLoc >= 56) {
            bestMove += "q";
          }
          return ReturnInfo{bestMove, hashedBoard.score, mateIn};
        }
        else {
          bestMove = TO_ALG[hashedBoard.move.fromLoc] + TO_ALG[hashedBoard.move.toLoc];
          if (hashedBoard.move.pieceFrom == 0 && hashedBoard.move.toLoc >= 56) {
            bestMove += "q";
          }
          return ReturnInfo{bestMove, hashedBoard.score, 0};
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


  // If it is white to move
  if (useMax) {


    int ret = -10000000;

    if (hashed && bitboard.IsMoveWhite(hashedBoard.move.fromLoc, hashedBoard.move.toLoc)) {
      bitboard.movePiece(hashedBoard.move.fromLoc, hashedBoard.move.toLoc);
      if (!bitboard.filterCheck(0)) {
        bitboard.undoMove();
      }
      else {
        numMoves++;
        // If three fold repetition, score a zero
        if (bitboard.isThreeFold()) {
          ret = std::max(ret, 0);
        }
        else {
          ret = std::max(ret, alphabetaR(false, bitboard, depth - 1, alpha, beta, maxDepth));
        }

        bitboard.undoMove();

        // Update the best move and alpha
        if (ret > alpha) {
          alpha = ret;
          bestMove = TO_ALG[hashedBoard.move.fromLoc] + TO_ALG[hashedBoard.move.toLoc];
          bestMoveM = hashedBoard.move;
        }

        if (ret >= beta) { //Cut off

          if (hashedBoard.move.quiet) {
            bitboard.InsertKiller(hashedBoard.move, depth);
            if (bestMoveM.quiet && bestMoveM.fromLoc < 64 && bestMoveM.toLoc < 64) {
              bitboard.history[bestMoveM.fromLoc][bestMoveM.toLoc] += depth * depth;
            }
          }

          bitboard.InsertLookup(hashedBoard.move, ret, alpha, beta, depth, 2, hashF);

          if (ret <= -MATE_VALUE + 100) {
            mateIn = (ret + MATE_VALUE);
          }
          else if (ret >= MATE_VALUE - 100) {
            mateIn = (ret - MATE_VALUE);
          }

          if (hashedBoard.move.pieceFrom == 0 && hashedBoard.move.toLoc >= 56) {
            bestMove += "q";
          }

          return ReturnInfo{bestMove, ret, mateIn};
        }



      }
    }

    // Generate moves and sort them
    std::vector<Bitboard::Move> vMoves = bitboard.allValidMoves(0);
    bitboard.sortMoves(vMoves, hashedBoard.move, depth);

    for (Bitboard::Move move : vMoves) {

      // If null move, continue
      if (move.fromLoc == 65 && move.toLoc == 65) {
        continue;
      }

      // Move piece
      bitboard.movePiece(move.fromLoc, move.toLoc);

      // If in check, continue
      if (!bitboard.filterCheck(0)) {
        bitboard.undoMove();
        continue;
      }
      numMoves++;
      // Continue search
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
          if (bestMoveM.quiet && bestMoveM.fromLoc < 64 && bestMoveM.toLoc < 64) {
            bitboard.history[bestMoveM.fromLoc][bestMoveM.toLoc] += depth * depth;
          }
        }

        bitboard.InsertLookup(move, ret, alpha, beta, depth, 1, hashF);

        if (ret <= -MATE_VALUE + 100) {
          mateIn = (ret + MATE_VALUE);
        }
        else if (ret >= MATE_VALUE - 100) {
          mateIn = (ret - MATE_VALUE);
        }

        if (move.pieceFrom == 0 && move.toLoc >= 56) {
          bestMove += "q";
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
      if (bestMoveM.quiet && bestMoveM.fromLoc < 64 && bestMoveM.toLoc < 64) {
        bitboard.history[bestMoveM.fromLoc][bestMoveM.toLoc] += depth * depth;
      }
      bitboard.InsertLookup(bestMoveM, ret, alpha, beta, depth, 0, hashF);
    }

    if (ret <= -MATE_VALUE + 100) {
      mateIn = (ret + MATE_VALUE);
    }
    else if (ret >= MATE_VALUE - 100) {
      mateIn = (ret - MATE_VALUE);
    }

    if (bestMoveM.pieceFrom == 0 && bestMoveM.toLoc >= 56) {
      bestMove += "q";
    }

    if (numMoves == 1) {
      exit_thread_flag = true;
    }

    return ReturnInfo{bestMove, ret, mateIn};
  }
  else {

    int ret = 10000000;

    if (hashed && bitboard.IsMoveBlack(hashedBoard.move.fromLoc, hashedBoard.move.toLoc)) {
      bitboard.movePiece(hashedBoard.move.fromLoc, hashedBoard.move.toLoc);
      if (!bitboard.filterCheck(1)) {
        bitboard.undoMove();
      }
      else {
        numMoves++;
        // If three fold repetition, score a zero
        if (bitboard.isThreeFold()) {
          ret = std::min(ret, 0);
        }
        else {
          ret = std::min(ret, alphabetaR(true, bitboard, depth - 1, alpha, beta, maxDepth));
        }

        bitboard.undoMove();

        // Update the best move and alpha
        if (ret < beta) {
          beta = ret;
          bestMove = TO_ALG[hashedBoard.move.fromLoc] + TO_ALG[hashedBoard.move.toLoc];
          bestMoveM = hashedBoard.move;
        }

        if (ret <= alpha) { //Cut off
          if (hashedBoard.move.quiet) {
            bitboard.InsertKiller(hashedBoard.move, depth);
            if (bestMoveM.quiet && bestMoveM.fromLoc < 64 && bestMoveM.toLoc < 64) {
              bitboard.history[bestMoveM.fromLoc][bestMoveM.toLoc] += depth * depth;
            }
          }

          bitboard.InsertLookup(hashedBoard.move, ret, alpha, beta, depth, 2, hashF);

          if (ret <= -MATE_VALUE + 100) {
            mateIn = (ret + MATE_VALUE);
          }
          else if (ret >= MATE_VALUE - 100) {
            mateIn = (ret - MATE_VALUE);
          }

          if (hashedBoard.move.pieceFrom == 0 && hashedBoard.move.toLoc <= 7) {
            bestMove += "q";
          }

          return ReturnInfo{bestMove, ret, mateIn};
        }



      }
    }

    // Generate moves and sort them
    std::vector<Bitboard::Move> vMoves = bitboard.allValidMoves(1);
    bitboard.sortMoves(vMoves, hashedBoard.move, depth);

    for (Bitboard::Move move : vMoves) {

      // If null move, continue
      if (move.fromLoc == 65 && move.toLoc == 65) {
        continue;
      }

      bitboard.movePiece(move.fromLoc, move.toLoc);

      if (!bitboard.filterCheck(1)) {
        bitboard.undoMove();
        continue;
      }
      numMoves++;
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
          if (bestMoveM.quiet && bestMoveM.fromLoc < 64 && bestMoveM.toLoc < 64) {
            bitboard.history[bestMoveM.fromLoc][bestMoveM.toLoc] += depth * depth;
          }
        }

        bitboard.InsertLookup(move, ret, alpha, beta, depth, 2, hashF);

        if (ret <= -MATE_VALUE + 100) {
          mateIn = (ret + MATE_VALUE);
        }
        else if (ret >= MATE_VALUE - 100) {
          mateIn = (ret - MATE_VALUE);
        }

        if (move.pieceFrom == 0 && move.toLoc <= 7) {
          bestMove += "q";
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
      if (bestMoveM.quiet && bestMoveM.fromLoc < 64 && bestMoveM.toLoc < 64) {
        bitboard.history[bestMoveM.fromLoc][bestMoveM.toLoc] += depth * depth;
      }
      bitboard.InsertLookup(bestMoveM, ret, alpha, beta, depth, 0, hashF);
      // bitboard.PVMoves.push_back(bestMove);

    }

    if (ret <= -MATE_VALUE + 100) {
      mateIn = (ret + MATE_VALUE);
    }
    else if (ret >= MATE_VALUE - 100) {
      mateIn = (ret - MATE_VALUE);
    }

    if (bestMoveM.pieceFrom == 0 && bestMoveM.toLoc <= 7) {
      bestMove += "q";
    }

    if (numMoves == 1) {
      exit_thread_flag = true;
    }

    return ReturnInfo{bestMove, ret, mateIn};
  }

}
