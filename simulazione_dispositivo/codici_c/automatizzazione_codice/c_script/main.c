#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h> // Includi la libreria cURL

#include "my_utility.h"
#include "puf_parameters.h"
#include "pc_utils.h"
#include "my_curl_request.h"
#include "puf.h"
#include "hkdf.h"
#include "cipher.h"

int main() {
    srand(time(NULL));
    
    // Address dei wallet dei due dispositivi
    char wallet_address_dispositivoA[CONTRACT_SIZE], wallet_address_dispositivoB[CONTRACT_SIZE];
    
    // lettura dei due address
    read_string_from_file(DISPOSITIVO_1_PATH"contract_address.txt", wallet_address_dispositivoA);
    read_string_from_file(DISPOSITIVO_2_PATH"contract_address.txt", wallet_address_dispositivoB);
   
    //Inizializzazione variabili utili dei due dispositivi
    uint8_t puf_responseA[PUF_SIZE] = {0U};  uint8_t puf_responseB[PUF_SIZE] = {0U};
    uint8_t id_dispositivoA[ID_SIZE] = {0U}; uint8_t id_dispositivoB[ID_SIZE] = {0U};
    uint8_t helper_dataA[PUF_SIZE] = {0U};   uint8_t helper_dataB[PUF_SIZE] = {0U};
    uint8_t ki[FE_KEY_SIZE] = {0U};          uint8_t kj[FE_KEY_SIZE] = {0U};
    uint8_t ninitA[NONCE_SIZE] = {0U};       uint8_t ninitB[NONCE_SIZE] = {0U};
                            /* FASE DI REGISTRAZIONE DISPOSITIVI*/
    // lettura delle puf response
    load_buffer_from_file(DISPOSITIVO_1_PATH"puf_response_dispositivo_1.puf", puf_responseA, PUF_SIZE);
    load_buffer_from_file(DISPOSITIVO_2_PATH"puf_response_dispositivo_2.puf", puf_responseB, PUF_SIZE);

    // registrazione dispositivo A
    KeyValuePair* registrazionePairsA = registraDispositivo(wallet_address_dispositivoA, puf_responseA);

    hex_string_to_bytes(registrazionePairsA[2].value, id_dispositivoA, ID_SIZE);
    hex_string_to_bytes(registrazionePairsA[3].value, helper_dataA, PUF_SIZE);

    free(registrazionePairsA);

    // registrazione dispositivo B
    KeyValuePair* registrazionePairsB = registraDispositivo(wallet_address_dispositivoB, puf_responseB);

    hex_string_to_bytes(registrazionePairsB[2].value, id_dispositivoB, ID_SIZE);
    hex_string_to_bytes(registrazionePairsB[3].value, helper_dataB, PUF_SIZE);

    free(registrazionePairsB);

    printf("****** Registrazione Dispositivi ******");
    printf("\n");
    printf("ID del dispositivo A:"); stampa(id_dispositivoA, ID_SIZE);
    printf("Helper data dispositivo A:"); stampa(helper_dataA, PUF_SIZE);
    printf("ID del dispositivo B:"); stampa(id_dispositivoB, ID_SIZE);
    printf("Helper data dispositivo B:"); stampa(helper_dataB, PUF_SIZE);
    printf("\n");

                            /*FASE GENERAZIONE CHIAVI E NONCE DI INIZZIALIZZAZIONE*/
    // genero la chiave ki
    puf_extract(puf_responseA, helper_dataA, ki);
    // genero il nonce di inizializzazione ninitA
    genera(ninitA,NONCE_SIZE);

    // genero la chiave kj
    puf_extract(puf_responseB, helper_dataB, kj);  
    // genero il nonce di inizializzazione ninitB
    genera(ninitB,NONCE_SIZE);

    printf("****** Generazione chiavi e nonce Dispositivi ******");
    printf("\n");
    printf("Nonce dispositivo A:"); stampa(ninitA, NONCE_SIZE);
    printf("Chiave dispositivo A:"); stampa(ki, FE_KEY_SIZE);
    printf("Nonce dispositivo B:"); stampa(ninitB, NONCE_SIZE);
    printf("Chiave dispositivo B:"); stampa(kj, FE_KEY_SIZE);
    printf("\n");

                            /*FASE AVVIO AUTENTICAZIONE*/
    

    avviaAutenticazione(wallet_address_dispositivoB, id_dispositivoA, helper_dataA, ninitA, id_dispositivoB, helper_dataB, ninitB);

    printf("****** Avvio fase autenticazione ******");
    printf("\n");
    printf("Il dispositivo B ha avviato la fase di autenticazione\n");
    printf("\n");
    

                        /*Dispositivo A verifica l'inizio della'autenticazione*/
    
    // generazione PRK de dispositivo A
    uint8_t PRK_A[FE_KEY_SIZE] = {0U};

    hkdf(SHA256, NULL, 0, ki, FE_KEY_SIZE, ninitA, NONCE_SIZE, PRK_A, FE_KEY_SIZE);

    KeyValuePair* verificaAutenticazionePairs;

    while(1){
        verificaAutenticazionePairs = verificaAvvioAutenticazione(ninitA);
    
        if(strcmp(verificaAutenticazionePairs[0].value, "true") == 0){
            break;
        } 

        free(verificaAutenticazionePairs); 
    }

    // Prendo il messaggio A e il suo digest
    uint8_t msg_A[MSG_A_SIZE] = {0U};
    uint8_t digest_msg_A[DIGEST_SIZE] = {0U};
   
    hex_string_to_bytes(verificaAutenticazionePairs[1].value, msg_A, MSG_A_SIZE);
    hex_string_to_bytes(verificaAutenticazionePairs[2].value, digest_msg_A, DIGEST_SIZE);

    // decripto il messaggio A
    Decrypt(msg_A, PRK_A);

    // Prenodi il nonce SA, nonceSB, msg_B
    uint8_t nonceSA[NONCE_SIZE], nonceSB[NONCE_SIZE], msg_B[MSG_B_SIZE];
    memcpy(nonceSA, msg_A + 14,6);
    memcpy(nonceSB, msg_A + 20,6);
    memcpy(msg_B, msg_A + 26,26);

    // cifratura del messaggio msg_a con la chiave del primo device
    Encrypt(msg_A, PRK_A);

    // Concatenazioni informazioni msg_A, ki e Ns-a per creare il  nuovo mac
    uint8_t conc_msg_A[MSG_A_CONC_SIZE] = {0U};
    memcpy(conc_msg_A, msg_A, MSG_A_SIZE);
    memcpy(conc_msg_A + MSG_A_SIZE, ki, FE_KEY_SIZE);
    memcpy(conc_msg_A + MSG_A_SIZE + FE_KEY_SIZE, nonceSA, NONCE_SIZE);
    
    uint8_t new_digest_msg_A[DIGEST_SIZE] = {0U};
    hmac(SHA256, conc_msg_A, MSG_A_CONC_SIZE, PRK_A, FE_KEY_SIZE, new_digest_msg_A);

    // Compara mac
    for (uint8_t i = 0U; (i < 32U); i++){
        if(new_digest_msg_A[i] != digest_msg_A[i]) {
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

    serverAVerificaMac(wallet_address_dispositivoA, msg_SA, server_digest, id_dispositivoA, helper_dataA,  msg_B);

                            /*Generazione messaggi per il dispositivo B*/

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

    printf("****** Dispositivo A verifica l'inizio della'autenticazione ******");
    printf("\n");
    printf("Il dispositivo A verifica se il mac del messaggio è uguale\n");
    printf("Il dispositivo A genera nonceA e MSG_SA\n");
    printf("Il dispositivo A avvia la verifica del mac da parte del server\n");
    printf("Il server conferma la verifica del mac\n");
    printf("Il dispositivo A crea un mac per msg_B\n");
    printf("Il dispositivo A genera nonceAB e un messaggio msg_BA e li invia al dispositivo B\n");
    printf("\n");


                            /*Dispositivo B conferma autenticazione*/

    // Chiave privata di B
    uint8_t PRK_B[FE_KEY_SIZE] = {0U};
    hkdf(SHA256, NULL, 0, kj, FE_KEY_SIZE, ninitB, NONCE_SIZE, PRK_B, FE_KEY_SIZE);

    // Dati estrati da B e verifichiamo il messaggio B
    uint8_t nonceSB_estratto[NONCE_SIZE] = {0U};

    Decrypt(msg_B, PRK_B);

    memcpy(nonceSB_estratto, msg_B + 20,6);
    
    Encrypt(msg_B, PRK_B);

    uint8_t msg_macB_nuovo[MSG_B_CONC_SIZE] = {0U};
    memcpy(msg_macB_nuovo, msg_B, MSG_B_SIZE);
    memcpy(msg_macB_nuovo + MSG_B_SIZE, nonceSB_estratto, NONCE_SIZE);

    // Lunghezza digest 32 per sha256
    uint8_t new_digest_msg_b[DIGEST_SIZE] = {0U};
    hmac(SHA256, msg_macB_nuovo, MSG_B_CONC_SIZE, nonceSB_estratto, NONCE_SIZE, new_digest_msg_b);

    // Compara mac
    for (uint8_t i = 0U; (i < 32U); i++){
        if(new_digest_msg_b[i] != digest_macB[i]) {
            printf("I mac sono  diversi.\n");
            exit(EXIT_FAILURE);
        }
    }

                                    /*-------      Controllo MSG_BA     --------*/

    KeyValuePair* nonceABPairs = ottenimentoNonceAB(msg_BA, digest_macBA, id_dispositivoA, id_dispositivoB);

    uint8_t msg_nonceAB[MSG_BA_SIZE] = {0U};
    uint8_t old_digest_msg_ba_server[DIGEST_SIZE] = {0U};
    
    hex_string_to_bytes(nonceABPairs[1].value, msg_nonceAB, MSG_BA_SIZE);
    hex_string_to_bytes(nonceABPairs[2].value, old_digest_msg_ba_server, DIGEST_SIZE);

    free(nonceABPairs);

    uint8_t nonceAB_estratto[NONCE_SIZE] = {0U};


    Decrypt(msg_nonceAB, PRK_B);

    memcpy(nonceAB_estratto, msg_nonceAB + 10,6);
    
    Encrypt(msg_nonceAB, PRK_B);


    // Sezione creazione mac msg_BA || NAB
    uint8_t msg_macBA_nuovo[MSG_BA_CONC_SIZE] = {0U};
    memcpy(msg_macBA_nuovo, msg_nonceAB, MSG_BA_SIZE);
    memcpy(msg_macBA_nuovo + MSG_BA_SIZE, nonceAB_estratto, NONCE_SIZE);

    // Lunghezza digest 32 per sha256
    uint8_t new_digest_msg_ba[DIGEST_SIZE] = {0U};
    hmac(SHA256, msg_macBA_nuovo, MSG_BA_CONC_SIZE , nonceSB_estratto, NONCE_SIZE, new_digest_msg_ba);

    // Compara mac
    for (uint8_t i = 0U; (i < 32U); i++){
        if(new_digest_msg_ba[i] != old_digest_msg_ba_server[i]) {
            printf("I mac sono 2 diversi.\n");
            exit(EXIT_FAILURE);
        }
    }

    /* Conferma autenticazione*/
    // Generazione nonce B
    uint8_t nonceB[NONCE_SIZE] = {0U};
    genera(nonceB,NONCE_SIZE);

    // Generazione messaggio server B
    uint8_t msg_serverB[MSG_SB_SIZE] = {0U};
    memcpy(msg_serverB, id_dispositivoB, ID_SIZE);
    memcpy(msg_serverB + ID_SIZE, nonceSB_estratto, NONCE_SIZE);
    memcpy(msg_serverB + ID_SIZE + NONCE_SIZE, nonceSB_estratto, NONCE_SIZE);
    
    Encrypt(msg_serverB, PRK_B);

    // Sezione creazione mac msg_BA || NAB
    uint8_t msg_mac_serverB[MSG_SB_CONC_SIZE] = {0U};
    memcpy(msg_mac_serverB, msg_serverB, MSG_SB_SIZE);
    memcpy(msg_mac_serverB + MSG_SB_SIZE, nonceSB_estratto, NONCE_SIZE);

    // Lunghezza digest 32 per sha256
    uint8_t digest_msg_server_b[DIGEST_SIZE] = {0U};
    hmac(SHA256, msg_mac_serverB, MSG_SB_CONC_SIZE, nonceSB_estratto, NONCE_SIZE, digest_msg_server_b);
    
    dispositivoBConfermaAutenticazione(wallet_address_dispositivoB, msg_serverB, digest_msg_server_b, id_dispositivoB, msg_B);
    
    /* mac da inviare ad A*/
    // Sezione creazione mac msg_BA || NAB
    uint8_t msg_mac_conferma_autenticazione[CONF_AUT_SIZE];
    memcpy(msg_mac_conferma_autenticazione, id_dispositivoA, ID_SIZE);
    memcpy(msg_mac_conferma_autenticazione + ID_SIZE, id_dispositivoB, ID_SIZE);
    memcpy(msg_mac_conferma_autenticazione + ID_SIZE + ID_SIZE, nonceAB_estratto, NONCE_SIZE);

    // Lunghezza digest 32 per sha256
    uint8_t digest_msg_mac_conferma_autenticazione[DIGEST_SIZE];
    hmac(SHA256, msg_mac_conferma_autenticazione, CONF_AUT_SIZE, nonceSB_estratto, NONCE_SIZE, digest_msg_mac_conferma_autenticazione);

    printf("****** Dispositivo B conferma l'autenticazione ******");
    printf("\n");
    printf("Il dispositivo B riceve i messaggi\n");
    printf("Il dispositivo B estrae da MSG B nonceSB e verifica il suo MAC\n");
    printf("Il dispositivo B ottiene dal server 2 il nonceAB crittato e ne verifica il mac\n");
    printf("Il dispositivo B invia un messaggio al server B per confermare l'autenticazione\n");
    printf("Il dispositivo B conferma l'autenticazione e avvisa il dispositivo A");
    printf("\n");

                                /* Dispositivo A conferma l'autenticazione*/
                                 // Sezione creazione mac msg_BA || NAB
    uint8_t msg_mac_conferma_autenticazione_nuovo[CONF_AUT_SIZE];
    memcpy(msg_mac_conferma_autenticazione_nuovo, id_dispositivoA, ID_SIZE);
    memcpy(msg_mac_conferma_autenticazione_nuovo + ID_SIZE, id_dispositivoB, ID_SIZE);
    memcpy(msg_mac_conferma_autenticazione_nuovo + ID_SIZE + ID_SIZE, nonceAB, NONCE_SIZE);
   
    // Lunghezza digest 32 per sha256
    uint8_t digest_msg_mac_conferma_autenticazione_nuovo[DIGEST_SIZE];
    hmac(SHA256, msg_mac_conferma_autenticazione_nuovo, CONF_AUT_SIZE, nonceSB, NONCE_SIZE, digest_msg_mac_conferma_autenticazione_nuovo);
    
    // Compara mac
    for (uint8_t i = 0U; (i < 32U); i++){
        if(digest_msg_mac_conferma_autenticazione_nuovo[i] != digest_msg_mac_conferma_autenticazione[i]) {
            printf("I mac sono  diversi.\n");
            exit(EXIT_FAILURE);
        }
    }

    dispositivoAConfermaAutenticazione(wallet_address_dispositivoA, id_dispositivoA, msg_B);
   
    printf("****** Dispositivo A conferma l'autenticazione ******");
    printf("\n");
    printf("Il dispositivo A riceve il mac\n");
    printf("Il dispositivo A verifica il MAC\n");
    printf("Il dispositivo A conferma l'autenticazione");
    printf("\n");

                                /* Fase verifica autenticazione stabilita*/
    
    verificaAvvenutaAutenticazione(wallet_address_dispositivoA, id_dispositivoA, msg_B);
    verificaAvvenutaAutenticazione(wallet_address_dispositivoB, id_dispositivoB, msg_B);

    printf("****** Controllo autenticazione stabilita ******");
    printf("\n");
    printf("Il dispositivo A Dice che l'autenticazione è stabilita\n");
    printf("Il dispositivo B Dice che l'autenticazione è stabilita\n");
    printf("\n");

    return 0;
}
