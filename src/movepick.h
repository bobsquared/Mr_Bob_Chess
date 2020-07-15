#pragma once
#include "defs.h"
#include "movegen.h"


class MovePick {

public:
    MovePick();
    void scoreMoves(MoveList &moveList, MoveInfoStack &moveHistory, int *pieceAt, int depth, bool toMove, MOVE pvMove);

private:

    void InitMvvLva();
    int mvvlva[6][6];

};
