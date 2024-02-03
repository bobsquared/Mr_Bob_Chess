
#include <vector>
#include <algorithm>

#define MAX_L1 512

class Accumulator {

public:

    struct Features {
        int pieceType;
        int location;

        Features(int pieceType, int location) : pieceType(pieceType), location(location) {}

        bool operator==(const Features& f) const { return (pieceType == f.pieceType) && (location == f.location); }
    };

    Accumulator();
    ~Accumulator();
    void accumulate_add(int pieceType, int location);
    void accumulate_remove(int pieceType, int location);
    void refresh_accumulator();
    void refresh_frequency_maps();
    void reset_frequency_index(int i, bool isAdd);
    std::vector<int>* getAddFeatures(bool toMove);
    std::vector<int>* getRemoveFeatures(bool toMove);
    float* getFeatures(bool toMove);
    bool resetFlag;

private:
    int freqMapAdd[768];
    int freqMapRemove[768];
    std::vector<int> *addWhiteFeatures;
    std::vector<int> *removeWhiteFeatures;
    float *whiteFeatures;

    std::vector<int> *addBlackFeatures;
    std::vector<int> *removeBlackFeatures;
    float *blackFeatures;
};
