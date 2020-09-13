#include <iostream>
#include <cstdint>
#include <regex>
#include <thread>
#include <climits>
#include "magic_bitboards.h"
#include "movegen.h"
#include "defs.h"
#include "perft.h"
#include "bitboard.h"
#include "search.h"
#include "uci.h"




MovePick *movePick = new MovePick();
MoveGen *moveGen = new MoveGen();
Magics *magics = new Magics();
extern int pieceValues[6];


int main() {

    InitColumnsMask();
    InitRowsMask();
    InitHistory();
    InitLateMoveArray();
    InitCounterMoves();
    Bitboard pos = Bitboard();
    UCI uci = UCI();

    std::regex setHash("setoption\\sname\\shash\\svalue\\s(\\d+)");
    std::regex wtime(".*wtime\\s(\\d+).*");
    std::regex btime(".*btime\\s(\\d+).*");

    std::regex winc(".*winc\\s(\\d+).*");
    std::regex binc(".*binc\\s(\\d+).*");

    std::regex movesToGo(".*movestogo\\s(\\d+).*");
    std::thread thr;

    // Initial print
    uci.startMessage();

    // Forever loop of awesomeness
    while (1) {

        std::smatch m;
        std::string command = "";


        // Await command
        std::getline(std::cin, command);

        std::string lowerCommand = command;
        std::transform(lowerCommand.begin(), lowerCommand.end(), lowerCommand.begin(), ::tolower);
        exit_thread_flag = false;


        // Quit the program
        if (command == "quit") {
            break;
        }

        // Give reply for isready
        if (command == "isready") {
            uci.readyCommand();
            continue;
        }

        if (command == "ucinewgame") {
            uci.newGameCommand(pos);
            continue;
        }

        // Print engine info, with manditory uciok at the end
        if (command == "uci") {
            uci.uciCommand();
            continue;
        }

        // set hash
        if (std::regex_search(lowerCommand, m, setHash)) {
            pos.replaceHash(std::stoi(m[1]));
            continue;
        }

        // Stop searching
        if (command == "stop") {
            exit_thread_flag = true;
            continue;
        }

        // Search (virtually) forever.
        if (command == "go infinite") {
            exit_thread_flag = false;
            thr = std::thread(search, std::ref(pos), 255, INT_MAX, INT_MAX, 0, 0, 0);
            thr.detach();
            continue;
        }

        // go command with time for each side
        if (command.substr(0, 3) == "go ") {
            exit_thread_flag = false;
            int whitetime = 0;
            int blacktime = 0;
            int whiteInc = 0;
            int blackInc = 0;
            int movestogo = 0;

            if (std::regex_search(command, m, btime)) {
                blacktime = std::stoi(m[1]);
            }

            if (std::regex_search(command, m, wtime)) {
                whitetime = std::stoi(m[1]);
            }

            if (std::regex_search(command, m, binc)) {
                blackInc = std::stoi(m[1]);
            }

            if (std::regex_search(command, m, winc)) {
                whiteInc = std::stoi(m[1]);
            }

            if (std::regex_search(command, m, movesToGo)) {
                movestogo = std::stoi(m[1]);
            }



            thr = std::thread(search, std::ref(pos), 99, whitetime, blacktime, whiteInc, blackInc, movestogo);
            thr.detach();
            continue;
        }

        // Perft.
        if (command == "perft") {
            PerftTest(pos);
            continue;
        }

        // Reset postion to starting position
        if (command == "position startpos") {
            pos.reset();
            continue;
        }

        // Make all moves in the list
        if (command.substr(0, 24) == "position startpos moves ") {
            pos.reset();
            uci.startPosMoves(pos, command.substr(24, command.size() - 24));
            continue;
        }

        // Set position to FEN position
        if (command.substr(0, 13) == "position fen ") {
            size_t indexMoves = command.find("moves ");
            pos.setPosFen(command.substr(13, indexMoves));

            if (indexMoves != std::string::npos) {
                uci.startPosMoves(pos, command.substr(indexMoves, command.size() - indexMoves));
            }
            continue;
        }

        // Print the current board position
        if (command == "print") {
            pos.printPretty();
            continue;
        }

        // Debug the evaluation
        if (command == "evaluate debug") {
            pos.evaluate_debug();
            continue;
        }

        // Test static SEE function
        if (command == "see") {
            SeeTest(pos, "1k1r4/1pp4p/p7/4p3/8/P5P1/1PP4P/2K1R3 w - -", 4, 36, EGVAL(pieceValues[0]));
            SeeTest(pos, "1k1r3q/1ppn3p/p4b2/4p3/8/P2N2P1/1PP1R1BP/2K1Q3 w - -", 19, 36, EGVAL(-pieceValues[1] + pieceValues[0]));
            SeeTest(pos, "4R3/2r3p1/5bk1/1p1r3p/p2PR1P1/P1BK1P2/1P6/8 b - -", 39, 30, 0);
            SeeTest(pos, "4R3/2r3p1/5bk1/1p1r1p1p/p2PR1P1/P1BK1P2/1P6/8 b - -", 39, 30, 0);
            SeeTest(pos, "4r1k1/5pp1/nbp4p/1p2p2q/1P2P1b1/1BP2N1P/1B2QPPK/3R4 b - -", 30, 21, EGVAL(pieceValues[1] - pieceValues[2]));
            SeeTest(pos, "2r1r1k1/pp1bppbp/3p1np1/q3P3/2P2P2/1P2B3/P1N1B1PP/2RQ1RK1 b - -", 43, 36, EGVAL(pieceValues[0]));
            SeeTest(pos, "7r/5qpk/p1Qp1b1p/3r3n/BB3p2/5p2/P1P2P2/4RK1R w - -", 4, 60, 0);
            SeeTest(pos, "6rr/6pk/p1Qp1b1p/2n5/1B3p2/5p2/P1P2P2/4RK1R w - -", 4, 60, EGVAL(-pieceValues[3]));
            SeeTest(pos, "7r/5qpk/2Qp1b1p/1N1r3n/BB3p2/5p2/P1P2P2/4RK1R w - -", 4, 60, EGVAL(-pieceValues[3]));
            // SeeTest(pos, "6RR/4bP2/8/8/5r2/3K4/5p2/4k3 w - -", 53, 61, EGVAL(pieceValues[2] - pieceValues[0]), QUEEN_PROMOTION_FLAG);
            // SeeTest(pos, "6RR/4bP2/8/8/5r2/3K4/5p2/4k3 w - -", 53, 61, EGVAL(pieceValues[1] - pieceValues[0]), KNIGHT_PROMOTION_FLAG);
            // SeeTest(pos, "7R/5P2/8/8/8/3K2r1/5p2/4k3 w - -", 53, 61, EGVAL(pieceValues[4] - pieceValues[0]), QUEEN_PROMOTION_FLAG);
            // SeeTest(pos, "7R/5P2/8/8/8/3K2r1/5p2/4k3 w - -", 53, 61, EGVAL(pieceValues[2] - pieceValues[0]), BISHOP_PROMOTION_FLAG);
            // SeeTest(pos, "7R/4bP2/8/8/1q6/3K4/5p2/4k3 w - -", 53, 61, EGVAL(-pieceValues[0]), ROOK_PROMOTION_FLAG);
            SeeTest(pos, "8/4kp2/2npp3/1Nn5/1p2PQP1/7q/1PP1B3/4KR1r b - -", 7, 5, 0);
            SeeTest(pos, "8/4kp2/2npp3/1Nn5/1p2P1P1/7q/1PP1B3/4KR1r b - -", 7, 5, 0);
            SeeTest(pos, "2r2r1k/6bp/p7/2q2p1Q/3PpP2/1B6/P5PP/2RR3K b - -", 34, 2, EGVAL(2 * pieceValues[3] - pieceValues[4]));
            SeeTest(pos, "r2qk1nr/pp2ppbp/2b3p1/2p1p3/8/2N2N2/PPPP1PPP/R1BQR1K1 w kq -", 21, 36, EGVAL(pieceValues[0]));
            SeeTest(pos, "6r1/4kq2/b2p1p2/p1pPb3/p1P2B1Q/2P4P/2B1R1P1/6K1 w - -", 29, 36, 0);
            SeeTest(pos, "3q2nk/pb1r1p2/np6/3P2Pp/2p1P3/2R4B/PQ3P1P/3R2K1 w - h6", 38, 47, 0);
            SeeTest(pos, "3q2nk/pb1r1p2/np6/3P2Pp/2p1P3/2R1B2B/PQ3P1P/3R2K1 w - h6", 38, 47, EGVAL(pieceValues[0]));
            // SeeTest(pos, "2r4r/1P4pk/p2p1b1p/7n/BB3p2/2R2p2/P1P2P2/4RK2 w - -", 18, 58, EGVAL(pieceValues[3]));
            // SeeTest(pos, "2r5/1P4pk/p2p1b1p/5b1n/BB3p2/2R2p2/P1P2P2/4RK2 w - -", 18, 58, EGVAL(pieceValues[3]));
            SeeTest(pos, "2r4k/2r4p/p7/2b2p1b/4pP2/1BR5/P1R3PP/2Q4K w - -", 18, 34, EGVAL(pieceValues[2]));
            SeeTest(pos, "8/pp6/2pkp3/4bp2/2R3b1/2P5/PP4B1/1K6 w - -", 14, 42, EGVAL(pieceValues[0] - pieceValues[2]));
            SeeTest(pos, "4q3/1p1pr1k1/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - -", 44, 28, EGVAL(pieceValues[0] - pieceValues[3]));
            SeeTest(pos, "4q3/1p1pr1kb/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - -", 55, 28, EGVAL(pieceValues[0]));
            // SeeTest(pos, "r2q1b1r/ppppP1Pp/3k4/8/8/8/PPPP1P1P/RNBQKBNR w KQ - 0 1", 54, 61, EGVAL(pieceValues[2] + pieceValues[3] - 2 * pieceValues[0]), QUEEN_PROMOTION_CAPTURE_FLAG);
            // SeeTest(pos, "r2q1b1r/ppppPRPp/3k4/8/8/8/PPP4P/RNBQKBN1 w Q - 0 1", 53, 61, EGVAL(pieceValues[2] - 2 * pieceValues[0]));
        }


    }

    delete movePick;
    delete moveGen;
    delete magics;
    return 0;
}
