
#ifndef OPTIMZER_H
#define OPTIMZER_H

#include "../nncpp.h"
#include <vector>

class Optimizer {
public:
    Optimizer(float lr) : lr(lr) {};
    virtual ~Optimizer() = default;

    void setLearningRate(float lr) {
        this->lr = lr;
    }

protected:
    float lr;
};


#endif