#ifndef PSRAM_INTERFACE_HPP
#define PSRAM_INTERFACE_HPP


#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"

class PSRAMDataStorage {
public:
    PSRAMDataStorage(size_t chunkSize) ;

    ~PSRAMDataStorage() ;

    // Add a chunk of data
    void addChunk(const void* chunk, size_t chunkSize) ;

    // Read data from a specific position
    void readData(size_t position, void* buffer, size_t bufferSize) const ;

    // Print total size
    void printTotalSize() const ;

    size_t get_chunk_size();
    
// private:
    size_t chunkSize;   // Size of each chunk
    size_t totalSize;   // Total size of all stored data
    void* data;         // Pointer to the allocated PSRAM
};

#endif 