
#ifndef MSE_H
#define MSE_H

#include "loss.h"
#include <vector>
#include <cmath>

template <int size>
class MSE : public Loss<MSE<size>, size> {
public:
    float execute(const std::vector<float> &inputs, const std::vector<float> &expected, float penalty=0);
    const float* dLoss(const std::vector<float> &inputs, const std::vector<float> &expected);
};

template <int size>
float MSE<size>::execute(const std::vector<float> &inputs, const std::vector<float> &expected, float penalty) {
    float loss = 0.0f;
    for (int i = 0; i < size; i++) {
        loss += std::pow(inputs[i] - expected[i], 2);
    }
    return loss / static_cast<float>(size) + penalty;
}

template <int size>
const float* MSE<size>::dLoss(const std::vector<float> &inputs, const std::vector<float> &expected) {
    for (int i = 0; i < size; i++) {
        this->dloss[i] = 2 * (inputs[i] - expected[i]) / static_cast<float>(size);
    }
    return this->dloss;
}


#endif