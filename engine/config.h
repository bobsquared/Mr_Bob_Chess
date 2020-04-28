// Config header



#include <string>
#include <iostream>
#include <unordered_map>


extern std::string version;
extern std::string engineName;
extern int hashSize;


// Number to algebra
extern const std::string TO_ALG[64];

// Algebra to number
extern std::unordered_map<std::string, uint8_t> TO_NUM;
