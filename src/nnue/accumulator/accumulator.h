#ifndef ACCUMULATOR_H
#define ACCUMULATOR_H


#include "../nncpp.h"
#include "../layers/layer.h"
#include <vector>
#include <algorithm>
#include <cmath>

constexpr inline int calculateBlackIndex(int loc) {
    return 64 * (((loc / 64) & 1) * -2 + 1) + ((loc - (loc / 64)) ^ 56);
}

template <int ins, int outs>
class Accumulator {

public:
    Accumulator();
    std::vector<int>& getAddFeaturesWhite();
    std::vector<int>& getAddFeaturesBlack();
    int16_t* getOutputVectorWhite();
    int16_t* getOutputVectorBlack();
    void Add(int loc);
    void Remove(int loc);
    void Refresh();
    void Accumulate(const Layer::Linear<int16_t, ins, outs>& layerWhite, const Layer::Linear<int16_t, ins, outs>& layerBlack, bool training = true);

private:
    void UpdateNNUEFeatures(const Layer::Linear<int16_t, ins, outs>& layer, std::vector<int>& addFeatures, std::vector<int>& removeFeatures, int16_t* outputVector, bool toRefresh);
    static constexpr int max_registers = (outs + 15) / 16;
    __m256i R[max_registers];
    alignas(32) int16_t outputVectorWhites[outs] = {0};
    alignas(32) int16_t outputVectorBlacks[outs] = {0};
    std::vector<int> addFeaturesWhite;
    std::vector<int> removeFeaturesWhite;
    std::vector<int> addFeaturesBlack;
    std::vector<int> removeFeaturesBlack;

    bool toRefresh;
    float scale;
};

template <int ins, int outs>
Accumulator<ins, outs>::Accumulator() : toRefresh(true) {
    addFeaturesWhite.reserve(128);
    removeFeaturesWhite.reserve(128);
    addFeaturesBlack.reserve(128);
    removeFeaturesBlack.reserve(128);
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
inline std::vector<int>& Accumulator<ins, outs>::getAddFeaturesWhite() {
    return addFeaturesWhite;
}

template <int ins, int outs>
inline std::vector<int>& Accumulator<ins, outs>::getAddFeaturesBlack() {
    return addFeaturesBlack;
}

template <int ins, int outs>
void Accumulator<ins, outs>::Refresh() {
    addFeaturesWhite.clear();
    addFeaturesBlack.clear();
    removeFeaturesWhite.clear();
    removeFeaturesBlack.clear();
    toRefresh = true;
}

template <int ins, int outs>
inline void Accumulator<ins, outs>::Add(int loc) {
    int whiteIndex = loc;
    int blackIndex = calculateBlackIndex(loc);

    addFeaturesWhite.push_back(whiteIndex);
    addFeaturesBlack.push_back(blackIndex);
}

template <int ins, int outs>
inline void Accumulator<ins, outs>::Remove(const int loc) {
    int whiteIndex = loc;
    int blackIndex = calculateBlackIndex(loc);

    removeFeaturesWhite.push_back(whiteIndex);
    removeFeaturesBlack.push_back(blackIndex);
}

#ifndef USE_AVX2
template <int ins, int outs>
void Accumulator<ins, outs>::UpdateNNUEFeatures(const Layer::Linear<int16_t, ins, outs>& layer, std::vector<int>& addFeatures, std::vector<int>& removeFeatures, int16_t* outputVector, bool toRefresh) {

    if (toRefresh) {
        for (int i = 0; i < max_registers; i++) {
            R[i] = _mm256_load_si256((__m256i*)&(layer.biases[i * 16]));
        }
    } 
    else {
        for (int i = 0; i < max_registers; i++) {
            R[i] = _mm256_load_si256((__m256i*)&(outputVector[i * 16]));
        }
    }
    
    // Process added features
    for (int j : addFeatures) {
        for (int i = 0; i < max_registers; i++) {
            R[i] = _mm256_add_epi16(R[i], _mm256_load_si256((__m256i*)&(layer.weights[j * outs + i * 16])));
        }
    }

    // Process removed features
    for (int j : removeFeatures) {
        for (int i = 0; i < max_registers; i++) {
            R[i] = _mm256_sub_epi16(R[i], _mm256_load_si256((__m256i*)&(layer.weights[j * outs + i * 16])));
        }
    }

    for (int i = 0; i < max_registers; i++) {
        _mm256_store_si256((__m256i*)&(outputVector[i * 16]), R[i]);
    }

}
#else
template <int ins, int outs>
void Accumulator<ins, outs>::UpdateNNUEFeatures(const Layer::Linear<int16_t, ins, outs>& layer, std::vector<int>& addFeatures, std::vector<int>& removeFeatures, int16_t* outputVector, bool toRefresh) {
    if (toRefresh) {
        for (int i = 0; i < outs; i++) {
            outputVector[i] = layer.biases[i];
        }
    }

    for (int i : addFeatures) {
        for (int j = 0; j < outs; j++) {
            outputVector[j] += layer.weights[i * outs + j];
        }
    }

    for (int i : removeFeatures) {
        for (int j = 0; j < outs; j++) {
            outputVector[j] -= layer.weights[i * outs + j];
        }
    }
}
#endif

template <int ins, int outs>
void Accumulator<ins, outs>::Accumulate(const Layer::Linear<int16_t, ins, outs>& layerWhite, const Layer::Linear<int16_t, ins, outs>& layerBlack, bool training) {
    // Add Features
    UpdateNNUEFeatures(layerWhite, addFeaturesWhite, removeFeaturesWhite, outputVectorWhites, toRefresh || training); 
    UpdateNNUEFeatures(layerBlack, addFeaturesBlack, removeFeaturesBlack, outputVectorBlacks, toRefresh || training); 

    if (!training) {
        addFeaturesWhite.clear();
        addFeaturesBlack.clear();
        removeFeaturesWhite.clear();
        removeFeaturesBlack.clear();
    }

    if (toRefresh || training) {
        toRefresh = false;
    }
    
}






#endif