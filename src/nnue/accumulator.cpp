#include "accumulator.h"
#include <iostream>

Accumulator::Accumulator() {
    whiteFeatures = new float[MAX_L1 + 8] ();
    blackFeatures = new float[MAX_L1 + 8] ();
    resetFlag = false;
    addWhiteFeatures = new std::vector<int>();
    removeWhiteFeatures = new std::vector<int>();
    addBlackFeatures = new std::vector<int>();
    removeBlackFeatures = new std::vector<int>();
}


Accumulator::~Accumulator() {
    delete [] whiteFeatures;
    delete addWhiteFeatures;
    delete removeWhiteFeatures;

    delete [] blackFeatures;
    delete addBlackFeatures;
    delete removeBlackFeatures;
}



float* Accumulator::getFeatures(bool toMove) {
    return toMove? blackFeatures : whiteFeatures;
}



std::vector<int>* Accumulator::getAddFeatures(bool toMove) {
    return toMove? addBlackFeatures : addWhiteFeatures;
}



std::vector<int>* Accumulator::getRemoveFeatures(bool toMove) {
    return toMove? removeBlackFeatures : removeWhiteFeatures;
}


void Accumulator::refresh_accumulator() {
    resetFlag = true;
    addWhiteFeatures->clear();
    addBlackFeatures->clear();
    removeWhiteFeatures->clear();
    removeBlackFeatures->clear();
    refresh_frequency_maps();
}



void Accumulator::refresh_frequency_maps() {
    for (int i = 0; i < 768; i++) {
        freqMapAdd[i] = 0;
        freqMapRemove[i] = 0;
    }
}


void Accumulator::reset_frequency_index(int i, bool isAdd) {
    isAdd? (freqMapAdd[i] = 0) : (freqMapRemove[i] = 0);
}


void Accumulator::accumulate_add(int pieceType, int location) {
    int whiteIndex = 64 * pieceType + location;
    
    if (freqMapRemove[whiteIndex] == 0) {
        int flipWB = pieceType + ((pieceType & 1) * -2) + 1;
        int blackIndex = 64 * flipWB + (location ^ 56);

        addWhiteFeatures->push_back(whiteIndex);
        addBlackFeatures->push_back(blackIndex);
        freqMapAdd[whiteIndex]++;
        
    }
    else {
        auto it = std::find(removeWhiteFeatures->begin(), removeWhiteFeatures->end(), whiteIndex);

        if (it != removeWhiteFeatures->end()) {
            size_t index = std::distance(removeWhiteFeatures->begin(), it);

            // Swap with the last element, White and black indices should be the same
            std::iter_swap(removeWhiteFeatures->begin() + index, removeWhiteFeatures->end() - 1);
            std::iter_swap(removeBlackFeatures->begin() + index, removeBlackFeatures->end() - 1);

            // Pop the back
            removeWhiteFeatures->pop_back();
            removeBlackFeatures->pop_back();
        }

        freqMapRemove[whiteIndex]--;
        
        
    }
}


void Accumulator::accumulate_remove(int pieceType, int location) {
    int whiteIndex = 64 * pieceType + location;

    if (freqMapAdd[whiteIndex] == 0) {
        int flipWB = pieceType + ((pieceType & 1) * -2) + 1;
        int blackIndex = 64 * flipWB + (location ^ 56);

        removeWhiteFeatures->push_back(whiteIndex);
        removeBlackFeatures->push_back(blackIndex);
        freqMapRemove[whiteIndex]++;
    }
    else {
        auto it = std::find(addWhiteFeatures->begin(), addWhiteFeatures->end(), whiteIndex);

        if (it != addWhiteFeatures->end()) {
            size_t index = std::distance(addWhiteFeatures->begin(), it);

            // Swap with the last element, White and black indices should be the same
            std::iter_swap(addWhiteFeatures->begin() + index, addWhiteFeatures->end() - 1);
            std::iter_swap(addBlackFeatures->begin() + index, addBlackFeatures->end() - 1);

            // Pop the back
            addWhiteFeatures->pop_back();
            addBlackFeatures->pop_back();
        }

        freqMapAdd[whiteIndex]--;
    }
}
