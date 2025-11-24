#ifndef RANDOM_KEY_GENERATOR_H
#define RANDOM_KEY_GENERATOR_H

#include "secp256k1.h"
#include <cstdint>

class RandomKeyGenerator {
public:
    // Generate a random 256-bit key for pre-2012 wallets
    static secp256k1::uint256 generateRandom256();
    
    // Generate a unique random starting point for a specific GPU
    static secp256k1::uint256 generateRandomStart(uint32_t gpuId, uint32_t totalGPUs);
};

#endif // RANDOM_KEY_GENERATOR_H

