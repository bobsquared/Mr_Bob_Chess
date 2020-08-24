#include "timeman.h"




TimeManager::TimeManager() {
    col = 0;
    timeLeft = 0;
    wtime = 0;
    btime = 0;
    winc = 0;
    binc = 0;
    movesToGo = 0;
    initTime = std::chrono::high_resolution_clock::now();
}



TimeManager::TimeManager(bool col, int wtime, int btime, int winc, int binc, int movesToGo, int numMoves) {

    this->col = col;
    this->wtime = wtime;
    this->btime = btime;
    this->winc = winc;
    this->binc = binc;
    this->movesToGo = movesToGo;
    initTime = std::chrono::high_resolution_clock::now();

    int timeDivisor = std::max(12, 24 - (numMoves / 7) - (movesToGo > 0? 4 : -1));
    if (col) {
        this->timeLeft = btime / (timeDivisor - (binc >= 100? std::min(5, (binc / 200)) : -1)) - (btime < 15000 && binc < 100 && movesToGo == 0? btime / 32: 0);
    }
    else {
        this->timeLeft = wtime / (timeDivisor - (winc >= 100? std::min(5, (winc / 200)) : -1)) - (wtime < 15000 && winc < 100 && movesToGo == 0? wtime / 32: 0);
    }

}



int TimeManager::getTimePassed() {
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    int diff = std::chrono::duration_cast<std::chrono::milliseconds> (t2 - initTime).count();
    return diff;
}



bool TimeManager::outOfTime() {
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    int diff = std::chrono::duration_cast<std::chrono::milliseconds> (t2 - initTime).count();
    return diff > timeLeft;
}
