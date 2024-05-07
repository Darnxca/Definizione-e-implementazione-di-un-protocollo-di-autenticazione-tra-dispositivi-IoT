#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h> // Includi la libreria cURL

#include "my_utility.h"
#include "puf_parameters.h"
#include "pc_utils.h"
#include "my_curl_request.h"

int main() {

    uint8_t puf_response[PUF_SIZE] = {0U};
    char puf_response_str[PUF_SIZE_STR]; // Ogni byte occupa 2 caratteri esadecimali, più 1 per il terminatore di stringa

    load_buffer_from_file(DISPOSITIVO_2_PATH"puf_response_dispositivo_2.puf", puf_response, PUF_SIZE);

    // Converte le stringhe esadecimali in array di byte
    if (bytes_to_hex_string(puf_response, PUF_SIZE, puf_response_str, sizeof(puf_response_str)) != 0) {
        printf("Errore nella conversione della risposta PUF in string.\n");
        return 1;
    }

    char wallet_address_dispositivo[CONTRACT_SIZE];
    read_string_from_file(DISPOSITIVO_1_PATH"contract_address.txt", wallet_address_dispositivo);

    KeyValuePair* registrazionePairs = registraDispositivo(wallet_address_dispositivo, puf_response);

    // Salvo gli id e gli helper data ottenuti dal server di registrazione
    write_string_to_file(DISPOSITIVO_2_PATH"my_id.puf", registrazionePairs[2].value);
  
    write_string_to_file(DISPOSITIVO_2_PATH"my_helper_data.puf", registrazionePairs[3].value);

    // Pulizia e liberazione della risposta
    free(registrazionePairs);

    return 0;
}
