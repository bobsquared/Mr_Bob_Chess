
#ifndef NNCPP_H
#define NNCPP_H

#include <vector>
#include <string>
#include "layers/relu.h"
#include "layers/layer.h"
#include "layers/sigmoid.h"
#include "optimizers/adamw.h"
#include "layers/concatenate.h"

// Example of a simple neural network class in C++ using the defined layers and optimizers
class NNCPP {

private:
    
    static constexpr float scaleA = 255.0f;
    static constexpr float scaleS = 64.0f;
    static constexpr float scaleO = 625.0f;

    // Define the Layers, boolean argument indicates that the layer has gradients and is trainable
    Layer::Linear<int16_t, 768, 768> *l1_pov = new Layer::Linear<int16_t, 768, 768>(false, scaleA, scaleS, Layer::INIT_HE, 1.0f);
    Layer::Linear<int16_t, 768, 768> *l1_vop = new Layer::Linear<int16_t, 768, 768>(false, scaleA, scaleS, Layer::INIT_HE, 1.0f);

    CRelu<1536> relu = CRelu<1536>(false);
    Layer::Linear<int8_t, 1536, 1> l2[9];
    AdamW* adam = nullptr;

    bool training;

    static constexpr int nPawnMapping[17] = {
        0, 0, 0, 0,  // 0-3
        1, 1, 1,  // 4-6
        2, 2,     // 7-8
        3,        // 9
        4,        // 10
        5,        // 11
        6,        // 12
        7, 7,     // 13-14
        8, 8      // 15-16
    };

public:

    NNCPP() {
        training = true;
        for (int i = 0; i < 9; i++) {
            l2[i] = Layer::Linear<int8_t, 1536, 1>(false, scaleA, scaleS, Layer::INIT_XAVIAR, scaleO);
        }
    }

    void eval() {
        training = false;
    }

    void train() {
        training = true;
    }

    const bool isTraining() {
        return training;
    }

    inline const auto& getL1White() const {
        return *l1_pov;
    }

    inline const auto& getL1Black() const {
        return *l1_vop;
    }

    int32_t evaluate(const int16_t *whitepov, const int16_t *blackpov, bool blackToMove, int nPawns) {
        alignas(64) int16_t catBuffer[1536] = {0};
        alignas(64) uint8_t l1ReluBuffer[1536] = {0};
        alignas(64) int32_t l2Buffer[4] = {0};

        if (blackToMove) {
            Layer::Concatenate::execute(blackpov, whitepov, catBuffer, 768, 768);  
        }
        else {
            Layer::Concatenate::execute(whitepov, blackpov, catBuffer, 768, 768);
        }

        relu.execute(catBuffer, l1ReluBuffer, scaleA);
        l2[nPawnMapping[nPawns]].execute(l1ReluBuffer, l2Buffer);

        return l2Buffer[0];
    }

    void LoadNetwork(std::string fileName) {
        std::ifstream in(fileName, std::ios::binary);

        if (!in) {
            std::cerr << "Error: Failed to open file " + fileName + " for reading." << std::endl;
            return;
        }

        l1_pov->Load(in);
        l1_vop->Load(in);
        for (int i = 0; i < 9; i++)
            l2[i].Load(in);
        in.close();
    }

};


#endif