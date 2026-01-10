#ifndef LAYER_H
#define LAYER_H

#include <vector>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <random>
#include <immintrin.h>

namespace Layer {

    enum INITIALIZER {
        INIT_XAVIAR,
        INIT_HE
    };

    template <int ins, int outs>
    struct alignas(64) LayerGrads {
        static constexpr int padded_ins = ((ins + 31) / 32) * 32;
        static constexpr int padded_outs = ((outs + 31) / 32) * 32;
        alignas(64) float gradWeights[padded_ins * padded_outs] = {0.0f};
        alignas(64) float shadowWeights[padded_ins * padded_outs] = {0.0f};

        alignas(64) float momentW[ins * outs] = {0.0f};
        alignas(64) float vW[ins * outs] = {0.0f};

        alignas(64) float gradBiases[padded_outs] = {0.0f};
        alignas(64) float shadowBiases[padded_outs] = {0.0f};

        alignas(64) float momentB[outs] = {0.0f};
        alignas(64) float vB[padded_outs] = {0.0f};

        alignas(64) uint8_t inputVector[ins] = {0}; 

    };

    template <typename T, int ins, int outs>
    struct Linear {
        static constexpr int padded_ins = ((ins + 31) / 32) * 32;
        static constexpr int padded_outs = ((outs + 31) / 32) * 32;

        LayerGrads<ins, outs>* grads = nullptr;

        alignas(64) T weights[padded_ins * padded_outs];
        alignas(64) T biases[padded_outs];

        float scaleA = 126.0f; // Scale for activation
        float scaleS = 63.0f;  // Scale for weights
        float scaleO = 10000.0f; // Scale for output
        float scaleC;

        Linear() {}

        Linear(bool training, float scaleA, float scaleS, INITIALIZER initializer, float scaleO=1.0f) {
            std::default_random_engine generator;
            std::normal_distribution<float> distribution(0.0, 1.0); // default, will be overwritten

            if (initializer == INIT_XAVIAR) {
                distribution = std::normal_distribution<float>(0.0, sqrt(2.0f / (ins + outs)));
            }
            else if (initializer == INIT_HE) {
                distribution = std::normal_distribution<float>(0.0, sqrt(2.0f / ins));
            }
            
            this->scaleA = scaleA;
            this->scaleS = scaleS;
            this->scaleO = scaleO;
            this->scaleC = scaleO / (scaleA * scaleS);

            if (scaleO == 1.0f) {
                this->scaleC = 1.0f / scaleS;
            }

            if (training) {
                grads = new LayerGrads<ins, outs>();
                // Initialize only the non-padded region
                for (size_t i = 0; i < outs; i++) {
                    for (size_t j = 0; j < ins; j++) {
                        grads->shadowWeights[j * outs + i] = distribution(generator);
                        weights[j * outs + i] = static_cast<T>(
                            std::roundf(std::clamp(grads->shadowWeights[j * outs + i] * scaleS, -127.0f, 127.0f))
                        );
                        
                    }
                    grads->shadowBiases[i] = distribution(generator);
                    biases[i] = static_cast<T>(
                        std::roundf(std::clamp(grads->shadowBiases[i] * scaleS, -127.0f, 127.0f))
                    );
                }
            }
        }

        #ifdef USE_AVX2
        void execute(const uint8_t* const inputVector, int32_t* outputVector) {
            const __m256i ones = _mm256_set1_epi16(1);

            if (grads) {
                for (int i = 0; i < ins; i += 32) {
                    __m256i v = _mm256_load_si256(reinterpret_cast<const __m256i*>(&inputVector[i]));
                    _mm256_store_si256(reinterpret_cast<__m256i*>(&grads->inputVector[i]), v);
                }
            }

            // Process outputs in blocks of 4 for AVX2
            for (int i = 0; i < outs; i += 4) {
                __m256i z1 = _mm256_setzero_si256();
                __m256i z2 = _mm256_setzero_si256();
                __m256i z3 = _mm256_setzero_si256();
                __m256i z4 = _mm256_setzero_si256();

                for (int j = 0; j < ins; j += 32) {
                    if (j + 32 < ins) {
                        _mm_prefetch(reinterpret_cast<const char*>(&inputVector[j + 32]), _MM_HINT_T0);
                        _mm_prefetch(reinterpret_cast<const char*>(&weights[i * ins + j + 32]), _MM_HINT_T0);
                        _mm_prefetch(reinterpret_cast<const char*>(&weights[(i + 1) * ins + j + 32]), _MM_HINT_T0);
                        _mm_prefetch(reinterpret_cast<const char*>(&weights[(i + 2) * ins + j + 32]), _MM_HINT_T0);
                        _mm_prefetch(reinterpret_cast<const char*>(&weights[(i + 3) * ins + j + 32]), _MM_HINT_T0);
                    }

                    __m256i in_vec = _mm256_load_si256(reinterpret_cast<const __m256i*>(&inputVector[j]));

                    z1 = _mm256_add_epi32(z1, _mm256_madd_epi16(_mm256_maddubs_epi16(in_vec, _mm256_load_si256(reinterpret_cast<const __m256i*>(&weights[i * ins + j]))), ones));
                    z2 = _mm256_add_epi32(z2, _mm256_madd_epi16(_mm256_maddubs_epi16(in_vec, _mm256_load_si256(reinterpret_cast<const __m256i*>(&weights[(i + 1) * ins + j]))), ones));
                    z3 = _mm256_add_epi32(z3, _mm256_madd_epi16(_mm256_maddubs_epi16(in_vec, _mm256_load_si256(reinterpret_cast<const __m256i*>(&weights[(i + 2) * ins + j]))), ones));
                    z4 = _mm256_add_epi32(z4, _mm256_madd_epi16(_mm256_maddubs_epi16(in_vec, _mm256_load_si256(reinterpret_cast<const __m256i*>(&weights[(i + 3) * ins + j]))), ones));
                }

                z1 = _mm256_hadd_epi32(z1, z2);
                z3 = _mm256_hadd_epi32(z3, z4);
                z1 = _mm256_hadd_epi32(z1, z3);

                _mm_store_si128(reinterpret_cast<__m128i*>(&outputVector[i]), _mm_add_epi32(_mm256_castsi256_si128(z1), _mm256_extracti128_si256(z1, 1)));
                
            }

            for (int i = 0; i < outs; i++) {
                outputVector[i] = static_cast<int32_t>(scaleC * (outputVector[i] + biases[i] * scaleA));
            }

        }

        #else
        void execute(const uint8_t* const inputVector, int32_t* outputVector) {
            if (grads) {
                for (int i = 0; i < ins; i++) {
                    grads->inputVector[i] = inputVector[i];
                }
            }

            for (int i = 0; i < outs; i++) {
                for (int j = 0; j < ins; j++) {
                    outputVector[i] += inputVector[j] * weights[i * ins + j];
                }
            }

            for (int i = 0; i < outs; i++) {
                outputVector[i] = static_cast<int32_t>(scaleC * (outputVector[i] + biases[i] * scaleA));
            }
        }
        #endif

        void backward(const float* const err, float* dlinear) {
            float scaleBias = scaleO / scaleS;
            float scaleWeight = scaleO / (scaleA * scaleS);
            std::fill(dlinear, dlinear + ins, 0.0f);

            for (int i = 0; i < outs; i++) {
                for (int j = 0; j < ins; j++) {
                    grads->gradWeights[i * ins + j] += err[i] * grads->inputVector[j] * scaleWeight;
                    dlinear[j] += err[i] * weights[i * ins + j] * scaleWeight;
                }
                grads->gradBiases[i] += err[i] * scaleBias;
            }
        }

        void backward(const float* const err, std::vector<int>& addAccumulate) {
            float scaleD = scaleA / scaleS;
            
            for (int i : addAccumulate) {
                for (int j = 0; j < outs; j++) {
                    grads->gradWeights[i * outs + j] += err[j] * scaleD;
                }
            }

            for (int i = 0; i < outs; i++) {
                grads->gradBiases[i] += err[i] * scaleD;
            }

            addAccumulate.clear();
        }

        void zeroGrads() {
            std::fill(grads->gradWeights, grads->gradWeights + padded_ins * padded_outs, 0.0f);
            std::fill(grads->gradBiases, grads->gradBiases + padded_outs, 0.0f);
        }

        void Save(std::ofstream& out) const {
            if (!out) {
                throw std::runtime_error("Failed to open file for saving.");
            }

            // Save weights
            out.write(reinterpret_cast<const char*>(weights), padded_ins * padded_outs * sizeof(T));
            if (!out) {
                throw std::runtime_error("Failed to write weights to file.");
            }

            // Save biases
            out.write(reinterpret_cast<const char*>(biases), padded_outs * sizeof(T));
            if (!out) {
                throw std::runtime_error("Failed to write biases to file.");
            }
        }

        void Load(std::ifstream& in) {
            if (!in) {
                throw std::runtime_error("Failed to open file for loading.");
            }

            // Load weights
            in.read(reinterpret_cast<char*>(weights), padded_ins * padded_outs * sizeof(T));
            if (!in) {
                throw std::runtime_error("Failed to read weights from file.");
            }

            // Load biases
            in.read(reinterpret_cast<char*>(biases), padded_outs * sizeof(T));
            if (!in) {
                throw std::runtime_error("Failed to read biases from file.");
            }
        }

    };

}

#endif