#include "accumulator.h"


Accumulator::Accumulator() {
    resetFlag = false;
    addFeatures = new std::vector<Features>();
    removeFeatures = new std::vector<Features>();
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
