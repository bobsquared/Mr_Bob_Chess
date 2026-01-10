
#ifndef ACTIVATION_H
#define ACTIVATION_H

#include <cstdint>

template <typename T_OUT, int outs>
struct alignas(64) ActivationTrainer {
    alignas(64) float grad[outs] = {0.0f};
    alignas(64) T_OUT backwardVector[outs] = {0};
};

#endif