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


int main(int argc, char *argv[]){
    srand(time(NULL));

    uint8_t id_dispositivoA[ID_SIZE] = {0U};
    uint8_t helper_dataA[PUF_SIZE] = {0U};
    uint8_t nInitA[NONCE_SIZE] = {0U};
    uint8_t key[FE_KEY_SIZE] = {0U};

 
    // conversione delle string in uint
    load_buffer_from_file(DISPOSITIVO_1_PATH"my_helper_data.puf", helper_dataA, PUF_SIZE);
    load_buffer_from_file(DISPOSITIVO_1_PATH"my_id.puf", id_dispositivoA, ID_SIZE);
    load_buffer_from_file(DISPOSITIVO_1_PATH"initA.puf", nInitA, NONCE_SIZE);
    load_buffer_from_file(DISPOSITIVO_1_PATH"my_key.puf", key, FE_KEY_SIZE);

    // generazione PRK
    uint8_t PRK_A[FE_KEY_SIZE] = {0U};

    hkdf(SHA256, NULL, 0, key, FE_KEY_SIZE, nInitA, NONCE_SIZE, PRK_A, FE_KEY_SIZE);

    KeyValuePair* verificaAutenticazionePairs;

    while(1){
        verificaAutenticazionePairs = verificaAvvioAutenticazione(nInitA);
    
        if(strcmp(verificaAutenticazionePairs[0].value, "true") == 0){
            break;
        } 

        free(verificaAutenticazionePairs); 
    }

    //PRK_A
    uint8_t msg_A[MSG_A_SIZE] = {0U};
    uint8_t old_digest[DIGEST_SIZE] = {0U};
    

    hex_string_to_bytes(verificaAutenticazionePairs[1].value, msg_A, MSG_A_SIZE);
    hex_string_to_bytes(verificaAutenticazionePairs[2].value, old_digest, DIGEST_SIZE);

    Decrypt(msg_A, PRK_A);

    
    uint8_t nonceSA[6], nonceSB[6], msg_B[26];

    memcpy(nonceSA, msg_A + 14,6);
    memcpy(nonceSB, msg_A + 20,6);
    memcpy(msg_B, msg_A + 26,26);


    // cifratura del messaggio msg_a con la chiave del primo device
    Encrypt(msg_A, PRK_A);
       
    // Concatenazioni informazioni msg_A, ki e Ns-a per creare il  nuovo mac
    uint8_t mConc[MSG_A_CONC_SIZE] = {0U};
    memcpy(mConc, msg_A, MSG_A_SIZE);
    memcpy(mConc + MSG_A_SIZE, key, FE_KEY_SIZE);
    memcpy(mConc + MSG_A_SIZE + FE_KEY_SIZE, nonceSA, NONCE_SIZE);
    
    uint8_t new_digest[DIGEST_SIZE] = {0U};
    hmac(SHA256, mConc, MSG_A_CONC_SIZE, PRK_A, FE_KEY_SIZE, new_digest);

    // Compara mac
    for (uint8_t i = 0U; (i < 32U); i++){
        if(new_digest[i] != old_digest[i]) {
            printf("I mac sono  diversi.\n");
            exit(EXIT_FAILURE);
        }
    }


    // Generazioni di due nuovi nonce
    uint8_t nonceA[NONCE_SIZE] = {0U};
   
    genera(nonceA,NONCE_SIZE);

    // Generazione messaggio SA e cifratura con PRK_A
    uint8_t msg_SA[MSG_SA_SIZE] = {0U};
    memcpy(msg_SA, id_dispositivoA, ID_SIZE);
    memcpy(msg_SA + ID_SIZE, nonceSA, NONCE_SIZE);
    memcpy(msg_SA + ID_SIZE + NONCE_SIZE, nonceA, NONCE_SIZE);
    Encrypt(msg_SA, PRK_A);
    
    // Sezione Server Verifica il MAC
    uint8_t msg_Server[MSG_SA_CONC_SIZE]  = {0U};
    memcpy(msg_Server, msg_SA, MSG_SA_SIZE);
    memcpy(msg_Server + MSG_SA_SIZE, nonceA, NONCE_SIZE);
    
    // Lunghezza digest 32 per sha256
    uint8_t server_digest[DIGEST_SIZE]  = {0U};
    hmac(SHA256, msg_Server, MSG_SA_CONC_SIZE, PRK_A, FE_KEY_SIZE, server_digest);
    
   // al server inviare msg_SA, il suo mac, MA cifrato con prk_A (riferimento autenticazioen), idA e helperdata

    char wallet_address_dispositivo[CONTRACT_SIZE];
    read_string_from_file(DISPOSITIVO_1_PATH"contract_address.txt", wallet_address_dispositivo);
    
    serverAVerificaMac(wallet_address_dispositivo, msg_SA, server_digest, id_dispositivoA, helper_dataA,  msg_B);

    // Sezione creazione mac msg_B || NSB
    uint8_t msg_macB[MSG_B_CONC_SIZE] = {0U};
    memcpy(msg_macB, msg_B, MSG_B_SIZE);
    memcpy(msg_macB + MSG_B_SIZE, nonceSB, NONCE_SIZE);

   
    // Lunghezza digest 32 per sha256
    uint8_t digest_macB[DIGEST_SIZE] = {0U};
    hmac(SHA256, msg_macB, MSG_B_CONC_SIZE, nonceSB, NONCE_SIZE, digest_macB);
   
    // Generazione messaggio BA e cifratura con N_SB
    uint8_t nonceAB[NONCE_SIZE] = {0U};
    genera(nonceAB,NONCE_SIZE);

    uint8_t msg_BA[MSG_BA_SIZE] = {0U};
    memcpy(msg_BA, id_dispositivoA, ID_SIZE);
    memcpy(msg_BA + ID_SIZE, nonceA, NONCE_SIZE);
    memcpy(msg_BA + ID_SIZE + NONCE_SIZE, nonceAB, NONCE_SIZE);

    Encrypt(msg_BA, PRK_A);

    // Sezione creazione mac msg_BA || NAB
    uint8_t msg_macBA[MSG_BA_CONC_SIZE] = {0U};
    memcpy(msg_macBA, msg_BA, MSG_BA_SIZE);
    memcpy(msg_macBA + MSG_BA_SIZE, nonceAB, NONCE_SIZE);


    // Lunghezza digest 32 per sha256
    uint8_t digest_macBA[DIGEST_SIZE] = {0U};
    hmac(SHA256, msg_macBA, MSG_BA_CONC_SIZE, nonceSB, NONCE_SIZE, digest_macBA);

    write_buffer_to_file(DISPOSITIVO_1_PATH"nonceAB.puf", nonceAB, NONCE_SIZE);
    write_buffer_to_file(DISPOSITIVO_1_PATH"nonceSB.puf", nonceSB, NONCE_SIZE);
    write_buffer_to_file(DISPOSITIVO_1_PATH"riferimento_aut.puf", msg_B, MSG_B_SIZE);

    // simulo l'invio
    write_buffer_to_file("../../file_dispositivi/A_invia_B/MSG_B.puf", msg_B, MSG_B_SIZE);
    write_buffer_to_file("../../file_dispositivi/A_invia_B/digest_msgB.puf", digest_macB, DIGEST_SIZE);
    write_buffer_to_file("../../file_dispositivi/A_invia_B/MSG_BA.puf", msg_BA, MSG_BA_SIZE);
    write_buffer_to_file("../../file_dispositivi/A_invia_B/digest_msgBA.puf", digest_macBA, DIGEST_SIZE);
    
    return 0;
}