#include <iostream>
#include <cstdint>
#include <regex>
#include <thread>
#include <climits>
#include "magic_bitboards.h"
#include "eval.h"
#include "movegen.h"
#include "defs.h"
#include "perft.h"
#include "bitboard.h"
#include "search.h"
#include "uci.h"





Zobrist *zobrist = new Zobrist();
Magics *magics;


void Bench(Bitboard &b, Search &s) {
    // positions from Ethereal
    static const char* Benchmarks[] = {
    #include "bench.csv"
        ""};

    int nodes = 0;
    int nn = 0;
    int time  = 0;

    s.willPrintInfo(false);
    for (int i = 0; strcmp(Benchmarks[i], ""); i++) {
        s.setSearch();
        b.setPosFen(Benchmarks[i]);
        int eva = s.beginSearch(b, 13, INT_MAX, INT_MAX, 0, 0, 0, true);
        nodes = s.getTotalNodesSearched();
        printf("Bench [# %2d] %12d nodes %8d nps %8d CP\n", i + 1, (int) nodes, (int) (1000.0f * nodes / (s.getTotalTime() + 1)), eva);
        nn += nodes;
        time += s.getTotalTime();
        b.reset();
    }
    s.willPrintInfo(true);

    printf("OVERALL: %53d nodes %8d nps\n", nn, (int) (1000.0f * nn / (time + 1)));
}



int main(int argc, char* argv[]) {

    InitColumnsMask();
    InitRowsMask();

    magics = new Magics();
    KPNNUE *model = new KPNNUE();
    Eval *eval = new Eval(model);                        /**< The evaluator to score the positions*/
    TranspositionTable *tt = new TranspositionTable();
    ThreadSearch *thread = new ThreadSearch[1];

    Search s = Search(eval, tt, thread);

    Bitboard pos = Bitboard();
    UCI uci = UCI(s, model);
    uci.newGameCommand();

    std::regex setNNUEFile("setoption\\sname\\snnue\\svalue\\s(.+)");
    std::regex setHash("setoption\\sname\\shash\\svalue\\s(\\d+)");
    std::regex setThreads("setoption\\sname\\sthreads\\svalue\\s(\\d+)");
    std::regex setMultiPv("setoption\\sname\\smultipv\\svalue\\s(\\d+)");

    std::regex setRFP("setoption\\sname\\srfp\\svalue\\s(\\d+)");
    std::regex setRazor("setoption\\sname\\srazor\\svalue\\s(\\d+)");
    std::regex setProbcut("setoption\\sname\\sprobcut\\svalue\\s(\\d+)");
    std::regex setFutility("setoption\\sname\\sfutility\\svalue\\s(\\d+)");
    std::regex setHistLMR("setoption\\sname\\shistlmr\\svalue\\s(\\d+)");
    std::regex setHistLMRNoisy("setoption\\sname\\shistlmrnoisy\\svalue\\s(\\d+)");

    std::regex wtime(".*wtime\\s(\\d+).*");
    std::regex btime(".*btime\\s(\\d+).*");

    std::regex winc(".*winc\\s(\\d+).*");
    std::regex binc(".*binc\\s(\\d+).*");

    std::regex movesToGo(".*movestogo\\s(\\d+).*");
    std::regex searchDepth(".*depth\\s(\\d+).*");
    std::regex number(".*(\\d+).*");
    std::thread thr;
    std::string command = "";

    // Initial print
    uci.startMessage();
    uci.setNNUEFileDefault();

    if (argc > 1 && strcmp(argv[1], "bench") == 0) {
        Bench(pos, s);
        return 0;
    }

    // Forever loop of awesomeness
    while (std::getline(std::cin, command)) {

        std::smatch m;
        std::string lowerCommand = command;
        std::transform(lowerCommand.begin(), lowerCommand.end(), lowerCommand.begin(), ::tolower);
        s.setSearch();


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
            uci.newGameCommand();
            continue;
        }

        // Print engine info, with manditory uciok at the end
        if (command == "uci") {
            uci.uciCommand();
            continue;
        }

        // set nnue
        if (std::regex_search(lowerCommand, m, setNNUEFile)) {
            s.stopSearch();
            uci.setNNUEFile(m[1]);
            continue;
        }

        // set hash
        if (std::regex_search(lowerCommand, m, setHash)) {
            s.stopSearch();
            uci.setHash(std::stoi(m[1]));
            continue;
        }

        // set threads
        if (std::regex_search(lowerCommand, m, setThreads)) {
            s.stopSearch();
            s.setNumThreads(std::stoi(m[1]));
            continue;
        }

        // set MultiPvs
        if (std::regex_search(lowerCommand, m, setMultiPv)) {
            s.stopSearch();
            uci.setMultiPV(std::stoi(m[1]));
            continue;
        }

        // set rfp
        if (std::regex_search(lowerCommand, m, setRFP)) {
            s.stopSearch();
            uci.setRFP(std::stoi(m[1]));
            continue;
        }

        // set razor
        if (std::regex_search(lowerCommand, m, setRazor)) {
            s.stopSearch();
            uci.setRazor(std::stoi(m[1]));
            continue;
        }

        // set probcut
        if (std::regex_search(lowerCommand, m, setProbcut)) {
            s.stopSearch();
            uci.setProbcut(std::stoi(m[1]));
            continue;
        }

        // set futility
        if (std::regex_search(lowerCommand, m, setFutility)) {
            s.stopSearch();
            uci.setFutility(std::stoi(m[1]));
            continue;
        }

        // set futility
        if (std::regex_search(lowerCommand, m, setHistLMR)) {
            s.stopSearch();
            uci.setHistoryLMR(std::stoi(m[1]));
            continue;
        }

        // set futility
        if (std::regex_search(lowerCommand, m, setHistLMRNoisy)) {
            s.stopSearch();
            uci.setHistoryLMRNoisy(std::stoi(m[1]));
            continue;
        }

        // Stop searching
        if (command == "stop") {
            s.stopSearch();
            continue;
        }

        // Search (virtually) forever.
        if (command == "go infinite") {
            thr = std::thread(&Search::beginSearch, std::ref(s), std::ref(pos), 99, INT_MAX, INT_MAX, 0, 0, 0, true);
            thr.detach();
            continue;
        }

        if (command.substr(0, 9) == "go perft ") {
            int depth = 1;
            if (std::regex_search(command, m, number)) {
                depth = std::stoi(m[1]);
            }
            Perft(pos, depth);
            continue;
        }

        // go command with time for each side
        if (command.substr(0, 3) == "go ") {
            int whitetime = 0;
            int blacktime = 0;
            int whiteInc = 0;
            int blackInc = 0;
            int movestogo = 0;
            int depth = 99;

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

            if (std::regex_search(command, m, searchDepth)) {
                if (whitetime <= 0) {
                    whitetime = INFINITY_VAL;
                }
                if (blacktime <= 0) {
                    blacktime = INFINITY_VAL;
                }
                depth = std::stoi(m[1]);
            }

            thr = std::thread(&Search::beginSearch, &s, std::ref(pos), depth, whitetime, blacktime, whiteInc, blackInc, movestogo, false);
            thr.detach();
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
            pos.reset();
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

    }

    s.cleanUpSearch();

    delete magics;
    delete zobrist;

    return 0;
}
