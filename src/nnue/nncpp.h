
#ifndef NNCPP_H
#define NNCPP_H

#include <vector>
#include <string>
#include "layers/relu.h"
#include "layers/layer.h"
#include "layers/sigmoid.h"
#include "layers/concatenate.h"

// Example of a simple neural network class in C++ using the defined layers and optimizers
class NNCPP {

private:
    
    static constexpr float scaleA = 255.0f;
    static constexpr float scaleS = 64.0f;
    static constexpr float scaleO = 625.0f;

    // Define the Layers, boolean argument indicates that the layer has gradients and is trainable
    Layer::Linear<int16_t, 768, 768> l1[14];

    CRelu<1536> relu = CRelu<1536>(false);
    Layer::Linear<int8_t, 1536, 1> l2[8];

    uint8_t NO_QUEEN[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 3, 3};
    uint8_t HAS_QUEEN[16] = {4, 4, 5, 5, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7};

    uint8_t halfKing[64] = {0, 0, 1, 2, 2, 1, 0, 0,
                            3, 3, 4, 4, 4, 4, 3, 3,
                            5, 5, 5, 5, 5, 5, 5, 5, 
                            6, 6, 6, 6, 6, 6, 6, 6,  
                            6, 6, 6, 6, 6, 6, 6, 6,  
                            6, 6, 6, 6, 6, 6, 6, 6,
                            6, 6, 6, 6, 6, 6, 6, 6,
                            6, 6, 6, 6, 6, 6, 6, 6};
public:

    NNCPP() {
        for (int i = 0; i < 8; i++) {
            l2[i] = Layer::Linear<int8_t, 1536, 1>(false, scaleA, scaleS, Layer::INIT_XAVIAR, scaleO);
        }

        for (int i = 0; i < 14; i++) {
            l1[i] = Layer::Linear<int16_t, 768, 768>(true, scaleA, scaleS, Layer::INIT_HE, 1.0f);
        }
    }

    inline uint8_t getHalfKing(int index) const {
        return halfKing[index];
    }

    inline const auto& getL1White(int index) const {
        return l1[halfKing[index] * 2];
    }

    inline const auto& getL1Black(int index) const {
        return l1[halfKing[index] * 2 + 1];
    }

    int32_t evaluate(const int16_t *whitepov, const int16_t *blackpov, bool blackToMove, int outBucketIndex) {
        alignas(64) int16_t catBuffer[1536];
        alignas(64) uint8_t l1ReluBuffer[1536];
        alignas(64) int32_t l2Buffer[4] = {0};

        if (blackToMove) {
            Layer::Concatenate::execute(blackpov, whitepov, catBuffer, 768, 768);  
        }
        else {
            Layer::Concatenate::execute(whitepov, blackpov, catBuffer, 768, 768);
        }

        bool hasQueen = (outBucketIndex & 16) != 0;
        if (hasQueen) {
            outBucketIndex = HAS_QUEEN[outBucketIndex & 15];
        }
        else {
            outBucketIndex = NO_QUEEN[outBucketIndex & 15];
        }

        relu.execute(catBuffer, l1ReluBuffer, scaleA);
        l2[outBucketIndex].execute(l1ReluBuffer, l2Buffer);

        return l2Buffer[0];
    }

    void LoadNetwork(std::string fileName) {
        std::ifstream in(fileName, std::ios::binary);

        if (!in) {
            std::cerr << "Error: Failed to open file " + fileName + " for reading." << std::endl;
            return;
        }

        for (int i = 0; i < 14; i++) {
            l1[i].Load(in);
        }
            
        for (int i = 0; i < 8; i++) {
            l2[i].Load(in);
        }
            
        in.close();
    }

};


#endif