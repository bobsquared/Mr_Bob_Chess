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
    void setHash(int hashSize);

private:

    const std::string NAME = "Mr Bob";
    const std::string VERSION = "v1.1.0";
    const std::string AUTHOR = "Vincent Yu";

};
