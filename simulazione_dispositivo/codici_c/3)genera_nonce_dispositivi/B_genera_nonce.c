#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <curl/curl.h> // Includi la libreria cURL

#include "my_utility.h"
#include "pc_utils.h"
#include "puf.h"

unsigned const int __attribute__((section(".version"))) optiboot_version = 2049U;
uint8_t __attribute__((section(".noinit"))) bss_puf[PUF_SIZE];

int main(void){
    srand(time(NULL));
        
    uint8_t puf_response[PUF_SIZE] = {0U};
    uint8_t id_dispositivo[ID_SIZE] = {0U};
    uint8_t helper_data[PUF_SIZE] = {0U};
    uint8_t kj[FE_KEY_SIZE] = {0U};
    uint8_t ninitB[NONCE_SIZE] = {0U};


    load_buffer_from_file(DISPOSITIVO_2_PATH"puf_response_dispositivo_2.puf", puf_response, PUF_SIZE);
    load_buffer_from_file(DISPOSITIVO_2_PATH"my_id.puf", id_dispositivo, ID_SIZE);
    load_buffer_from_file(DISPOSITIVO_2_PATH"my_helper_data.puf", helper_data, PUF_SIZE);

    // genero la chiave kj
    puf_extract(puf_response, helper_data, kj);
    // genero il nonce di inizializzazione ninitA
    genera(ninitB,sizeof(ninitB));

    // Salvo la chiave e il vettore di inizializzazione
    write_buffer_to_file(DISPOSITIVO_2_PATH"initB.puf", ninitB, NONCE_SIZE);
    write_buffer_to_file(DISPOSITIVO_2_PATH"my_key.puf", kj, FE_KEY_SIZE);

    
}