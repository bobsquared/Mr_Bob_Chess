
#ifndef SIGMOID_H
#define SIGMOID_H


#include "activation.h"
#include <cmath>

template <int size>
struct Sigmoid {
    ActivationTrainer<float, size>* trainer = nullptr;

    Sigmoid(bool training) {
        if (training) {
            trainer = new (static_cast<std::align_val_t>(32)) ActivationTrainer<float, size>;
        }
    }

    ~Sigmoid() {
        if (trainer) {
            delete trainer;
        }
    };

    void execute(const int32_t* inputVector, float* outputVector, float factor);
    const float* backward(const float* const err);
    
};

template <int size>
void Sigmoid<size>::execute(const int32_t* inputVector, float* outputVector, float factor) {
    for (int i = 0; i < size; i++) {
        outputVector[i] = 1.0 / (1.0 + std::exp(-inputVector[i] / factor));
    }

    if (this->trainer) {
        for (int i = 0; i < size; i++) {
            trainer->backwardVector[i] = outputVector[i];
        }
    }
}

template <int size>
const float* Sigmoid<size>::backward(const float* const err) {
    for (int i = 0; i < size; i++) {
        trainer->grad[i] = err[i] * trainer->backwardVector[i] * (1.0 - trainer->backwardVector[i]);
    }
    return trainer->grad;
}

#endif