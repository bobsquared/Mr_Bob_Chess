#pragma once
#include "../syzygy/fathom/src/tbprobe.h"
#include "bitboard.h"


namespace SYZYGY_PROBE {

    void setPath(std::string path);

    int probe_wdl(BoardState &bs, int ply);

    MOVE probe_dtz(BoardState &bs, int &score, int &depth);

}