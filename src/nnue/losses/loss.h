
#ifndef LOSS_H
#define LOSS_H

#include <vector>

template <typename DERIVED, int size>
class Loss {
public:
    float execute(const std::vector<float> &inputs, const std::vector<float> &expected, float penalty=0) {
        return static_cast<DERIVED*>(this)->execute(inputs, expected, penalty);
    }

    const float* dLoss(const std::vector<float> &inputs, const std::vector<float> &expected) {
        return static_cast<DERIVED*>(this)->dLoss(inputs, expected);
    }

protected:
    float dloss[size] = {0.0f};
};


#endif