
#ifndef RELU_H
#define RELU_H


#include "activation.h"
#include <algorithm>
#include <immintrin.h>

template <int size>
struct CRelu {
    ActivationTrainer<int32_t, size>* trainer = nullptr;
    float scale;

    CRelu(bool training) {
        if (training) {
            trainer = new (static_cast<std::align_val_t>(64)) ActivationTrainer<int32_t, size>;
        }
    }

    ~CRelu() {
        if (trainer) {
            delete trainer;
        }
    };

    void execute(const int32_t* const inputs, uint8_t* outputs, float scale);
    void execute(const int16_t* const inputs, uint8_t* outputs, float scale);
    const float* backward(const float* const err);
};


template <int size>
void CRelu<size>::execute(const int32_t* const inputVector, uint8_t* outputVector, float scale) {

    if (trainer) {
        for (int i = 0; i < size; i++) {
            trainer->backwardVector[i] = inputVector[i];
        }
    }

    for (int i = 0; i < size; i++) {
        outputVector[i] = std::clamp(inputVector[i], 0, static_cast<int>(scale));
    }

}


template <int size>
void CRelu<size>::execute(const int16_t* const inputVector, uint8_t* outputVector, float scale) {

    if (trainer) {
        for (int i = 0; i < size; i++) {
            trainer->backwardVector[i] = inputVector[i];
        }
    }

    for (int i = 0; i < size; i++) {
        outputVector[i] = std::clamp(inputVector[i], static_cast<int16_t>(0), static_cast<int16_t>(scale));
    }
    
}


template <int size>
const float* CRelu<size>::backward(const float* const err) {
    for (int i = 0; i < size; i++) {
        trainer->grad[i] = (trainer->backwardVector[i] > 0 && trainer->backwardVector[i] <= 255)? err[i] : 0.0;
    }

    return trainer->grad;
}

#endif