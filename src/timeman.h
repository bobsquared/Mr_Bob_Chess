#include <chrono>
#include <algorithm>



class TimeManager {


public:

    TimeManager();
    TimeManager(bool col, int wtime, int btime, int winc, int binc, int movesToGo);

    int getTimePassed();
    bool outOfTime();
    void setTimer(int numMoves);


private:

    int timeLeft;
    int wtime;
    int btime;
    int winc;
    int binc;
    int movesToGo;
    bool col;

    std::chrono::high_resolution_clock::time_point initTime;

};
