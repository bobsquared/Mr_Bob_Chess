#include "timeman.h"
#include <cstdint>




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
    int time = col? btime : wtime;
    int inc = col? binc : winc;
    bool hasInc = inc > 0;

    bool bmode = time < 15000 && inc == 0 && movesToGo == 0;
    int timeDivisor = std::max(12000, 29434 + bmode * std::min(25, 50000 / time) * 1000 - numMoves * 133 - (movesToGo > 0? 4000 : -1000) - hasInc * 4000);
    int incReduction = inc >= 10? std::min(5000, inc * 5) : -1000;
    this->timeLeft = (int64_t) time * 1000 / (timeDivisor - incReduction) + inc * 88 / 1000;

    int totalTime = time;
    if (movesToGo <= 18 && movesToGo > 0) {
        timeLeft = (movesToGo == 1? 0 : (timeLeft / (19 - movesToGo))) + (totalTime / (0.5 + movesToGo));
    }

    this->maxTimeLeft = (bmode || (movesToGo <= 2 && movesToGo > 0))? timeLeft : (int64_t) timeLeft * 4328 / 1000;

    timeLeft = std::min(timeLeft, time);
    maxTimeLeft = std::min(maxTimeLeft, time);
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
    return (int64_t) diff * 1000 > (int64_t) timeLeft * 744;
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
