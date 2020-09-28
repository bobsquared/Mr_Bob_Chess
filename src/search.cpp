#include "search.h"



uint64_t nodes;
int totalTime;
int seldepth;
int ply;
std::atomic<bool> exit_thread_flag;
TimeManager tm;
bool nullMoveTree;
bool printInfo = true;
SearchStack searchStack[128] = {};

const int seePruningMargin[4] = {0, 0, -350, -500};
const int lateMoveMargin[2][6] = {{0, 5, 8, 13, 23, 34}, {0, 7, 10, 17, 29, 43}};


extern int pieceValues[6];
extern MovePick *movePick;
extern MoveGen *moveGen;
extern Eval *eval;


int lmrReduction[64][64];
void InitLateMoveArray() {
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            lmrReduction[i][j] = (exp(2) * log(i) * log(j) + 12) / (exp(exp(1.02)));
        }
    }
}



int qsearch(Bitboard &b, int depth, int alpha, int beta, int ply) {

    #ifdef DEBUGHASH
    b.debugZobristHash();
    #endif

    nodes++; // update nodes searched
    seldepth = std::max(ply, seldepth); // update seldepth

    // stop the search
    if (exit_thread_flag || tm.outOfTime()) {
        return 0;
    }

    // determine if it is a draw
    if (b.isDraw(ply)) {
        return 0;
    }

    // Probe Transpostion Table:
    ZobristVal hashedBoard;
    uint64_t posKey = b.getPosKey();
    bool ttRet = false;
    b.probeTTQsearch(posKey, hashedBoard, ttRet, alpha, beta, ply);

    if (ttRet) {
        return hashedBoard.score;
    }


    bool inCheck = b.InCheck();
    int prevAlpha = alpha;
    int stand_pat = inCheck? -MATE_VALUE + ply : 0;
    if (!inCheck) {
        stand_pat = eval->evaluate(b);

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
    movePick->scoreMoves(moveList, b, 0, hashedBoard.move);
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
        int score = -qsearch(b, depth - 1, -beta, -alpha, ply + 1);
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
                    b.saveTT(move, score, depth, 1, posKey, ply);
                    return score;
                }
            }
        }

    }

    if (numMoves > 0) {
        if (prevAlpha >= stand_pat) {
            assert (bestMove != 0);
            b.saveTT(bestMove, stand_pat, depth, 2, posKey, ply);
        }
        else {
            assert (bestMove != 0);
            b.saveTT(bestMove, stand_pat, depth, 0, posKey, ply);
        }
    }

    return stand_pat;

}





int pvSearch(Bitboard &b, int depth, int alpha, int beta, bool canNullMove, int ply) {

    #ifdef DEBUGHASH
    b.debugZobristHash();
    #endif

    seldepth = std::max(ply, seldepth); // update seldepth

    // Check if there are any potential wins that don't require help mate.
    if (beta > 0 && b.noPotentialWin()) {
        if (alpha >= 0) {
            nodes++;
            return 0;
        }
    }

    // Dive into Quiesence search
    if (depth <= 0) {
        return qsearch(b, depth - 1, alpha, beta, ply);
    }

    nodes++; // Increment number of nodes

    // Stop the search
    if (exit_thread_flag || tm.outOfTime()) {
        return 0;
    }

    // Determine if the position is a textbook draw
    if (b.isDraw(ply)) {
        return 0;
    }



    MOVE move;
    MOVE bestMove = NO_MOVE;
    MoveList moveList;

    int ret = -INFINITY_VAL;
    int numMoves = 0;
    int prevAlpha = alpha;
    bool isPv = alpha == beta - 1? false : true;


    // Probe Transpostion Table:
    ZobristVal hashedBoard;
    uint64_t posKey = b.getPosKey();
    bool ttRet = false;
    bool hashed = b.probeTT(posKey, hashedBoard, depth, ttRet, alpha, beta, ply);

    if (ttRet) {
        return hashedBoard.score;
    }


    int staticEval = hashed? hashedBoard.score : eval->evaluate(b);
    searchStack[ply].eval = staticEval;
    bool improving = ply >= 2? staticEval > searchStack[ply - 2].eval : false;
    bool isCheck = b.InCheck();
    b.removeKiller(ply + 1);


    // Razoring
    if (!isPv && !isCheck && depth <= 1 && staticEval <= alpha - 350) {
        return qsearch(b, -1, alpha, beta, ply);
    }


    // Reverse futility pruning
    if (!isPv && !isCheck && depth <= 6 && staticEval - 220 * depth + (55 * depth * improving) >= beta && std::abs(beta) < MATE_VALUE_MAX) {
        return staticEval;
    }


    // Null move pruning
    if (!isPv && canNullMove && !isCheck && staticEval >= beta && depth >= 2 && nullMoveTree && b.nullMoveable()) {
        int R = 3 + depth / 8;
        b.make_null_move();
        int nullRet = -pvSearch(b, depth - R - 1, -beta, -beta + 1, false, ply + 1);
        b.undo_null_move();

        if (nullRet >= beta && std::abs(nullRet) < MATE_VALUE_MAX) {

            if (depth >= 8) {
                nullMoveTree = false;
                nullRet = pvSearch(b, depth - R - 1, beta - 1, beta, false, ply + 1);
                nullMoveTree = true;
            }

            if (nullRet >= beta) {
                return nullRet;
            }

        }
    }


    // Mate distance pruning
    int mateDistance = MATE_VALUE - ply;
    if (mateDistance < beta) {
        beta = mateDistance;
        if (alpha >= mateDistance) {
            return mateDistance;
        }
    }

    mateDistance = -MATE_VALUE + ply;
    if (mateDistance > alpha) {
        alpha = mateDistance;
        if (beta <= mateDistance) {
            return mateDistance;
        }
    }


    // Search
    int quietsSearched = 0;
    MOVE quiets[MAX_NUM_MOVES];
    moveGen->generate_all_moves(moveList, b); // Generate moves
    movePick->scoreMoves(moveList, b, ply, hashedBoard.move);
    while (moveList.get_next_move(move)) {
        int score;
        int extension = 0;
        bool isQuiet = (move & (CAPTURE_FLAG | PROMOTION_FLAG)) == 0;

        if (!isPv && ret > -MATE_VALUE_MAX) {
            if (isQuiet) {

                // Futility pruning
                if (depth <= 6 && numMoves > 0 && staticEval + 215 * depth <= alpha && std::abs(alpha) < MATE_VALUE_MAX) {
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


        // Check extension
        if (isCheck) {
            extension = 1;
        }

        int newDepth = depth + extension; // Extend

        b.make_move(move); // Make move

        // First move search at full depth and full window
        if (numMoves == 0) {
            score = -pvSearch(b, newDepth - 1, -beta, -alpha, true, ply + 1);
        }
        // Late move reductions
        else if (depth >= 3 && isQuiet && !isCheck) {
            int lmr = lmrReduction[std::min(63, numMoves)][std::min(63, depth)]; // Base reduction

            lmr -= b.isKiller(ply, move); // Don't reduce as much for killer moves
            lmr += !improving; // Reduce if evaluation is improving
            lmr -= 2 * isPv; // Don't reduce as much for PV nodes

            lmr = std::min(depth - 2, std::max(lmr, 0));
            score = -pvSearch(b, newDepth - 1 - lmr, -alpha - 1, -alpha, true, ply + 1);
            if (score > alpha) {
                if (lmr > 0) {
                    score = -pvSearch(b, newDepth - 1, -alpha - 1, -alpha, true, ply + 1);
                }
                if (score > alpha && score < beta) {
                    score = -pvSearch(b, newDepth - 1, -beta, -alpha, true, ply + 1);
                }
            }
        }
        // Null window search
        else {
            score = -pvSearch(b, newDepth - 1, -alpha - 1, -alpha, true, ply + 1);
            if (score > alpha && score < beta) {
                score = -pvSearch(b, newDepth - 1, -beta, -alpha, true, ply + 1);
            }
        }

        b.undo_move(move); // Undo move

        // Stop the search
        if (exit_thread_flag || tm.outOfTime()) {
            return 0;
        }

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


    // Update Histories
    if (alpha >= beta && ((bestMove & (CAPTURE_FLAG | PROMOTION_FLAG)) == 0)) {
        b.insertKiller(ply, bestMove);
        b.insertCounterMove(bestMove);

        int hist = history[b.getSideToMove()][get_move_from(bestMove)][get_move_to(bestMove)] * std::min(depth, 20) / 23;
        history[b.getSideToMove()][get_move_from(bestMove)][get_move_to(bestMove)] += 32 * (depth * depth) - hist;

        for (int i = 0; i < quietsSearched; i++) {
            int hist2 = history[b.getSideToMove()][get_move_from(quiets[i])][get_move_to(quiets[i])] * std::min(depth, 20) / 23;
            history[b.getSideToMove()][get_move_from(quiets[i])][get_move_to(quiets[i])] += 30 * (-depth * depth) - hist2;
        }
    }


    // Check for checkmates and stalemates
    if (numMoves == 0) {
        if (isCheck) {
            return -MATE_VALUE + ply;
        }
        else {
            return 0;
        }
    }

    // Stop the search
    if (exit_thread_flag || tm.outOfTime()) {
        return 0;
    }

    // Update Transposition tables
    assert(alpha >= prevAlpha);
    if (alpha >= beta) {
        assert(move != 0);
        b.saveTT(bestMove, ret, depth, 1, posKey, ply);
    }
    else if (prevAlpha >= ret) {
        assert (bestMove != 0);
        b.saveTT(bestMove, ret, depth, 2, posKey, ply);
    }
    else {
        assert (bestMove != 0);
        b.saveTT(bestMove, ret, depth, 0, posKey, ply);
    }

    return ret;

}



BestMoveInfo pvSearchRoot(Bitboard &b, int depth, MoveList moveList, int alpha, int beta, bool analysis) {

    nodes++;
    MOVE move;
    MOVE bestMove = 0;
    int numMoves = 0;
    int ret = -INFINITY_VAL;
    int ply = 0;
    bool inCheck = b.InCheck();

    // Probe transposition table:
    ZobristVal hashedBoard;
    uint64_t posKey = b.getPosKey();
    bool ttRet = false;
    bool hashed = b.probeTT(posKey, hashedBoard, depth, ttRet, alpha, beta, ply);


    // Initialize evaluation stack
    searchStack[ply].eval = hashed? hashedBoard.score : eval->evaluate(b);


    while (moveList.get_next_move(move)) {

        int tempRet;

        // Check for legality
        if (!b.isLegal(move)) {
            continue;
        }

        // UCI information
        if (totalTime > 3000 && printInfo) {
            std::cout << "info depth " << depth << " currmove " << TO_ALG[get_move_from(move)] + TO_ALG[get_move_to(move)] << " currmovenumber "<< numMoves + 1 << std::endl;
        }


        b.make_move(move); // Make the move

        // First move search at full depth and full window
        if (numMoves == 0) {
            tempRet = -pvSearch(b, depth - 1, -beta, -alpha, true, ply + 1);
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

            lmr = std::min(depth - 1, std::max(lmr, 0));
            tempRet = -pvSearch(b, depth - 1 - lmr, -alpha - 1, -alpha, true, ply + 1);
            if (tempRet > alpha) {
                if (lmr > 0) {
                    tempRet = -pvSearch(b, depth - 1, -alpha - 1, -alpha, true, ply + 1);
                }
                if (tempRet > alpha && tempRet < beta) {
                    tempRet = -pvSearch(b, depth - 1, -beta, -alpha, true, ply + 1);
                }
            }
        }
        // Null window search
        else {
            tempRet = -pvSearch(b, depth - 1, -alpha - 1, -alpha, true, ply + 1);
            if (tempRet > alpha && tempRet < beta) {
                tempRet = -pvSearch(b, depth - 1, -beta, -alpha, true, ply + 1);
            }
        }

        b.undo_move(move); // Undo move

        numMoves++;

        // Stop the search
        if (exit_thread_flag || tm.outOfTime()) {
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
    if (numMoves == 0 || (depth == 2 && numMoves == 1 && !analysis)) {
        exit_thread_flag = true;
    }

    // Update transposition table
    if (!exit_thread_flag && !tm.outOfTime()) {
        assert (bestMove != 0);
        b.saveTT(bestMove, ret, depth, 0, posKey, ply);
    }

    return BestMoveInfo(bestMove, ret);

}



void search(Bitboard &b, int depth, int wtime, int btime, int winc, int binc, int movesToGo, bool analysis) {

    MOVE bestMove;
    MoveList moveList;
    std::string algMove;
    uint64_t nps;
    nodes = 0;
    totalTime = 0;
    nullMoveTree = true;

    ZobristVal hashedBoard;
    uint64_t posKey = b.getPosKey();
    bool ttRet = false;

    int alpha;
    int beta;
    int tempAlpha;
    int tempBeta;
    int score = 0;
    std::string cpScore;

    b.clearHashStats();
    b.setTTAge();
    moveGen->generate_all_moves(moveList, b);
    movePick->scoreMoves(moveList, b, 0, NO_MOVE);

    tm = TimeManager(b.getSideToMove(), wtime, btime, winc, binc, movesToGo, moveList.count);
    for (int i = 1; i <= depth; i++) {

        int delta = ASPIRATION_DELTA;
        int aspNum = 0;

        seldepth = 1;

        // Use aspiration window with depth >= 4
        if (i >= 4) {
            alpha = hashedBoard.score - delta;
            beta = hashedBoard.score + delta;
        }
        else {
            alpha = -INFINITY_VAL;
            beta = INFINITY_VAL;
        }


        while (true) {

            pvSearchRoot(b, i, moveList, alpha, beta, analysis);
            bool hashed = b.probeTT(posKey, hashedBoard, i, ttRet, tempAlpha, tempBeta, 0);

            if (i > 1 && !exit_thread_flag && !tm.outOfTime()) {
                assert(hashed);
                (void) hashed;
            }

            moveList.set_score_move(hashedBoard.move, 1400000 + (i * 100) + aspNum);

            if (exit_thread_flag || tm.outOfTime()) {
                break;
            }

            if (hashedBoard.score > alpha) {
                bestMove = hashedBoard.move;
                algMove = TO_ALG[get_move_from(bestMove)] + TO_ALG[get_move_to(bestMove)];

                switch (bestMove & MOVE_FLAGS) {
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
            }


            cpScore = " score cp ";
            score = hashedBoard.score;
            if (std::abs(hashedBoard.score) >= MATE_VALUE_MAX) {
                score = (MATE_VALUE - std::abs(hashedBoard.score) + 1) / 2;
                if (hashedBoard.score < 0) {
                    score = -score;
                }
                cpScore = " score mate ";
            }

            totalTime = tm.getTimePassed();
            nps = (uint64_t) ((double) nodes * 1000.0) / ((double) totalTime + 1);

            // Update the aspiration score
            // Fail high
            if (hashedBoard.score >= beta) {
                beta = hashedBoard.score + delta;

                if (totalTime > 3000 && printInfo) {
                    std::cout << "info depth " << i << " seldepth " << seldepth << cpScore << score << " lowerbound"
                        " nodes " << nodes << " nps " << nps << " hashfull " << b.getHashFull() << " time " << totalTime << " pv" << b.getPv() << std::endl;
                }
            }
            // Fail low
            else if (hashedBoard.score <= alpha) {
                beta = (alpha + beta) / 2;
                alpha = hashedBoard.score - delta;

                if (totalTime > 3000 && printInfo) {
                    std::cout << "info depth " << i << " seldepth " << seldepth << cpScore << score << " upperbound"
                        " nodes " << nodes << " nps " << nps << " hashfull " << b.getHashFull() << " time " << totalTime << " pv" << b.getPv() << std::endl;
                }
            }
            // exact
            else {
                break;
            }

            aspNum++;
            delta += delta / 3 + 3;

        }

        if (exit_thread_flag || tm.outOfTime()) {
            break;
        }

        if (printInfo) {
            std::cout << "info depth " << i << " seldepth " << seldepth << cpScore << score <<
                " nodes " << nodes << " nps " << nps << " hashfull " << b.getHashFull() << " time " << totalTime << " pv" << b.getPv() << std::endl;
        }


    }

    std::cout << "bestmove " << algMove << std::endl;

}
