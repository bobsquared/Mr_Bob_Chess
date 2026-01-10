#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <cstddef>
#include <chrono>
#include <string>

class ProgressBar {
public:
    ProgressBar(std::size_t totalSize);
    void ShowProgress(const std::string prefix, const std::string postfix);
    void ShowProgress(int epoch, float training_loss, float validation_loss, float lr);

private:
    std::size_t totalSize;
    std::size_t iterator;
    std::size_t printStep;

    int barWidth = 70;
    std::chrono::time_point<std::chrono::system_clock> start;
    std::chrono::time_point<std::chrono::system_clock> end;

    void PrintProgress();
    bool EndProgress();
};


#endif