#ifndef ACCUMULATOR_H
#define ACCUMULATOR_H


#include "../nncpp.h"
#include "../layers/layer.h"
#include "bitaccu.h"
#include <vector>
#include <algorithm>
#include <cmath>

template <int ins, int outs>
class Accumulator {

public:
    Accumulator();
    int16_t* getOutputVectorWhite();
    int16_t* getOutputVectorBlack();
    void Add(int piece, int loc);
    void Remove(int piece, int loc);
    void Refresh();
    void Accumulate(const Layer::Linear<int16_t, ins, outs>& layerWhite, const Layer::Linear<int16_t, ins, outs>& layerBlack, bool training = true);

private:
    void UpdateNNUEFeatures(const Layer::Linear<int16_t, ins, outs>& layer, uint64_t* addFeatures, uint64_t* removeFeatures, int16_t* outputVector, bool toRefresh);
    static constexpr int max_registers = (outs + 15) / 16;
    alignas(32) int16_t outputVectorWhites[outs] = {0};
    alignas(32) int16_t outputVectorBlacks[outs] = {0};
    BITACCU::BitAccu bitAcc;

    bool toRefresh;
    float scale;
};

template <int ins, int outs>
Accumulator<ins, outs>::Accumulator() : toRefresh(true) {
    BITACCU::Refresh(bitAcc);
} 

template <int ins, int outs>
inline int16_t* Accumulator<ins, outs>::getOutputVectorWhite() {
    return outputVectorWhites;
}

template <int ins, int outs>
inline int16_t* Accumulator<ins, outs>::getOutputVectorBlack() {
    return outputVectorBlacks;
}

template <int ins, int outs>
void Accumulator<ins, outs>::Refresh() {
    BITACCU::Refresh(bitAcc);
    toRefresh = true;
}

template <int ins, int outs>
inline void Accumulator<ins, outs>::Add(int piece, int loc) {
    BITACCU::Add(piece, loc, bitAcc);
}

template <int ins, int outs>
inline void Accumulator<ins, outs>::Remove(int piece, int loc) {
    BITACCU::Remove(piece, loc, bitAcc);
}

template <int ins, int outs>
void Accumulator<ins, outs>::UpdateNNUEFeatures(const Layer::Linear<int16_t, ins, outs>& layer, uint64_t* addFeatures, uint64_t* removeFeatures, int16_t* outputVector, bool toRefresh) {
    if (toRefresh) {
        for (int i = 0; i < outs; i++) {
            outputVector[i] = layer.biases[i];
        }
    }

    for (int i = 0; i < 12; i++) {
        while (addFeatures[i]) {
            int bitIndex = __builtin_ctzll(addFeatures[i]);
            int loc = (i << 6) + bitIndex;
            for (int j = 0; j < outs; j++) {
                outputVector[j] += layer.weights[loc * outs + j];
            }
            addFeatures[i] &= addFeatures[i] - 1;
        }
    }

    for (int i = 0; i < 12; i++) {
        while (removeFeatures[i]) {
            int bitIndex = __builtin_ctzll(removeFeatures[i]);
            int loc = (i << 6) + bitIndex;
            for (int j = 0; j < outs; j++) {
                outputVector[j] -= layer.weights[loc * outs + j];
            }
            removeFeatures[i] &= removeFeatures[i] - 1;
        }
    }

}

template <int ins, int outs>
void Accumulator<ins, outs>::Accumulate(const Layer::Linear<int16_t, ins, outs>& layerWhite, const Layer::Linear<int16_t, ins, outs>& layerBlack, bool training) {
    // Add Features
    UpdateNNUEFeatures(layerWhite, bitAcc.addsWhites, bitAcc.removesWhites, outputVectorWhites, toRefresh || training); 
    UpdateNNUEFeatures(layerBlack, bitAcc.addsBlacks, bitAcc.removesBlacks, outputVectorBlacks, toRefresh || training); 

    if (!training) {
        BITACCU::Refresh(bitAcc);
    }

    if (toRefresh || training) {
        toRefresh = false;
    }
    
}






#endif