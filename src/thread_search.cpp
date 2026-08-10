#include "thread_search.h"





namespace THREAD {

    std::vector<ThreadData> threadData;



    void InitKillers() {
        for (std::size_t i = 0; i < threadData.size(); i++) {
            for (int j = 0; j < MAX_PLY; j++) {
                for (int k = 0; k < 2; k++) {
                    threadData[i].historyData.killers[j][k] = 0;
                }
            }
        }
    }



    void InitCounterMoves() {
        for (std::size_t i = 0; i < threadData.size(); i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 64; k++) {
                    for (int l = 0; l < 64; l++) {
                        threadData[i].historyData.counterMove[j][k][l] = 0;
                    }
                }
            }
        }
    }



    void ResetHistories() {
        for (std::size_t i = 0; i < threadData.size(); i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 64; k++) {
                    for (int l = 0; l < 64; l++) {
                        threadData[i].historyData.quietHistory[j][k][l] = 0;
                        threadData[i].historyData.captureHistory[j][k][l] = 0;
                    }
                }
            }

            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 6; k++) {
                    for (int l = 0; l < 64; l++) {
                        for (int m = 0; m < 6; m++) {
                            for (int n = 0; n < 64; n++) {
                                threadData[i].historyData.counterHistory[j][k][l][m][n] = 0;
                            }
                        }
                    }
                }
            }
        }
    }



    void ClearData() {
        for (std::size_t i = 0; i < threadData.size(); i++) {
            threadData[i].id = i;
            threadData[i].ttWrites = 0;
            threadData[i].tbHits = 0;
            threadData[i].nodes = 0;
            threadData[i].seldepth = 0;
            threadData[i].bestMove = NO_MOVE;
            threadData[i].nullMoveTree = true;
        }
    }



    int getNThreads() {
        return static_cast<int>(threadData.size());
    }



    void setNThreads(int n) {
        threadData.resize(n);
        InitKillers();
        InitCounterMoves();
        ClearData();
        ResetHistories();
    }



    void UpdateHistories(const Board &b, const PrevMoveInfo &prev, HistoryData& hd, const MOVE *quietMoves, const MOVE *noisyMoves, int quietCount, int noisyCount, int depth, MOVE ttMove, MOVE bestMove) {
        bool toMove = b.state.toMove;
        int bestPiece = b.state.pieceAt[get_move_from(bestMove)] >> 1;
        int histScalar = 32;

        if (bestMove == ttMove) {
            histScalar += 8;
        }

        MOVE prevMove = prev.prevMove;
        int prevMoveTo = prev.prevMoveTo;
        int prevPiece = prev.prevPiece;

        int depthScore = histScalar * depth * depth;
        int depthScoreDiv = std::min(depth, 20);

        if (isQuietMove(bestMove)) {
            int hist = hd.quietHistory[toMove][get_move_from(bestMove)][get_move_to(bestMove)] * depthScoreDiv / 23;
            hd.quietHistory[toMove][get_move_from(bestMove)][get_move_to(bestMove)] += depthScore - hist;

            for (int i = 0; i < quietCount; i++) {
                int from = get_move_from(quietMoves[i]);
                int to = get_move_to(quietMoves[i]);

                int hist = hd.quietHistory[toMove][from][to] * depthScoreDiv / 23;
                hd.quietHistory[toMove][from][to] += -depthScore - hist;
            }

            if (prevMove != NULL_MOVE) {
                hist = hd.counterHistory[toMove][prevPiece][prevMoveTo][bestPiece][get_move_to(bestMove)] * depthScoreDiv / 23;
                hd.counterHistory[toMove][prevPiece][prevMoveTo][bestPiece][get_move_to(bestMove)] += depthScore - hist;
                hd.counterMove[b.state.toMove][prev.prevMoveFrom][prevMoveTo] = bestMove;

                for (int i = 0; i < quietCount; i++) {
                    int from = get_move_from(quietMoves[i]);
                    int to = get_move_to(quietMoves[i]);
                    int piece = b.state.pieceAt[from] >> 1;

                    int hist = hd.counterHistory[toMove][prevPiece][prevMoveTo][piece][to] * depthScoreDiv / 23;
                    hd.counterHistory[toMove][prevPiece][prevMoveTo][piece][to] += -depthScore - hist;
                }
            }

        }
        else {
            int hist = hd.captureHistory[toMove][get_move_from(bestMove)][get_move_to(bestMove)] * depthScoreDiv / 23;
            hd.captureHistory[toMove][get_move_from(bestMove)][get_move_to(bestMove)] += depthScore - hist;
        }

        for (int i = 0; i < noisyCount; i++) {
            int noisyFrom = get_move_from(noisyMoves[i]);
            int noisyTo = get_move_to(noisyMoves[i]);

            int hist = hd.captureHistory[b.state.toMove][noisyFrom][noisyTo] * depthScoreDiv / 23;
            hd.captureHistory[b.state.toMove][noisyFrom][noisyTo] += -depthScore - hist;
        }

    }

}
