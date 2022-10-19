// Implementation of Andrew Grant's texel tuning method with adam optimizer
// from https://pdfhost.io/v/Q~Yrnl8kj_Tuningpdf.pdf

#include <fstream>
#include "../bitboard.h"
#include "../eval.h"
#include "../search.h"
#include "../nnue/KPNNUE.h"
#include <vector>


class Tuner {

public:
    Tuner();
    Tuner(Eval &evalObject, Bitboard &b, std::string fileName);
    void printParams();
    void getFile(std::string fileName, Bitboard &b);
    void tune_adam(int epochs, int batchSize, double lr, double beta1, double beta2);
    

private:

    struct Position {
        std::string fen;
        float result;

        Position() : fen(""), result(0.0) {}
        Position(std::string fen, float result) : fen(fen), result(result) {}

        friend std::ostream& operator<<(std::ostream& os, const Position& pos) {
            os << pos.fen << " =" << pos.result;
            return os; 
        }
    };

    struct Coefficient {
        uint16_t index;
        uint8_t size;
        uint8_t wcoeff;
        uint8_t bcoeff;

        Coefficient() :
            index(0), size(0), wcoeff(0), bcoeff(0) {}

        Coefficient(uint16_t index, uint8_t size, uint8_t wcoeff, uint8_t bcoeff) :
            index(index), size(size), wcoeff(wcoeff), bcoeff(bcoeff) {}
    };

    struct KSFeatures {
        int knightChecksCount[2] = {0};
        int bishopChecksCount[2] = {0};
        int rookChecksCount[2] = {0};
        int queenChecksCount[2] = {0};
        int attackScaleCount[2] = {0};
        int ksAttackersCount[2] = {0};
        int ksAttackersWeight[2] = {0};
        int ksAttacks[2] = {0};
        bool noQueen[2] = {0};
    };

    struct EvalInfo {
        Coefficient *coeffs;
        KSFeatures ksFeatures;
        int evalME;
        int16_t eval;
        int KPeval;
        int16_t phase;
        int16_t scaleEval;
        int16_t nParams;
        double pFactorsMG;
        double pFactorsEG;

        float result;
        bool sideToMove;

        EvalInfo() :
            evalME(0), eval(0), KPeval(0), phase(0), scaleEval(0), nParams(0), result(0), sideToMove(false) {}

        EvalInfo(int eval, int evalME, int KPeval, int phase, int16_t scaleEval, int16_t nParams, float result, bool sideToMove) :
            evalME(evalME), eval(eval), KPeval(KPeval), phase(phase), scaleEval(scaleEval), nParams(nParams), result(result), sideToMove(sideToMove) {}
    };

    struct CoefficientPrint {
        std::string name;
        uint8_t size;

        CoefficientPrint() :
            name("ori"), size(0) {}

        CoefficientPrint(std::string name, uint8_t size) :
            name(name), size(size) {}
    };

    int dataSize;
    int numParams;
    Eval &eval;
    std::vector<CoefficientPrint> eio;
    EvalInfo *evalInfo;
    double **params;
    double *paramsKS;

    void clearArrays();
    void setCoefficient(std::string name, int *coeff, std::vector<Coefficient> &tempCoeffs, int &numCoeff, int index);
    void setCoefficientArr(std::string name, int coeff[][2], std::vector<Coefficient> &tempCoeffs, int &numCoeff, int index, int size);
    void getCoefficients(EvalInfo *evalInfo, int index);
    void getEvalInfo(Bitboard &b, Position *positions);
    double remakeEval(int index);
    double errorRemakeEval(double K);
    double errorTrueEval(double K);
    double sigmoid(double K, double S);
    double computeK(double &bestErr);
    void updateGradient(double K, double **gradient, int index);

};