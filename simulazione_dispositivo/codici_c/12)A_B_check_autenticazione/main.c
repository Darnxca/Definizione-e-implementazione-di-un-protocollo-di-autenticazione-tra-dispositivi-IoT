#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h> // Includi la libreria cURL

#include "my_utility.h"
#include "puf_parameters.h"
#include "pc_utils.h"
#include "my_curl_request.h"

int main() {

    char wallet_address_dispositivoA[CONTRACT_SIZE], wallet_address_dispositivoB[CONTRACT_SIZE];
    
    // lettura dei due address
    read_string_from_file(DISPOSITIVO_1_PATH"contract_address.txt", wallet_address_dispositivoA);
    read_string_from_file(DISPOSITIVO_2_PATH"contract_address.txt", wallet_address_dispositivoB);

    
    uint8_t id_dispositivoA[ID_SIZE] = {0U}; uint8_t id_dispositivoB[ID_SIZE] = {0U};

    load_buffer_from_file(DISPOSITIVO_1_PATH"my_id.puf", id_dispositivoA, ID_SIZE);
    load_buffer_from_file(DISPOSITIVO_2_PATH"my_id.puf", id_dispositivoB, ID_SIZE);

    uint8_t riferimentoAutenticazione[MSG_B_SIZE] = {0U};
    load_buffer_from_file(DISPOSITIVO_1_PATH"riferimento_aut.puf", riferimentoAutenticazione, MSG_B_SIZE);


    verificaAvvenutaAutenticazione(wallet_address_dispositivoA, id_dispositivoA, riferimentoAutenticazione);
    verificaAvvenutaAutenticazione(wallet_address_dispositivoB, id_dispositivoB, riferimentoAutenticazione);

    return 0;   
}
