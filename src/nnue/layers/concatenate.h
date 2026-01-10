#ifndef CONCATENATE_H
#define CONCATENATE_H

#include <cstdint>

namespace Layer {

    namespace Concatenate {
        void execute(const int16_t* const inputVectorOne, const int16_t* const inputVectorTwo, int16_t* outputVector, int sizeOne, int sizeTwo);
        void backward(const float* const err, float* outputVectorOne, float* outputVectorTwo, int sizeOne, int sizeTwo);
    }
    
}

#endif