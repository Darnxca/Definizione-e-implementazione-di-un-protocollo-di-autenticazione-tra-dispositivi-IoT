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

// Funzione per concatenare "_prk_A.puf" alla stringa esadecimale
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

char* append_nonceSB_suffix(uint8_t* array, size_t size) {
    // Ottieni la stringa esadecimale
    char* hex_string = uint8_t_to_hex_string(array, size);

    // Concatena "_prk_A.puf" alla stringa esadecimale
    char* result = malloc(strlen("key_device/") + strlen(hex_string) + strlen("_nonceSB.puf") + 1);
    if (result == NULL) {
        fprintf(stderr, "Errore: impossibile allocare memoria.\n");
        exit(EXIT_FAILURE);
    }
    strcat(result, "key_device/");
    strcat(result, hex_string);
    strcat(result, "_nonceSB.puf");
    
    // Libera la memoria allocata per la stringa esadecimale
    free(hex_string);

    return result;
}
/*

./ottenimento_nonceAB "3675A0FCC268EBD6A9C72C8681C02F17" "F930F46C6E235B76B8A01912CF6C04D1ABF3D0FD95FF5F8FA41D707B8FA60CFE" "460AAF1D61185F2754CA314F9C6B1878" "44086A9C6CCB2E95260FC36341E5F8E5" "9C7D6654CB9B"

*/
int main(int argc, char *argv[]){
    srand(time(NULL));

    /**
     * argv[0]: Nome del programma eseguibile
     * argv[1]: msg_BA cifrato con prkA
     * argv[2]: digest msg_BA
     * argv[3]: id_dispositivoA
     * argv[4]: id_dispositivoB
    */
    if (argc != 5) {
        printf("Usage: %d <PUF_response>\n", argc);
        return 1;
    }
   
   /*-------      Controllo MSG_BA     --------*/

    uint8_t msg_BA[MSG_BA_SIZE] = {0U};
    uint8_t old_digest_msg_ba[DIGEST_SIZE] = {0U};
    uint8_t PRK_A[FE_KEY_SIZE] = {0U};
    uint8_t PRK_B[FE_KEY_SIZE] = {0U};
    uint8_t nonceSB[NONCE_SIZE] = {0U};
    uint8_t device_idA[ID_SIZE] = {0U};
    uint8_t device_idB[ID_SIZE] = {0U};

    // conversione delle string in uint
    hex_string_to_bytes(argv[1], msg_BA, MSG_BA_SIZE);
    hex_string_to_bytes(argv[2], old_digest_msg_ba, DIGEST_SIZE);
    hex_string_to_bytes(argv[3], device_idA, ID_SIZE);
    hex_string_to_bytes(argv[4], device_idB, ID_SIZE);

    // Ottieni il percorso completo con il suffisso "_prk_A.puf"
    char* path_prkA = append_prk_suffix(device_idA, ID_SIZE);
    char* path_prkB = append_prk_suffix(device_idB, ID_SIZE);
    char* path_nonceSB = append_nonceSB_suffix(device_idB, ID_SIZE);

    load_buffer_from_file(path_prkA, PRK_A, FE_KEY_SIZE);
    load_buffer_from_file(path_prkB, PRK_B, FE_KEY_SIZE);
    load_buffer_from_file(path_nonceSB, nonceSB, NONCE_SIZE);

    uint8_t nonceAB[NONCE_SIZE] = {0U};
    Decrypt(msg_BA, PRK_A);
    
    memcpy(nonceAB, msg_BA + 10,6);

    Encrypt(msg_BA, PRK_A);

    // Sezione creazione mac msg_BA || NAB
    uint8_t msg_macBA[MSG_BA_CONC_SIZE] = {0U};
    memcpy(msg_macBA, msg_BA, MSG_BA_SIZE);
    memcpy(msg_macBA + MSG_BA_SIZE, nonceAB, NONCE_SIZE);

    // Lunghezza digest 32 per sha256
    uint8_t new_digest_msg_ba[DIGEST_SIZE] = {0U};
    hmac(SHA256, msg_macBA, MSG_BA_CONC_SIZE, nonceSB, NONCE_SIZE, new_digest_msg_ba);

    // Compara mac
    for (uint8_t i = 0U; (i < 32U); i++){
        if(new_digest_msg_ba[i] != old_digest_msg_ba[i]) {
            printf("I mac sono 2 diversi.\n");
            exit(EXIT_FAILURE);
        }
    }

    uint8_t salt_nonce[10U] = {0U};
    genera(salt_nonce,10U);

    uint8_t msg_NA[MSG_BA_SIZE] = {0U};
    memcpy(msg_NA, salt_nonce, sizeof(salt_nonce));
    memcpy(msg_NA + 10, nonceAB, NONCE_SIZE);

    Encrypt(msg_NA, PRK_B);
    
    uint8_t msg_mac[MSG_BA_CONC_SIZE] = {0U};
    memcpy(msg_mac, msg_NA, 16);
    memcpy(msg_mac + MSG_BA_SIZE, nonceAB, NONCE_SIZE);

    // Lunghezza digest 32 per sha256
    uint8_t new_digest_msg_na[DIGEST_SIZE] = {0U};
    hmac(SHA256, msg_mac, MSG_BA_CONC_SIZE, nonceSB, NONCE_SIZE, new_digest_msg_na);

    
    print_buffer_as_json("nonceAB", msg_NA, MSG_BA_SIZE,
                            "digest", new_digest_msg_na, DIGEST_SIZE,
                            NULL);   
    return 0;
}