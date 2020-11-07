#include "search.h"


int totalTime;
std::atomic<bool> exit_thread_flag;
TimeManager tm;
bool printInfo = true;
ThreadSearch thread[256] = {};

int nThreads = 1;
bool stopable = false;
const int seePruningMargin[4] = {0, 0, -350, -500};
const int lateMoveMargin[2][6] = {{0, 5, 8, 13, 23, 34}, {0, 7, 10, 17, 29, 43}};


extern int pieceValues[6];
extern MovePick *movePick;
extern MoveGen *moveGen;
extern Eval *eval;
extern TranspositionTable *tt;


int lmrReduction[64][64];
void InitLateMoveArray() {
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            lmrReduction[i][j] = (exp(2) * log(i) * log(j) + 12) / (exp(exp(1.02)));
        }
    }
}



int qsearch(Bitboard &b, ThreadSearch *th, int depth, int alpha, int beta, int ply) {

    #ifdef DEBUGHASH
    b.debugZobristHash();
    #endif

    th->nodes++; // update nodes searched
    th->seldepth = std::max(ply, th->seldepth); // update seldepth

    // stop the search
    #ifndef TUNER
    if (stopable && exit_thread_flag) {
        return 0;
    }
    #endif

    // determine if it is a draw
    if (b.isDraw(ply)) {
        return 0;
    }

    // Probe Transpostion Table:
    ZobristVal hashedBoard;
    uint64_t posKey = b.getPosKey();
    bool ttRet = false;

    #ifndef TUNER
    bool hashed = b.probeTTQsearch(posKey, hashedBoard, ttRet, alpha, beta, ply);

    if (ttRet) {
        return hashedBoard.score;
    }
    #endif


    bool inCheck = b.InCheck();
    int prevAlpha = alpha;
    int stand_pat = inCheck? -MATE_VALUE + ply : 0;
    int staticEval = hashed? hashedBoard.staticScore : eval->evaluate(b, th);
    if (!inCheck) {
        stand_pat = staticEval;

        // standing pat
        if (stand_pat >= beta) {
            return stand_pat;
        }

        if (alpha < stand_pat) {
            alpha = stand_pat;
        }

        // delta pruning
        if (stand_pat < alpha - MGVAL(pieceValues[4])) {
            return stand_pat;
        }
    }

    MOVE move;
    MOVE bestMove = NO_MOVE;
    MoveList moveList;
    int numMoves = 0;

    inCheck? moveGen->generate_all_moves(moveList, b) : moveGen->generate_captures_promotions(moveList, b);
    movePick->scoreMoves(moveList, b, th, 0, hashedBoard.move);
    while (moveList.get_next_move(move)) {

        // Prune negative SEE
        if (!inCheck && (move & PROMOTION_FLAG) == 0 && b.seeCapture(move) < 0) {
            continue;
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

        if (bestMove == NO_MOVE) {
            bestMove = move;
        }

        if (score > stand_pat) {
            stand_pat = score;
            bestMove = move;
            if (score > alpha) {
                alpha = score;
                if (score >= beta) {
                    break;
                }
            }
        }

    }

    #ifndef TUNER
    if (stopable && exit_thread_flag) {
        return 0;
    }
    #endif

    #ifndef TUNER
    if (numMoves > 0) {
        assert (bestMove != 0);
        int bound = prevAlpha >= stand_pat? UPPER_BOUND : (stand_pat >= beta? LOWER_BOUND : EXACT);
        tt->saveTT(th, bestMove, stand_pat, staticEval, depth, bound, posKey, ply);
    }
    #endif

    return stand_pat;

}





int pvSearch(Bitboard &b, ThreadSearch *th, int depth, int alpha, int beta, bool canNullMove, int ply) {

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
    if (depth <= 0) {
        return qsearch(b, th, depth - 1, alpha, beta, ply);
    }

    th->nodes++; // Increment number of nodes

    // Stop the search
    if (stopable && (exit_thread_flag || tm.outOfTime())) {
        return 0;
    }

    // Determine if the position is a textbook draw
    if (b.isDraw(ply)) {
        return 0;
    }



    int prevAlpha = alpha;
    bool isPv = alpha == beta - 1? false : true;
    MOVE singMove = th->searchStack[ply].singMove;


    // Probe Transpostion Table:
    ZobristVal hashedBoard;
    uint64_t posKey = b.getPosKey();
    bool ttRet = false;
    bool hashed = false;

    if (singMove == NO_MOVE) {
        hashed = b.probeTT(posKey, hashedBoard, depth, ttRet, alpha, beta, ply);

        if (ttRet && !isPv) {
            return hashedBoard.score;
        }
    }


    // Mate distance pruning
    beta = MATE_VALUE - ply < beta? MATE_VALUE - ply : beta;
    alpha = -MATE_VALUE + ply > alpha? -MATE_VALUE + ply : alpha;

    if (alpha >= beta) {
        return alpha == -MATE_VALUE + ply? alpha : beta;
    }

    int staticEval = hashed? hashedBoard.staticScore : eval->evaluate(b, th);
    bool improving = ply >= 2? staticEval > th->searchStack[ply - 2].eval : false;
    bool failing = ply >= 2? staticEval + 32 * depth < th->searchStack[ply - 2].eval : false;
    bool isCheck = b.InCheck();

    b.removeKiller(th, ply + 1);
    th->searchStack[ply].eval = staticEval;


    // Razoring
    if (!isPv && !isCheck && depth <= 1 && staticEval <= alpha - 350) {
        return qsearch(b, th, -1, alpha, beta, ply);
    }

    // Reverse futility pruning
    if (!isPv && !isCheck && depth <= 8 && singMove == NO_MOVE && staticEval - 185 * depth + (55 * depth * improving) >= beta && std::abs(beta) < MATE_VALUE_MAX) {
        return staticEval;
    }

    // Null move pruning
    if (!isPv && canNullMove && !isCheck && staticEval >= beta && depth >= 2 && th->nullMoveTree && b.nullMoveable()) {
        int R = 3 + depth / (7 - improving);

        b.make_null_move();
        int nullRet = -pvSearch(b, th, depth - R - 1, -beta, -beta + 1, false, ply + 1);
        b.undo_null_move();

        if (nullRet >= beta && std::abs(nullRet) < MATE_VALUE_MAX) {

            if (depth >= 8) {
                th->nullMoveTree = false;
                nullRet = pvSearch(b, th, depth - R - 1, beta - 1, beta, false, ply + 1);
                th->nullMoveTree = true;
            }

            if (nullRet >= beta) {
                return nullRet;
            }

        }
    }


    // Search
    int ret = -INFINITY_VAL;
    MOVE bestMove = NO_MOVE;
    MOVE move;
    int quietsSearched = 0;
    int numMoves = 0;
    MoveList moveList;
    MOVE quiets[MAX_NUM_MOVES];

    moveGen->generate_all_moves(moveList, b); // Generate moves
    movePick->scoreMoves(moveList, b, th, ply, hashedBoard.move);
    while (moveList.get_next_move(move)) {
        bool isQuiet = (move & (CAPTURE_FLAG | PROMOTION_FLAG)) == 0;

        if (singMove == move) {
            continue;
        }

        if (!isPv && ret > -MATE_VALUE_MAX) {
            if (isQuiet) {

                // Futility pruning
                if (depth <= 6 && numMoves > 0 && staticEval + 135 * depth <= alpha && std::abs(alpha) < MATE_VALUE_MAX) {
                    continue;
                }

                // Late move pruning
                if (depth <= 5 && quietsSearched > lateMoveMargin[improving][depth]) {
                    break;
                }

            }

            // SEE pruning
            if (depth <= 3 && numMoves > 0 && (move & PROMOTION_FLAG) == 0 && b.seeCapture(move) < seePruningMargin[depth]) {
                continue;
            }
        }

        // Skip the move it is not legal
        if (!b.isLegal(move)) {
            continue;
        }

        int score;
        int extension = 0;

        // Check extension
        if (isCheck) {
            extension = 1;
        }

        if (depth >= 8 && !extension && hashed && hashedBoard.move == move && hashedBoard.flag != UPPER_BOUND && hashedBoard.depth >= depth - 3 && std::abs(hashedBoard.score) < MATE_VALUE_MAX) {
            int singVal = hashedBoard.score - 2 * depth;
            th->searchStack[ply].singMove = move;
            score = pvSearch(b, th, depth / 2, singVal - 1, singVal, false, ply);
            th->searchStack[ply].singMove = NO_MOVE;

            if (score < singVal) {
                extension = 1;
            }

        }

        int newDepth = depth + extension; // Extend

        b.make_move(move); // Make move

        // First move search at full depth and full window
        if (numMoves == 0) {
            score = -pvSearch(b, th, newDepth - 1, -beta, -alpha, true, ply + 1);
        }
        // Late move reductions
        else if (depth >= 3 && isQuiet && !isCheck) {
            int lmr = lmrReduction[std::min(63, numMoves)][std::min(63, depth)]; // Base reduction

            lmr -= b.isKiller(th, ply, move); // Don't reduce as much for killer moves
            lmr += !improving + failing; // Reduce if evaluation is improving (reduce more if evaluation fails)
            lmr -= 2 * isPv; // Don't reduce as much for PV nodes
            lmr -= th->history[!b.getSideToMove()][get_move_from(move)][get_move_to(move)] / 2500;

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
            bestMove = move;
            if (score > alpha) {
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

    }

    // Stop the search
    if (stopable && (exit_thread_flag || tm.outOfTime())) {
        return 0;
    }

    // Check for checkmates and stalemates
    if (numMoves == 0) {
        return singMove != NO_MOVE? alpha : (isCheck? -MATE_VALUE + ply : 0);
    }

    // Update Histories
    if (alpha >= beta && ((bestMove & (CAPTURE_FLAG | PROMOTION_FLAG)) == 0) && singMove == NO_MOVE) {
        b.insertKiller(th, ply, bestMove);
        b.insertCounterMove(th, bestMove);

        int hist = th->history[b.getSideToMove()][get_move_from(bestMove)][get_move_to(bestMove)] * std::min(depth, 20) / 23;
        th->history[b.getSideToMove()][get_move_from(bestMove)][get_move_to(bestMove)] += 32 * (depth * depth) - hist;

        for (int i = 0; i < quietsSearched; i++) {
            hist = th->history[b.getSideToMove()][get_move_from(quiets[i])][get_move_to(quiets[i])] * std::min(depth, 20) / 23;
            th->history[b.getSideToMove()][get_move_from(quiets[i])][get_move_to(quiets[i])] += 30 * (-depth * depth) - hist;
        }
    }

    // Update Transposition tables
    assert(alpha >= prevAlpha);
    assert (bestMove != 0);
    if (singMove == NO_MOVE) {
        int bound = prevAlpha >= ret? UPPER_BOUND : (alpha >= beta? LOWER_BOUND : EXACT);
        tt->saveTT(th, bestMove, ret, staticEval, depth, bound, posKey, ply);
    }


    return ret;

}



BestMoveInfo pvSearchRoot(Bitboard &b, ThreadSearch *th, int depth, MoveList moveList, int alpha, int beta, bool analysis, int id) {

    th->nodes++;
    MOVE move;
    MOVE bestMove = 0;
    int numMoves = 0;
    int ret = -INFINITY_VAL;
    int ply = 0;
    int staticEval = eval->evaluate(b, th);
    bool inCheck = b.InCheck();

    // Probe transposition table:
    ZobristVal hashedBoard;
    uint64_t posKey = b.getPosKey();
    bool ttRet = false;
    bool hashed = b.probeTT(posKey, hashedBoard, depth, ttRet, alpha, beta, ply);


    // Initialize evaluation stack
    th->searchStack[ply].eval = hashed? hashedBoard.staticScore : staticEval;


    while (moveList.get_next_move(move)) {

        int tempRet;

        // Check for legality
        if (!b.isLegal(move)) {
            continue;
        }

        // UCI information
        if (totalTime > 3000 && printInfo && id == 0) {
            std::cout << "info depth " << depth << " currmove " << TO_ALG[get_move_from(move)] + TO_ALG[get_move_to(move)] << " currmovenumber "<< numMoves + 1 << std::endl;
        }


        b.make_move(move); // Make the move

        // First move search at full depth and full window
        if (numMoves == 0) {
            tempRet = -pvSearch(b, th, depth - 1, -beta, -alpha, true, ply + 1);
            if (tempRet <= alpha) {
                numMoves++;
                bestMove = move;
                ret = tempRet;
                b.undo_move(move);
                break;
            }
        }
        // Late move reductions
        else if (depth >= 3 && !inCheck && (move & CAPTURE_FLAG) == 0 && (move & PROMOTION_FLAG) == 0) {
            int lmr = lmrReduction[std::min(63, numMoves)][std::min(63, depth)];
            lmr -= th->history[!b.getSideToMove()][get_move_from(move)][get_move_to(move)] / 1500;
            lmr -= 2;

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
            bestMove = move;
            if (tempRet > alpha) {
                alpha = tempRet;
                if (tempRet >= beta) {
                    break;
                }
            }
        }

    }

    // Stop the search
    if (numMoves == 0 || (id == 0 && depth == 2 && numMoves == 1 && !analysis)) {
        exit_thread_flag = true;
    }

    // Update transposition table
    if (!exit_thread_flag && !tm.outOfTime()) {
        assert (bestMove != 0);
        tt->saveTT(th, bestMove, ret, staticEval, depth, EXACT, posKey, ply);
    }

    return BestMoveInfo(bestMove, ret);

}



int getSeldepth() {
    int ret = 0;
    for (int id = 0; id < nThreads; id++) {
        ret = std::max(ret, thread[id].seldepth);
    }
    return ret;
}



uint64_t getTotalNodesSearched() {
    uint64_t ret = 0;
    for (int id = 0; id < nThreads; id++) {
        ret += thread[id].nodes;
    }
    return ret;
}



uint64_t getHashFullTotal(Bitboard &b) {
    uint64_t writes = 0;
    for (int id = 0; id < nThreads; id++) {
        writes += thread[id].ttWrites;
    }
    return b.getHashFull(writes);
}



void search(int id, ThreadSearch *th, int depth, bool analysis, Bitboard b) {

    MOVE bestMove = NO_MOVE;
    MoveList moveList;
    uint64_t nps;
    uint64_t nodes;


    int alpha;
    int beta;
    int score = 0;
    int compareScore = 0;
    std::string cpScore;

    moveGen->generate_all_moves(moveList, b);
    movePick->scoreMoves(moveList, b, th, 0, NO_MOVE);

    if (id == 0) {
        tm.setTimer(moveList.count);
    }

    for (int i = 1; i <= depth; i++) {

        int delta = ASPIRATION_DELTA;
        int aspNum = 0;

        th->seldepth = 1;

        // Use aspiration window with depth >= 4
        if (i >= 4) {
            alpha = compareScore - delta;
            beta = compareScore + delta;
        }
        else {
            alpha = -INFINITY_VAL;
            beta = INFINITY_VAL;
        }

        if (id == 0 && i > 1) {
            stopable = true;
        }


        while (true) {

            BestMoveInfo bm = pvSearchRoot(b, th, i, moveList, alpha, beta, analysis, id);
            compareScore = bm.eval;
            moveList.set_score_move(bm.move, 1400000 + (i * 100) + aspNum);

            if (stopable && (exit_thread_flag || tm.outOfTime())) {
                break;
            }

            bestMove = bm.move;
            cpScore = " score cp ";
            score = compareScore;
            if (std::abs(compareScore) >= MATE_VALUE_MAX) {
                score = (MATE_VALUE - std::abs(compareScore) + 1) / 2;
                if (compareScore < 0) {
                    score = -score;
                }
                cpScore = " score mate ";
            }



            if (id == 0) {
                totalTime = tm.getTimePassed();
                nodes = getTotalNodesSearched();
                nps = (uint64_t) ((double) nodes * 1000.0) / ((double) totalTime + 1);
            }

            // Update the aspiration score
            // Fail high
            if (compareScore >= beta) {
                beta = compareScore + delta;

                if (id == 0 && totalTime > 3000 && printInfo) {
                    std::cout << "info depth " << i << " seldepth " << getSeldepth() << cpScore << score << " lowerbound"
                        " nodes " << nodes << " nps " << nps << " hashfull " << getHashFullTotal(b) << " time " << totalTime << " pv" << b.getPv() << std::endl;
                }
            }
            // Fail low
            else if (compareScore <= alpha) {
                beta = (alpha + beta) / 2;
                alpha = compareScore - delta;

                if (id == 0 && totalTime > 3000 && printInfo) {
                    std::cout << "info depth " << i << " seldepth " << getSeldepth() << cpScore << score << " upperbound"
                        " nodes " << nodes << " nps " << nps << " hashfull " << getHashFullTotal(b) << " time " << totalTime << " pv" << b.getPv() << std::endl;
                }
            }
            // exact
            else {
                break;
            }

            aspNum++;
            delta += delta / 3 + compareScore <= alpha? 5 : 3;

        }

        if (stopable && (exit_thread_flag || tm.outOfTime())) {
            break;
        }

        if (id == 0 && printInfo) {
            std::cout << "info depth " << i << " seldepth " << getSeldepth() << cpScore << score <<
                " nodes " << nodes << " nps " << nps << " hashfull " << getHashFullTotal(b) << " time " << totalTime << " pv" << b.getPv() << std::endl;
        }

    }

    th->bestMove = bestMove;

    if (id == 0) {
        exit_thread_flag = true;
    }

}



void beginSearch(Bitboard &b, int depth, int wtime, int btime, int winc, int binc, int movesToGo, bool analysis) {
    tm = TimeManager(b.getSideToMove(), wtime, btime, winc, binc, movesToGo);
    b.setTTAge();

    stopable = false;
    totalTime = 0;
    std::deque<std::thread> threads;
    for (int id = 1; id < nThreads; id++) {
        thread[id].nodes = 0;
        thread[id].seldepth = 0;
        thread[id].nullMoveTree = true;
        thread[id].bestMove = NO_MOVE;
        threads.push_back(std::thread(search, id, &thread[id], depth, analysis, b));
    }

    thread[0].nodes = 0;
    thread[0].seldepth = 0;
    thread[0].nullMoveTree = true;
    thread[0].bestMove = NO_MOVE;
    search(0, &thread[0], depth, analysis, b);

    for (int i = 1; i < nThreads; i++) {
        threads.back().join();
        threads.pop_back();
    }

    MOVE bestMove = thread[0].bestMove;
    assert(bestMove != NO_MOVE && bestMove != NULL_MOVE);
    std::cout << "bestmove " << moveToString(bestMove) << std::endl;
}
