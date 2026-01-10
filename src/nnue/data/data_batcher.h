#ifndef DATA_BATCHER_H
#define DATA_BATCHER_H

#include <vector>
#include <algorithm>
#include <random>

namespace Data {
    template <class T, class U, int size>
    struct alignas(64) FeatureLabel {
        static T features[size];
        static U labels[size];
    };

    template <class T, class U>
    struct alignas(64) VectorFeatureLabel {
        std::vector<T> features;
        std::vector<U> labels;
    };

    template <class T, class U, int size>
    inline T FeatureLabel<T, U, size>::features[size];

    template <class T, class U, int size>
    inline U FeatureLabel<T, U, size>::labels[size];

    template <class T, class U>
    class DataBatcher {

    public:
        DataBatcher(VectorFeatureLabel<T, U>& featureLabels, int batchSize, bool shuffle) : 
            featureLabels(featureLabels), batchSize(batchSize), shuffle(shuffle)  {
            iteratorIndex = 0;

            indices.resize(featureLabels.features.size());
            for (int i = 0; i < featureLabels.features.size(); i++) {
                indices[i] = i;
            }

            if (shuffle) {
                std::shuffle(std::begin(indices), std::end(indices), rng);
            }
        }

        template <int size>
        bool GetNextBatch(FeatureLabel<T, U, size>& batch) {
            thisBatchSize = batchSize;

            if (iteratorIndex >= featureLabels.features.size()) {
                iteratorIndex = 0;
                thisBatchSize = 0;
                return false;
            }

            if (iteratorIndex + batchSize < featureLabels.features.size()) {
                for (int i = iteratorIndex; i < iteratorIndex + batchSize; i++) {
                    batch.features[i - iteratorIndex] = featureLabels.features[indices[i]];
                    batch.labels[i - iteratorIndex] = featureLabels.labels[indices[i]];
                }
                iteratorIndex += batchSize;
            }
            else {
                thisBatchSize = featureLabels.features.size() - iteratorIndex;
                for (int i = iteratorIndex; i < featureLabels.features.size(); i++) {
                    batch.features[i - iteratorIndex] = featureLabels.features[indices[i]];
                    batch.labels[i - iteratorIndex] = featureLabels.labels[indices[i]];
                }
                iteratorIndex += batchSize;

                if (shuffle) {
                    std::shuffle(std::begin(indices), std::end(indices), rng);
                }
            }
            return true;
        }

        int getCurrentBatchSize() const {
            return thisBatchSize;
        }

    private:
        VectorFeatureLabel<T, U>& featureLabels;
        std::vector<int> indices;
        std::default_random_engine rng = std::default_random_engine {};

        bool shuffle;
        int batchSize;
        int thisBatchSize;
        int iteratorIndex;
    };

}


#endif