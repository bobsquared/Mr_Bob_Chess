#include "syzygy_probe.h"
#include "../defs.h"
#include "movegen.h"


namespace SYZYGY_PROBE {

    void setPath(std::string path) {
        std::cout << path << std::endl;
        if (tb_init(path.c_str())) {
            std::cerr << "Syzygy tablebase initialized successfully." << std::endl;
                    std::cout << TB_LARGEST << std::endl;
        }
        else {
            std::cerr << "Error initializing Syzygy tablebase. Check the path and try again." << std::endl;
        }
    }

    int probe_wdl(BoardState &bs, int ply) {
        if (count_population(bs.occupied) > (signed) TB_LARGEST)
            return -1;

        if (bs.castleRights != 0 || bs.enpassantSq != 0)
            return -1;

        uint64_t pawns = bs.pieces[0] | bs.pieces[1];
        uint64_t knights = bs.pieces[2] | bs.pieces[3];
        uint64_t bishops = bs.pieces[4] | bs.pieces[5];
        uint64_t rooks = bs.pieces[6] | bs.pieces[7];
        uint64_t queens = bs.pieces[8] | bs.pieces[9];
        uint64_t kings = bs.pieces[10] | bs.pieces[11];
        

        unsigned result = tb_probe_wdl(bs.color[0], bs.color[1], kings, queens, rooks, bishops, knights, pawns,
            0, 0, 0, !bs.toMove);

        if (result == TB_LOSS) {
            return -MATE_VALUE_MAX + ply;
        }
        if (result == TB_WIN) {
            return MATE_VALUE_MAX - ply;
        }
        if (result == TB_BLESSED_LOSS) {
            return -16;
        }
        if (result == TB_CURSED_WIN) {
            return 16;
        }
        if (result == TB_DRAW) {
            return 0;
        }
        return -1;
    }

    MOVE probe_dtz(BoardState &bs, int &score, int &depth) {
        if (count_population(bs.occupied) > (signed) TB_LARGEST)
            return NO_MOVE;

        if (bs.castleRights != 0 || bs.enpassantSq != 0)
            return NO_MOVE;

        uint64_t pawns = bs.pieces[0] | bs.pieces[1];
        uint64_t knights = bs.pieces[2] | bs.pieces[3];
        uint64_t bishops = bs.pieces[4] | bs.pieces[5];
        uint64_t rooks = bs.pieces[6] | bs.pieces[7];
        uint64_t queens = bs.pieces[8] | bs.pieces[9];
        uint64_t kings = bs.pieces[10] | bs.pieces[11];
        

        unsigned result = tb_probe_root(bs.color[0], bs.color[1], kings, queens, rooks, bishops, knights, pawns,
            0, static_cast<unsigned>(bs.castleRights), static_cast<unsigned>(bs.enpassantSq), !bs.toMove, NULL);

        if (result == TB_RESULT_FAILED) {
            return NO_MOVE;
        }

        unsigned wdl = TB_GET_WDL(result);
        unsigned dtz = TB_GET_DTZ(result);
        depth = static_cast<int>(dtz);

        if (wdl == TB_LOSS) {
            score = -MATE_VALUE + dtz;
        }
        if (wdl == TB_WIN) {
            score = MATE_VALUE - dtz;
        }
        if (wdl == TB_BLESSED_LOSS) {
            score = -16;
        }
        if (wdl == TB_CURSED_WIN) {
            score = 16;
        }
        if (wdl == TB_DRAW) {
            score = 0;
        }

        MoveList moveList;
        MOVE_GEN::generate_all_moves(moveList, bs);
        int dtzFrom = TB_GET_FROM(result);
        int dtzTo = TB_GET_TO(result);
        int tbPromo = static_cast<int>(TB_GET_PROMOTES(result));

        for (int i = 0; i < moveList.count; i++) {
            MOVE move = moveList.moves[i];
            int from = get_move_from(move);
            int to = get_move_to(move);

            if (from != dtzFrom || to != dtzTo) {
                continue;
            }

            if (tbPromo != TB_PROMOTES_NONE) {
                int moveFlags = move & MOVE_FLAGS;
                int movePromo = moveFlags - 7;

                bool promoMatch = false;
                switch (tbPromo) {
                    case TB_PROMOTES_QUEEN:  promoMatch = (movePromo == 4);  break;
                    case TB_PROMOTES_ROOK:   promoMatch = (movePromo == 3);   break;
                    case TB_PROMOTES_BISHOP: promoMatch = (movePromo == 2); break;
                    case TB_PROMOTES_KNIGHT: promoMatch = (movePromo == 1); break;
                }
                if (!promoMatch) continue;
            }

            return move;
        }

        return NO_MOVE;
    }

}