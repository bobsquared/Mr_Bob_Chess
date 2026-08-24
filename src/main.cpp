#include <iostream>
#include <cstdint>
#include <thread>
#include <climits>
#include <cstring>
#include "board/magic_bitboards.h"
#include "eval.h"
#include "board/movegen.h"
#include "defs.h"
#include "perft.h"
#include "board/bitboard.h"
#include "board/fen.h"
#include "search.h"
#include "uci_commands.h"
#include "uci.h"
#include "nnue/nncpp.h"
#include "transpositionTable.h"
#include "board/syzygy_probe.h"




void Bench(Board &b, Search &s) {
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
        BITBOARD::setPosFen(b.state, b.acc, Benchmarks[i]);
        Search::SearchInfo ret = s.beginSearch(b, 13, INT_MAX, INT_MAX, 0, 0, 0, true);
        nodes = s.getTotalNodesSearched();
        printf("Bench [# %2d] %12d nodes %8d nps %8d CP\n", i + 1, (int) nodes, (int) (1000.0f * nodes / (s.getTotalTime() + 1)), ret.score);
        nn += nodes;
        time += s.getTotalTime();
        BITBOARD::reset(b);
    }
    s.willPrintInfo(true);

    printf("OVERALL: %53d nodes %8d nps\n", nn, (int) (1000.0f * nn / (time + 1)));
}



int main(int argc, char* argv[]) {

    MAGIC_BITBOARDS::InitMagicBitboards();
    EVAL::InitEval("nets/b_bobbrain_31_0.002846.nnue");
    THREAD::setNThreads(1);
    TT::InitTT(HASH_SIZE);                       /**< The transposition table for storing previously searched positions*/

    Search s = Search();

    Board pos;
    BITBOARD::InitBoard();
    BITBOARD::reset(pos);

    std::string command = "";

    UCIOptions options = UCIOptions();
    UCIParameters params = UCIParameters();
    UCICommandInvoker uci;

    CommandInterface* c = nullptr;
    
    options.addOption([&](int n){TT::setSize(n);}, "Hash", 256, 1, 524288);
    options.addOption([&](int n){THREAD::setNThreads(n);}, "Threads", 1, 1, 2048);
    options.addOption([&](int n){s.setMultiPVSearch(n);}, "MultiPV", 1, 1, 256);

    options.addOption([&](int n){s.setRFPsearch(n);}, "rfp", 64, 1, 500);
    options.addOption([&](int n){s.setRazorsearch(n);}, "razor", 361, 1, 1000);
    options.addOption([&](int n){s.setProbcutsearch(n);}, "probcut", 335, 1, 1000);
    options.addOption([&](int n){s.setFutilitysearch(n);}, "futility", 214, 1, 1000);
    options.addOption([&](int n){s.setHistoryLMRsearch(n);}, "histlmr", 2084, 1, 10000);
    options.addOption([&](int n){s.setHistoryLMRNoisysearch(n);}, "histlmrnoisy", 2534, 1, 10000);
    options.addOption([&](std::string path){SYZYGY_PROBE::setPath(path);}, "SyzygyPath", "tb/syzygy");
    options.addOption([&](std::string path){EVAL::InitEval(path);}, "EvalFile", "nets/b_bobbrain_31_0.002846.nnue");

    params.addParameter("movetime");
    params.addParameter("wtime");
    params.addParameter("btime");
    params.addParameter("winc");
    params.addParameter("binc");
    params.addParameter("movestogo");
    params.addParameter("depth");
    params.addParameter("perft");

    c = new StartCommand();
    uci.setCommand(c);
    uci.executeUCI();
    uci.cleanCommand();

    c = new NewGameCommand(s);
    uci.setCommand(c);
    uci.executeUCI();
    uci.cleanCommand();

    if (argc > 1 && strcmp(argv[1], "bench") == 0) {
        Bench(pos, s);
        return 0;
    }

    // Forever loop of awesomeness
    while (std::getline(std::cin, command)) {
        s.setSearch();
        std::string commandType = command.substr(0, command.find(' '));

        // Quit the program
        if (command == "quit") {
            s.stopSearch();
            break;
        }

        else if (commandType == "uci") {
            c = new UCICommand(&options);
            uci.setCommand(c);
        }

        else if (commandType == "isready") {
            c = new ReadyCommand();
            uci.setCommand(c);
        }

        else if (command == "ucinewgame") {
            c = new NewGameCommand(s);
            uci.setCommand(c);
        }

        else if (command == "stop") {
            s.stopSearch();
            continue;
        }

        else if (commandType == "go") {
            c = new GoCommand(&params, command, s, pos);
            uci.setCommand(c);
        }

        else if (commandType == "setoption") {
            options.setOption(command);
        }

        else if (commandType == "position") {
            c = new PositionCommand(command, pos);
            uci.setCommand(c);
        }
        
        uci.executeUCI();
        uci.cleanCommand();

    }

    s.cleanUpSearch();

    return 0;
}
