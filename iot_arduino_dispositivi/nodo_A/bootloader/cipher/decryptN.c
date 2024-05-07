#include <stdint.h>
#include "cipher.h"
#include "constants.h"
#include "primitives.h"
#include <string.h>
/*
void RemovePadding(uint8_t *block, size_t *blockLength);

void DecryptN(uint8_t *message, size_t messageLength, uint8_t *roundKeys)
{
    // Calculate the number of blocks
    size_t numBlocks = (messageLength + BLOCK_SIZE - 1) / BLOCK_SIZE;

    for (size_t blockIndex = 0; blockIndex < numBlocks; ++blockIndex)
    {
        uint8_t *block = message + blockIndex * BLOCK_SIZE;

        // Perform decryption
        Decrypt(block, roundKeys);

        // Remove padding from the last block if needed
        if (blockIndex == numBlocks - 1)
        {
            size_t blockLength = messageLength % BLOCK_SIZE;
            RemovePadding(block, &blockLength);
        }
    }
}

void RemovePadding(uint8_t *block, size_t *blockLength)
{
    // Determine the number of padding bytes
    uint8_t numPaddingBytes = block[BLOCK_SIZE - 1];

    // Validate the padding (e.g., all padding bytes should have the same value)
    for (size_t i = BLOCK_SIZE - numPaddingBytes; i < BLOCK_SIZE - 1; ++i)
    {
        if (block[i] != numPaddingBytes)
        {
            // Invalid padding
            // Handle the error or raise an exception
            return;
        }
    }

    // Update the block length after removing padding
    *blockLength -= numPaddingBytes;
}*/
void DecryptN(uint8_t *message, size_t messageLength, uint8_t *roundKeys)
{
    // Calculate the number of blocks
    size_t numBlocks = (messageLength + BLOCK_SIZE - 1) / BLOCK_SIZE;

    for (size_t blockIndex = 0; blockIndex < numBlocks; ++blockIndex)
    {
        uint8_t *block = message + blockIndex * BLOCK_SIZE;

        // Perform decryption
        Decrypt(block, roundKeys);
    }
}
