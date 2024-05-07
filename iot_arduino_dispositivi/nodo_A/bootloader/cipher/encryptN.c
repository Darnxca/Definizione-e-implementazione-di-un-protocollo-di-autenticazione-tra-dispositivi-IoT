#include <stdint.h>
#include "cipher.h"
#include "constants.h"
#include "primitives.h"
#include <string.h>

/*
void AddPadding(uint8_t *block, size_t *blockLength);

void EncryptN(uint8_t *message, size_t messageLength, uint8_t *roundKeys)
{
    // Calculate the number of blocks
    size_t numBlocks = (messageLength + BLOCK_SIZE - 1) / BLOCK_SIZE;

    for (size_t blockIndex = 0; blockIndex < numBlocks; ++blockIndex)
    {
        uint8_t *block = message + blockIndex * BLOCK_SIZE;

        // Pad the last block if needed
        if (blockIndex == numBlocks - 1)
        {
            size_t remainingBytes = messageLength % BLOCK_SIZE;
            if (remainingBytes != 0)
            {
                // Add padding
                AddPadding(block, &remainingBytes);
            }
        }

        // Perform encryption
        Encrypt(block, roundKeys);
    }
}

void AddPadding(uint8_t *block, size_t *blockLength)
{
    // Calculate the number of padding bytes
    uint8_t numPaddingBytes = BLOCK_SIZE - *blockLength;

    // Add padding
    for (size_t i = *blockLength; i < BLOCK_SIZE; ++i)
    {
        block[i] = numPaddingBytes;
    }

    // Update the block length after adding padding
    *blockLength = BLOCK_SIZE;
}*/

//Codice per cifratura con lunghezza input arbitrario
void EncryptN(uint8_t *message, size_t messageLength, uint8_t *roundKeys)
{
    // Calculate the number of blocks
    size_t numBlocks = (messageLength + BLOCK_SIZE - 1) / BLOCK_SIZE;

    for (size_t blockIndex = 0; blockIndex < numBlocks; ++blockIndex)
    {
        uint8_t *block = message + blockIndex * BLOCK_SIZE;

        // Pad the last block if needed
        if (blockIndex == numBlocks - 1)
        {
            size_t remainingBytes = messageLength % BLOCK_SIZE;
            if (remainingBytes != 0)
            {
                // Add padding (e.g., with zeros)
                memset(block + remainingBytes, 0, BLOCK_SIZE - remainingBytes);
            }
        }

       Encrypt(block, roundKeys);
    }
}
