#include "magic_bitboards.h"
#include "dumb7flooding.h"

#undef NDEBUG
#include <cassert>



Magics::~Magics() {
    delete [] rookComb;
    delete [] bishopComb;
}



Magics::Magics(uint64_t *rookMoves, uint64_t *bishopMoves) : rookMoves(rookMoves), bishopMoves(bishopMoves) {

    // Initialize magic numbers
    uint64_t magicR[64];
    uint64_t magicB[64];
    optimalMagicRook(magicR);
    optimalMagicBishop(magicB);

    for (uint8_t i = 0; i < 64; i++) {

        uint64_t mrMasked = 0;
        if (i == 0) {
            mrMasked = 72057594037927935U & 9187201950435737471U;
        }
        else if (i == 7) {
            mrMasked = 72057594037927935U & 18374403900871474942U;
        }
        else if (i == 63) {
            mrMasked = 18446744073709551360U & 18374403900871474942U;
        }
        else if (i == 56) {
            mrMasked = 18446744073709551360U & 9187201950435737471U;
        }
        else if (i < 7 && i > 0) {
            mrMasked = 72057594037927935U & 18374403900871474942U & 9187201950435737471U;
        }
        else if (i < 63 && i > 56) {
            mrMasked = 18446744073709551360U & 18374403900871474942U & 9187201950435737471U;
        }
        else if (i == 8 || i == 16 || i == 24 || i == 32 || i == 40 || i == 48) {
            mrMasked = 72057594037927935U & 9187201950435737471U & 18446744073709551360U;
        }
        else if (i == 15 || i == 23 || i == 31 || i == 39 || i == 47 || i == 55) {
            mrMasked = 72057594037927935U & 18374403900871474942U & 18446744073709551360U;
        }
        else {
            mrMasked = 35604928818740736U;
        }

        mrMasked &= rookMoves[i];
        uint8_t count = count_population(mrMasked);
        uint8_t mrShift = 64 - count;
        uint64_t mrMagic = magicR[i];

        const MagicPro mp = MagicPro(rookMoves[i], mrShift, mrMagic, mrMasked);
        attacksR[i] = mp;

    }

    for (uint8_t i = 0; i < 64; i++) {
        uint64_t mrMasked = 35604928818740736U & bishopMoves[i];
        uint8_t count = count_population(mrMasked);
        uint8_t mrShift = 64 - count;
        uint64_t mrMagic = magicB[i];

        const MagicPro mp = MagicPro(bishopMoves[i], mrShift, mrMagic, mrMasked);
        attacksB[i] = mp;
    }

    rookComb = new uint64_t[262144];
    bishopComb = new uint64_t[32768];

    for (int i = 0; i < 262144; i++) {
        rookComb[i] = 0;
    }

    for (int i = 0; i < 32768; i++) {
        bishopComb[i] = 0;
    }

    // Assert to make sure everything works before going any further
    for (uint8_t i = 0; i < 64; i++) {
        assert(InitBlocksRook(rookMoves[i], i, magicR[i]));
        assert(InitBlocksBishop(bishopMoves[i], i, magicB[i]));
    }
}



// Magic bishop numbers for each square. numbers generated with Generate_Magic_Rooks function
void Magics::optimalMagicRook(uint64_t *magicR) {
    magicR[0] = 36029348655939588ULL;
    magicR[1] = 1170971087756869632ULL;
    magicR[2] = 2954370427062910992ULL;
    magicR[3] = 792651127156903192ULL;
    magicR[4] = 144134979419643920ULL;
    magicR[5] = 144133922724252680ULL;
    magicR[6] = 1224981864905722624ULL;
    magicR[7] = 36029896532706432ULL;
    magicR[8] = 1176002594440085568ULL;
    magicR[9] = 1176002594440085568ULL;
    magicR[10] = 720857489461223424ULL;
    magicR[11] = 563233431752968ULL;
    magicR[12] = 612771093152793856ULL;
    magicR[13] = 2306406440271102464ULL;
    magicR[14] = 4901886943039603200ULL;
    magicR[15] = 81627760442819714ULL;
    magicR[16] = 73219228345303168ULL;
    magicR[17] = 13515472343416832ULL;
    magicR[18] = 53876606698049ULL;
    magicR[19] = 1153204079900499976ULL;
    magicR[20] = 363121411788701700ULL;
    magicR[21] = 144397762973533184ULL;
    magicR[22] = 1225053866778034440ULL;
    magicR[23] = 72567772480644ULL;
    magicR[24] = 36029072970620928ULL;
    magicR[25] = 153123625355968832ULL;
    magicR[26] = 2380187600323363073ULL;
    magicR[27] = 597923640185473536ULL;
    magicR[28] = 2306406036477059204ULL;
    magicR[29] = 1198520459426533540ULL;
    magicR[30] = 3476787725604028944ULL;
    magicR[31] = 108086949402902593ULL;
    magicR[32] = 5944821895705657472ULL;
    magicR[33] = 2287015328485378ULL;
    magicR[34] = 585487743337906560ULL;
    magicR[35] = 4922434534500207376ULL;
    magicR[36] = 5919981847684841536ULL;
    magicR[37] = 721138907520963200ULL;
    magicR[38] = 9016064671220257ULL;
    magicR[39] = 551534199060ULL;
    magicR[40] = 3062447884323602432ULL;
    magicR[41] = 3756037342331682816ULL;
    magicR[42] = 563260004892704ULL;
    magicR[43] = 17703859519520ULL;
    magicR[44] = 1729426237660659748ULL;
    magicR[45] = 72620578623848484ULL;
    magicR[46] = 4521510043910161ULL;
    magicR[47] = 2765963264001ULL;
    magicR[48] = 54044844816867456ULL;
    magicR[49] = 4521466693419072ULL;
    magicR[50] = 1166432853781647488ULL;
    magicR[51] = 9043483311472896ULL;
    magicR[52] = 10141899550622976ULL;
    magicR[53] = 563018807902720ULL;
    magicR[54] = 26668627411968ULL;
    magicR[55] = 73192582104023552ULL;
    magicR[56] = 145258839090921537ULL;
    magicR[57] = 1225330946669088801ULL;
    magicR[58] = 576814967383134465ULL;
    magicR[59] = 4182999766471278661ULL;
    magicR[60] = 72621231253753922ULL;
    magicR[61] = 154811290358403106ULL;
    magicR[62] = 1765416045839388676ULL;
    magicR[63] = 6341349750872344705ULL;

}



// Magic bishop numbers for each square. numbers generated with Generate_Magic_Bishops function
void Magics::optimalMagicBishop(uint64_t *magicB) {
    magicB[0] = 18058413343254592ULL;
    magicB[1] = 580969858422935552ULL;
    magicB[2] = 4774382545141760288ULL;
    magicB[3] = 866954202594672640ULL;
    magicB[4] = 72622812002529568ULL;
    magicB[5] = 2308377521225204227ULL;
    magicB[6] = 2882603482052296708ULL;
    magicB[7] = 576814796190524416ULL;
    magicB[8] = 145245503276450816ULL;
    magicB[9] = 571758948256256ULL;
    magicB[10] = 1225067076759339146ULL;
    magicB[11] = 9912801435648ULL;
    magicB[12] = 4522361118261760ULL;
    magicB[13] = 11559171662021664ULL;
    magicB[14] = 11276874991802378ULL;
    magicB[15] = 10152891463766192ULL;
    magicB[16] = 4773815640718049410ULL;
    magicB[17] = 585469119940399234ULL;
    magicB[18] = 362557448223204105ULL;
    magicB[19] = 145245649256972544ULL;
    magicB[20] = 2310927168246382594ULL;
    magicB[21] = 281483573134336ULL;
    magicB[22] = 288318345689696288ULL;
    magicB[23] = 723109766066734344ULL;
    magicB[24] = 290517919356420480ULL;
    magicB[25] = 2260733424304256ULL;
    magicB[26] = 290486575219875872ULL;
    magicB[27] = 290284021825552ULL;
    magicB[28] = 1153765998273249280ULL;
    magicB[29] = 864766994766891040ULL;
    magicB[30] = 2535473852067841ULL;
    magicB[31] = 75435573485045889ULL;
    magicB[32] = 288828579433549888ULL;
    magicB[33] = 77726689016418308ULL;
    magicB[34] = 4612816326066178304ULL;
    magicB[35] = 144431881785114961ULL;
    magicB[36] = 162129724562604160ULL;
    magicB[37] = 4684033987151528448ULL;
    magicB[38] = 4516794874267904ULL;
    magicB[39] = 148918134047375616ULL;
    magicB[40] = 4612829691747764224ULL;
    magicB[41] = 1297321501090906884ULL;
    magicB[42] = 581599883627073537ULL;
    magicB[43] = 72057731879536904ULL;
    magicB[44] = 387045542527744ULL;
    magicB[45] = 9361250622383120ULL;
    magicB[46] = 4540991679889600ULL;
    magicB[47] = 4612852342570877188ULL;
    magicB[48] = 866423857610760ULL;
    magicB[49] = 866423857610760ULL;
    magicB[50] = 576464331153080960ULL;
    magicB[51] = 290482193691058176ULL;
    magicB[52] = 144115806584836096ULL;
    magicB[53] = 9064391442041408ULL;
    magicB[54] = 37159936853286912ULL;
    magicB[55] = 37159936853286912ULL;
    magicB[56] = 4630369471107178512ULL;
    magicB[57] = 612489832815665152ULL;
    magicB[58] = 9896695701760ULL;
    magicB[59] = 4503599670625280ULL;
    magicB[60] = 38284995181281792ULL;
    magicB[61] = 3260606405228512289ULL;
    magicB[62] = 18084777051894536ULL;
    magicB[63] = 1733890258883450368ULL;
}



// Get the bitboard for bishop blockers
uint64_t Magics::bishopAttacksMask(uint64_t occupations, int index){
    occupations &= attacksB[index].mask;
    occupations = ((attacksB[index].magic * occupations) >> attacksB[index].shift);
    return bishopComb[index * 512 + occupations];
}



// Get the bitboard for rook blockers
uint64_t Magics::rookAttacksMask(uint64_t occupations, int index) {
    occupations &= attacksR[index].mask;
    occupations = ((attacksR[index].magic * occupations) >> attacksR[index].shift);
    return rookComb[index * 4096 + occupations];
}



// Get the bitboard for queen blockers
uint64_t Magics::queenAttacksMask(uint64_t occupations, int index) {
    return bishopAttacksMask(occupations, index) | rookAttacksMask(occupations, index);
}



// X-ray attacks for bishops
uint64_t Magics::xrayAttackRook(uint64_t occ, uint64_t blockers, int index) {
    uint64_t bb = rookAttacksMask(occ, index);
    return bb ^ rookAttacksMask(occ ^ (bb & blockers), index);
}



// X-ray attacks for rooks
uint64_t Magics::xrayAttackBishop(uint64_t occ, uint64_t blockers, int index) {
    uint64_t bb = bishopAttacksMask(occ, index);
    return bb ^ bishopAttacksMask(occ ^ (bb & blockers), index);
}



// Bit combinations of set bits.
// Adapted from https://stackoverflow.com/questions/30680559/how-to-find-magic-bitboards
uint64_t Magics::bitCombinations(uint64_t index, uint64_t bitboard) {
    uint8_t bindex = 0;
    uint64_t board = bitboard;

    for (uint8_t i = 0; i < 64; i++){
        uint64_t bitToFlip = 1ULL << i;

        if ((bitToFlip & bitboard) != 0) {
            if ((index & (1ULL << bindex)) == 0) {
                board &= ~bitToFlip;
            }
            bindex++;
        }
    }

    return board;
}



// Initializes the blocking bitboards for rooks
bool Magics::InitBlocksRook(uint64_t bitboard, uint64_t index, uint64_t magic) {

    uint64_t bitboardMasked = bitboard & attacksR[index].mask;
    uint64_t indexP = 1ULL << index;
    uint8_t countMasked = count_population(bitboardMasked);

    for (uint64_t i = 0; i < 1ULL << countMasked; i++) {
        uint64_t r = bitCombinations(i, bitboardMasked);

        uint64_t res = (dumb7FloodingN(indexP, r) | dumb7FloodingE(indexP, r) | dumb7FloodingS(indexP, r) | dumb7FloodingW(indexP, r)) & (indexP ^ 18446744073709551615U);
        uint64_t magicI = ((r * magic) >> attacksR[index].shift);

        if (rookComb[index * 4096 + magicI] != 0) {
            return false;
        }

        rookComb[index * 4096 + magicI] = res;
    }

    return true;
}



// Initializes the blocking bitboards for bishops
bool Magics::InitBlocksBishop(uint64_t bitboard, uint8_t index, uint64_t magic) {

    uint64_t bitboardMasked = bitboard & 35604928818740736U;
    uint64_t indexP = 1ULL << index;
    uint8_t countMasked = count_population(bitboardMasked);

    for (uint64_t i = 0; i < 1ULL << countMasked; i++) {
        uint64_t r = bitCombinations(i, bitboardMasked);

        uint64_t res = (dumb7FloodingNE(indexP, r) | dumb7FloodingSE(indexP, r) | dumb7FloodingSW(indexP, r) | dumb7FloodingNW(indexP, r)) & (indexP ^ 18446744073709551615U);
        uint64_t magicI = ((r * magic) >> attacksB[index].shift);

        if (bishopComb[index * 512 + magicI]  != 0) {
            return false;
        }

        bishopComb[index * 512 + magicI] = res;
    }

    return true;
}



// Used to generate numbers in the function optimalMagicRook
void Magics::Generate_Magic_Rooks() {
    uint64_t x = (rand() & 0xffff) | ((rand() & 0xffff) << 16) | (((uint64_t)rand() & 0xffff) << 32) | (((uint64_t)rand() & 0xffff) << 48);

    for (uint8_t i = 0; i < 64; i++) {

        while(!InitBlocksRook(rookMoves[i], i, x)) {
            uint64_t x1 = (rand() & 0xffff) | ((rand() & 0xffff) << 16) | (((uint64_t)rand() & 0xffff) << 32) | (((uint64_t)rand() & 0xffff) << 48);
            uint64_t x2 = (rand() & 0xffff) | ((rand() & 0xffff) << 16) | (((uint64_t)rand() & 0xffff) << 32) | (((uint64_t)rand() & 0xffff) << 48);
            uint64_t x3 = (rand() & 0xffff) | ((rand() & 0xffff) << 16) | (((uint64_t)rand() & 0xffff) << 32) | (((uint64_t)rand() & 0xffff) << 48);

            // AND 3 numbers together to get a number with less bits
            x = x1 & x2 & x3;
        }
        std::cout << "  magicR[" << unsigned(i) << "] = " << x << "ULL;" << std::endl;
    }
}



// Used to generate numbers in the function optimalMagicBishop
void Magics::Generate_Magic_Bishops() {
    uint64_t x = (rand() & 0xffff) | ((rand() & 0xffff) << 16) | (((uint64_t)rand() & 0xffff) << 32) | (((uint64_t)rand() & 0xffff) << 48);

    for (uint8_t i = 0; i < 64; i++) {
        while(!InitBlocksBishop(bishopMoves[i], i, x)) {
            uint64_t x1 = (rand() & 0xffff) | ((rand() & 0xffff) << 16) | (((uint64_t)rand() & 0xffff) << 32) | (((uint64_t)rand() & 0xffff) << 48);
            uint64_t x2 = (rand() & 0xffff) | ((rand() & 0xffff) << 16) | (((uint64_t)rand() & 0xffff) << 32) | (((uint64_t)rand() & 0xffff) << 48);
            uint64_t x3 = (rand() & 0xffff) | ((rand() & 0xffff) << 16) | (((uint64_t)rand() & 0xffff) << 32) | (((uint64_t)rand() & 0xffff) << 48);

            // AND 3 numbers together to get a number with less bits
            x = x1 & x2 & x3;
        }
        std::cout << "  magicB[" << unsigned(i) << "] = " << x << "ULL;" << std::endl;
    }
}
