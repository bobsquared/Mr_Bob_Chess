#include <iostream>
#include "bitboard.h"
#include "transpositionTable.h"
#include <string>
#include <chrono>
#include <unordered_map>
#include <regex>
#include "search.h"
#include <thread>
#include <vector>
#include <cmath>
#include "config.h"




// Debug printing function. Includes effective branching factor to see how effective the pruning is.
void printInfo(int depth, std::string move, float branchingFactor, long long int time) {

  // If time is equal to zero, change nps.
  if ((double) (time / 1000000000.0) == 0){
    std::cout << "Iterative Deepening: Depth " << depth << " Number of Nodes traversed: " << traversedNodes << " NPS: " << " inf " << " time: " <<  time / 1000000.0 << "ms" << std::endl;
    std::cout << "	Best move found: " << move <<  " Effective Branching Factor: " << branchingFactor << std::endl;
  }
  else {
    std::cout << "Iterative Deepening: Depth " << depth << " Number of Nodes traversed: " << traversedNodes << " KNPS: " << (uint64_t)((traversedNodes / 1000.0) / (double) (time / 1000000000.0)) << " time: " << time / 1000000.0 << "ms" << std::endl;
    std::cout << "	Best move found: " << move <<  " Effective Branching Factor: " << branchingFactor << std::endl;
  }

}





// Info printing for Universal Chess Interface.
// This function is used to communicate searching info to the chess GUI
void printInfoUCI(int depth, int seldepth, long long int time, int cp, int mateInPlies, std::string pv) {

  // Print the score in centipawns
  // positive cp indicates that the engine is at an advantage, does not matter if engine is playing white or black.
  // Negative cp indicates that the engine is at a disadvantage.
  std::cout << "info depth " << depth << " seldepth " << seldepth << " score cp " << cp;

  // If a mate threat is found, print mate in number of moves.
  // Positive indicates that engine will checkmate
  // Negative indicates that engine will be checkmated
  if (mateInPlies) {
    std::cout << " score mate " << std::ceil((double)(mateInPlies / 2.0));
  }

  // Search efficiency and principal variation
  std::cout << " nodes " << traversedNodes << " nps " << (uint64_t)(traversedNodes / (double)(time / 1000000000.0)) << " time " <<  (uint64_t)(time / 1000000.0) << " pv" << pv << std::endl;

}





// Universal Chess Interface position startpos moves command
// Moves the pieces on the board according to the command.
void startPosMoves(bool &color, Bitboard & bitboard, std::string moves) {

  // Reset the board to default position
  bitboard.resetBoard();
  color = true;

  // Make all the moves.
  while (moves.find(' ') != std::string::npos) {
    std::vector<Bitboard::Move> vMoves = bitboard.allValidMoves(!color);
    for (Bitboard::Move move : vMoves) {
      if (move.fromLoc == TO_NUM[moves.substr(0, 2)] && move.toLoc == TO_NUM[moves.substr(2, 2)]) {
        bitboard.movePiece(move);
        break;
      }
    }


    color = !color;
    moves = moves.erase(0, moves.find(' ') + 1);
  }

  // If only one more move in the list
  if (moves.find(' ') == std::string::npos && (moves.size() >= 4)) {
    std::vector<Bitboard::Move> vMoves = bitboard.allValidMoves(!color);
    for (Bitboard::Move move : vMoves) {
      if (move.fromLoc == TO_NUM[moves.substr(0, 2)] && move.toLoc == TO_NUM[moves.substr(2, 2)]) {
        bitboard.movePiece(move);
        break;
      }
    }
    color = !color;
  }

}






// Search for the best move in the position.
// print the best move and the info for the search.
// Position to search is the position of the bitboard reference passed in.
void search(Bitboard &bitboard, TranspositionTable &tt, int depth, bool color, unsigned int timeAllocated) {

  float branchingFactor = 0;
  int prevNodes = 0;

  // Keep track of previous variables to use if stop command is called.
  std::string prevBestMove = "";
  std::string bestMove = "";
  std::string pv = "";

  int cp = 0;
  int alpha;
  int beta;
  int delta = 35;
  int seldepth;
  uint8_t numBestMove = 0;

  ReturnInfo bMove;
  Bitboard::Move tempM;



  std::vector<Bitboard::Move> vMoves = bitboard.allValidMoves(!color);
  bitboard.scoreMoves(vMoves, tempM, 1, !color);
  std::stable_sort(vMoves.begin(), vMoves.end());
  // Searching with Iterative deepining
  // Increment the depth each search and keep the positions in memory.
  for (int i = 1; i < depth + 1; i++) {

    // If stop is called, cancel search and use previous iteration serach
    if (exit_thread_flag) {
      break;
    }
    seldepth = i;

    if (i >= 5) {
      delta = 35;
      alpha = cp - delta;
      beta = cp + delta;
    }
    else {
      alpha = -INFINITY_VAL;
      beta = INFINITY_VAL;
    }


    auto t1 = std::chrono::high_resolution_clock::now();
    while (true) {

      bMove = searchRoot(color, bitboard, tt, i, seldepth, vMoves, alpha, beta);
      for (std::vector<Bitboard::Move>::iterator it = vMoves.begin(); it != vMoves.end(); ++it) {
        if (*it == bMove.move) {
          it->score = 4000000 + i;
        }
      }

      bitboard.scoreMoves(vMoves, tempM, i, !color);
      std::stable_sort(vMoves.begin(), vMoves.end());


      // Record best move and the score
      bestMove = bMove.bestMove;
      cp = bMove.score;

      if (cp >= beta) {
        beta = cp + delta;
      }
      else if (cp <= alpha || (bMove.move.fromLoc == 0 && bMove.move.toLoc == 0)) {
        beta = (alpha + beta) / 2;
        alpha = cp - delta;
      }
      else {
        break;
      }

      delta = delta * 1.25 + 4;

    }
    auto t2 = std::chrono::high_resolution_clock::now();


    seldepth = std::abs(seldepth) - 1 + i;
    pv = tt.getPV(bitboard);


    // If checkmate is found, store in variable to print later.
    // if (bMove.mateIn) {
    //   mateInPlies = i - bMove.mateIn;
    // }
    // else {
    //   mateInPlies = 0;
    // }

    if (prevBestMove == bestMove) {
      numBestMove++;
    }
    else {
      numBestMove = 0;
    }


    // If stop is not called, then store current variables in previous variables (update)
    if (!exit_thread_flag || prevBestMove == "") {
      prevBestMove = bestMove;
    }


    // Debug print
    std::cout << (double)(pruning) / (double)(pruningTotal) << " " << pruning << " " << pruningTotal << std::endl;
    // std::cout << (double)(pruningTT) / (double)(pruningTotalTT) << " " << pruningTT << " " << pruningTotalTT << std::endl;
    pruning = 0;
    pruningTotal = 0;
    pruningTT = 0;
    pruningTotalTT = 0;
    //----/

    // Calculate effective branching factor.
    if (branchingFactor == 0.0) {
      branchingFactor = traversedNodes;
      prevNodes = traversedNodes;
    }
    else {
      branchingFactor = (float)(traversedNodes) / (float)(prevNodes);
      prevNodes = traversedNodes;
    }

    // Get the time the search took in nanoseconds
    auto diff = std::chrono::duration_cast<std::chrono::nanoseconds> (t2 - t1).count();
    auto diff2 = std::chrono::duration_cast<std::chrono::milliseconds> (t2 - t1).count();


    // If stop is not called, then print the info
    if (!exit_thread_flag) {
      if (!color) {
        printInfoUCI(i, seldepth, diff, cp, 0, pv);
      }
      else {
        printInfoUCI(i, seldepth, diff, cp, 0, pv);
      }
    }

    if (diff2 >= (timeAllocated / 2)) {
      exit_thread_flag = true;
    }

    // if (numBestMove == 8 && timeAllocated != 0xFFFFFFFFU) {
    //   exit_thread_flag = true;
    // }

    // Reset this variable and restart search
    traversedNodes = 0;


  }


  tt.updateHalfMove();
  // Print the best move and clear the transposition table
  std::cout << "bestmove " << prevBestMove << std::endl;

}










// Main function holds the loop of the program.
// Universal Chess Interface: Loops and awaits command until program is terminated.
int main() {

  // Set seed for random
  srand(time(NULL));

  // Initialize bitboard
  Bitboard x = Bitboard();
  TranspositionTable tt = TranspositionTable();
  bool color = true;

  // Regular expressions, r for finding a Number
  // r2 for go command with time.
  std::regex r("\\d+");
  std::regex r2("go\\swtime\\s(\\d+)\\sbtime\\s(\\d+)");

  // Initialize a second thread for Searching
  // This way the stop command can be called to stop the search from another thread.
  std::thread th1;

  // Initial print
  std::cout << engineName << " " << version << " UCI engine by Vincent Yu" << std::endl;



  // Forever loop of awesomeness
  while (1) {


    std::smatch m;
    std::string command = "";

    // Await command
    std::getline(std::cin, command);


    // Quit the program
    if (command == "quit") {
      break;
    }

    // Give reply for isready
    if (command == "isready") {
      std::cout << "readyok" << std::endl;
      continue;
    }

    // Print engine info, with manditory uciok at the end
    if (command == "uci") {
      std::cout << "id name " << engineName << " " << version << std::endl;
      std::cout << "id author Vincent Yu" << std::endl;
      std::cout << "uciok" << std::endl;
      continue;
    }

    // // Print engine info, with manditory uciok at the end
    // if (command == "test") {
    //   std::cout << searchRoot(color, x, 4).score << std::endl;
    //   continue;
    // }

    // Debugging color command to switch sides
    if (command.substr(0, 5) == "color") {
      std::regex_search(command, m, r);
      color = std::stoi(m[0]);
      continue;
    }

    // Stop searching
    if (command == "stop") {
      if (th1.joinable()) {
        exit_thread_flag = true;
        th1.join();
        exit_thread_flag = false;
      }
      continue;
    }

    // Search (virtually) forever.
    if (command == "go infinite") {
      th1 = std::thread(search, std::ref(x), std::ref(tt), 512, color, 0xFFFFFFFFU);
      continue;
    }

    // go command with time for each side
    if (std::regex_search(command, m, r2)) {

      unsigned int time = 0;
      if (color) {
        time = std::stoi(m[1]);
      }
      else {
        time = std::stoi(m[2]);
      }


      th1 = std::thread(search, std::ref(x), std::ref(tt), 99, color, time / 32);
      std::this_thread::sleep_for(std::chrono::milliseconds(time / 32));
      if (th1.joinable()) {
        exit_thread_flag = true;
        th1.join();
        exit_thread_flag = false;
      }

      continue;
    }

    // Go search for depth 10
    if (command.substr(0, 2) == "go") {
      std::cout << command << std::endl;
      std::regex_search(command, m, r);
      th1 = std::thread(search, std::ref(x), std::ref(tt), 10, color, 0xFFFFFFFFU);
      th1.join();
      continue;
    }

    // Reset postion to starting position
    if (command == "position startpos") {
      color = true;
      x.resetBoard();
      continue;
    }

    // Make all moves in the list
    if (command.substr(0, 24) == "position startpos moves ") {
      color = true;
      startPosMoves(color, x, command.substr(24, command.size() - 24));
      continue;
    }

    // Print current fen position
    if (command == "print fen") {
      std::cout << x.posToFEN() << std::endl;
      continue;
    }

    // Print current board position
    if (command == "print") {
      x.printPretty();
      continue;
    }

    // Evaluate current board position
    if (command == "evaluate") {
      std::cout << x.evaluate() << std::endl;
      continue;
    }

    // Undo current position
    if (command == "undo") {
      x.undoMove();
      continue;
    }


  }


  // If search is still ongoing, terminate thread
  if (th1.joinable()) {
    exit_thread_flag = true;
    th1.join();
    exit_thread_flag = false;
  }


  // Terminate program
  return 0;
}
