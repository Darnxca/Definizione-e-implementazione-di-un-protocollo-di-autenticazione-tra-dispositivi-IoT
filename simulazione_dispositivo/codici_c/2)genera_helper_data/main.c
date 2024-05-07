#include "reedmuller.h"
#include "dm_hash.h"
#include "pc_utils.h"
#include "puf_parameters.h"
#include "my_utility.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <PUF_response>\n", argv[0]);
        return 1;
    }

    uint8_t puf_response[PUF_SIZE] = {0U};
    uint8_t helper_data[PUF_SIZE] = {0U};

    uint8_t cs[CS_LEN_BYTE] = {0U};
    int16_t codeword[CW_LEN_BIT] = {0};
    uint8_t codeword_bytes[PUF_SIZE] = {0U};

    uint16_t count;

    char *puf_response_str = argv[1];

    // Converte le stringhe esadecimali in array di byte
    if (hex_string_to_bytes(puf_response_str, puf_response, PUF_SIZE) != 0) {
        printf("Errore nella conversione della risposta PUF in byte.\n");
        return 1;
    }

    // creazione del secret symbol
    for(uint32_t i = 0U; i < CS_LEN_BYTE; i++){
        cs[i] = (uint8_t) i + 0xA0;
    }
    
    cs[CS_LEN_BYTE-1] &= (uint8_t) (0xff << ((8*CS_LEN_BYTE) % CS_LEN_BIT));
    
    for(uint32_t i = 0U; i < CS_LEN_BYTE; i++){
        // Codifica il secret symbol
        encode(&cs[i], &codeword[i * 128]);
    }

    // converti la codeword in byte (352)
    count = 0;
    for (uint16_t i = 0U; i < CW_LEN_BIT; i += 8U){
        codeword_bytes[count] = cw_to_byte(&codeword[i]);
        count++;
    }

    // Calcola Helper Data = PUF XOR Codeword
    for(uint16_t i = 0U; i < PUF_SIZE; i++) {
        helper_data[i] = puf_response[i] ^ codeword_bytes[i];
    }
   
    print_buffer_as_json("Helper_Data", helper_data, PUF_SIZE,
                         NULL);

    return 0;
}