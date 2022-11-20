
#include <vector>

#define MAX_L1 256

class Accumulator {

public:

    struct Features {
        int pieceType;
        int location;

        Features(int pieceType, int location) : pieceType(pieceType), location(location) {}
    };

    Accumulator();
    ~Accumulator();
    void accumulate_add(int pieceType, int location);
    void accumulate_remove(int pieceType, int location);
    void refresh_accumulator();
    std::vector<Features>* getAddFeatures();
    std::vector<Features>* getRemoveFeatures();
    float* getFeatures();
    bool resetFlag;

private:
    std::vector<Features> *addFeatures;
    std::vector<Features> *removeFeatures;
    float *features;
};
