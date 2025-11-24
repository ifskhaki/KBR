#include "RandomKeyGenerator.h"
#include "secp256k1.h"
#include "CryptoUtil.h"
#include <cstring>

secp256k1::uint256 RandomKeyGenerator::generateRandom256() {
    crypto::Rng rng;
    unsigned char buf[32];
    rng.get(buf, 32);
    
    // Convert to uint256 (BigEndian)
    secp256k1::uint256 key(buf, 32, secp256k1::uint256::BigEndian);
    
    // Ensure valid range (1 to N-1)
    if (key.isZero() || key.cmp(secp256k1::N) >= 0) {
        key = secp256k1::uint256(1);
    }
    
    return key;
}

secp256k1::uint256 RandomKeyGenerator::generateRandomStart(uint32_t gpuId, uint32_t totalGPUs) {
    crypto::Rng rng;
    unsigned char buf[32];
    rng.get(buf, 32);
    
    // Add GPU-specific entropy to ensure different starting points
    uint64_t gpuEntropy = static_cast<uint64_t>(gpuId) << 32;
    memcpy(&buf[24], &gpuEntropy, sizeof(uint64_t));
    
    // Add additional entropy based on total GPUs
    uint32_t totalEntropy = totalGPUs;
    memcpy(&buf[20], &totalEntropy, sizeof(uint32_t));
    
    secp256k1::uint256 key(buf, 32, secp256k1::uint256::BigEndian);
    
    // Ensure valid range
    if (key.isZero() || key.cmp(secp256k1::N) >= 0) {
        // Fallback: use GPU ID as seed
        key = secp256k1::uint256(static_cast<uint64_t>(gpuId + 1) * 0x1234567890ABCDEFULL);
        if (key.isZero() || key.cmp(secp256k1::N) >= 0) {
            key = secp256k1::uint256(1);
        }
    }
    
    return key;
}

