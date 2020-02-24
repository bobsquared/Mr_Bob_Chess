#include "search.h"



// Debugging variables
uint64_t traversedNodes = 0;
std::atomic<bool> exit_thread_flag;
uint64_t pruning = 0;
uint64_t pruningTotal = 0;
uint64_t pruningTT = 0;
uint64_t pruningTotalTT = 0;


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


// Quiesce Search is needed to minimize the horizon effect.
// Adapted from https://www.chessprogramming.org/Quiescence_Search
int quiesceSearchR(bool whiteMove, Bitboard &bitboard, int alpha, int beta, int depth=-1) {
  int origAlpha = alpha;
  traversedNodes++;

  if (exit_thread_flag) {
    return 0;
  }

  if (bitboard.isThreeFold() && alpha < 0) {
    if (beta <= 0) {
      return 0;
    }
  }

  // Transposition table:
  // Get the hash key
  uint64_t hashF = bitboard.getPosKey();
  Bitboard::ZobristVal hashedBoard = Bitboard::ZobristVal();
  if (bitboard.lookup2[hashF % bitboard.numHashes].posKey == hashF) {
    // Store the hash table value
    hashedBoard = bitboard.lookup2[hashF % bitboard.numHashes];

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


  int ret = bitboard.evaluate(alpha, beta);
  if (ret >= beta) {
    return ret;
  }

  if (ret > alpha) {
    alpha = ret;
  }


  Bitboard::Move bestMove;

  std::vector<Bitboard::Move> vMoves;
  vMoves = bitboard.allValidCaptures(!whiteMove);
  bitboard.scoreMoves(vMoves, bestMove, depth);


  while (!vMoves.empty()) {

    Bitboard::Move move = bitboard.pickMove(vMoves);
    bitboard.movePiece(move);

    if (!bitboard.filterCheck(!whiteMove)) {
      bitboard.undoMove();
      continue;
    }

    ret = std::max(ret, -quiesceSearchR(!whiteMove, bitboard, -beta, -alpha, depth - 1));
    bitboard.undoMove();


    if (ret > alpha) {
      if (ret >= beta) {
        bitboard.InsertLookup(move, ret, depth, 1, hashF);
        return ret;
      }
      alpha = ret;
      bestMove = move;
    }

  }

  if (origAlpha != alpha) {
    bitboard.InsertLookup(bestMove, ret, depth, 0, hashF);
  }
  else {
    bitboard.InsertLookup(bestMove, ret, depth, 2, hashF);
  }


  return ret;

}




// OKAY LETS START FROM SCRATCH
// WE'RE DOING THINGS PROPERLY THIS TIME
// FAIL SOFT ARCHITECTURE
int searchR(bool whiteMove, Bitboard &bitboard, int depth, int alpha, int beta, bool nullMoves) {


  bool IsPv = beta != alpha + 1;

  // Atomic Boolean flag will return 0 if the search has ran out of time.
  if (exit_thread_flag) {
    return 0;
  }

  // Check for any draws (currently checks threefold repetition)
  // If there is a draw, return 0.
  if (bitboard.isThreeFold() && alpha < 0) {
    if (beta <= 0) {
      return 0;
    }
  }


  // Transposition table for duplicate detection:
  // Get the hash key
  uint64_t hashF = bitboard.getPosKey();
  bool hashed = false;
  Bitboard::ZobristVal hashedBoard = Bitboard::ZobristVal();
  if (bitboard.lookup2[hashF % bitboard.numHashes].posKey == hashF) {
    hashed = true;
    // Store the hash table value
    hashedBoard = bitboard.lookup2[hashF % bitboard.numHashes];

    // Ensure hashedBoard depth >= current depth
    if (hashedBoard.depth >= depth) {

      if (hashedBoard.flag == 0) { //Exact
        traversedNodes++;
        return hashedBoard.score;
      }
      else if (hashedBoard.flag == 1) { // Low bound
        alpha = std::max(alpha, hashedBoard.score);
      }
      else if (hashedBoard.flag == 2) { // Upper bound
        beta = std::min(beta, hashedBoard.score);
      }

      if (alpha >= beta) {
        traversedNodes++;
        return hashedBoard.score;
      }

    }
  }



  // If depth goes to zero, go into quiescence search.
  if (depth <= 0) {
    return quiesceSearchR(whiteMove, bitboard, alpha, beta);
  }

  traversedNodes++;


  int ret = alpha;
  int moveNumber = 0;
  int origAlpha = alpha;
  int newDepth = depth;
  bool hasMove = false;
  Bitboard::Move bestMove;
  int eval = bitboard.evaluate(alpha, beta);
  // Determine if player is in check.
  bool isCheck = !bitboard.filterCheck(!whiteMove);


  // if (depth == 1 && eval >= beta + 325 && eval < 900) {
  //   return eval;
  // }
  //
  // if (depth == 2 && eval >= beta + 550 && eval < 900) {
  //   return eval;
  // }

  // SEARCHING STEP 1: Null Move Pruning:
  // Skip the player's turn and search the other player's turn using a reduced depth.
  // This can be used to detect threats or mates.
  if (!IsPv && !nullMoves && depth >= 1 && !isCheck && eval >= beta - 50) {
    int R = 3 + depth / 8;
    Bitboard::Move nullMove = Bitboard::Move{65, 65, true, 0, 0, 0, 0, false};

    bitboard.movePiece(nullMove);
    int nullRet = -searchR(!whiteMove, bitboard, depth - R - 1, -beta, -beta + 1, true);
    bitboard.undoMove();

    if (nullRet >= beta) {

      if (abs(nullRet) >= MATE_VALUE) {
        nullRet = beta;
      }
      int v = searchR(!whiteMove, bitboard, depth - R - 1, beta - 1, beta, false);
      if (v >= beta) {
        return nullRet;
      }

    }

  }



  // SEARCHING STEP 2: Transposition Principal variation search:
  // If there is a TT hit for this position, try this move first.
  // Many cutoffs will occur in doing this.
  hasMove = whiteMove? bitboard.IsMoveWhite(hashedBoard.move) : bitboard.IsMoveBlack(hashedBoard.move);
  if (hashed && hasMove) {
    bitboard.movePiece(hashedBoard.move);
    if (!bitboard.filterCheck(!whiteMove)) {
      bitboard.undoMove();
    }
    else {

      bool giveCheck = !bitboard.filterCheck(whiteMove);

      // Extend if there is a check given.
      if (giveCheck) {
        newDepth = depth + 1;
      }

      ret = std::max(ret, -searchR(!whiteMove, bitboard, newDepth - 1, -beta, -alpha, nullMoves));
      bitboard.undoMove();


      if (ret > alpha) {
        if (ret >= beta) { //Cut off

          // Update killer moves and history heuristic
          if (hashedBoard.move.quiet) {
            bitboard.InsertKiller(hashedBoard.move, depth);
            bitboard.history[whiteMove][hashedBoard.move.fromLoc][hashedBoard.move.toLoc] += depth * depth;
          }

          ++pruning;
          ++pruningTotal;

          bitboard.InsertLookup(hashedBoard.move, ret, depth, 1, hashF);
          return beta;
        }

        alpha = ret;
        bestMove = hashedBoard.move;
      }

      ++pruningTotal;
      ++moveNumber;
      hasMove = true;
    }
  }





  // Generate all pseudo-legal moves from this position and sort them.
  std::vector<Bitboard::Move> vMoves = bitboard.allValidMoves(!whiteMove);





  // SEARCHING STEP 3: Internal Iterative Deepening
  // If there is no TT hit, then we can search for the best move in this position at a reduced depth
  // and search that move first.
  if (!hashed && depth >= 6) {

    // Bitboard::Move iidMove = searchRoot(whiteMove, bitboard, depth - 4, vMoves, alpha, beta, false).move;
    // for (uint8_t i = 0; i < vMoves.size(); i++) {
    //   if (vMoves[i] == iidMove) {
    //     vMoves[i].score = 3000000;
    //     break;
    //   }
    // }
    bitboard.scoreMoves(vMoves, hashedBoard.move, depth);
  }
  else {
    bitboard.scoreMoves(vMoves, hashedBoard.move, depth);
  }




  // Loop through all the moves.
  while (!vMoves.empty()) {
    Bitboard::Move move = bitboard.pickMove(vMoves);
    int reduction = 1;
    newDepth = depth;
    bitboard.movePiece(move);

    if (!bitboard.filterCheck(!whiteMove)) {
      bitboard.undoMove();
      continue;
    }

    bool giveCheck = !bitboard.filterCheck(whiteMove);

    // Extensions
    if (giveCheck) {
      newDepth = depth + 1;
    }



    // SEARCHING STEP 4: PV search
    // Assume the first move in this list is the principal variation. The rest of the moves will be serached with null window.
    if (moveNumber == 0) {
      ret = std::max(ret, -searchR(!whiteMove, bitboard, newDepth - 1, -beta, -alpha, nullMoves));
    }
    else {



      // SERACHING STEP 5: Late Move Reduction:
      // We can also reduce the depth of late moves to reduce the size of the tree.
      // Again, this is assuming that the good moves are on the front of the list.
      if (moveNumber >= 3 && depth >= 3 && !isCheck && move.quiet && !giveCheck) {




        if (moveNumber > 6) {
          reduction += 1;
        }


        // Reduce reduction if in PV
        if (IsPv) {
          reduction--;
        }


        reduction = std::max(0, reduction);
        ret = std::max(ret, -searchR(!whiteMove, bitboard, newDepth - reduction - 1, -alpha-1, -alpha, nullMoves));

        // If the result is greater than alpha, then we have to research at full depth.
        if (ret > alpha && reduction <= 0) {
          ret = std::max(ret, -searchR(!whiteMove, bitboard, newDepth - 1, -alpha-1, -alpha, nullMoves));
        }

      }
      else {
        ret = std::max(ret, -searchR(!whiteMove, bitboard, newDepth - 1, -alpha-1, -alpha, nullMoves));
      }

      // If the result is greater than alpha and less than beta, then research with full window.
      if (ret > alpha && ret < beta) {
        ret = std::max(ret, -searchR(!whiteMove, bitboard, newDepth - 1, -beta, -alpha, nullMoves));
      }

    }
    bitboard.undoMove();


    if (ret > alpha) {


      if (ret >= beta) {
        // Update killer moves and history heuristic
        if (move.quiet) {
          bitboard.InsertKiller(move, depth);
          bitboard.history[whiteMove][move.pieceFrom][move.toLoc] += depth * depth;
        }
        bitboard.InsertLookup(move, ret, depth, 1, hashF);

        if (moveNumber == 0) {
          ++pruning;
          ++pruningTotal;
        }

        return ret;
      }

      alpha = ret;
      bestMove = move;
    }

    if (moveNumber == 0) {
      ++pruningTotal;
    }
    hasMove = true;
    ++moveNumber;
  }

  // If no legal move is found then a check should be done to see if king is in check
  // If king is in check, then this is check mate.
  // Else it is stalemate
  if (!hasMove) {
    if (isCheck) {
      return -MATE_VALUE - depth;
    }
    else {
      return 0;
    }
  }

  // Update transposition table.
  if (origAlpha != alpha) {
    if (bestMove.quiet) {
      bitboard.history[whiteMove][bestMove.pieceFrom][bestMove.toLoc] += depth;
    }
    bitboard.InsertLookup(bestMove, ret, depth, 0, hashF);
  }
  else {
    bitboard.InsertLookup(bestMove, ret, depth, 2, hashF);
  }

  return ret;

}






// Minimax root node
ReturnInfo searchRoot(bool whiteMove, Bitboard &bitboard, int depth, std::vector<Bitboard::Move> &vMoves, int alpha, int beta, bool isMain) {

  ReturnInfo retInfo = ReturnInfo{};
  uint64_t hashF = bitboard.getPosKey();
  int ret = alpha;
  int origAlpha = alpha;
  uint8_t numMoves = 0;
  bool isCheck = !bitboard.filterCheck(!whiteMove);
  int newDepth = depth;




  Bitboard::Move bestMove;

  for (Bitboard::Move move : vMoves) {
    newDepth = depth;
    bitboard.movePiece(move);

    if (!bitboard.filterCheck(!whiteMove)) {
      bitboard.undoMove();
      continue;
    }

    bool giveCheck = !bitboard.filterCheck(whiteMove);
    if (!giveCheck) {
      newDepth = depth + 1;
    }

    if (numMoves == 0) {
      ret = std::max(ret, -searchR(!whiteMove, bitboard, newDepth - 1, -beta, -alpha, false));
    }
    else {

      //Late Move Reduction
      if (numMoves >= 6 && depth >= 3 && !isCheck && move.quiet && !giveCheck) {
        int reduction = 1;

        if (numMoves > 14) {
          reduction += 1;
        }

        // if (numMoves > 20) {
        //   reduction += 1 + depth / 8;
        // }

        ret = std::max(ret, -searchR(!whiteMove, bitboard, newDepth - reduction - 1, -alpha-1, -alpha, false));

        if (ret > alpha && reduction != 0) {
          ret = std::max(ret, -searchR(!whiteMove, bitboard, newDepth - 1, -alpha-1, -alpha, false));
        }

      }
      else {
        ret = std::max(ret, -searchR(!whiteMove, bitboard, newDepth - 1, -alpha-1, -alpha, false));
      }

      if (ret > alpha && ret < beta) {
        ret = std::max(ret, -searchR(!whiteMove, bitboard, newDepth - 1, -beta, -alpha, false));
      }
    }
    bitboard.undoMove();

    if (ret > alpha) {
      alpha = ret;
      bestMove = move;
      retInfo.move = move;
      retInfo.score = ret;
      retInfo.bestMove = TO_ALG[move.fromLoc] + TO_ALG[move.toLoc];
    }

    ++numMoves;
  }

  if (numMoves == 1 && isMain) {
    exit_thread_flag = true;
  }


  if (origAlpha != alpha) {
    bitboard.InsertLookup(bestMove, ret, depth, 0, hashF);
  }
  else {
    bitboard.InsertLookup(bestMove, ret, depth, 2, hashF);
  }

  if (bestMove.promotion == 4) {
    retInfo.bestMove += "q";
  }


  return retInfo;


}
