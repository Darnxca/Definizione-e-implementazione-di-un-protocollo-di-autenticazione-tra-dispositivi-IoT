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
 * ./serverA_verifica_mac "EE7CCEEDEEC975CDD62CFEFB2FBC4AFF3E3FF4EFADDE616FFF7F7BBF7F83B77CCDFF796FFF7576D61F1F1F01C6F5F5FD6FBB571FBBDEAAABE7EF45ED95B4E7D9D7ED37696F2BC7D70C64BA277CB230D6673FA3FC77BFFDE96EFEEB57D3B9FFE097FB27F7A6FEE7EAFFFBA7DE5309CBAD870BBEEF99CAAFAC716F1FAEF3FEBE18EFAAAE1FDFCCBEE5647FFD78F8B625A6B9B6FD9D6FF6DD1EC6B7EF9DDB06EBA6FEF9DBC4DD8FAF6F327D6D6EC8B1B2F9DF5DCEFBF7AF7ABBEEEF79FB650597DE571BABDFF8FD5FECFF7A6BFF8AFFF0536B1D2AEABEDCEFFE762A73FFB367FEEC28338DAD19EBAA532FC6BC6B247FEE1BFF7387F3B6BBBFEBFF7A7A07D0EF6EB6FE7FBD7FC7FEFA24BF2FD89CEFCF9E99B6E22EF7F3BCBE5FBF8FDFCBFF761D95E5D33BBABDE569FBD2DF3B88BC315BBF" "11833112EEC975CD29D301042FBC4AFF94955E45F88B343A55D5D1152AD6E2290133B5A3CC4645E5D3D3D3CDF5C6C6CEF622CE86DDB8CCCD7E76DC74F3D281BF271DC7996024C8D8FC944AD773BD3FD9C29A06592DE5A7B3CB5B4EF289E3A5BA5438E4349AC2DBD63C38641D6F35F791119D2879F0A3C6C5E7F989389A97D77110AA511FDF33BE1A9B7F0278F849255913E357C83A5C88B46CE245C88EACBE0C32CA17F7EE439CA3FE4EA15DFB7D8135463B579D91361C227789E09D039CF147A7145BD0F70D501C0F759BF0850FFFA3CE478FB0E479B55BD370D6A5E9C2A449EB0F4E9125289690ECFA7F5718BCD2D8691A119ADF2DD67D6913EC6EB97907200180BD7FC7FE05DB40D0D89CEFCF61661C487BA2A6E914F515258A9EAA23B73F291F08898ED6A5371E1308BB8F029773" "F405F3C2" "3387B64C4D67" "51F382248294" "3168424419E2" "18605262289CD656594ABC71CF22D558" "4BC93050B7475D45CD41123AAD4FF32FEB43E416D31167347EF3DFC8A0FF71AF"
 * 
 * 
 * 
*/
int main(int argc, char *argv[]){
    srand(time(NULL));
 
    /**
     * argv[0]: Nome del programma eseguibile
     * argv[1]: Dump A
     * argv[2]: Helper data A
     * argv[3]: ID A
     * argv[4]: Ninit A
     * argv[5]: Nonce SA
     * argv[6]: Nonce SB
     * argv[7]: msg_SA
     * argv[8]: digest
     * argv[9]: chiaveA
    */
    if (argc != 10) {
        printf("Usage: %d <PUF_response>\n", argc);
        return 1;
    }
    
    uint8_t puf_responseA[PUF_SIZE] = {0U};
    uint8_t helper_dataA[PUF_SIZE] = {0U};
    uint8_t device_idA[4U] = {0U};
    uint8_t ninitA[6];
    uint8_t nSA[6];
    uint8_t nSB[6];

    // conversione delle string in uint
    hex_string_to_bytes(argv[1], puf_responseA, PUF_SIZE);
    hex_string_to_bytes(argv[2], helper_dataA, PUF_SIZE);
    hex_string_to_bytes(argv[4], ninitA, sizeof(ninitA));
    hex_string_to_bytes(argv[5], nSA, sizeof(nSA));
    hex_string_to_bytes(argv[6], nSB, sizeof(nSB));

    uint8_t ki[16U] = {0U};
    //puf_extract(puf_responseA, helper_dataA, ki);
    hex_string_to_bytes(argv[9], ki, FE_KEY_SIZE);
    // generazione PRK_a
    uint8_t PRK_A[16U];

    hkdf(SHA256, NULL, 0, ki, FE_KEY_SIZE, ninitA, sizeof(ninitA), PRK_A, FE_KEY_SIZE);
    

    // decriptiamo il messaggio
    size_t len_msg_SA = strlen(argv[7])/2; 
    uint8_t msg_SA[len_msg_SA];
    uint8_t old_digest[32U];

    hex_string_to_bytes(argv[7], msg_SA, sizeof(msg_SA));
    hex_string_to_bytes(argv[8], old_digest, sizeof(old_digest));

    Decrypt(msg_SA, PRK_A);

    uint8_t nonceA[6];
    memcpy(nonceA, msg_SA + 10,6);
    
    // cifratura del messaggio msg_a con la chiave del primo device
    Encrypt(msg_SA, PRK_A);
    
    // Concatenazioni informazioni msg_SA, Ns-a per creare il  nuovo mac
    uint8_t mConc[sizeof(msg_SA) + sizeof(nonceA)];
    for (int i = 0; i < sizeof(msg_SA); i++){
        mConc[i] = msg_SA[i];
    }
    for (int i = 0; i < sizeof(nonceA); i++){
        mConc[i + sizeof(msg_SA)] = nonceA[i];
    }

    uint8_t new_digest[32U];

    hmac(SHA256, mConc, sizeof(mConc), PRK_A, sizeof(PRK_A), new_digest);

    // Compara mac
    char * success = "true";
    for (uint8_t i = 0U; (i < 32U); i++){
        if(new_digest[i] != old_digest[i]) {
            success = "false";
            break;
        }
    }

    printf("%s", success);

    return 0;
}

