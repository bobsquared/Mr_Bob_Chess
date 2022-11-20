#include "accumulator.h"


Accumulator::Accumulator() {
    features = new float[MAX_L1 + 8];
    resetFlag = false;
    addFeatures = new std::vector<Features>();
    removeFeatures = new std::vector<Features>();
}


Accumulator::~Accumulator() {
    delete [] features;
    delete addFeatures;
    delete removeFeatures;
}



float* Accumulator::getFeatures() {
    return features;
}



std::vector<Accumulator::Features>* Accumulator::getAddFeatures() {
    return addFeatures;
}



std::vector<Accumulator::Features>* Accumulator::getRemoveFeatures() {
    return removeFeatures;
}


void Accumulator::refresh_accumulator() {
    resetFlag = true;
}

void Accumulator::accumulate_add(int pieceType, int location) {
    addFeatures->push_back(Features(64 * pieceType, location));
}


void Accumulator::accumulate_remove(int pieceType, int location) {
    removeFeatures->push_back(Features(64 * pieceType, location));
}
