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

    uint8_t ninitB[NONCE_SIZE] = {0U};
    uint8_t key[FE_KEY_SIZE] = {0U};
   
    load_buffer_from_file(DISPOSITIVO_2_PATH"initB.puf", ninitB, NONCE_SIZE);
    load_buffer_from_file(DISPOSITIVO_2_PATH"my_key.puf", key, FE_KEY_SIZE);

    uint8_t msg_B[MSG_B_SIZE] = {0U};
    uint8_t old_digest_msg_b[DIGEST_SIZE] = {0U};

    load_buffer_from_file("../../file_dispositivi/A_invia_B/MSG_B.puf", msg_B, MSG_B_SIZE);
    load_buffer_from_file("../../file_dispositivi/A_invia_B/digest_msgB.puf", old_digest_msg_b, DIGEST_SIZE);

    uint8_t PRK_B[FE_KEY_SIZE] = {0U};

    // Ottengo nonceSB
    uint8_t nonceSB[NONCE_SIZE] = {0U};

    hkdf(SHA256, NULL, 0, key, FE_KEY_SIZE, ninitB, sizeof(ninitB), PRK_B, FE_KEY_SIZE);

    Decrypt(msg_B, PRK_B);

    memcpy(nonceSB, msg_B + 20,6);
    
    // Verifico ora il digest di msg_b
    Encrypt(msg_B, PRK_B);
    
    uint8_t msg_macB[MSG_B_CONC_SIZE] = {0U};
    memcpy(msg_macB, msg_B, MSG_B_SIZE);
    memcpy(msg_macB + MSG_B_SIZE, nonceSB, NONCE_SIZE);

    // Lunghezza digest 32 per sha256
    uint8_t new_digest_msg_b[DIGEST_SIZE] = {0U};
    hmac(SHA256, msg_macB, MSG_B_CONC_SIZE, nonceSB, NONCE_SIZE, new_digest_msg_b);

    // Compara mac
    for (uint8_t i = 0U; (i < 32U); i++){
        if(new_digest_msg_b[i] != old_digest_msg_b[i]) {
            printf("I mac sono  diversi.\n");
            exit(EXIT_FAILURE);
        }
    }
   
   /*-------      Controllo MSG_BA     --------*/

    uint8_t nonceAB[NONCE_SIZE] = {0U};
    uint8_t msg_BA[MSG_BA_SIZE] = {0U};
    uint8_t digest_msg_ba[DIGEST_SIZE] = {0U};
    
    load_buffer_from_file("../../file_dispositivi/A_invia_B/MSG_BA.puf", msg_BA, MSG_BA_SIZE);
    load_buffer_from_file("../../file_dispositivi/A_invia_B/digest_msgBA.puf", digest_msg_ba, DIGEST_SIZE);

    uint8_t id_dispositivoB[ID_SIZE] = {0U};
    uint8_t id_dispositivoA[ID_SIZE] = {0U};
    load_buffer_from_file(DISPOSITIVO_2_PATH"my_id.puf", id_dispositivoB, ID_SIZE);
    load_buffer_from_file(DISPOSITIVO_1_PATH"my_id.puf", id_dispositivoA, ID_SIZE);

    // recuper il nonceAB
    KeyValuePair* nonceABPairs = ottenimentoNonceAB(msg_BA, digest_msg_ba, id_dispositivoA, id_dispositivoB);

    uint8_t msg_nonceAB[MSG_BA_SIZE] = {0U};
    uint8_t old_digest_msg_ba[DIGEST_SIZE] = {0U};
    
    hex_string_to_bytes(nonceABPairs[1].value, msg_nonceAB, MSG_BA_SIZE);
    hex_string_to_bytes(nonceABPairs[2].value, old_digest_msg_ba, DIGEST_SIZE);

    Decrypt(msg_nonceAB, PRK_B);

    memcpy(nonceAB, msg_nonceAB + 10,6);
    
    Encrypt(msg_nonceAB, PRK_B);
    //Encrypt(msg_BA, nonceSB);

    // Sezione creazione mac msg_BA || NAB
    uint8_t msg_macBA[MSG_BA_CONC_SIZE] = {0U};
    memcpy(msg_macBA, msg_nonceAB, MSG_BA_SIZE);
    memcpy(msg_macBA + MSG_BA_SIZE, nonceAB, NONCE_SIZE);

    // Lunghezza digest 32 per sha256
    uint8_t new_digest_msg_ba[DIGEST_SIZE] = {0U};
    hmac(SHA256, msg_macBA, sizeof(msg_macBA), nonceSB, sizeof(nonceSB), new_digest_msg_ba);

    // Compara mac
    for (uint8_t i = 0U; (i < 32U); i++){
        if(new_digest_msg_ba[i] != old_digest_msg_ba[i]) {
            printf("I mac sono 2 diversi.\n");
            exit(EXIT_FAILURE);
        }
    }


    ///// continua da qui
    // Generazione nonce B
    uint8_t nonceB[NONCE_SIZE] = {0U};
    genera(nonceB,NONCE_SIZE);

    // Generazione messaggio server B
    uint8_t msg_serverB[MSG_SB_SIZE] = {0U};
    memcpy(msg_serverB, id_dispositivoB, ID_SIZE);
    memcpy(msg_serverB + ID_SIZE, nonceSB, NONCE_SIZE);
    memcpy(msg_serverB + ID_SIZE + NONCE_SIZE, nonceB, NONCE_SIZE);
    
    Encrypt(msg_serverB, PRK_B);

    // Sezione creazione mac msg_BA || NAB
    uint8_t msg_mac_serverB[MSG_SB_CONC_SIZE] = {0U};
    memcpy(msg_mac_serverB, msg_serverB, MSG_SB_SIZE);
    memcpy(msg_mac_serverB + MSG_SB_SIZE, nonceB, NONCE_SIZE);

    // Lunghezza digest 32 per sha256
    uint8_t digest_msg_server_b[DIGEST_SIZE] = {0U};
    hmac(SHA256, msg_mac_serverB, MSG_SB_CONC_SIZE, nonceB, NONCE_SIZE, digest_msg_server_b);

    // invio al server id_dispositivoB, msg_B, msg_serverB, digest_msg_server_b
    char wallet_address_dispositivo[CONTRACT_SIZE];
    read_string_from_file(DISPOSITIVO_2_PATH"contract_address.txt", wallet_address_dispositivo);
    
    dispositivoBConfermaAutenticazione(wallet_address_dispositivo, msg_serverB, digest_msg_server_b, id_dispositivoB, msg_B);

   // Sezione creazione mac msg_BA || NAB
    uint8_t msg_mac_conferma_autenticazione[CONF_AUT_SIZE];
    memcpy(msg_mac_conferma_autenticazione, id_dispositivoA, ID_SIZE);
    memcpy(msg_mac_conferma_autenticazione + ID_SIZE, id_dispositivoB, ID_SIZE);
    memcpy(msg_mac_conferma_autenticazione + ID_SIZE + ID_SIZE, nonceAB, NONCE_SIZE);

    // Lunghezza digest 32 per sha256
    uint8_t digest_msg_mac_conferma_autenticazione[DIGEST_SIZE];
    hmac(SHA256, msg_mac_conferma_autenticazione, CONF_AUT_SIZE, nonceSB, NONCE_SIZE, digest_msg_mac_conferma_autenticazione);
    
    
    write_buffer_to_file("../../file_dispositivi/B_invia_A/digest_auth.puf", digest_msg_mac_conferma_autenticazione, DIGEST_SIZE);
    
    return 0;
}
