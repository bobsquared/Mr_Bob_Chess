

/** @file search.cpp
* This file contains the main search for the engine
*
* @author Vincent Yu
*/



#include "search.h"
#include <cmath>
#include <thread>



Search::Search(Eval *eval, TranspositionTable *tt, ThreadSearch *thread) : eval(eval), tt(tt), thread(thread) {
    nThreads = 1;                   /**< Number of threads to search, default is 1.*/
    multiPv = 1;                   /**< Number of pvs to search, default is 1.*/
    stopable = false;              /**< Used to ensure that we search atleast a depth one 1.*/
    canPrintInfo = true;

    movePick = new MovePick;               /**< The move picker gives a score to each generated move*/
    moveGen = new MoveGen;                  /**< The move generator generates all pseudo legal moves in a given position*/
    InitLateMoveArray();
}



ThreadSearch* Search::getThreads() {
    return thread;
}



int Search::getNThreads() {
    return nThreads;
}



int Search::getTotalTime() {
   return totalTime;
}



void Search::stopSearch() {
    exit_thread_flag = true;
}



void Search::setSearch() {
    exit_thread_flag = false;
}



void Search::willPrintInfo(bool b) {
    canPrintInfo = b;
}



/**
* Clear the transposition table
*/
void Search::clearTT() {
    tt->clearHashTable();
}



/**
* Initialize the late move reduction array
*/
void Search::InitLateMoveArray() {
    for (int i = 1; i < 64; i++) {
        for (int j = 1; j < 64; j++) {
            lmrReduction[i][j] = (exp(3) * log(i) * log(j) + 72) / 54;
        }
    }
}



/**
* Deallocate memory in the search file
*/
void Search::cleanUpSearch() {
    delete tt;
    delete movePick;
    delete moveGen;
    delete eval;
    delete [] thread;
}



void Search::setNumThreads(const int numThreads) {
    nThreads = numThreads;
    delete [] thread;
    thread = new ThreadSearch[numThreads];
}


void Search::setMultiPVSearch(int pvs) {
    multiPv = pvs;
}



void Search::setTTSize(int hashSize) {
    tt->setSize(hashSize);
}



void Search::setRFPsearch(const int value) {
    rfpVal = value;
}



void Search::setRazorsearch(const int value) {
    razorVal = value;
}



void Search::setProbcutsearch(const int value) {
    probcutVal = value;
}



void Search::setFutilitysearch(const int value) {
    futilityVal = value;
}



void Search::setHistoryLMRsearch(const int value) {
    historyLmrVal = value;
}



void Search::setHistoryLMRNoisysearch(const int value) {
    historyLmrNoisyVal = value;
}



PrevMoveInfo GetPreviousMoveInfo(Bitboard &b) {
    MOVE prevMove = b.moveHistory.count > 0? b.moveHistory.move[b.moveHistory.count - 1].move : NO_MOVE;
    int prevMoveTo = get_move_to(prevMove);
    return PrevMoveInfo(prevMove, get_move_from(prevMove), prevMoveTo, b.pieceAt[prevMoveTo] / 2);
}


/**
* The function that searches only noisy moves
*
* Qsearch is used to improve the playing strength by evaluating only quiet positions.
* Therefore we search all noisy moves (captures and promotions and checks) until only a
* quiet position is left.
*
* @param[in, out] b     The board representation.
* @param[in, out] th    A pointer to the thread data that called the function.
* @param[in]      depth The current depth to search the current position at.
* @param[in]      alpha AB pruning alpha value.
* @param[in]      beta  AB pruning beta value.
* @param[in]      ply   The current ply/height that the search is at.
* @return               The score of the best move in the position.
*/
int Search::qsearch(Bitboard &b, ThreadSearch *th, int depth, int alpha, int beta, int ply) {

    #ifdef DEBUGHASH
    b.debugZobristHash();
    #endif

    th->nodes++; // update nodes searched

    // stop the search
    if (stopable && exit_thread_flag) {
        return 0;
    }

    // determine if it is a draw
    if (b.isDraw(ply)) {
        return 2 * (th->nodes & 1) - 1;
    }

    // Probe Transpostion Table:
    ZobristVal hashedBoard;
    uint64_t posKey = b.getPosKey();
    bool ttRet = false;
    MOVE ttMove = NO_MOVE;
    int prevAlpha = alpha;

    bool hashed = tt->probeTTQsearch(posKey, hashedBoard, ttRet, ttMove, alpha, beta, ply);

    if (ttRet) {
        return hashedBoard.score;
    }

    bool inCheck = b.InCheck();
    int stand_pat = inCheck? -MATE_VALUE + ply : 0;
    int staticEval = hashed? hashedBoard.staticScore : eval->evaluate(b);

    if (!inCheck) {
        stand_pat = staticEval;

        // standing pat
        if (stand_pat >= beta) {
            return stand_pat;
        }

        if (alpha < stand_pat) {
            alpha = stand_pat;
        }
    }

    MOVE move;
    MOVE bestMove = NULL_MOVE;
    MoveList moveList;
    int numMoves = 0;

    MOVE prevMove = b.moveHistory.move[b.moveHistory.count - 1].move;
    int prevMoveTo = get_move_to(prevMove);

    inCheck? moveGen->generate_all_moves(moveList, b) : moveGen->generate_captures_promotions(moveList, b);
    movePick->scoreMovesQS(moveList, b, ttMove);
    while (moveList.get_next_move(move)) {

        if (!inCheck) {
            int see = b.seeCapture(move);

            // Prune negative SEE
            if (see < 0) {
                continue;
            }

            if (prevMoveTo != get_move_to(move)) {
                int seeEval = see + staticEval;
                if (seeEval >= beta + 200 + std::max(-150, (10 * depth))) {
                    return seeEval;
                }

                if (seeEval >= beta && stand_pat > prevAlpha && numMoves > 0) {
                    return seeEval;
                }
            }
        }

        // Check legality
        if (!b.isLegal(move)) {
            continue;
        }

        // Search more captures
        numMoves++;
        b.make_move(move);
        int score = -qsearch(b, th, depth - 1, -beta, -alpha, ply + 1);
        b.undo_move(move);

        if (score > stand_pat) {
            stand_pat = score;
            if (score > alpha) {
                bestMove = move;
                alpha = score;
                if (score >= beta) {
                    break;
                }
            }
        }

    }

    if (numMoves > 0) {
        int bound = prevAlpha >= stand_pat? UPPER_BOUND : (stand_pat >= beta? LOWER_BOUND : EXACT);
        tt->saveTT(th, bestMove, stand_pat, staticEval, depth, bound, posKey, ply);
    }

    return stand_pat;
}



/**
* The function that searches the non root nodes of the current position
*
* Uses various techniques such as null move pruning, reverse futility pruning, late move pruning, etc...
*
* @param[in, out] b           The board representation.
* @param[in, out] th          A pointer to the thread data that called the function.
* @param[in]      depth       The current depth to search the current position at.
* @param[in]      alpha       AB pruning alpha value.
* @param[in]      beta        AB pruning beta value.
* @param[in]      canNullMove True if we use can null move pruning in the current call.
* @param[in]      ply         The current ply/height that the search is at.
* @return                     The score of the best move in the position.
*/
int Search::pvSearch(Bitboard &b, ThreadSearch *th, int depth, int alpha, int beta, bool canNullMove, int ply) {

    #ifdef DEBUGHASH
    b.debugZobristHash();
    #endif

    th->seldepth = std::max(ply, th->seldepth); // update seldepth

    // Check if there are any potential wins that don't require help mate.
    if (beta > 0 && b.noPotentialWin()) {
        if (alpha >= 0) {
            th->nodes++;
            return 0;
        }
    }

    // Dive into Quiesence search
    if (depth <= 0 || ply >= MAX_PLY - 1) {
        return qsearch(b, th, depth - 1, alpha, beta, ply);
    }

    th->nodes++; // Increment number of nodes

    // Stop the search
    if (stopable && (exit_thread_flag || ((th->nodes & 1) && tm.outOfTime()))) {
        return 0;
    }

    // Determine if the position is a textbook draw
    if (b.isDraw(ply)) {
        return 2 * (th->nodes & 1) - 1;
    }

    // Mate distance pruning
    beta = MATE_VALUE - ply < beta? MATE_VALUE - ply : beta;
    alpha = -MATE_VALUE + ply > alpha? -MATE_VALUE + ply : alpha;

    if (alpha >= beta) {
        return alpha == -MATE_VALUE + ply? alpha : beta;
    }

    int prevAlpha = alpha;
    bool isPv = alpha == beta - 1? false : true;
    MOVE singMove = th->searchStack[ply].singMove;
    bool hasSingMove = singMove != NO_MOVE;


    // Probe Transpostion Table:
    ZobristVal hashedBoard;
    uint64_t posKey = b.getPosKey();
    bool ttRet = false;
    MOVE ttMove = NO_MOVE;
    bool hashed = hasSingMove? false : tt->probeTT(posKey, hashedBoard, depth, ttRet, ttMove, alpha, beta, ply);

    if (ttRet && !isPv) {
        return hashedBoard.score;
    }

    bool isCheck = b.InCheck();
    int staticEval = isCheck? MATE_VALUE + 1 : (hashed? hashedBoard.staticScore : eval->evaluate(b));
    bool improving = !isCheck && (ply >= 2? staticEval > th->searchStack[ply - 2].eval : false);
    bool ttFailLow = (ttRet && hashedBoard.flag == UPPER_BOUND);
    int extLevel = th->searchStack[ply].extLevel;
    int extLevelMax = std::min(20, extLevel);
    int hashLevel = th->searchStack[ply].hashLevel;
    int phase =  eval->getPhase(b);

    th->removeKiller(ply + 1);
    th->searchStack[ply].eval = staticEval;
    th->searchStack[ply + 1].hashLevel = hashLevel + hashed;


    if (!isPv && !isCheck && !hasSingMove) {
        // Razoring
        if (depth <= 3 && staticEval + razorVal * depth <= alpha) {
            int score = qsearch(b, th, -1, alpha, beta, ply);
            if (depth == 1 || score <= alpha) {
                return score;
            }
        }

        // Reverse futility pruning
        int rfpMargin = rfpVal 
                        - extLevelMax 
                        + hashLevel 
                        + ((256 - phase) / 16)
                        - 12 * (hashed && hashedBoard.flag != UPPER_BOUND);

        if (depth <= 7 && staticEval - rfpMargin * (depth - improving) >= beta && std::abs(staticEval) < MATE_VALUE_MAX) {
            return staticEval;
        }

        // Null move pruning
        if (canNullMove && staticEval >= beta + 25 * (phase >= 200) && depth >= 2 && th->nullMoveTree && b.nullMoveable()) {
            int R = 3 + depth / 6 + std::min((staticEval - beta) / 300, 3);
            th->searchStack[ply + 1].extLevel = extLevel;

            b.make_null_move();
            int nullRet = -pvSearch(b, th, depth - R - 1, -beta, -beta + 1, false, ply + 1);
            b.undo_null_move();

            if (nullRet >= beta && std::abs(nullRet) < MATE_VALUE_MAX) {

                if (depth >= 8) {
                    th->nullMoveTree = false;
                    nullRet = pvSearch(b, th, depth - R - 1, beta - 1, beta, false, ply);
                    th->nullMoveTree = true;
                }

                if (nullRet >= beta) {
                    return nullRet;
                }

            }
        }

        // Probcut
        int probBeta = beta + probcutVal;
        if (depth > 4 && !(hashed && hashedBoard.depth >= depth - 3 && hashedBoard.score < probBeta) && std::abs(beta) < MATE_VALUE_MAX) {
            MoveList moveList;
            MOVE move;

            moveGen->generate_captures_promotions(moveList, b);
            movePick->scoreMovesQS(moveList, b, ttMove);
            while (moveList.get_next_move(move)) {

                // Skip the move it is not legal
                if (!b.isLegal(move)) {
                    continue;
                }

                b.make_move(move);
                int score = -qsearch(b, th, -1, -probBeta, -probBeta + 1, ply);

                if (score >= probBeta) {
                    score = -pvSearch(b, th, depth - 4, -probBeta, -probBeta + 1, true, ply + 1);
                }
                b.undo_move(move); 

                if (score >= probBeta) {
                    tt->saveTT(th, move, score, staticEval, depth - 3, LOWER_BOUND, posKey, ply);
                    return score;
                }
            }
        }
    }
    

    // Decrease depth for positions not in tt
    // Ed Schröder's iid alternative
    // http://talkchess.com/forum3/viewtopic.php?f=7&t=74769&sid=85d340ce4f4af0ed413fba3188189cd1
    if (depth >= 6 - 3 * isPv - !improving && !hashed) {
        depth--;
    }


    // Search
    int ret = -INFINITY_VAL;
    MOVE bestMove = NULL_MOVE;
    MOVE move;
    int quietsSearched = 0;
    int noisysSearched = 0;
    int numMoves = 0;
    bool isSingular = false;
    MoveList moveList;
    MOVE quiets[MAX_NUM_MOVES];
    MOVE noisys[MAX_NUM_MOVES];
    PrevMoveInfo prev = GetPreviousMoveInfo(b);

    moveGen->generate_all_moves(moveList, b); // Generate moves
    movePick->scoreMoves(moveList, b, prev, th, ply, ttMove);
    while (moveList.get_next_move(move)) {
        bool isQuiet = isQuietMove(move);
        int moveFrom = get_move_from(move);
        int moveTo = get_move_to(move);
        int hist = th->getHistory(b.toMove, isQuiet, moveFrom, moveTo);
        int cmh = isQuiet * th->getCounterHistory(b, prev, moveFrom, moveTo);
        int seeScore = 0;

        if (singMove == move) {
            continue;
        }

        if (numMoves > 0 && ret > -MATE_VALUE_MAX) {
            if (isQuiet) {

                // Futility pruning
                if (!isCheck && depth <= 8 && staticEval + (futilityVal - extLevelMax) * depth <= alpha && std::abs(alpha) < MATE_VALUE_MAX) {
                    continue;
                }

                // Late move pruning
                if (depth <= 8 && quietsSearched > lateMoveMargin[improving][std::max(1, depth - 2 * ttFailLow)]) {
                    continue;
                }

                // History move pruning
                if (depth <= 3 && quietsSearched >= 3 && hist < depth * depth * (-100 - (150 * improving))) {
                    continue;
                }

                // Counter move history pruning
                if (depth <= 3 && quietsSearched >= 3 && cmh < depth * depth * (-125 - (200 * improving))) {
                    continue;
                }

            }

            // SEE pruning
            seeScore = b.seeCapture(move);
            if (depth <= 5 && seeScore < seePruningMargin[isQuiet][depth]) {
                continue;
            }
        }

        // Skip the move it is not legal
        if (!b.isLegal(move)) {
            continue;
        }

        int score;
        int extension = 0;

        // Check extension, passed pawn extension
        if ((isCheck && (extLevel <= 5 || !isQuiet)) || (b.getPiece(moveFrom) == 0 && b.getRankFromSideToMove(moveTo) == 6)) {
            extension = 1;
        }

        // Singular extensions
        if ((depth >= 8 || (extLevel <= 2 && depth >= 6)) && !extension && ttMove == move && hashedBoard.flag != UPPER_BOUND 
            && hashedBoard.depth >= depth - 3 && std::abs(hashedBoard.score) < MATE_VALUE_MAX) {
            int singVal = hashedBoard.score - (1 + isPv) * depth;

            th->searchStack[ply].singMove = move;
            score = pvSearch(b, th, depth / 2 - 1, singVal - 1, singVal, false, ply);
            th->searchStack[ply].singMove = NO_MOVE;

            if (score < singVal) {
                isSingular = true;
                extension = phase >= 213 && !isPv? 2 : 1;

                if (!isPv && depth <= 7 && score < singVal - 2 * depth) {
                    depth++;
                }
            }
            else if (singVal >= beta) {
                return singVal;
            }
            else if (depth >= 8 && hashedBoard.flag == LOWER_BOUND) {
                if (hashedBoard.score >= beta) {
                    extension = -1 - !isPv;
                }
            }
        }

        int newDepth = depth + extension; // Extend
        th->searchStack[ply + 1].extLevel = extLevel + extension;

        b.make_move(move); // Make move

        // First move search at full depth and full window
        if (numMoves == 0) {
            score = -pvSearch(b, th, newDepth - 1, -beta, -alpha, true, ply + 1);
        }
        // Late move reductions
        else if (depth >= 3 && numMoves > isPv) {
            int lmr = lmrReduction[std::min(63, numMoves)][std::min(63, depth)] * (100 + extLevel) / 100; // Base reduction

            lmr -= th->isKiller(ply, move); // Don't reduce as much for killer moves
            lmr -= !isQuiet && seeScore > 0;
            lmr += !improving; // Reduce if evaluation is improving
            lmr -= isPv; // Don't reduce as much for PV nodes
            lmr -= (hist + (!isQuiet * historyLmrNoisyVal) + cmh) / historyLmrVal; // Increase/decrease depth based on histories
            lmr += isQuiet * (quietsSearched > (improving? 40 : 60)); //Adjust if very late move

            if (hashed && !isPv && hashedBoard.flag == UPPER_BOUND && hashedBoard.depth >= depth - 2) {
                lmr -= improving && seeScore >= 0;
            }

            lmr = std::min(depth - 2, std::max(lmr, 0));
            score = -pvSearch(b, th, newDepth - 1 - lmr, -alpha - 1, -alpha, true, ply + 1);
            if (score > alpha) {
                if (lmr > 0) {
                    score = -pvSearch(b, th, newDepth - 1, -alpha - 1, -alpha, true, ply + 1);
                }
                if (score > alpha && score < beta) {
                    score = -pvSearch(b, th, newDepth - 1, -beta, -alpha, true, ply + 1);
                }
            }
        }
        // Null window search
        else {
            score = -pvSearch(b, th, newDepth - 1, -alpha - 1, -alpha, true, ply + 1);
            if (score > alpha && score < beta) {
                score = -pvSearch(b, th, newDepth - 1, -beta, -alpha, true, ply + 1);
            }
        }

        b.undo_move(move); // Undo move

        numMoves++;
        if (score > ret) {
            ret = score;
            if (score > alpha) {
                bestMove = move;
                alpha = score;
                if (score >= beta) {
                    break;
                }
            }
        }

        if (isQuiet) {
            quiets[quietsSearched] = move;
            quietsSearched++;
        }
        else {
            noisys[noisysSearched] = move;
            noisysSearched++;
        }

    }

    // Stop the search
    if (stopable && exit_thread_flag) {
        return 0;
    }

    // Check for checkmates and stalemates
    if (numMoves == 0) {
        return hasSingMove? alpha : (isCheck? -MATE_VALUE + ply : 0);
    }

    // Update Histories
    if (alpha >= beta) {
        if (isQuietMove(bestMove)) {
            th->insertKiller(ply, bestMove);
        }
        th->UpdateHistories(b, prev, quiets, noisys, quietsSearched, noisysSearched, depth + isSingular, ttMove, bestMove);
    }

    // Update Transposition tables
    assert(alpha >= prevAlpha);
    assert (bestMove != 0);
    if (!hasSingMove) {
        int bound = prevAlpha >= ret? UPPER_BOUND : (alpha >= beta? LOWER_BOUND : EXACT);
        tt->saveTT(th, bestMove, ret, staticEval, depth, bound, posKey, ply);
    }
    

    return ret;

}



/**
* The function that searches the root nodes of the current position
*
* The pvSearch function is separated from the pvSearchRoot function as
* the two functions are handled slightly differently.
*
* @param[in, out] b        The board representation.
* @param[in, out] th       A pointer to the thread data that called the function.
* @param[in]      depth    The current depth to search the current position at.
* @param[in]      moveList A list of all pseudo legal moves in the position.
* @param[in]      alpha    AB pruning alpha value.
* @param[in]      beta     AB pruning beta value.
* @param[in]      analysis True if we are in analysis mode.
* @param[in]      id       The ID of the thread that calls it.
* @return                  The info of the best move in the position.
*/
Search::BestMoveInfo Search::pvSearchRoot(Bitboard &b, ThreadSearch *th, int depth, MoveList moveList, int alpha, int beta, bool analysis, int id) {

    th->nodes++;
    MOVE move;
    MOVE bestMove = NULL_MOVE;
    int numMoves = 0;
    int ret = -INFINITY_VAL;
    int ply = 0;
    bool inCheck = b.InCheck();

    // Probe transposition table:
    ZobristVal hashedBoard;
    uint64_t posKey = b.getPosKey();
    bool ttRet = false;
    MOVE ttMove = NO_MOVE;
    bool hashed = tt->probeTT(posKey, hashedBoard, depth, ttRet, ttMove, alpha, beta, ply);

    PrevMoveInfo prev = GetPreviousMoveInfo(b);

    // Initialize evaluation stack
    int staticEval = inCheck? MATE_VALUE + 1 : (hashed? hashedBoard.staticScore : eval->evaluate(b));
    th->searchStack[ply].eval = hashed? hashedBoard.staticScore : staticEval;
    int quietsSearched = 0;

    while (moveList.get_next_move(move)) {

        int tempRet;

        bool isQuiet = isQuietMove(move);
        int moveFrom = get_move_from(move);
        int moveTo = get_move_to(move);
        int hist = th->getHistory(b.toMove, isQuiet, moveFrom, moveTo);
        int cmh = isQuiet? th->getCounterHistory(b, prev, moveFrom, moveTo) : 0;

        // Check for legality
        if (!b.isLegal(move)) {
            continue;
        }

        // UCI information
        if (totalTime > 3000 && canPrintInfo && id == 0 && multiPv == 1) {
            std::cout << "info depth " << depth << " currmove " << TO_ALG[get_move_from(move)] + TO_ALG[get_move_to(move)] << " currmovenumber "<< numMoves + 1 << std::endl;
        }

        b.make_move(move); // Make the move

        // First move search at full depth and full window
        if (numMoves == 0) {
            tempRet = -pvSearch(b, th, depth - 1, -beta, -alpha, true, ply + 1);
        }
        // Late move reductions
        else if (depth >= 3 && numMoves > 1) {
            int lmr = lmrReduction[std::min(63, numMoves)][std::min(63, depth)];
            lmr -= (hist + (!isQuiet * historyLmrNoisyVal) + cmh) / historyLmrVal; // Increase/decrease depth based on histories
            lmr--;

            lmr = std::min(depth - 2, std::max(lmr, 0));
            tempRet = -pvSearch(b, th, depth - 1 - lmr, -alpha - 1, -alpha, true, ply + 1);
            if (tempRet > alpha) {
                if (lmr > 0) {
                    tempRet = -pvSearch(b, th, depth - 1, -alpha - 1, -alpha, true, ply + 1);
                }
                if (tempRet > alpha && tempRet < beta) {
                    tempRet = -pvSearch(b, th, depth - 1, -beta, -alpha, true, ply + 1);
                }
            }
        }
        // Null window search
        else {
            tempRet = -pvSearch(b, th, depth - 1, -alpha - 1, -alpha, true, ply + 1);
            if (tempRet > alpha && tempRet < beta) {
                tempRet = -pvSearch(b, th, depth - 1, -beta, -alpha, true, ply + 1);
            }
        }

        b.undo_move(move); // Undo move

        numMoves++;

        // Stop the search
        if (stopable && (exit_thread_flag || tm.outOfTime())) {
            break;
        }

        if (tempRet > ret) {
            ret = tempRet;
            if (tempRet > alpha) {
                bestMove = move;
                alpha = tempRet;
                if (tempRet >= beta) {
                    break;
                }
            }
        }

        if (isQuiet) {
            quietsSearched++;
        }

    }

    if (numMoves == 0) {
        return BestMoveInfo(NO_MOVE, -9000);
    }

    // Stop the search
    if ((id == 0 && depth == 2 && numMoves == 1 && !analysis)) {
        exit_thread_flag = true;
    }

    // Update transposition table
    if (!exit_thread_flag && !tm.outOfTime()) {
        assert (bestMove != 0);
        tt->saveTT(th, bestMove, ret, staticEval, depth, EXACT, posKey, ply);
    }

    return BestMoveInfo(bestMove, ret);

}



/**
* The function gets the max seldepth across all threads
*
* @return Returns the seldepth.
*/
int Search::getSeldepth() {
    int ret = 0;
    for (int id = 0; id < nThreads; id++) {
        ret = std::max(ret, thread[id].seldepth);
    }
    return ret;
}



/**
* The function gets the total nodes searched across all threads
*
* @return Returns the total nodes searched.
*/
uint64_t Search::getTotalNodesSearched() {
    uint64_t ret = 0;
    for (int id = 0; id < nThreads; id++) {
        ret += thread[id].nodes;
    }
    return ret;
}



/**
* The function that gets the hash usage across all threads
*
* @return Returns the hash usage in permill.
*/
uint64_t Search::getHashFullTotal() {
    uint64_t writes = 0;
    for (int id = 0; id < nThreads; id++) {
        writes += thread[id].ttWrites;
    }
    return tt->getHashFull(writes);
}



/**
* Return true if the eval is mating
*/
bool Search::isMateScore(int eval) {
    if (std::abs(eval) >= MATE_VALUE_MAX) {
        return true;
    }
    return false;
}



/**
* The score of the search, convert to mate score if a mate is found.
*/
int Search::getSearchedScore(int eval) {
    int score = eval;

    // just cause it looks prettier to me
    if (score <= 1 && score >= -1) {
        score = 0;
    }

    if (isMateScore(eval)) {
        score = (MATE_VALUE - std::abs(eval) + 1) / 2;
        if (eval < 0) {
            score = -score;
        }
    }

    return score;
}



/**
* Set the search info
*/
void Search::setSearchInfo(SearchInfo &printInfo, Bitboard &board, int depth, int eval) {
    printInfo.nodes = getTotalNodesSearched();
    printInfo.totalTime = tm.getTimePassed();
    printInfo.nps = (uint64_t) (printInfo.nodes * 1000) / ((double) printInfo.totalTime + 1);
    printInfo.depth = depth;
    printInfo.seldepth = getSeldepth();
    printInfo.score = getSearchedScore(eval);
    printInfo.eval = eval;
    printInfo.hashUsage = getHashFullTotal();
    printInfo.pv = tt->getPv(board);
}



/**
* Print the search info (UCI)
*/
void Search::printSearchInfo(SearchInfo &printInfo, std::string &pstring, MOVE move, int bound, int pv) {

    if (move == NO_MOVE || move == NULL_MOVE) {
        std::cout << pstring << std::endl;
        pstring = "";
        return;
    }

    std::string cpScoreOrMate = " score cp ";
    if (isMateScore(printInfo.eval)) {
        cpScoreOrMate = " score mate ";
    }

    std::string printedBound = "";
    if (bound == LOWER_BOUND) {
        printedBound = " lowerbound";
    }
    else if (bound == UPPER_BOUND) {
        printedBound = " upperbound";
    }

    pstring += "info depth " + std::to_string(printInfo.depth) + " seldepth " + std::to_string(printInfo.seldepth) +
        " multipv " + std::to_string(pv) + cpScoreOrMate + std::to_string(printInfo.score) + printedBound +
        " nodes " + std::to_string(printInfo.nodes) + " nps " + std::to_string(printInfo.nps) + " hashfull " + std::to_string(printInfo.hashUsage) + 
        " time " + std::to_string(printInfo.totalTime) + " pv" + printInfo.pv;

    if (pv == multiPv) {
        std::cout << pstring << std::endl;
        pstring = "";
    }
    else {
        pstring += "\n";
    }
}



/**
* The function that that prepares and calls the search
*
* This includes iterative deepening, generating root moves, and aspiration windows.
* If SMP is enabled, multiple threads will call this function where ID 0 is the main thread
* and the other IDs are helper threads.
*
* @param[in] id       The ID of the thread that calls it.
* @param[in] th       A pointer to the thread data that called the function.
* @param[in] depth    The maximum depth that can be searched.
* @param[in] analysis True if we are in analysis mode.
* @param[in] b        The board representation.
*/
Search::SearchInfo Search::search(int id, ThreadSearch *th, int depth, bool analysis, Bitboard b) {

    MOVE tempBestMove = NO_MOVE;
    MOVE bestMove = NO_MOVE;
    MOVE prevBestMove = NO_MOVE;
    int searchedEval = 0;
    int alpha;
    int beta;
    std::string cpScore;
    SearchInfo printInfo;
    std::string pstring = "";

    PrevMoveInfo prev = GetPreviousMoveInfo(b);

    MoveList moveListOriginal;
    MoveList moveList;
    moveGen->generate_all_moves(moveListOriginal, b);
    movePick->scoreMoves(moveListOriginal, b, prev, th, 0, NO_MOVE);

    if (id == 0) {
        tm.setTimer(moveListOriginal.count);
    }

    for (int i = 1; i <= depth; i++) {

        int delta = ASPIRATION_DELTA + std::abs(searchedEval / 750);
        int aspNum = 0;

        // Use aspiration window with depth >= 4
        alpha = i >= 4? searchedEval - delta : -INFINITY_VAL;
        beta = i >= 4? searchedEval + delta : INFINITY_VAL;

        if (id == 0 && i > 1) {
            stopable = true;
        }

        moveList = moveListOriginal;
        int d = i;
        for (int pv = 1; pv < multiPv + 1; pv++) {
            while (true) {
                d = std::min(i, std::max(d, 1));
                th->seldepth = 1;
                BestMoveInfo bm = pvSearchRoot(b, th, d, moveList, alpha, beta, analysis, id);
                moveList.set_score_move(bm.move, 1400000 + (i * 100) + aspNum);
                searchedEval = bm.eval;

                if (stopable && (exit_thread_flag || tm.outOfTime())) {
                    break;
                }

                if (pv == 1 && bm.move != NULL_MOVE) {
                    moveListOriginal.set_score_move(bm.move, 1400000 + (i * 100) + aspNum);
                    prevBestMove = bestMove;
                    bestMove = bm.move;
                }
                tempBestMove = bm.move;

                if (tempBestMove == NO_MOVE) {
                    break;
                }

                if (id == 0) {
                    setSearchInfo(printInfo, b, i, searchedEval);
                    totalTime = tm.getTimePassed();
                }

                int bound;

                // Fail high
                if (searchedEval >= beta) {
                    beta = searchedEval + delta;
                    bound = LOWER_BOUND;
                    d--;
                }
                // Fail low
                else if (searchedEval <= alpha) {
                    beta = (alpha + beta) / 2;
                    alpha = searchedEval - delta;
                    bound = UPPER_BOUND;
                    d += 2;
                }
                // exact
                else {
                    break;
                }

                aspNum++;
                delta += 10 * delta / std::max(23, 35 - i) + 2;

                if (id == 0 && totalTime > 3000 && canPrintInfo && multiPv == 1) {
                    printSearchInfo(printInfo, pstring, 1, bound, pv);
                }

            }

            // Do not print the search info if the time ran out during a search
            if (stopable && (exit_thread_flag || tm.outOfTime())) {
                break;
            }

            if (id == 0 && canPrintInfo) {
                printSearchInfo(printInfo, pstring, tempBestMove, EXACT, pv);
            }

            if (tempBestMove == NO_MOVE) {
                break;
            }
            
            if (pv < multiPv && !(exit_thread_flag || tm.outOfTimeRoot())) { 
                MOVE checkmove;
                moveList.get_next_move(checkmove);
                assert (checkmove == tempBestMove);
            }

        }

        // Do not print the search info if the time ran out during a search
        if (stopable && (exit_thread_flag || (id == 0 && tm.outOfTime()))) {
            break;
        }

        if (bestMove != prevBestMove && depth > 4) {
            continue;
        }

        if (id == 0 && tm.outOfTimeRootThreshold()) {
            break;
        }
        
        // Print search info if time runs out before next iteration
        if (stopable &&  (exit_thread_flag || (id == 0 && tm.outOfTimeRoot()))) {
            break;
        }

    }

    th->bestMove = bestMove;

    if (id == 0) {
        exit_thread_flag = true;
    }

    return printInfo;
}



/**
* Clears all the data for all threads
*/
void Search::clearThreadData() {
    for (int id = 0; id < nThreads; id++) {
        thread[id].nodes = 0;
        thread[id].seldepth = 0;
        thread[id].nullMoveTree = true;
        thread[id].bestMove = NO_MOVE;
    }
}



void Search::moveToStruct(SearchInfo &si, MOVE move) {
    std::string algMove = "";
    
    switch (move & MOVE_FLAGS) {
        case QUEEN_PROMOTION_FLAG:
            algMove += "q";
            break;
        case QUEEN_PROMOTION_CAPTURE_FLAG:
            algMove += "q";
            break;
        case ROOK_PROMOTION_FLAG:
            algMove += "r";
            break;
        case ROOK_PROMOTION_CAPTURE_FLAG:
            algMove += "r";
            break;
        case BISHOP_PROMOTION_FLAG:
            algMove += "b";
            break;
        case BISHOP_PROMOTION_CAPTURE_FLAG:
            algMove += "b";
            break;
        case KNIGHT_PROMOTION_FLAG:
            algMove += "n";
            break;
        case KNIGHT_PROMOTION_CAPTURE_FLAG:
            algMove += "n";
            break;
    }

    si.s1 = TO_ALG[get_move_from(move)];
    si.s2 = TO_ALG[get_move_to(move)];
    si.promo = algMove;

}


/**
* The function that main calls to get the best move
*
* @param[in, out] b         The board representation.
* @param[in]      depth     The maximum depth that can be searched.
* @param[in]      wtime     The amount of time white has.
* @param[in]      btime     The amount of time black has.
* @param[in]      winc      The increment white has.
* @param[in]      binc      The increment black has.
* @param[in]      movesToGo Moves to go until the next time control.
* @param[in]      analysis  True if we are in analysis mode.
*/
Search::SearchInfo Search::beginSearch(Bitboard &b, int depth, int wtime, int btime, int winc, int binc, int movesToGo, bool analysis) {
    stopable = false;
    totalTime = 0;

    tm = TimeManager(b.getSideToMove(), wtime, btime, winc, binc, movesToGo);
    tt->setTTAge(b.moveHistory.count);
    clearThreadData();

    std::deque<std::thread> threads;
    for (int id = 1; id < nThreads; id++) {
        threads.push_back(std::thread(&Search::search, this, id, &thread[id], depth, analysis, b));
    }

    Search::SearchInfo ret = search(0, &thread[0], depth, analysis, b);

    for (int i = 1; i < nThreads; i++) {
        threads.back().join();
        threads.pop_back();
    }

    MOVE bestMove = thread[0].bestMove;
    assert(bestMove != NULL_MOVE);
    if (canPrintInfo) {
        std::cout << "bestmove " << moveToString(bestMove) << std::endl;
    }

    moveToStruct(ret, bestMove);

    return ret;
}
