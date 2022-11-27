#include "timeman.h"




TimeManager::TimeManager() {
    col = 0;
    timeLeft = 0;
    maxTimeLeft = 0;
    wtime = 0;
    btime = 0;
    winc = 0;
    binc = 0;
    movesToGo = 0;
    initTime = std::chrono::high_resolution_clock::now();
}



TimeManager::TimeManager(bool col, int wtime, int btime, int winc, int binc, int movesToGo) {

    this->col = col;
    this->wtime = std::max(10, wtime - 50);
    this->btime = std::max(10, btime - 50);
    this->winc = winc;
    this->binc = binc;
    this->movesToGo = movesToGo;

}



void TimeManager::setTimer(int numMoves) {
    bool hasInc = col? (binc > 0) : (winc > 0);

    bool bmode = col? (btime < 15000 && binc == 0 && movesToGo == 0) : (wtime < 15000 && winc == 0 && movesToGo == 0);
    int timeDivisor = std::max(12, 32 + bmode * std::min(25, 50000 / (col? btime : wtime)) - (numMoves / 7) - (movesToGo > 0? 4 : -1) - (hasInc * 4));
    if (col) {
        this->timeLeft = btime / (timeDivisor - (binc >= 10? std::min(5, (binc / 200)) : -1));
    }
    else {
        this->timeLeft = wtime / (timeDivisor - (winc >= 10? std::min(5, (winc / 200)) : -1));
    }

    int totalTime = col? btime : wtime;
    if (movesToGo <= 18 && movesToGo > 0) {
        timeLeft = (movesToGo == 1? 0 : (timeLeft / (19 - movesToGo))) + (totalTime / (0.5 + movesToGo));
    }

    this->maxTimeLeft = timeLeft * ((bmode || (movesToGo <= 2 && movesToGo > 0))? 1 : 4);
    initTime = std::chrono::high_resolution_clock::now();
}



int TimeManager::getTimePassed() {
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    int diff = std::chrono::duration_cast<std::chrono::milliseconds> (t2 - initTime).count();
    return diff;
}



bool TimeManager::outOfTimeRootThreshold() {
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    int diff = std::chrono::duration_cast<std::chrono::milliseconds> (t2 - initTime).count();
    return ((double) diff) / timeLeft > 0.75;
}



bool TimeManager::outOfTime() {
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    int diff = std::chrono::duration_cast<std::chrono::milliseconds> (t2 - initTime).count();
    return diff > maxTimeLeft;
}



bool TimeManager::outOfTimeRoot() {
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    int diff = std::chrono::duration_cast<std::chrono::milliseconds> (t2 - initTime).count();
    return diff > timeLeft;
}
