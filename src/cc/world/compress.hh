#include <zlib.h>
#include <vector>
#include <stdexcept>

#pragma once

namespace world {
    uLongf compress(const std::vector<uint8_t>& data, std::vector<uint8_t>& compressed) {
        uLongf compressed_size = compressBound(data.size());

        int result = compress2(compressed.data(), &compressed_size,
                            data.data(), data.size(), Z_BEST_COMPRESSION);
        
        if (result != Z_OK) {
            throw std::runtime_error("Compression failed");
        }
        
        return compressed_size;
    }

    void decompress(const std::vector<uint8_t>& compressed, std::vector<uint8_t>& decompressed, size_t compressed_size) {
        uLongf decompressed_size = decompressed.size();
        
        int result = uncompress(decompressed.data(), &decompressed_size,
                            compressed.data(), compressed_size);
        
        if (result != Z_OK) {
            throw std::runtime_error("Decompression failed");
        }
    }
}