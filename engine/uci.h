#include <string>
#include "bitboard.h"

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
    const std::string VERSION = "v0.6.1";
    const std::string AUTHOR = "Vincent Yu";

};
