#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <curl/curl.h> // Includi la libreria cURL

#include "my_utility.h"
#include "pc_utils.h"
#include "puf.h"
#include "my_curl_request.h"

unsigned const int __attribute__((section(".version"))) optiboot_version = 2049U;
uint8_t __attribute__((section(".noinit"))) bss_puf[PUF_SIZE];

int main(void) 
{
    srand(time(NULL));
    
    // id dispositivi
    uint8_t id_dispositivoA[ID_SIZE] = {0U};
    uint8_t id_dispositivoB[ID_SIZE] = {0U};

    // helper_data    
    uint8_t helper_dataA[PUF_SIZE] = {0U};
    uint8_t helper_dataB[PUF_SIZE] = {0U};

    // Ninit
    uint8_t nInitA[NONCE_SIZE] = {0U};
    uint8_t nInitB[NONCE_SIZE] = {0U};

    load_buffer_from_file(DISPOSITIVO_1_PATH"my_id.puf", id_dispositivoA, ID_SIZE);
    load_buffer_from_file(DISPOSITIVO_2_PATH"my_id.puf", id_dispositivoB,ID_SIZE);
    load_buffer_from_file(DISPOSITIVO_1_PATH"my_helper_data.puf", helper_dataA, PUF_SIZE);
    load_buffer_from_file(DISPOSITIVO_2_PATH"my_helper_data.puf", helper_dataB,PUF_SIZE);
    load_buffer_from_file(DISPOSITIVO_1_PATH"initA.puf", nInitA, NONCE_SIZE);
    load_buffer_from_file(DISPOSITIVO_2_PATH"initB.puf", nInitB,NONCE_SIZE);

    
    char wallet_address_dispositivo[CONTRACT_SIZE];
    read_string_from_file(DISPOSITIVO_2_PATH"contract_address.txt", wallet_address_dispositivo);
    
    avviaAutenticazione(wallet_address_dispositivo, id_dispositivoA, helper_dataA, nInitA, id_dispositivoB, helper_dataB, nInitB);


}