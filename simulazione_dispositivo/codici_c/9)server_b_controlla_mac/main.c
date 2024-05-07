#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "my_utility.h"
#include "pc_utils.h"
#include "puf.h"
#include "hkdf.h"
#include "cipher.h"

unsigned const int __attribute__((section(".version"))) optiboot_version = 2049U;
uint8_t __attribute__((section(".noinit"))) bss_puf[PUF_SIZE];

/**
 *  Input di prova per testare:
 ./serverB_verifica_mac 1857D888 2F4B778A25AB331DD04467A5D1BFA285 A69C498E853B6E996F6E4B74EAD050C65F366270CA0D7DB3431044BF9EBA3ACB
 * 
 * 
 * /*
 * 
*/

char* append_prk_suffix(uint8_t* array, size_t size) {
    // Ottieni la stringa esadecimale
    char* hex_string = uint8_t_to_hex_string(array, size);

    // Concatena "_prk_A.puf" alla stringa esadecimale
    char* result = malloc(strlen("key_device/") + strlen(hex_string) + strlen("_prk.puf") + 1);
    if (result == NULL) {
        fprintf(stderr, "Errore: impossibile allocare memoria.\n");
        exit(EXIT_FAILURE);
    }
    strcat(result, "key_device/");
    strcat(result, hex_string);
    strcat(result, "_prk.puf");
    
    // Libera la memoria allocata per la stringa esadecimale
    free(hex_string);

    return result;
}

int main(int argc, char *argv[]){
    srand(time(NULL));
 
    /**
     * argv[0]: Nome del programma eseguibile
     * argv[1]: id_dispositivoB
     * argv[2]: msg_SB
     * argv[3]: digest
    */
    if (argc != 4) {
        printf("Usage: %d <PUF_response>\n", argc);
        return 1;
    }
    
    uint8_t idDispositivoB[ID_SIZE] = {0U};
    uint8_t msg_SB[MSG_SB_SIZE] = {0U};
    uint8_t digest[DIGEST_SIZE] = {0U};

    // conversione delle string in uint
    hex_string_to_bytes(argv[1], idDispositivoB, ID_SIZE);
    hex_string_to_bytes(argv[2], msg_SB, sizeof(msg_SB));
    hex_string_to_bytes(argv[3], digest, sizeof(digest));

    // generazione PRK_a
    uint8_t PRK_B[FE_KEY_SIZE] = {0U};

    char* path_prkB = append_prk_suffix(idDispositivoB, ID_SIZE);
    load_buffer_from_file(path_prkB, PRK_B, FE_KEY_SIZE);
    
    uint8_t nonceB[NONCE_SIZE];

    Decrypt(msg_SB, PRK_B);

    memcpy(nonceB, msg_SB + 10,6);

    Encrypt(msg_SB, PRK_B);

    // Sezione creazione mac msg_BA || NAB
    uint8_t msg_mac_serverB[MSG_SB_CONC_SIZE];
    memcpy(msg_mac_serverB, msg_SB, MSG_SB_SIZE);
    memcpy(msg_mac_serverB + MSG_SB_SIZE, nonceB, NONCE_SIZE);

    // Lunghezza digest 32 per sha256
    uint8_t new_digest_msg_server_b[DIGEST_SIZE];
    hmac(SHA256, msg_mac_serverB, MSG_SB_CONC_SIZE, nonceB, NONCE_SIZE, new_digest_msg_server_b);

    // Compara mac
    char * success = "true";
    for (uint8_t i = 0U; (i < 32U); i++){
        if(new_digest_msg_server_b[i] != digest[i]) {
            success = "false";
            break;
        }
    }

    printf("%s", success);

    return 0;
}

