#include <string>
#include "bitboard.h"
#include "movepick.h"

class UCI {

public:
    UCI();
    void startMessage();
    void uciCommand();
    void readyCommand();
    void startPosMoves(Bitboard & b, std::string moves);
    void newGameCommand(Bitboard &b);

private:

    const std::string NAME = "Mr Bob";
    const std::string VERSION = "v0.8.0";
    const std::string AUTHOR = "Vincent Yu";

};
