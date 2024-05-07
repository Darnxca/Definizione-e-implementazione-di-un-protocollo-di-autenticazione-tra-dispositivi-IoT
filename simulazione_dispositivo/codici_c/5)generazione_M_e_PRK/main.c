#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <curl/curl.h> // Includi la libreria cURL

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
    char* result = malloc(strlen("key_device/") + strlen(hex_string) + strlen("_prk_A.puf") + 1);
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

/**
 *  Input di prova per testare:
 * 
 ./genera_M_e_PRK "EE7CCEEDEEC975CDD62CFEFB2FBC4AFF3E3FF4EFADDE616FFF7F7BBF7F83B77CCDFF796FFF7576D61F1F1F01C6F5F5FD6FBB571FBBDEAAABE7EF45ED95B4E7D9D7ED37696F2BC7D70C64BA277CB230D6673FA3FC77BFFDE96EFEEB57D3B9FFE097FB27F7A6FEE7EAFFFBA7DE5309CBAD870BBEEF99CAAFAC716F1FAEF3FEBE18EFAAAE1FDFCCBEE5647FFD78F8B625A6B9B6FD9D6FF6DD1EC6B7EF9DDB06EBA6FEF9DBC4DD8FAF6F327D6D6EC8B1B2F9DF5DCEFBF7AF7ABBEEEF79FB650597DE571BABDFF8FD5FECFF7A6BFF8AFFF0536B1D2AEABEDCEFFE762A73FFB367FEEC28338DAD19EBAA532FC6BC6B247FEE1BFF7387F3B6BBBFEBFF7A7A07D0EF6EB6FE7FBD7FC7FEFA24BF2FD89CEFCF9E99B6E22EF7F3BCBE5FBF8FDFCBFF761D95E5D33BBABDE569FBD2DF3B88BC315BBF" "EE7CCEEDEEC975CD562CFEFB2FBC4AFF3E3FF4EFADDE616FFF7F7FBF7F83B77CCDFF396FFF7576D61F1F1F03C6F5F5FD6FBB571FBBDEAAABE3EF45ED95B4E7D9D7EDBF6D6F2BC7D70C64BA277CA231D6673FA3FCE7BFFDE96EFEEB57D3B9FFE097FB27F7A6FEE7EAFF7BA7FE5309CBAD870BBEEF89EAAF8C716F1FAEF3FEBE18EFAAAE1BDFCCAEE5647FFD78F8B625A6B9B6FD9D6FF6DD0EC6B7EF9DDB06EBA4FEF9DB44DDCFAF6F327D6D6EC8B1B2F9DF5DCEFBF3AF72BBEEEF78FA650597DE571BABDFF8F55FECFF7A6BFB0AFFF0536B1D2AEABEDC6EFE763A73FBB367FEEC29338DAD19EBAA532FC6BC6B247FAE1BFF7387F396BBBFEBFF7A7A07D0EF6EB6FE7FBD7FC7FEFA24972FD89CEFCF9E99B6E22EF7F3BCBE5FBF8FDF5BFF361D95E5D33BBABDE569FBD2DF3B88BC315ABF" "11833112EEC975CD29D301042FBC4AFF94955E45F88B343A55D5D1152AD6E2290133B5A3CC4645E5D3D3D3CDF5C6C6CEF622CE86DDB8CCCD7E76DC74F3D281BF271DC7996024C8D8FC944AD773BD3FD9C29A06592DE5A7B3CB5B4EF289E3A5BA5438E4349AC2DBD63C38641D6F35F791119D2879F0A3C6C5E7F989389A97D77110AA511FDF33BE1A9B7F0278F849255913E357C83A5C88B46CE245C88EACBE0C32CA17F7EE439CA3FE4EA15DFB7D8135463B579D91361C227789E09D039CF147A7145BD0F70D501C0F759BF0850FFFA3CE478FB0E479B55BD370D6A5E9C2A449EB0F4E9125289690ECFA7F5718BCD2D8691A119ADF2DD67D6913EC6EB97907200180BD7FC7FE05DB40D0D89CEFCF61661C487BA2A6E914F515258A9EAA23B73F291F08898ED6A5371E1308BB8F029773" "11833112EEC975CDA9D301042FBC4AFF94955E45F88B343A55D5D5152AD6E2290133F5A3CC4645E5D3D3D3CFF5C6C6CEF622CE86DDB8CCCD7A76DC74F3D281BF271D4F9D6024C8D8FC944AD773AD3ED9C29A0659BDE5A7B3CB5B4EF289E3A5BA5438E4349AC2DBD63CB8643D6F35F791119D2879E083C6E5E7F989389A97D77110AA511BDF33AE1A9B7F0278F849255913E357C83A5C88A46CE245C88EACBE0E32CA1777EE039CA3FE4EA15DFB7D8135463B579D953614227789E19C039CF147A7145BD0F705501C0F759BF4050FFFA3CE478FB0E479345BD360D6A1E9C2A449EA0F4E9125289690ECFA7F5718BC92D8691A119AFF2DD67D6913EC6EB97907200180BD7FC7FE05DB68D0D89CEFCF61661C487BA2A6E914F515258A0EAA63B73F291F08898ED6A5371E1308BB8F029673" "3387B64C4D67" "652F4FF6BD8A" "F405F3C2" "522BA5D0"
 * 
*/
int main(int argc, char *argv[]){
    srand(time(NULL));
 
    /**
     * argv[0]: Nome del programma eseguibile
     * argv[1]: Dump A
     * argv[2]: Dump B
     * argv[3]: Helper data A
     * argv[4]: Helper data B
     * argv[5]: Ninit A
     * argv[6]: Ninit B
     * argv[7]: ID A
     * argv[8]: ID B
     * argv[9] : chiave A
     * argv[10]: chiave B
    */
    if (argc != 11) {
        printf("Usage: %d <PUF_response>\n", argc);
        return 1;
    }

    uint8_t puf_responseA[PUF_SIZE] = {0U};
    uint8_t puf_responseB[PUF_SIZE] = {0U};
    uint8_t helper_dataA[PUF_SIZE] = {0U};
    uint8_t helper_dataB[PUF_SIZE] = {0U};
    uint8_t device_idA[ID_SIZE] = {0U};
    uint8_t device_idB[ID_SIZE] = {0U};
    uint8_t ninitA[NONCE_SIZE] = {0U};
    uint8_t ninitB[NONCE_SIZE] = {0U};

    // conversione delle string in uint
    hex_string_to_bytes(argv[1], puf_responseA, PUF_SIZE);
    hex_string_to_bytes(argv[2], puf_responseB, PUF_SIZE);
    hex_string_to_bytes(argv[3], helper_dataA, PUF_SIZE);
    hex_string_to_bytes(argv[4], helper_dataB, PUF_SIZE);
    hex_string_to_bytes(argv[5], ninitA, NONCE_SIZE);
    hex_string_to_bytes(argv[6], ninitB, NONCE_SIZE);
    hex_string_to_bytes(argv[7], device_idA, ID_SIZE);
    hex_string_to_bytes(argv[8], device_idB, ID_SIZE);

    // generazioni delle chiavi ki e kj
    uint8_t ki[FE_KEY_SIZE] = {0U};
    uint8_t kj[FE_KEY_SIZE] = {0U};
    
    //puf_extract(puf_responseA, helper_dataA, ki);
    //puf_extract(puf_responseB, helper_dataB, kj);

    hex_string_to_bytes(argv[9], ki, FE_KEY_SIZE);
    hex_string_to_bytes(argv[10], kj, FE_KEY_SIZE);

    // generazione Ns-a e Ns-b
    uint8_t nonceSA[NONCE_SIZE] = {0U};
    uint8_t nonceSB[NONCE_SIZE] = {0U};

    genera(nonceSA,NONCE_SIZE);
    genera(nonceSB,NONCE_SIZE);

    // generazioni PRK_a e PRK_b
    uint8_t PRK_A[FE_KEY_SIZE]  = {0U};
    uint8_t PRK_B[FE_KEY_SIZE]  = {0U};

    hkdf(SHA256, NULL, 0, ki, FE_KEY_SIZE, ninitA, NONCE_SIZE, PRK_A, FE_KEY_SIZE);
    hkdf(SHA256, NULL, 0, kj, FE_KEY_SIZE, ninitB, NONCE_SIZE, PRK_B, FE_KEY_SIZE);

    // inizio creazione messaggio msg_b
    uint8_t msg_B[MSG_B_SIZE];
    memcpy(msg_B, device_idA, ID_SIZE);
    memcpy(msg_B + ID_SIZE, device_idB, ID_SIZE);
    memcpy(msg_B + ID_SIZE + ID_SIZE, ninitB, NONCE_SIZE);
    memcpy(msg_B + ID_SIZE + ID_SIZE + NONCE_SIZE, nonceSA, NONCE_SIZE);
    memcpy(msg_B + ID_SIZE + ID_SIZE + NONCE_SIZE + NONCE_SIZE, nonceSB, NONCE_SIZE);
    
    Encrypt(msg_B, PRK_B);

    // inizio creazione msg_a
    uint8_t msg_A[MSG_A_SIZE];
    memcpy(msg_A, device_idA, ID_SIZE);
    memcpy(msg_A + ID_SIZE, device_idB, ID_SIZE);
    memcpy(msg_A + ID_SIZE + ID_SIZE, ninitA, NONCE_SIZE);
    memcpy(msg_A + ID_SIZE + ID_SIZE + NONCE_SIZE, nonceSA, NONCE_SIZE);
    memcpy(msg_A + ID_SIZE + ID_SIZE + NONCE_SIZE + NONCE_SIZE, nonceSB, NONCE_SIZE);
    memcpy(msg_A + ID_SIZE + ID_SIZE + NONCE_SIZE + NONCE_SIZE + NONCE_SIZE, msg_B, MSG_B_SIZE);

    // cifratura del messaggio msg_a con la chiave del primo device
    Encrypt(msg_A, PRK_A);
    
    // Concatenazioni informazioni msg_A, ki e Ns-a per creare il mac
    uint8_t mConc[MSG_A_CONC_SIZE];
    memcpy(mConc, msg_A, MSG_A_SIZE);
    memcpy(mConc + MSG_A_SIZE, ki, FE_KEY_SIZE);
    memcpy(mConc + MSG_A_SIZE + FE_KEY_SIZE, nonceSA, NONCE_SIZE);
  
    // Lunghezza digest 64 per sha256
    uint8_t digest[DIGEST_SIZE];

    hmac(SHA256, mConc, MSG_A_CONC_SIZE, PRK_A, FE_KEY_SIZE, digest);

    // Ottieni il percorso completo con il suffisso "_prk_A.puf"
    char* path_prkA = append_prk_suffix(device_idA, ID_SIZE);
    char* path_prkB = append_prk_suffix(device_idB, ID_SIZE);
    char* path_nonceSB = append_nonceSB_suffix(device_idB, ID_SIZE);

    write_buffer_to_file(path_prkA, PRK_A, FE_KEY_SIZE);
    write_buffer_to_file(path_prkB, PRK_B, FE_KEY_SIZE);
    write_buffer_to_file(path_nonceSB, nonceSB, NONCE_SIZE);
    
    
    print_buffer_as_json("NinitA", ninitA, NONCE_SIZE,
                            "Ns_a", nonceSA, NONCE_SIZE,
                            "Ns_b", nonceSB, NONCE_SIZE,
                            "MA", msg_A, MSG_A_SIZE,
                            "MB", msg_B, MSG_B_SIZE,
                            "digest", digest, DIGEST_SIZE,
                            NULL);
}