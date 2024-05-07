#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <curl/curl.h> // Includi la libreria cURL

#include "my_curl_request.h"
#include "my_utility.h"
#include "pc_utils.h"
#include "puf.h"
#include "hkdf.h"
#include "cipher.h"

unsigned const int __attribute__((section(".version"))) optiboot_version = 2049U;
uint8_t __attribute__((section(".noinit"))) bss_puf[PUF_SIZE];


int main(){
    srand(time(NULL));
    
    uint8_t id_dispositivoB[ID_SIZE] = {0U};
    uint8_t id_dispositivoA[ID_SIZE] = {0U};
    uint8_t nonceSB[NONCE_SIZE] = {0U};
    uint8_t nonceAB[NONCE_SIZE] = {0U};
    uint8_t old_digest_msg_autenticazione[DIGEST_SIZE] = {0U};
    uint8_t riferimento[MSG_B_SIZE] = {0U};

    load_buffer_from_file(DISPOSITIVO_2_PATH"my_id.puf", id_dispositivoB, ID_SIZE);
    load_buffer_from_file(DISPOSITIVO_1_PATH"my_id.puf", id_dispositivoA, ID_SIZE);
    load_buffer_from_file(DISPOSITIVO_1_PATH"nonceAB.puf", nonceAB, NONCE_SIZE);
    load_buffer_from_file(DISPOSITIVO_1_PATH"nonceSB.puf", nonceSB, NONCE_SIZE);
    load_buffer_from_file("../../file_dispositivi/B_invia_A/digest_auth.puf", old_digest_msg_autenticazione, DIGEST_SIZE);
    load_buffer_from_file("../../file_dispositivi/A_invia_B/MSG_B.puf", riferimento, MSG_B_SIZE);


    // Sezione creazione mac msg_BA || NAB
    uint8_t msg_mac_conferma_autenticazione[CONF_AUT_SIZE];
    memcpy(msg_mac_conferma_autenticazione, id_dispositivoA, ID_SIZE);
    memcpy(msg_mac_conferma_autenticazione + ID_SIZE, id_dispositivoB, ID_SIZE);
    memcpy(msg_mac_conferma_autenticazione + ID_SIZE + ID_SIZE, nonceAB, NONCE_SIZE);
   
    // Lunghezza digest 32 per sha256
    uint8_t digest_msg_mac_conferma_autenticazione[DIGEST_SIZE];
    hmac(SHA256, msg_mac_conferma_autenticazione, CONF_AUT_SIZE, nonceSB, NONCE_SIZE, digest_msg_mac_conferma_autenticazione);
    
    // Compara mac
    for (uint8_t i = 0U; (i < 32U); i++){
        if(digest_msg_mac_conferma_autenticazione[i] != old_digest_msg_autenticazione[i]) {
            printf("I mac sono  diversi.\n");
            exit(EXIT_FAILURE);
        }
    }

    
    // invio al server id_dispositivoB, msg_B, msg_serverB, digest_msg_server_b
    char wallet_address_dispositivo[CONTRACT_SIZE];
    read_string_from_file(DISPOSITIVO_1_PATH"contract_address.txt", wallet_address_dispositivo);
    
    dispositivoAConfermaAutenticazione(wallet_address_dispositivo, id_dispositivoA, riferimento);

    return 0;
}
