
#ifndef ADAMW_H
#define ADAMW_H

#include "optimizer.h"
#include "../layers/layer.h"

class AdamW : public Optimizer {
public:
    AdamW(float lr, float beta1, float beta2, float weight_decay) : Optimizer(lr), beta1(beta1), beta2(beta2), beta1t(beta1), beta2t(beta2), weight_decay(weight_decay) {}

    template <typename T, int ins, int outs>
    void updateWeights(Layer::Linear<T, ins, outs>& layer, float accScale, float nnueScale, int batchSize) {
        auto& grads = layer.grads;

        for (int j = 0; j < ins; j++) {
            for (int k = 0; k < outs; k++) {
                float& shadowWeight = grads->shadowWeights[j * outs + k];
                float& momentW = grads->momentW[j * outs + k];
                float& vW = grads->vW[j * outs + k];

                momentW = beta1 * momentW + (1 - beta1) * grads->gradWeights[j * outs + k] / batchSize;
                vW = beta2 * vW + (1 - beta2) * std::pow(grads->gradWeights[j * outs + k] / batchSize, 2);

                float mhat = momentW / (1 - beta1t);
                float vhat = vW / (1 - beta2t);

                shadowWeight -= lr * weight_decay * shadowWeight;
                shadowWeight = shadowWeight - lr * mhat / (sqrt(vhat) + 1e-8);
                layer.weights[j * outs + k] = static_cast<T>(std::floor(std::clamp(shadowWeight * nnueScale, -127.0f, 127.0f) * accScale + 0.5f));
            }
        }

        for (int j = 0; j < outs; j++) {
            float& shadowBias = grads->shadowBiases[j];
            float& momentB = grads->momentB[j];
            float& vB = grads->vB[j];
            float gradB = grads->gradBiases[j];

            momentB = beta1 * momentB + (1 - beta1) * gradB / batchSize;
            vB = beta2 * vB + (1 - beta2) * std::pow(gradB / batchSize, 2);

            float mhat = momentB / (1 - beta1t);
            float vhat = vB / (1 - beta2t);
                
            shadowBias = shadowBias - lr * mhat / (sqrt(vhat) + 1e-8);
            layer.biases[j] = static_cast<T>(std::floor(std::clamp(shadowBias * nnueScale, -127.0f, 127.0f) * accScale + 0.5f));
        }
    }

    void finalize() {
        beta1t *= beta1;
        beta2t *= beta2;
    }

private:
    float beta1;
    float beta2;
    float weight_decay;

    double beta1t;
    double beta2t;
};

#endif