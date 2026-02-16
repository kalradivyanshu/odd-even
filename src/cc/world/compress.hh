#include <zlib.h>
#include <vector>
#include <stdexcept>

#pragma once

namespace world {
    std::vector<uint8_t> compress(const std::vector<uint8_t>& data) {
        uLongf compressed_size = compressBound(data.size());
        std::vector<uint8_t> compressed(compressed_size);
        
        int result = compress2(compressed.data(), &compressed_size,
                            data.data(), data.size(), Z_BEST_COMPRESSION);
        
        if (result != Z_OK) {
            throw std::runtime_error("Compression failed");
        }
        
        compressed.resize(compressed_size);
        return compressed;
    }

    std::vector<uint8_t> decompress(const std::vector<uint8_t>& compressed, 
                                    size_t original_size) {
        std::vector<uint8_t> decompressed(original_size);
        uLongf decompressed_size = original_size;
        
        int result = uncompress(decompressed.data(), &decompressed_size,
                            compressed.data(), compressed.size());
        
        if (result != Z_OK) {
            throw std::runtime_error("Decompression failed");
        }
        
        return decompressed;
    }
}