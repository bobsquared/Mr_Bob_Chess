// Implementation of Andrew Grant's texel tuning method with adam optimizer
// from https://pdfhost.io/v/Q~Yrnl8kj_Tuningpdf.pdf


#include "tuner.h"

extern int pieceValues[6];

extern int doublePawnValue;
extern int isolatedPawnValue;
extern int opposedPawnValue[8];
extern int backwardPawnValue;

extern int kingPawnDistFriendly[8];
extern int kingPawnDistEnemy[8];
extern int kingPassedDistFriendly[8];
extern int kingPassedDistEnemy[8];

extern int passedPawnWeight[7];
extern int supportedPawnWeight[64];
extern int adjacentPawnWeight[64];
extern int freePasser[7];

extern int knightWeight[9];
extern int rookWeight[9];
extern int bishopWeight[9];

extern int knightMobilityBonus[9];
extern int bishopMobilityBonus[14];
extern int rookMobilityBonus[15];
extern int queenMobilityBonus[28];

extern int knightPair;
extern int rookPair;
extern int noPawns;

extern int rookOnOpen;
extern int rookOnSemiOpen;
extern int rookOnQueen;

extern int PAWN_TABLE[64];
extern int KNIGHT_TABLE[64];
extern int BISHOP_TABLE[64];
extern int ROOK_TABLE[64];
extern int QUEEN_TABLE[64];
extern int KING_TABLE[64];


extern int knightThreatPiece[5];
extern int bishopThreatPiece[5];
extern int rookThreatPiece[5];
extern int queenThreatPiece[5];
extern int kingThreatPiece[5];

extern int pawnThreat;
extern int pawnPushThreat;

extern int pawnShield[64];
extern int pawnStorm[64];
extern int pawnStormBlockade[64];
extern int kingPawnFront;
extern int kingPawnFrontN;

extern int trappedRook;
extern int rookBehindPasser;
extern int tempoBonus;

extern int blockedPawns5th;
extern int blockedPawns6th;

extern int KNIGHT_OUTPOST[64];
extern int KNIGHT_OUTPOST_JUMP[64];
extern int BISHOP_OUTPOST[64];
extern int BISHOP_OUTPOST_JUMP[64];

extern int attacksSafety;
extern int queenCheckVal;
extern int rookCheckVal;
extern int bishopCheckVal;
extern int knightCheckVal;
extern int KSOffset;
extern int noQueen;


Tuner::Tuner(Eval &evalObject, Bitboard &b, KPNNUE &model, std::string fileName) : eval(evalObject), board(b), model(model) {
    dataSize = 0;

    clearArrays();
    getFile(fileName);

    params = new double *[eio.size()];
    paramsKS = new double[7]();

    for (size_t i = 0; i < eio.size(); i++) {
        params[i] = new double[2]();
    }

}



void Tuner::getFile(std::string fileName) {

    std::string tempFen;
    std::string fen;
    std::string stringDelimiter = " [";

    float score;
    int count = 0;

    std::ifstream Myfile(fileName);
    while (std::getline(Myfile, tempFen)) {
        // if (count == 100000) {
        //     break;
        // }
        count++;
    }
    dataSize = count;

    Myfile.clear();
    Myfile.seekg(0, std::ios::beg);

    evalInfo = new EvalInfo[dataSize];

    count = 0;
    while (std::getline(Myfile, tempFen)) {

        // if (count == 100000) {
        //     break;
        // }
        
        if (count % (dataSize / 100) == 0) {
            std::cout << "Filling data...   Iteration: " << count << std::endl;
        }
        
        size_t delimiter = tempFen.find(stringDelimiter);
        fen = tempFen.substr(0, delimiter);
        score = std::stof(tempFen.substr(delimiter + stringDelimiter.size(), tempFen.size() - 1 - delimiter - stringDelimiter.size()));

        board.setPosFen(fen);
        eval.clearTrace();

        int phase = TOTALPHASE;
        phase -= (board.pieceCount[0] + board.pieceCount[1]) * PAWNPHASE;
        phase -= (board.pieceCount[2] + board.pieceCount[3]) * KNIGHTPHASE;
        phase -= (board.pieceCount[4] + board.pieceCount[5]) * BISHOPPHASE;
        phase -= (board.pieceCount[6] + board.pieceCount[7]) * ROOKPHASE;
        phase -= (board.pieceCount[8] + board.pieceCount[9]) * QUEENPHASE;

        int phaseTotal = (phase * 256 + (TOTALPHASE / 2)) / TOTALPHASE;
        int evalME = eval.evaluate(board, &thread[0]);
        int scaleEval = eval.scaleEndgame(board, EGVAL(evalME));
        int seval = ((MGVAL(evalME) * (256 - phaseTotal)) + (EGVAL(evalME) * phaseTotal * scaleEval / 256)) / 256;
        evalInfo[count] = EvalInfo(seval, evalME, phaseTotal, scaleEval, 0, score, board.toMove);

        evalInfo[count].pFactorsMG = 1 - phase / 24.0;
        evalInfo[count].pFactorsEG = phase / 24.0;

        getCoefficients(&evalInfo[count], count);

        for (int j = 0; j < 2; j++) {
            evalInfo[count].ksFeatures.knightChecksCount[j] = eval.kingSafetyTrace.knightChecksCount[j];
            evalInfo[count].ksFeatures.bishopChecksCount[j] = eval.kingSafetyTrace.bishopChecksCount[j];
            evalInfo[count].ksFeatures.rookChecksCount[j] = eval.kingSafetyTrace.rookChecksCount[j];
            evalInfo[count].ksFeatures.queenChecksCount[j] = eval.kingSafetyTrace.queenChecksCount[j];
            evalInfo[count].ksFeatures.attackScaleCount[j] = eval.kingSafetyTrace.attackScaleCount[j];
            evalInfo[count].ksFeatures.ksAttackersCount[j] = thread[0].KSAttackersCount[j];
            evalInfo[count].ksFeatures.ksAttackersWeight[j] = thread[0].KSAttackersWeight[j];
            evalInfo[count].ksFeatures.ksAttacks[j] = thread[0].KSAttacks[j];
            evalInfo[count].ksFeatures.noQueen[j] = eval.kingSafetyTrace.noQueen[j];
        }

        count++;
    }

}



void Tuner::printParams() {
    std::cout << "******* Tuned parameter values *******" << std::endl;
    for (size_t i = 0; i < eio.size(); i++) {
        if (eio[i].size > 1) {
            std::cout << "int " << eio[i].name << "[" << (int) eio[i].size << "]" << " = ";
        }
        else {
            std::cout << "int " << eio[i].name << " = ";
        }
        

        if (eio[i].size > 1) {
            std::cout << "{";
        }
        for (int j = 0; j < eio[i].size; j++) {
            if (j < eio[i].size - 1) {
                std::cout << "S(" << (int) params[i + j][0] << ", " << (int) params[i + j][1] << "), ";
            }
            else {
                std::cout << "S(" << (int) params[i + j][0] << ", " << (int) params[i + j][1] << ")";
                if (eio[i].size == 1) {
                    std::cout << ";" << std::endl;
                }
            }
            if ((eio[i].size == 64 && j % 8 == 7) || (eio[i].size == 32 && j % 4 == 3)) {
                std::cout << std::endl;
            }
        }

        if (eio[i].size > 1) {
            std::cout << "};" << std::endl;
        }

        i += eio[i].size - 1;

    }
    std::cout << "**************************************" << std::endl;
}


void Tuner::clearArrays() {
    // std::fill_n(pieceValues, 5, 0);
    std::fill_n(passedPawnWeight, 7, 0);
    std::fill_n(supportedPawnWeight, 64, 0);
    std::fill_n(adjacentPawnWeight, 64, 0);
    std::fill_n(freePasser, 7, 0);
    std::fill_n(opposedPawnValue, 8, 0);

    std::fill_n(knightWeight, 9, 0);
    std::fill_n(rookWeight, 9, 0);
    std::fill_n(bishopWeight, 9, 0);

    std::fill_n(knightMobilityBonus, 9, 0);
    std::fill_n(bishopMobilityBonus, 14, 0);
    std::fill_n(rookMobilityBonus, 15, 0);
    std::fill_n(queenMobilityBonus, 28, 0);


    std::fill_n(knightThreatPiece, 5, 0);
    std::fill_n(bishopThreatPiece, 5, 0);
    std::fill_n(rookThreatPiece, 5, 0);

    std::fill_n(PAWN_TABLE, 64, 0);
    std::fill_n(KNIGHT_TABLE, 64, 0);
    std::fill_n(BISHOP_TABLE, 64, 0);
    std::fill_n(ROOK_TABLE, 64, 0);
    std::fill_n(QUEEN_TABLE, 64, 0);
    std::fill_n(KING_TABLE, 64, 0);

    std::fill_n(KNIGHT_OUTPOST, 64, 0);
    std::fill_n(KNIGHT_OUTPOST_JUMP, 64, 0);
    std::fill_n(BISHOP_OUTPOST, 64, 0);
    std::fill_n(BISHOP_OUTPOST_JUMP, 64, 0);

    std::fill_n(kingPawnDistFriendly, 8, 0);
    std::fill_n(kingPawnDistEnemy, 8, 0);
    std::fill_n(kingPassedDistFriendly, 8, 0);
    std::fill_n(kingPassedDistEnemy, 8, 0);

    std::fill_n(pawnShield, 64, 0);
    std::fill_n(pawnStorm, 64, 0);
    std::fill_n(pawnStormBlockade, 64, 0);

    knightPair = 0;
    rookPair = 0;

    rookOnOpen = 0;
    rookOnSemiOpen = 0;
    rookOnQueen = 0;

    kingPawnFront = 0;
    kingPawnFrontN = 0;

    pawnThreat = 0;
    pawnPushThreat = 0;

    trappedRook = 0;
    rookBehindPasser = 0;
    tempoBonus = 0;

    doublePawnValue = 0;
    isolatedPawnValue = 0;
    backwardPawnValue = 0;

    blockedPawns5th = 0;
    blockedPawns6th = 0;

    eval.InitPieceBoards();
    eval.InitOutpostMask();

}



void Tuner::setCoefficient(std::string name, int *coeff, std::vector<Coefficient> &tempCoeffs, int &numCoeff, int index) {
    if (coeff[0] != coeff[1]) {
        tempCoeffs.push_back(Coefficient(numCoeff, 1, coeff[0], coeff[1]));
    }

    if (index == 0) {
        eio.push_back(CoefficientPrint(name, 1));
    }

    numCoeff++;
}



void Tuner::setCoefficientArr(std::string name, int coeff[][2], std::vector<Coefficient> &tempCoeffs, int &numCoeff, int index, int size) {

    if (index == 0) {
        for (int i = 0; i < size; i++) {
            eio.push_back(CoefficientPrint(name, size));
        }
    }

    for (int i = 0; i < size; i++) {
        if (coeff[i][0] != coeff[i][1]) {
            tempCoeffs.push_back(Coefficient(numCoeff, size, coeff[i][0], coeff[i][1]));
        }
        numCoeff++;
    }

}







void Tuner::getCoefficients(EvalInfo *evalInfo, int index) {

    int numCoeff = 0;
    std::vector<Coefficient> tempCoeffs;
    // setCoefficient("PawnValue", eval.evalTrace.pawnCoeff, tempCoeffs, numCoeff, index);
    // setCoefficient("KnightValue", eval.evalTrace.knightCoeff, tempCoeffs, numCoeff, index);
    // setCoefficient("BishopValue", eval.evalTrace.bishopCoeff, tempCoeffs, numCoeff, index);
    // setCoefficient("RookValue", eval.evalTrace.rookCoeff, tempCoeffs, numCoeff, index);
    // setCoefficient("QueenValue", eval.evalTrace.queenCoeff, tempCoeffs, numCoeff, index);

    // PAWNS
    setCoefficientArr("PAWN_TABLE", eval.evalTrace.pawnPstCoeff, tempCoeffs, numCoeff, index, 64);
    setCoefficientArr("supportedPawnWeight", eval.evalTrace.supportedPawnsCoeff, tempCoeffs, numCoeff, index, 64);
    setCoefficientArr("adjacentPawnWeight", eval.evalTrace.adjacentPawnsCoeff, tempCoeffs, numCoeff, index, 64);
    

    setCoefficientArr("passedPawnWeight", eval.evalTrace.passedPawnCoeff, tempCoeffs, numCoeff, index, 7);
    setCoefficientArr("freePasser", eval.evalTrace.freePasserCoeff, tempCoeffs, numCoeff, index, 7);
    setCoefficientArr("opposedPawnValue", eval.evalTrace.opposedPawnCoeff, tempCoeffs, numCoeff, index, 8);

    setCoefficient("doublePawnValue", eval.evalTrace.doubledPawnsCoeff, tempCoeffs, numCoeff, index);
    setCoefficient("isolatedPawnValue", eval.evalTrace.isolatedPawnsCoeff, tempCoeffs, numCoeff, index);
    setCoefficient("backwardPawnValue", eval.evalTrace.backwardPawnCoeff, tempCoeffs, numCoeff, index);
    setCoefficient("blockedPawns5th", eval.evalTrace.blockedPawns5thCoeff, tempCoeffs, numCoeff, index);
    setCoefficient("blockedPawns6th", eval.evalTrace.blockedPawns6thCoeff, tempCoeffs, numCoeff, index);

    setCoefficientArr("kingPawnDistFriendly", eval.evalTrace.kingPawnDistFriendlyCoeff, tempCoeffs, numCoeff, index, 8);
    setCoefficientArr("kingPawnDistEnemy", eval.evalTrace.kingPawnDistEnemyCoeff, tempCoeffs, numCoeff, index, 8);
    setCoefficientArr("kingPassedDistFriendly", eval.evalTrace.kingPassedDistFriendlyCoeff, tempCoeffs, numCoeff, index, 8);
    setCoefficientArr("kingPassedDistEnemy", eval.evalTrace.kingPassedDistEnemyCoeff, tempCoeffs, numCoeff, index, 8);

    setCoefficient("pawnThreat", eval.evalTrace.pawnThreatCoeff, tempCoeffs, numCoeff, index);
    setCoefficient("pawnPushThreat", eval.evalTrace.pawnPushThreatCoeff, tempCoeffs, numCoeff, index);

    // KNIGHTS
    setCoefficientArr("KNIGHT_TABLE", eval.evalTrace.knightPstCoeff, tempCoeffs, numCoeff, index, 64);
    setCoefficientArr("KNIGHT_OUTPOST", eval.evalTrace.knightOutpostCoeff, tempCoeffs, numCoeff, index, 64);
    setCoefficientArr("KNIGHT_OUTPOST_JUMP", eval.evalTrace.knightOutpostJumpCoeff, tempCoeffs, numCoeff, index, 64);
    setCoefficientArr("knightMobilityBonus", eval.evalTrace.knightMobilityCoeff, tempCoeffs, numCoeff, index, 9);
    setCoefficientArr("knightWeight", eval.evalTrace.knightWeightCoeff, tempCoeffs, numCoeff, index, 9);
    setCoefficientArr("knightThreatPiece", eval.evalTrace.knightThreatCoeff, tempCoeffs, numCoeff, index, 5);
    setCoefficient("knightPair", eval.evalTrace.knightPairCoeff, tempCoeffs, numCoeff, index);

    // BISHOPS
    setCoefficientArr("BISHOP_TABLE", eval.evalTrace.bishopPstCoeff, tempCoeffs, numCoeff, index, 64);
    setCoefficientArr("BISHOP_OUTPOST", eval.evalTrace.bishopOutpostCoeff, tempCoeffs, numCoeff, index, 64);
    setCoefficientArr("BISHOP_OUTPOST_JUMP", eval.evalTrace.knightOutpostJumpCoeff, tempCoeffs, numCoeff, index, 64);
    setCoefficientArr("bishopMobilityBonus", eval.evalTrace.bishopMobilityCoeff, tempCoeffs, numCoeff, index, 14);
    setCoefficientArr("bishopWeight", eval.evalTrace.bishopWeightCoeff, tempCoeffs, numCoeff, index, 9);
    setCoefficientArr("bishopThreatPiece", eval.evalTrace.bishopThreatCoeff, tempCoeffs, numCoeff, index, 5);

    // ROOKS
    setCoefficientArr("ROOK_TABLE", eval.evalTrace.rookPstCoeff, tempCoeffs, numCoeff, index, 64);
    setCoefficientArr("rookMobilityBonus", eval.evalTrace.rookMobilityCoeff, tempCoeffs, numCoeff, index, 15);
    setCoefficientArr("rookWeight", eval.evalTrace.rookWeightCoeff, tempCoeffs, numCoeff, index, 9);
    setCoefficientArr("rookThreatPiece", eval.evalTrace.rookThreatCoeff, tempCoeffs, numCoeff, index, 5);
    setCoefficient("rookPair", eval.evalTrace.rookPairCoeff, tempCoeffs, numCoeff, index);
    setCoefficient("rookOnOpen", eval.evalTrace.rookOnOpenCoeff, tempCoeffs, numCoeff, index);
    setCoefficient("rookOnSemiOpen", eval.evalTrace.rookOnSemiOpenCoeff, tempCoeffs, numCoeff, index);
    setCoefficient("rookOnQueen", eval.evalTrace.rookOnQueenCoeff, tempCoeffs, numCoeff, index);
    setCoefficient("rookBehindPasser", eval.evalTrace.rookBehindPasserCoeff, tempCoeffs, numCoeff, index);
    setCoefficient("trappedRook", eval.evalTrace.trappedRookCoeff, tempCoeffs, numCoeff, index);

    // QUEENS
    setCoefficientArr("QUEEN_TABLE", eval.evalTrace.queenPstCoeff, tempCoeffs, numCoeff, index, 64);
    setCoefficientArr("queenMobilityBonus", eval.evalTrace.queenMobilityCoeff, tempCoeffs, numCoeff, index, 28);

    // KINGS
    setCoefficientArr("KING_TABLE", eval.evalTrace.kingPstCoeff, tempCoeffs, numCoeff, index, 64);
    setCoefficient("kingPawnFront", eval.evalTrace.kingPawnFrontCoeff, tempCoeffs, numCoeff, index);
    setCoefficient("kingPawnFrontN", eval.evalTrace.kingPawnFrontNCoeff, tempCoeffs, numCoeff, index);
    setCoefficientArr("pawnShield", eval.evalTrace.pawnShieldCoeff, tempCoeffs, numCoeff, index, 64);
    setCoefficientArr("pawnStorm", eval.evalTrace.pawnStormCoeff, tempCoeffs, numCoeff, index, 64);
    setCoefficientArr("pawnStormBlockade", eval.evalTrace.pawnStormBlockadeCoeff, tempCoeffs, numCoeff, index, 64);
    setCoefficient("tempoBonus", eval.evalTrace.tempoCoeff, tempCoeffs, numCoeff, index);

    evalInfo->nParams = (int) tempCoeffs.size();
    evalInfo->coeffs = new Coefficient[evalInfo->nParams];
    for (int i = 0; i < evalInfo->nParams; i++) {
        evalInfo->coeffs[i] = tempCoeffs[i];
    }

}


double Tuner::remakeEval(int index) {
    int ret = 0;
    int createdScore[2] = {0};

    for (int i = 0; i < evalInfo[index].nParams; i++) {
        createdScore[0] += evalInfo[index].coeffs[i].wcoeff * params[evalInfo[index].coeffs[i].index][0];
        createdScore[1] += evalInfo[index].coeffs[i].wcoeff * params[evalInfo[index].coeffs[i].index][1];
        createdScore[0] -= evalInfo[index].coeffs[i].bcoeff * params[evalInfo[index].coeffs[i].index][0];
        createdScore[1] -= evalInfo[index].coeffs[i].bcoeff * params[evalInfo[index].coeffs[i].index][1];
    }
    
    // King safety
    for (int i = 0; i < 2; i++) {
        int kingSafe = 0;
        if (evalInfo[index].ksFeatures.ksAttackersCount[i] > 1) {
            kingSafe = evalInfo[index].ksFeatures.ksAttackersWeight[i];
            kingSafe += queenCheckVal * evalInfo[index].ksFeatures.queenChecksCount[i];
            kingSafe += rookCheckVal * evalInfo[index].ksFeatures.rookChecksCount[i];
            kingSafe += bishopCheckVal * evalInfo[index].ksFeatures.bishopChecksCount[i];
            kingSafe += knightCheckVal * evalInfo[index].ksFeatures.knightChecksCount[i];
            kingSafe += noQueen * evalInfo[index].ksFeatures.noQueen[i];
            kingSafe += (attacksSafety * evalInfo[index].ksFeatures.ksAttacks[i]) / evalInfo[index].ksFeatures.attackScaleCount[i];
            kingSafe -= KSOffset;

            if (kingSafe > 0) {
                if (i == 0) {
                    createdScore[0] += kingSafe;
                }
                else {
                    createdScore[0] -= kingSafe;
                }
            }
        }
    }

    ret = (((MGVAL(evalInfo[index].evalME) + createdScore[0]) * (256 - evalInfo[index].phase)) + 
           ((EGVAL(evalInfo[index].evalME) + createdScore[1]) * evalInfo[index].phase * evalInfo[index].scaleEval / 256)) / 256;

    return ret;
}


double Tuner::sigmoid(double K, double S) {
    return 1.0 / (1.0 + exp(-K * S / 400));
}


double Tuner::errorTrueEval(double K) {
    double ret = 0.0;

    for (int i = 0; i < dataSize; i++) {
        ret += std::pow(evalInfo[i].result - sigmoid(K, (double) evalInfo[i].eval), 2);
    }

    return ret / dataSize;
}


double Tuner::errorRemakeEval(double K) {
    double ret = 0.0;
    for (int i = 0; i < dataSize; i++) {
        ret += std::pow(evalInfo[i].result - sigmoid(K, remakeEval(i)), 2);
    }

    return ret / dataSize;
}



double Tuner::computeK(double &bestErr) {
    double start = 0.0;
    double end = 10.0;
    double step = 1.0;
    double window = 9.0;

    double err = 1.0;
    double bestK = 0;
    int iteration = 0;

    std::cout << "************* Compute K **************" << std::endl;
    for (int i = 0; i < 3; i++) {
        for (double j = start; j < end; j += step) {
            err = errorTrueEval(j);
            if (err < bestErr) {
                bestK = j;
                bestErr = err;
            }
            iteration++;

            std::cout << "Iteration: " << iteration << ", Current K: " << j << ", Best K: " << bestK << ", Current Error: " << err << ", Best Error: " << bestErr << std::endl;
        }
        step /= 10.0;
        window /= 10.0;
        end = bestK + window + step;
        start = bestK - window;
    }
    std::cout << "**************************************" << std::endl << std::endl;

    return bestK;
}


void Tuner::updateGradient(double K, double **gradient, int index) {

    double E = remakeEval(index);
    double S = sigmoid(K, E);
    double A = (evalInfo[index].result - S) * S * (1 - S);

    double mgBase = A * evalInfo[index].pFactorsMG;
    double egBase = A * evalInfo[index].pFactorsEG;
    
    for (int i = 0; i < evalInfo[index].nParams; i++) {

        int wcoeff = (evalInfo[index].coeffs[i]).wcoeff;
        int bcoeff = (evalInfo[index].coeffs[i]).bcoeff;
        assert(wcoeff != bcoeff);

        gradient[evalInfo[index].coeffs[i].index][0] += mgBase * (wcoeff - bcoeff);
        gradient[evalInfo[index].coeffs[i].index][1] += egBase * (wcoeff - bcoeff) * evalInfo[index].scaleEval;
        
    }
    
}



void Tuner::tune_adam(int epochs, int batchSize, double lr, double beta1, double beta2) {
    const size_t nParams = eio.size();

    double minErr = 10000;
    double K = computeK(minErr);

    std::vector<double> alpha(nParams, 0.0);
    std::vector<std::vector<double>> moment;
    moment.resize(nParams, std::vector<double>(2, 0.0));

    std::vector<std::vector<double>> v;
    v.resize(nParams, std::vector<double>(2, 0.0));

    for (int epoch = 0; epoch < epochs; epoch++) {

        printParams();
        double err = errorRemakeEval(K);
        std::cout << "Starting Epoch: " << epoch << "  -  latest error: " << err << std::endl;

        for (int batch = 0; batch < dataSize / batchSize + 1; batch++) {
            if (batch % (((dataSize / batchSize) / 20) + 1) == 0) {
                std::cout << "Batch [" << batch << " / " << (dataSize / batchSize) + 1 << "]  -  " << batch * 100 / ((dataSize / batchSize) + 1)<< "%" << std::endl;
            }

            double **gradient = new double*[nParams];
            for (size_t i = 0; i < nParams; i++) {
                gradient[i] = new double[2]();
            }
        
            for (int i = batch * batchSize; i < (batch + 1) * batchSize && i < dataSize; i++) {
                updateGradient(K, gradient, i);
            }

            uint64_t t = batch + epoch * batchSize;
            for (size_t j = 0; j < nParams; j++) {
                alpha[j] = lr * sqrt(1 - std::pow(beta2, t + 1)) / (1 - std::pow(beta1, t + 1));

                moment[j][0] = beta1 * moment[j][0] + (1 - beta1) * gradient[j][0] / batchSize;
                moment[j][1] = beta1 * moment[j][1] + (1 - beta1) * gradient[j][1] / batchSize;

                v[j][0] = beta2 * v[j][0] + (1 - beta2) * std::pow(gradient[j][0] / batchSize, 2);
                v[j][1] = beta2 * v[j][1] + (1 - beta2) * std::pow(gradient[j][1] / batchSize, 2);

                params[j][0] = params[j][0] + alpha[j] * moment[j][0] / (sqrt(v[j][0]) + 1e-8);
                params[j][1] = params[j][1] + alpha[j] * moment[j][1] / (sqrt(v[j][1]) + 1e-8);
            }

            for (size_t i = 0; i < nParams; i++) {
                delete [] gradient[i];
            }
            delete [] gradient;
        }

    }

}

