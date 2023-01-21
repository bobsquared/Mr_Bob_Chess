#include <string>
#include "bitboard.h"
#include "transpositionTable.h"
#include "movepick.h"
#include "search.h"

class UCI {

public:
    UCI();
    void startMessage();
    void uciCommand();
    void readyCommand();
    void startPosMoves(Bitboard & b, std::string moves);
    void newGameCommand();
    void setNNUEFile(std::string filename);
    void setNNUEFileDefault();
    void setHash(int hashSize);
    void setMultiPV(int pvs);

private:

    const std::string NAME = "Mr Bob";
    const std::string VERSION = "v1.1.0";
    const std::string AUTHOR = "Vincent Yu";
    const std::string DEFAULT_NETWORK = "nets/bob_brain-012022e141.nnue";

};
