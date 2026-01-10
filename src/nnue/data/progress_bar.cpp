#include "progress_bar.h"

#include <iostream>
#include <iomanip>
#include <cmath>


ProgressBar::ProgressBar(std::size_t totalSize) : totalSize(totalSize), iterator(0), printStep(totalSize / 100) {
    start = std::chrono::system_clock::now();
}


void ProgressBar::ShowProgress(const std::string prefix, const std::string postfix) {

    if (EndProgress()) {
        return;
    }
    else if (iterator % printStep == 0) {
        std::cout << prefix;
        PrintProgress();
        std::cout << postfix << "\r";

        std::cout.flush();
    } 

    iterator++;
}


void ProgressBar::ShowProgress(int epoch, float training_loss, float validation_loss, float lr) {

    if (EndProgress()) {
        return;
    }
    else if (iterator % printStep == 0) {
        std::cout << "Epoch " << std::setw(3) << epoch << "  :  ";
        PrintProgress();
        std::cout << std::setprecision(8) << std::fixed << " - [Train loss: " << std::setw(9) << training_loss << "] [" 
        << "Validation loss: " << validation_loss <<  "] [" << "LR: " << lr << "]\r";

        std::cout.flush();
    } 

    iterator++;
}


void ProgressBar::PrintProgress() {
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    float fraction = static_cast<float>(iterator) / totalSize;
    int barLength = static_cast<int>(std::ceil(fraction * barWidth));

    std::cout << "[";
    for (int i = 0; i < barLength; ++i) {
        std::cout << "=";
    }

    for (int i = barLength; i < barWidth; ++i) {
        std::cout << " ";
    }

    std::cout << "] " << std::setw(3) << static_cast<int>(std::ceil(fraction * 100.0f)) << "%  - ";
    std::cout << " (" << std::setprecision(2) << std::fixed << elapsed_seconds.count() << "s)";
}


bool ProgressBar::EndProgress() {
    if (iterator >= totalSize) {
        iterator = 0;
        std::cout << std::endl;
        start = std::chrono::system_clock::now();
        return true;
    }

    return false;
}