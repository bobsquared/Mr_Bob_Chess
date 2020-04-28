#include "search.h"



// Debugging variables
uint64_t traversedNodes = 0;
std::atomic<bool> exit_thread_flag;
uint64_t pruning = 0;
uint64_t pruningTotal = 0;
uint64_t pruningTT = 0;
uint64_t pruningTotalTT = 0;





// Quiesce Search is needed to minimize the horizon effect.
// Adapted from https://www.chessprogramming.org/Quiescence_Search
int quiesceSearchR(bool whiteMove, Bitboard &bitboard, TranspositionTable &tt, int alpha, int beta, int &seldepth, int depth=-1) {

  traversedNodes++;
  int origAlpha = alpha;

  if (depth < seldepth) {
    seldepth = depth;
  }

  if (exit_thread_flag) {
    return 0;
  }

  // Transposition table:
  // Get the hash key
  TranspositionTable::ZobristVal hashedBoard;
  uint64_t hashF = bitboard.getPosKey();
  bool ttRet = false;
  bool hashed = tt.probeTT(hashF, hashedBoard, depth, ttRet, alpha, beta);

  if (ttRet) {
    return hashedBoard.score;
  }


  int standPat = bitboard.evaluate(alpha, beta);
  if (standPat >= beta) {
    return standPat;
  }

  if (standPat > alpha) {
    alpha = standPat;
  }

  int ret = -1000000;
  Bitboard::Move bestMove;

  // SEARCHING STEP 2: Transposition Principal variation search:
  // If there is a TT hit for this position, try this move first.
  // Many cutoffs will occur in doing this.
  // if (hashed) {
  //   int prevRet = ret;
  //   bitboard.movePiece(hashedBoard.move);
  //
  //   if (!bitboard.filterCheck(!whiteMove)) {
  //     bitboard.undoMove();
  //   }
  //   else {
  //
  //     ret = std::max(ret, -quiesceSearchR(!whiteMove, bitboard, tt, -beta, -alpha, seldepth, depth - 1));
  //     bitboard.undoMove();
  //
  //     if (ret > alpha) {
  //       if (ret >= beta) { //Cut off
  //         tt.saveTT(hashedBoard.move, ret, depth, 1, hashF);
  //         return ret;
  //       }
  //       alpha = ret;
  //     }
  //
  //     if (ret != prevRet) {
  //       bestMove = hashedBoard.move;
  //     }
  //
  //   }
  // }

  std::vector<Bitboard::Move> vMoves = bitboard.allValidCaptures(!whiteMove);
  bitboard.scoreMoves(vMoves, bestMove, depth, !whiteMove);

  while (!vMoves.empty()) {
    int prevRet = ret;
    Bitboard::Move move = bitboard.pickMove(vMoves);
    bitboard.movePiece(move);

    if (!bitboard.filterCheck(!whiteMove)) {
      bitboard.undoMove();
      continue;
    }

    ret = std::max(ret, -quiesceSearchR(!whiteMove, bitboard, tt, -beta, -alpha, seldepth, depth - 1));
    bitboard.undoMove();


    if (ret > alpha) {
      if (ret >= beta) {
        tt.saveTT(move, ret, depth, 1, hashF);
        return ret;
      }
      alpha = ret;
    }

    if (ret != prevRet) {
      bestMove = move;
    }

  }

  if (ret != -1000000) {

    if (ret <= origAlpha) {
      tt.saveTT(bestMove, ret, depth, 2, hashF);
    }
    else {
      tt.saveTT(bestMove, ret, depth, 0, hashF);
    }

  }



  return ret == -1000000? standPat : ret;

}




// OKAY LETS START FROM SCRATCH
// WE'RE DOING THINGS PROPERLY THIS TIME
// FAIL SOFT ARCHITECTURE
int searchR(bool whiteMove, Bitboard &bitboard, TranspositionTable &tt, int depth, int alpha, int beta, int &seldepth, bool nullMoves) {

  bool IsPv = beta != alpha + 1;
  int origAlpha = alpha;

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


  // If depth goes to zero, go into quiescence search.
  if (depth <= 0) {
    return quiesceSearchR(whiteMove, bitboard, tt, alpha, beta, seldepth);
  }


  // Transposition table for duplicate detection:
  // Get the hash key
  TranspositionTable::ZobristVal hashedBoard;
  uint64_t hashF = bitboard.getPosKey();
  bool ttRet = false;
  bool hashed = tt.probeTT(hashF, hashedBoard, depth, ttRet, alpha, beta);


  if (ttRet) {
    traversedNodes++;
    return hashedBoard.score;
  }

  traversedNodes++;


  int ret = -1000000;
  int moveNumber = 0;
  int newDepth = depth;
  bool hasMove = false;
  Bitboard::Move bestMove;
  int eval = bitboard.evaluate(alpha, beta);
  bool isCheck = !bitboard.filterCheck(!whiteMove); // Determine if player is in check.


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
  if (!IsPv && !nullMoves && !isCheck && eval >= beta - 50 && bitboard.canNullMove()) {
    int R = 3 + depth / 8;
    Bitboard::Move nullMove = Bitboard::Move{65, 65, true, 0, 0, 0, 0, false};

    bitboard.movePiece(nullMove);
    int nullRet = -searchR(!whiteMove, bitboard, tt, depth - R - 1, -beta, -beta + 1, seldepth, true);
    bitboard.undoMove();

    if (nullRet >= beta) {

      if (abs(nullRet) >= MATE_VALUE) {
        nullRet = beta;
      }
      int v = searchR(!whiteMove, bitboard, tt, depth - R - 1, beta - 1, beta, seldepth, false);
      if (v >= beta) {
        return nullRet;
      }

    }

  }



  // SEARCHING STEP 2: Transposition Principal variation search:
  // If there is a TT hit for this position, try this move first.
  // Many cutoffs will occur in doing this.
  hasMove = whiteMove? bitboard.IsMoveWhite(hashedBoard.move) : bitboard.IsMoveBlack(hashedBoard.move);
  if (hashed) {
    int prevRet = ret;
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

      if (bitboard.isPassedPawn(hashedBoard.move.fromLoc, !whiteMove)) {
        newDepth = depth + 1;
      }

      ret = std::max(ret, -searchR(!whiteMove, bitboard, tt, newDepth - 1, -beta, -alpha, seldepth, nullMoves));
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

          tt.saveTT(hashedBoard.move, ret, depth, 1, hashF);
          return ret;
        }

        alpha = ret;

      }

      if (prevRet != ret) {
        bestMove = hashedBoard.move;
      }

      ++pruningTotal;
      ++moveNumber;
      hasMove = true;
    }
  }





  // Generate all pseudo-legal moves from this position and sort them.
  std::vector<Bitboard::Move> vMoves = bitboard.allValidMoves(!whiteMove);
  bitboard.scoreMoves(vMoves, hashedBoard.move, depth, !whiteMove);

  // Loop through all the moves.
  while (!vMoves.empty()) {
    int prevRet = ret;
    Bitboard::Move move = bitboard.pickMove(vMoves);


    // if (!IsPv && !move.quiet && !isCheck && depth == 1) {
    //   if (bitboard.evaluate(alpha, beta) + 250 < beta) {
    //     continue;
    //   }
    // }


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

    if (bitboard.isPassedPawn(move.fromLoc, !whiteMove)) {
      newDepth = depth + 1;
    }


    // SEARCHING STEP 3: PV search
    // Assume the first move in this list is the principal variation. The rest of the moves will be serached with null window.
    if (moveNumber == 0) {
      ret = std::max(ret, -searchR(!whiteMove, bitboard, tt, newDepth - 1, -beta, -alpha, seldepth, nullMoves));
    }
    else {

      // SERACHING STEP 4: Late Move Reduction:
      // We can also reduce the depth of late moves to reduce the size of the tree.
      // Again, this is assuming that the good moves are on the front of the list.
      if (moveNumber > 2 && depth >= 3 && move.quiet && !isCheck && !giveCheck) {
        int reduction = 1;

        if (moveNumber > 6) {
          reduction += 1;
        }

        // if (moveNumber > 14) {
        //   reduction += 1;
        // }
        //
        // if (moveNumber > 19) {
        //   reduction += 1;
        // }

        // Reduce reduction if in PV
        if (IsPv || (hashed && hashedBoard.flag == 0)) {
          reduction -= 2;
        }

        // if (hashed && hashedBoard.flag == 1) {
        //   reduction += 2;
        // }

        reduction = std::max(0, reduction);
        ret = std::max(ret, -searchR(!whiteMove, bitboard, tt, newDepth - reduction - 1, -alpha-1, -alpha, seldepth, nullMoves));

        // If the result is greater than alpha, then we have to research at full depth.
        if (ret > alpha && reduction > 0) {
          ret = std::max(ret, -searchR(!whiteMove, bitboard, tt, newDepth - 1, -alpha-1, -alpha, seldepth, nullMoves));
        }

      }
      else {
        ret = std::max(ret, -searchR(!whiteMove, bitboard, tt, newDepth - 1, -alpha-1, -alpha, seldepth, nullMoves));
      }

      // If the result is greater than alpha and less than beta, then research with full window.
      if (ret > alpha && ret < beta) {
        ret = std::max(ret, -searchR(!whiteMove, bitboard, tt, newDepth - 1, -beta, -alpha, seldepth, nullMoves));
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
        tt.saveTT(move, ret, depth, 1, hashF);

        if (moveNumber == 0) {
          ++pruning;
        }
        ++pruningTotal;

        return ret;
      }

      alpha = ret;

    }

    if (prevRet != ret) {
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
  if (ret <= origAlpha) {
    tt.saveTT(bestMove, ret, depth, 2, hashF);
  }
  else {
    if (bestMove.quiet) {
      bitboard.history[whiteMove][bestMove.pieceFrom][bestMove.toLoc] += depth * depth;
    }
    tt.saveTT(bestMove, ret, depth, 0, hashF);
  }

  return ret;

}






// Minimax root node
ReturnInfo searchRoot(bool whiteMove, Bitboard &bitboard, TranspositionTable &tt, int depth, int &seldepth, std::vector<Bitboard::Move> &vMoves, int alpha, int beta, bool isMain) {

  ReturnInfo retInfo = ReturnInfo{};
  uint64_t hashF = bitboard.getPosKey();
  int ret = alpha;
  uint8_t numMoves = 0;
  bool isCheck = !bitboard.filterCheck(!whiteMove);
  int newDepth = depth;
  bool hasMove = false;


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

    if (bitboard.isPassedPawn(move.fromLoc, !whiteMove)) {
      newDepth = depth + 1;
    }

    if (numMoves == 0) {
      ret = std::max(ret, -searchR(!whiteMove, bitboard, tt, newDepth - 1, -beta, -alpha, seldepth, false));
    }
    else {

      //Late Move Reduction
      if (numMoves >= 14 && depth >= 3 && !isCheck && move.quiet && !giveCheck) {
        int reduction = 1;

        if (numMoves > 25) {
          reduction += 1;
        }

        ret = std::max(ret, -searchR(!whiteMove, bitboard, tt, newDepth - reduction - 1, -alpha-1, -alpha, seldepth, false));

        if (ret > alpha && reduction != 0) {
          ret = std::max(ret, -searchR(!whiteMove, bitboard, tt, newDepth - 1, -alpha-1, -alpha, seldepth, false));
        }

      }
      else {
        ret = std::max(ret, -searchR(!whiteMove, bitboard, tt, newDepth - 1, -alpha-1, -alpha, seldepth, false));
      }

      if (ret > alpha && ret < beta) {
        ret = std::max(ret, -searchR(!whiteMove, bitboard, tt, newDepth - 1, -beta, -alpha, seldepth, false));
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


  tt.saveTT(bestMove, ret, depth, 0, hashF);

  if (bestMove.promotion == 4) {
    retInfo.bestMove += "q";
  }


  return retInfo;


}
