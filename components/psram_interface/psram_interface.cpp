#include <stdio.h>
#include "psram_interface.hpp"


PSRAMDataStorage::PSRAMDataStorage(size_t chunkSize) 
    : chunkSize(chunkSize), totalSize(0), data(nullptr) {}

PSRAMDataStorage::~PSRAMDataStorage() {
    if (data) {
        free(data);
    }
}

// Add a chunk of data
void PSRAMDataStorage::addChunk(const void* chunk, size_t chunkSize) {
    // Reallocate memory to accommodate the new chunk
    size_t newSize = totalSize + chunkSize;
    void* newData = realloc(data, newSize);
    if (newData == nullptr) {
        // Handle memory allocation failure
        printf("Failed to allocate memory\n");
        return;
    }

    data = newData;

    // Copy the chunk into the newly allocated memory
    memcpy(static_cast<char*>(data) + totalSize, chunk, chunkSize);
    totalSize += chunkSize;
}

// Read data from a specific position
void PSRAMDataStorage::readData(size_t position, void* buffer, size_t bufferSize) const {
    if (position + bufferSize > totalSize) {
        // Handle out of bounds access
        printf("Read out of bounds\n");
        return;
    }

    memcpy(buffer, static_cast<const char*>(data) + position, bufferSize);
}

// Print total size
void PSRAMDataStorage::printTotalSize() const {
    printf("Total size: %zu bytes\n", totalSize);
}

size_t PSRAMDataStorage::get_chunk_size(){
    return totalSize;
}