#include "search.h"



uint64_t nodes;
double totalTime;
int seldepth;
int height;
std::atomic<bool> exit_thread_flag;



int qsearch(Bitboard &b, int depth, int alpha, int beta) {

    #ifdef DEBUGHASH
    b.debugZobristHash();
    #endif

    nodes++;
    seldepth = std::min(depth, seldepth);
    if (exit_thread_flag) {
        return 0;
    }

    if (b.isDraw()) {
        return 0;
    }

    bool inCheck = b.InCheck();
    int stand_pat = inCheck? -9999 + depth : 0;
    if (!inCheck) {
        stand_pat = b.evaluate();
        if (stand_pat >= beta) {
            return stand_pat;
        }

        if (alpha < stand_pat) {
            alpha = stand_pat;
        }

        if (stand_pat < alpha - pieceValues[4]) {
            return stand_pat;
        }
    }

    MOVE move;
    MoveList moveList;
    int ret = -INFINITY;
    int numMoves = 0;

    inCheck? b.generate(moveList, 0, NO_MOVE) : b.generate_captures_promotions(moveList, NO_MOVE);
    while (moveList.get_next_move(move)) {
        // assert(CAPTURE_FLAG & move || PROMOTION_FLAG & move);

        b.make_move(move);
        if (b.InCheckOther()) {
            b.undo_move(move);
            continue;
        }


        numMoves++;
        // if ((move & MOVE_FLAGS) != QUIET_MOVES_FLAG && (move & PROMOTION_FLAG) == 0 && b.seeCapture(move) < 0) {
        //     b.undo_move(move);
        //     continue;
        // }

        int score = -qsearch(b, depth - 1, -beta, -alpha);
        b.undo_move(move);
        if (score > ret) {
            ret = score;
            if (score > alpha) {
                alpha = score;
                if (score >= beta) {
                    return score;
                }
            }
        }

    }


    return numMoves == 0? stand_pat : ret;

}





int pvSearch(Bitboard &b, int depth, int alpha, int beta, bool canNullMove, int height) {

    #ifdef DEBUGHASH
    b.debugZobristHash();
    #endif

    if (exit_thread_flag) {
        return 0;
    }

    if (b.isDraw()) {
        return 0;
    }

    if (alpha < 0 && b.isRepetition()) {
        if (beta <= 0) {
            return 0;
        }
    }

    if (depth <= 0) {
        return qsearch(b, depth - 1, alpha, beta);
    }

    nodes++;

    MOVE move;
    MOVE bestMove;
    MoveList moveList;
    bool ttRet = false;
    int ret = -INFINITY;
    int numMoves = 0;
    int prevAlpha = alpha;
    bool isPv = alpha == beta - 1? false : true;


    // Transposition table for duplicate detection:
    // Get the hash key
    ZobristVal hashedBoard;
    uint64_t posKey = b.getPosKey();
    bool hashed = b.probeTT(posKey, hashedBoard, depth, ttRet, alpha, beta);

    if (ttRet) {
        return hashedBoard.score;
    }


    int eval = hashed? hashedBoard.score : b.evaluate();
    evalStack[height] = eval;
    bool improving = height >= 2? eval > evalStack[height - 2] : false;
    bool isCheck = b.InCheck();
    if (!isPv && !isCheck && depth == 1 && eval - 220 >= beta &&  eval < 5000) {
        return eval;
    }


    if (!isPv && canNullMove && !isCheck && eval >= beta && depth >= 2 && b.nullMoveable()) {
        int R = 3 + depth / 8;
        b.make_move(NULL_MOVE);
        int nullRet = -pvSearch(b, depth - R - 1, -beta, -beta + 1, false, height + 1);
        b.undo_move(NULL_MOVE);

        if (nullRet >= beta && nullRet < 9000) {
            return nullRet;
        }
    }



    b.generate(moveList, depth, hashedBoard.move);
    while (moveList.get_next_move(move)) {
        int score;
        b.make_move(move);
        if (b.InCheckOther()) {
            b.undo_move(move);
            continue;
        }

        int extension = 0;
        bool giveCheck = b.InCheck();
        if (giveCheck) {
            extension = 1;
        }

        int newDepth = depth + extension;
        if (numMoves == 0) {
            score = -pvSearch(b, newDepth - 1, -beta, -alpha, true, height + 1);
        }
        else if (depth >= 3 && (move & CAPTURE_FLAG) == 0 && (move & PROMOTION_FLAG) == 0 && !isCheck && !giveCheck && !extension) {
            int lmr = 1;

            // if (numMoves > 6) {
            //     lmr += 1;
            // }

            if (numMoves > 14) {
                lmr += 1;
            }

            lmr -= b.isKiller(depth, move);
            lmr += !improving;
            lmr -= 2 * isPv;


            lmr = std::min(depth - 1, std::max(lmr, 0));
            score = -pvSearch(b, newDepth - 1 - lmr, -alpha - 1, -alpha, true, height + 1);
            if (score > alpha) {
                score = -pvSearch(b, newDepth - 1, -alpha - 1, -alpha, true, height + 1);
                if (score > alpha && score < beta) {
                    score = -pvSearch(b, newDepth - 1, -beta, -alpha, true, height + 1);
                }
            }
        }
        else {
            score = -pvSearch(b, newDepth - 1, -alpha - 1, -alpha, true, height + 1);
            if (score > alpha && score < beta) {
                score = -pvSearch(b, newDepth - 1, -beta, -alpha, true, height + 1);
            }
        }
        b.undo_move(move);

        if (exit_thread_flag) {
            return 0;
        }

        if (score > ret) {
            ret = score;
            if (score > alpha) {
                if (score >= beta) {
                    if (((move & CAPTURE_FLAG) == 0 && (move & PROMOTION_FLAG) == 0)){
                        b.insertKiller(depth, move);
                        history[b.getSideToMove()][get_move_from(move)][get_move_to(move)] += depth * depth;
                    }
                    assert(move != 0);
                    b.saveTT(move, score, depth, 1, posKey);
                    return ret;
                }
                alpha = score;
            }
            else {
                history[b.getSideToMove()][get_move_from(move)][get_move_to(move)] -= depth;
            }
            bestMove = move;
        }

        if (std::abs(history[b.getSideToMove()][get_move_from(move)][get_move_to(move)]) > 600000) {
            for (int i = 0; i < 64; i++) {
                for (int j = 0; j < 64; j++) {
                    history[b.getSideToMove()][i][j] /= 4;
                }
            }
        }

        numMoves++;
    }

    if (numMoves == 0) {
        if (isCheck) {
            return -9999 - depth;
        }
        else {
            return 0;
        }
    }

    if (exit_thread_flag) {
        return 0;
    }

    assert(alpha >= prevAlpha);
    if (prevAlpha >= ret) {
        assert (bestMove != 0);
        b.saveTT(bestMove, ret, depth, 2, posKey);
    }
    else {
        assert (bestMove != 0);
        b.saveTT(bestMove, ret, depth, 0, posKey);
    }

    return ret;

}



BestMoveInfo pvSearchRoot(Bitboard &b, int depth, MoveList moveList, int alpha, int beta) {

    nodes++;
    MOVE move;
    MOVE bestMove = 0;
    // MoveList moveList;
    int numMoves = 0;
    int prevAlpha = alpha;
    int ret = -INFINITY;
    int height = 0;

    // Transposition table for duplicate detection:
    ZobristVal hashedBoard;
    uint64_t posKey = b.getPosKey();
    bool ttRet = false;
    bool hashed = b.probeTT(posKey, hashedBoard, depth, ttRet, alpha, beta);

    int eval = hashed? hashedBoard.score : b.evaluate();
    evalStack[height] = eval;
    // b.generate(moveList, depth, hashedBoard.move);
    while (moveList.get_next_move(move)) {

        int tempRet;
        b.make_move(move);
        if (b.InCheckOther()) {
            b.undo_move(move);
            continue;
        }

        if (totalTime > 3000) {
            std::cout << "info depth " << depth << " currmove " << TO_ALG[get_move_from(move)] + TO_ALG[get_move_to(move)] << " currmovenumber "<< numMoves + 1 << std::endl;
        }

        if (numMoves == 0) {
            tempRet = -pvSearch(b, depth - 1, -beta, -alpha, true, height + 1);
        }
        else {
            tempRet = -pvSearch(b, depth - 1, -alpha - 1, -alpha, true, height + 1);
            if (tempRet > alpha && tempRet < beta) {
                tempRet = -pvSearch(b, depth - 1, -beta, -alpha, true, height + 1);
            }
        }
        b.undo_move(move);

        if (exit_thread_flag) {
            break;
        }

        if (tempRet > ret) {
            ret = tempRet;
            bestMove = move;
            if (tempRet > alpha) {
                if (tempRet >= beta) {
                    break;
                }
                alpha = tempRet;
            }
        }

        numMoves++;
    }


    if (!exit_thread_flag) {
        assert(alpha >= prevAlpha);
        assert (bestMove != 0);
        b.saveTT(bestMove, ret, depth, 0, posKey);
    }

    return BestMoveInfo(bestMove, ret);

}



void search(Bitboard &b, int depth) {

    MOVE bestMove;
    MoveList moveList;
    std::string algMove;
    uint64_t nps;
    totalTime = 0;

    ZobristVal hashedBoard;
    uint64_t posKey = b.getPosKey();
    bool ttRet = false;

    int alpha;
    int beta;
    int tempAlpha;
    int tempBeta;

    b.generate(moveList, 0, NO_MOVE);
    for (int i = 1; i <= depth; i++) {

        int delta = ASPIRATION_DELTA;
        int aspNum = 0;
        nodes = 0;
        seldepth = i;

        // Use aspiration window with depth >= 4
        if (i >= 4) {
            alpha = hashedBoard.score - delta;
            beta = hashedBoard.score + delta;
        }
        else {
            alpha = -INFINITY;
            beta = INFINITY;
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        while (true) {

            pvSearchRoot(b, i, moveList, alpha, beta);
            bool hashed = b.probeTT(posKey, hashedBoard, i, ttRet, tempAlpha, tempBeta);

            if (i > 1 && !exit_thread_flag) {
                assert(hashed);
            }

            moveList.set_score_move(hashedBoard.move, 1400000 + (i * 100) + aspNum);

            if (exit_thread_flag) {
                break;
            }

            // Update the aspiration score
            // Fail high
            if (hashedBoard.score >= beta) {
                beta = hashedBoard.score + delta;
            }
            // Fail low
            else if (hashedBoard.score <= alpha) {
                beta = (alpha + beta) / 2;
                alpha = hashedBoard.score - delta;
            }
            // exact
            else {
                break;
            }

            aspNum++;
            delta += delta / 3 + 3;

        }
        auto t2 = std::chrono::high_resolution_clock::now();

        if (exit_thread_flag) {
            break;
        }

        auto diff = std::chrono::duration_cast<std::chrono::nanoseconds> (t2 - t1).count();
        totalTime += ((double) diff / 1000000.0);

        if ((double) diff == 0) {
            nps = 0;
        }
        else {
            nps = (uint64_t) ((double) nodes * 1000000000.0) / ((double) diff);
        }

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

        std::cout << "info depth " << i << " seldepth " << std::abs(seldepth) - 1 + i << " score cp " << hashedBoard.score <<
            " nodes " << nodes << " nps " << nps << " time " << (int) totalTime << " pv" << b.getPv() << std::endl;

    }

    std::cout << "bestmove " << algMove << std::endl;

}
