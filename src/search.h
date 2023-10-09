#ifndef SEARCH_H
#define SEARCH_H

#include <atomic>
#include <chrono>
#include "defs.h"
#include "eval.h"
#include "bitboard.h"
#include "movepick.h"
#include "movegen.h"
#include "timeman.h"
#include "transpositionTable.h"
#include "thread_search.h"




#define ASPIRATION_DELTA 11


class Search {
public:

    struct SearchInfo {
        uint64_t nodes;
        uint64_t nps;
        int depth;
        int seldepth;
        int score;
        int eval;
        int totalTime;
        int hashUsage;
        std::string s1;
        std::string s2;
        std::string promo;
        std::string pv;
    };

    Search(Eval *eval, TranspositionTable *tt, ThreadSearch *thread);

    void InitLateMoveArray();
    void willPrintInfo(bool b);
    
    // void setNNUE(const std::string file);
    void setNumThreads(const int numThreads);
    void setRFPsearch(const int value);
    void setRazorsearch(const int value);
    void setProbcutsearch(const int value);
    void setFutilitysearch(const int value);
    void setHistoryLMRsearch(const int value);
    void setHistoryLMRNoisysearch(const int value);
    void setTTSize(int hashSize);
    void setMultiPVSearch(int pvs);

    ThreadSearch* getThreads();
    uint64_t getTotalNodesSearched();
    int getNThreads();
    int getTotalTime();

    void clearTT();
    void cleanUpSearch();
    void stopSearch();
    void setSearch();

    SearchInfo beginSearch(Bitboard &b, int depth, int wtime, int btime, int winc, int binc, int movesToGo, bool analysis);

private:

    struct BestMoveInfo {
        MOVE move;
        int eval;

        BestMoveInfo(uint16_t move, int eval) : move(move), eval(eval) {}
    };
    struct PrintInfo {
        uint64_t nodes;
        uint64_t nps;
        int depth;
        int seldepth;
        int score;
        int eval;
        int totalTime;
        int hashUsage;
        std::string pv;
    };

    void clearThreadData();
    void printSearchInfo(SearchInfo &printInfo, std::string &pstring, MOVE move, int bound, int pv);
    void moveToStruct(SearchInfo &si, MOVE move);

    void setSearchInfo(SearchInfo &printInfo, Bitboard &board, int depth, int eval);
    uint64_t getHashFullTotal();
    int getSeldepth();
    
    static bool isMateScore(int eval);
    static int getSearchedScore(int eval);
    
    SearchInfo search(int id, ThreadSearch *th, int depth, bool analysis, Bitboard b);
    BestMoveInfo pvSearchRoot(Bitboard &b, ThreadSearch *th, int depth, MoveList moveList, int alpha, int beta, bool analysis, int id);
    int pvSearch(Bitboard &b, ThreadSearch *th, int depth, int alpha, int beta, bool canNullMove, int ply);
    int qsearch(Bitboard &b, ThreadSearch *th, int depth, int alpha, int beta, int ply);

    std::atomic<bool> exit_thread_flag;
    int totalTime;
    bool canPrintInfo;
    int nThreads;
    int multiPv;                   /**< Number of pvs to search, default is 1.*/
    bool stopable;              /**< Used to ensure that we search atleast a depth one 1.*/
    int lmrReduction[64][64];           /**< A 2D array of reduction values for LMR given depth and move count.*/
    TimeManager tm;                     /**< The time manager determines when to stop the search given time parameters.*/

    Eval *eval;                        /**< The evaluator to score the positions*/
    TranspositionTable *tt;
    MovePick *movePick;
    ThreadSearch *thread;
    MoveGen *moveGen;

    const int seePruningMargin[2][6] = {{0, -100, -175, -275, -400, -600}, {0, -125, -200, -275, -350, -425}}; /**< Margins for SEE pruning in pvSearch*/
    const int lateMoveMargin[2][9] = {{0, 3, 5, 7, 10, 14, 20, 26, 32}, {0, 6, 9, 13, 19, 27, 35, 43, 50}};    /**< Margins for late move pruning in pvSearch*/

    int rfpVal = 136;
    int razorVal = 392;
    int probcutVal = 251;
    int futilityVal = 328;
    int historyLmrVal = 2084;
    int historyLmrNoisyVal = 2534;
    
};


#endif