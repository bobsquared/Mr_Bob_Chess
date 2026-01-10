#include "concatenate.h"

namespace Layer {

    namespace Concatenate {
        void execute(const int16_t* const inputVectorOne, const int16_t* const inputVectorTwo, int16_t* outputVector, int sizeOne, int sizeTwo) {
            for (int i = 0; i < sizeOne; i++) {
                outputVector[i] = inputVectorOne[i];
            }

            for (int i = 0; i < sizeTwo; i++) {
                outputVector[sizeOne + i] = inputVectorTwo[i];
            }
        }

        void backward(const float* const err, float* outputVectorOne, float* outputVectorTwo, int sizeOne, int sizeTwo) {
            for (int i = 0; i < sizeOne; i++) {
                outputVectorOne[i] = err[i];
            }

            for (int i = 0; i < sizeTwo; i++) {
                outputVectorTwo[i] = err[sizeOne + i];
            }
        }
    }
    
}
