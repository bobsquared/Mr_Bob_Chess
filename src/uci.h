#include <string>
#include "bitboard.h"
#include "transpositionTable.h"
#include "movepick.h"
#include "search.h"

class UCI {

public:
    UCI(Search &s, KPNNUE *model);
    void startMessage();
    void uciCommand();
    void readyCommand();
    void startPosMoves(Bitboard & b, std::string moves);
    void newGameCommand();
    void setNNUEFile(std::string filename);
    void setNNUEFileDefault();
    void setHash(int hashSize);
    void setMultiPV(int pvs);

    void setRFP(int value);
    void setRazor(int value);
    void setProbcut(int value);
    void setFutility(int value);
    void setHistoryLMR(int value);
    void setHistoryLMRNoisy(int value);

private:

    const std::string NAME = "Mr Bob";
    const std::string VERSION = "v1.2.0";
    const std::string AUTHOR = "Vincent Yu";
    const std::string DEFAULT_NETWORK = "nets/bob_brain-041523e378.nnue";
    Search &search;
    KPNNUE *model;
};
