#include "magic_bitboards.h"

#undef NDEBUG
#include <cassert>


namespace MAGIC_BITBOARDS{
    
    MagicPro attacksR[64];
    MagicPro attacksB[64];

    uint64_t rookComb[64 * 4096] = {0};
    uint64_t bishopComb[64 * 512] = {0};



    // Get all bishop moves in an empty board given an index
    // Used for generating magic Bitboards
    uint64_t getBishopMoves(int index) {
        uint64_t index_bb = 1ULL << index;
        uint64_t res = dumb7FloodingNE(index_bb, 0);
        res |= dumb7FloodingSE(index_bb, 0);
        res |= dumb7FloodingSW(index_bb, 0);
        res |= dumb7FloodingNW(index_bb, 0);
        return res;
    }



    // Get all rook moves in an empty board given an index
    // Used for generating magic Bitboards
    uint64_t getRookMoves(int index) {
        uint64_t index_bb = 1ULL << index;
        uint64_t res = dumb7FloodingN(index_bb, 0);
        res |= dumb7FloodingE(index_bb, 0);
        res |= dumb7FloodingS(index_bb, 0);
        res |= dumb7FloodingW(index_bb, 0);
        return res;
    }



    void InitializeRookMagicsInfo() {
        uint64_t magicR[64] = {
            36029348655939588ULL, 1170971087756869632ULL, 2954370427062910992ULL, 792651127156903192ULL,
            144134979419643920ULL, 144133922724252680ULL, 1224981864905722624ULL, 36029896532706432ULL,
            1176002594440085568ULL, 1176002594440085568ULL, 720857489461223424ULL, 563233431752968ULL,
            612771093152793856ULL, 2306406440271102464ULL, 4901886943039603200ULL, 81627760442819714ULL,
            73219228345303168ULL, 13515472343416832ULL, 53876606698049ULL, 1153204079900499976ULL,
            363121411788701700ULL, 144397762973533184ULL, 1225053866778034440ULL, 72567772480644ULL,
            36029072970620928ULL, 153123625355968832ULL, 2380187600323363073ULL, 597923640185473536ULL,
            2306406036477059204ULL, 1198520459426533540ULL, 3476787725604028944ULL, 108086949402902593ULL,
            5944821895705657472ULL, 2287015328485378ULL, 585487743337906560ULL, 4922434534500207376ULL,
            5919981847684841536ULL, 721138907520963200ULL, 9016064671220257ULL, 551534199060ULL,
            3062447884323602432ULL, 3756037342331682816ULL, 563260004892704ULL, 17703859519520ULL,
            1729426237660659748ULL, 72620578623848484ULL, 4521510043910161ULL, 2765963264001ULL,
            54044844816867456ULL, 4521466693419072ULL, 1166432853781647488ULL, 9043483311472896ULL,
            10141899550622976ULL, 563018807902720ULL, 26668627411968ULL, 73192582104023552ULL,
            145258839090921537ULL, 1225330946669088801ULL, 576814967383134465ULL, 4182999766471278661ULL,
            72621231253753922ULL, 154811290358403106ULL, 1765416045839388676ULL, 6341349750872344705ULL
        };

        for (uint8_t i = 0; i < 64; i++) {
            uint64_t all_bits_set = 18446744073709551615U;
            uint64_t rookMask = all_bits_set;

            bool indexIsFirstRank = (i / 8) == 0;
            bool indexIsEighthRank = (i / 8) == 7;
            bool indexIsFirstFile = (i % 8) == 0;
            bool indexIsLastFile = (i % 8) == 7;

            if (indexIsFirstRank || ((indexIsFirstFile || indexIsLastFile) && !indexIsEighthRank)) {
                rookMask &= ~rowMask[56];
            }

            if (indexIsFirstFile || ((indexIsFirstRank || indexIsEighthRank) && !indexIsLastFile)) {
                rookMask &= ~columnMask[7];
            }

            if (indexIsEighthRank || ((indexIsFirstFile || indexIsLastFile) && !indexIsFirstRank)) {
                rookMask &= ~rowMask[0];
            }

            if (indexIsLastFile || ((indexIsFirstRank || indexIsEighthRank) && !indexIsFirstFile)) {
                rookMask &= ~columnMask[0];
            }

            if (rookMask == all_bits_set) {
                rookMask = ~rowMask[56] & ~columnMask[7] & ~rowMask[0] & ~columnMask[0];
            }

            attacksR[i].magic = magicR[i];
            attacksR[i].mask = rookMask & getRookMoves(i);
            attacksR[i].shift = 64 - count_population(attacksR[i].mask);
        }
    }



    void InitializeBishopMagicsInfo() {
        uint64_t magicB[64] = {
            18058413343254592ULL, 580969858422935552ULL, 4774382545141760288ULL, 866954202594672640ULL,
            72622812002529568ULL, 2308377521225204227ULL, 2882603482052296708ULL, 576814796190524416ULL,
            145245503276450816ULL, 571758948256256ULL, 1225067076759339146ULL, 9912801435648ULL,
            4522361118261760ULL, 11559171662021664ULL, 11276874991802378ULL, 10152891463766192ULL,
            4773815640718049410ULL, 585469119940399234ULL, 362557448223204105ULL, 145245649256972544ULL,
            2310927168246382594ULL, 281483573134336ULL, 288318345689696288ULL, 723109766066734344ULL,
            290517919356420480ULL, 2260733424304256ULL, 290486575219875872ULL, 290284021825552ULL,
            1153765998273249280ULL, 864766994766891040ULL, 2535473852067841ULL, 75435573485045889ULL,
            288828579433549888ULL, 77726689016418308ULL, 4612816326066178304ULL, 144431881785114961ULL,
            162129724562604160ULL, 4684033987151528448ULL, 4516794874267904ULL, 148918134047375616ULL,
            4612829691747764224ULL, 1297321501090906884ULL, 581599883627073537ULL, 72057731879536904ULL,
            387045542527744ULL, 9361250622383120ULL, 4540991679889600ULL, 4612852342570877188ULL,
            866423857610760ULL, 866423857610760ULL, 576464331153080960ULL, 290482193691058176ULL,
            144115806584836096ULL, 9064391442041408ULL, 37159936853286912ULL, 37159936853286912ULL,
            4630369471107178512ULL, 612489832815665152ULL, 9896695701760ULL, 4503599670625280ULL,
            38284995181281792ULL, 3260606405228512289ULL, 18084777051894536ULL, 1733890258883450368ULL
        };

        for (uint8_t i = 0; i < 64; i++) {
            attacksB[i].magic = magicB[i];
            attacksB[i].mask = (~rowMask[56] & ~columnMask[7] & ~rowMask[0] & ~columnMask[0]) & getBishopMoves(i);
            attacksB[i].shift = 64 - count_population(attacksB[i].mask);
        }
    }



    // Bit combinations of set bits.
    // Adapted from https://stackoverflow.com/questions/30680559/how-to-find-magic-bitboards
    uint64_t bitCombinations(uint64_t index, uint64_t bitboard) {
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
    bool InitBlocksRook(uint64_t index, uint64_t magic) {
        uint64_t bitboardMasked = getRookMoves(index) & attacksR[index].mask;
        uint64_t indexP = 1ULL << index;
        uint8_t countMasked = count_population(bitboardMasked);

        for (uint64_t i = 0; i < 1ULL << countMasked; i++) {
            uint64_t r = bitCombinations(i, bitboardMasked);

            uint64_t res = (dumb7FloodingN(indexP, r) | dumb7FloodingE(indexP, r) | dumb7FloodingS(indexP, r) | dumb7FloodingW(indexP, r)) & ~indexP;
            uint64_t magicI = ((r * magic) >> attacksR[index].shift);

            if (rookComb[index * 4096 + magicI] != 0) {
                for (uint64_t k = 0; k < i; k++) {
                    uint64_t r2 = bitCombinations(k, bitboardMasked);
                    uint64_t magicI2 = ((r2 * magic) >> attacksR[index].shift);
                    rookComb[index * 4096 + magicI2] = 0;
                }
                return false;
            }

            rookComb[index * 4096 + magicI] = res;
        }

        return true;
    }



    // Initializes the blocking bitboards for bishops
    bool InitBlocksBishop(uint8_t index, uint64_t magic) {
        uint64_t bitboardMasked = getBishopMoves(index) & 35604928818740736U;
        uint64_t indexP = 1ULL << index;
        uint8_t countMasked = count_population(bitboardMasked);

        for (uint64_t i = 0; i < 1ULL << countMasked; i++) {
            uint64_t r = bitCombinations(i, bitboardMasked);

            uint64_t res = (dumb7FloodingNE(indexP, r) | dumb7FloodingSE(indexP, r) | dumb7FloodingSW(indexP, r) | dumb7FloodingNW(indexP, r)) & ~indexP;
            uint64_t magicI = ((r * magic) >> attacksB[index].shift);

            if (bishopComb[index * 512 + magicI]  != 0) {
                for (uint64_t k = 0; k < i; k++) {
                    uint64_t r2 = bitCombinations(k, bitboardMasked);
                    uint64_t magicI2 = ((r2 * magic) >> attacksB[index].shift);
                    bishopComb[index * 512 + magicI2] = 0;
                }
                return false;
            }

            bishopComb[index * 512 + magicI] = res;
        }

        return true;
    }



    // Used to generate numbers in the function optimalMagicRook
    void Generate_Magic_Rooks() {
        uint64_t x = (rand() & 0xffff) | ((rand() & 0xffff) << 16) | (((uint64_t)rand() & 0xffff) << 32) | (((uint64_t)rand() & 0xffff) << 48);

        for (uint8_t i = 0; i < 64; i++) {

            while(!InitBlocksRook(i, x)) {
                uint64_t x1 = (rand() & 0xffff) | ((rand() & 0xffff) << 16) | (((uint64_t)rand() & 0xffff) << 32) | (((uint64_t)rand() & 0xffff) << 48);
                uint64_t x2 = (rand() & 0xffff) | ((rand() & 0xffff) << 16) | (((uint64_t)rand() & 0xffff) << 32) | (((uint64_t)rand() & 0xffff) << 48);
                uint64_t x3 = (rand() & 0xffff) | ((rand() & 0xffff) << 16) | (((uint64_t)rand() & 0xffff) << 32) | (((uint64_t)rand() & 0xffff) << 48);

                // AND 3 numbers together to get a number with less bits
                x = x1 & x2 & x3;
            }
            std::cout << "  magicR[" << unsigned(i) << "] = " << x << "ULL;" << std::endl;
        }
        std::cout << std::endl;
    }



    // Used to generate numbers in the function optimalMagicBishop
    void Generate_Magic_Bishops() {
        uint64_t x = (rand() & 0xffff) | ((rand() & 0xffff) << 16) | (((uint64_t)rand() & 0xffff) << 32) | (((uint64_t)rand() & 0xffff) << 48);

        for (uint8_t i = 0; i < 64; i++) {
            while(!InitBlocksBishop(i, x)) {
                uint64_t x1 = (rand() & 0xffff) | ((rand() & 0xffff) << 16) | (((uint64_t)rand() & 0xffff) << 32) | (((uint64_t)rand() & 0xffff) << 48);
                uint64_t x2 = (rand() & 0xffff) | ((rand() & 0xffff) << 16) | (((uint64_t)rand() & 0xffff) << 32) | (((uint64_t)rand() & 0xffff) << 48);
                uint64_t x3 = (rand() & 0xffff) | ((rand() & 0xffff) << 16) | (((uint64_t)rand() & 0xffff) << 32) | (((uint64_t)rand() & 0xffff) << 48);

                // AND 3 numbers together to get a number with less bits
                x = x1 & x2 & x3;
            }
            std::cout << "  magicB[" << unsigned(i) << "] = " << x << "ULL;" << std::endl;
        }
        std::cout << std::endl;
    }



     // X-ray attacks for bishops
    uint64_t xrayAttackRook(uint64_t occ, uint64_t blockers, int index) {
        uint64_t bb = rookAttacksMask(occ, index);
        return bb ^ rookAttacksMask(occ ^ (bb & blockers), index);
    }



    // X-ray attacks for rooks
    uint64_t xrayAttackBishop(uint64_t occ, uint64_t blockers, int index) {
        uint64_t bb = bishopAttacksMask(occ, index);
        return bb ^ bishopAttacksMask(occ ^ (bb & blockers), index);
    }


    // Call this function once to initialize magic bitboards
    void InitMagicBitboards() {
        InitializeRookMagicsInfo();
        InitializeBishopMagicsInfo();

        // Assert to make sure everything works before going any further
        // Functions need to be called for initialization of magics
        for (uint8_t i = 0; i < 64; i++) {
            assert(InitBlocksRook(i, attacksR[i].magic));
            assert(InitBlocksBishop(i, attacksB[i].magic));
        }
    }


}