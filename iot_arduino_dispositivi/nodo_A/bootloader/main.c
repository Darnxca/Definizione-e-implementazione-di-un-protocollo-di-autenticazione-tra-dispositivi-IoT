#include "usart.h"
// #include "stk500.h"
#include <avr/pgmspace.h>
#include <avr/io.h>
#include "puf.h"
//#include <time.h>
#include "utils_arduino.h"
#include "hkdf.h"
#include "cipher.h"

#define BAUD_RATE 115200UL
#define BAUD_SETTING ((F_CPU / (8UL * BAUD_RATE)) - 1UL)

unsigned const int __attribute__((section(".version"))) optiboot_version = 2049U;
uint8_t __attribute__((section(".noinit"))) bss_puf[PUF_SIZE];

/** Functions Prototypes */
void __init0(void) __attribute__((naked)) __attribute__((section(".init0")));
void __init1(void) __attribute__((naked)) __attribute__((section(".init1")));
void __start(void)  __attribute__((section(".startup")));

const uint8_t helper_data[304] = {
    0x98, 0xB0, 0xE5, 0x00, 0x6B, 0xA6, 0x7A, 0xF7, 0x87, 0xD2, 0x45, 0x22, 0x0C, 0xA5, 0x7F, 0xDF, 0x57, 0x05, 0x5F, 0x5D, 0x2A, 0xAC, 0x81, 0x2C, 0x14, 0x55, 0x34, 0x1D, 0xAA, 0x82, 0xCA, 0x03, 0x79, 0x33, 0x33, 0xF4, 0x88, 0xEC, 0xAC, 0xD6, 0x36, 0x72, 0xBB, 0x33, 0xF4, 0x8B, 0xF8, 0x54, 0x66, 0xEE, 0x74, 0x22, 0xC8, 0xDB, 0x98, 0x90, 0x76, 0x77, 0x66, 0xA0, 0xD5, 0xA1, 0xDE, 0x98, 0x04, 0xE7, 0xC1, 0x4F, 0x73, 0xF6, 0xF9, 0xE8, 0x0F, 0x47, 0xA4, 0x1D, 0xD8, 0xE5, 0x86, 0xF8, 0x22, 0x5A, 0x7A, 0x0A, 0x84, 0xA5, 0xAD, 0xE1, 0x7E, 0x5E, 0x78, 0x4A, 0xEA, 0xFD, 0x22, 0xA5, 0x32, 0xB2, 0x2D, 0x2C, 0xA2, 0x71, 0x8B, 0xC3, 0x9F, 0x34, 0x3B, 0x9C, 0xC1, 0xC7, 0x93, 0x03, 0x5D, 0xCF, 0x4E, 0xEF, 0x87, 0x12, 0xC4, 0x9E, 0xED, 0x41, 0x64, 0x2B, 0xDB, 0xC3, 0xC6, 0xF7, 0x48, 0x27, 0xA1, 0x4F, 0x5B, 0x14, 0x2D, 0x20, 0x5A, 0x5E, 0x0E, 0x7B, 0x79, 0xB0, 0xEE, 0x02, 0x15, 0x2D, 0x99, 0xE3, 0x3A, 0x8D, 0xA0, 0x75, 0x45, 0xAC, 0x49, 0x87, 0xF6, 0x15, 0xAA, 0x14, 0xF8, 0x5C, 0x23, 0xBA, 0x7A, 0xBE, 0xE5, 0x11, 0x21, 0xF8, 0x03, 0xD8, 0xF8, 0x33, 0xC8, 0x08, 0x44, 0xDD, 0x77, 0xD9, 0x9B, 0x43, 0x2B, 0xEC, 0xB6, 0xCD, 0x3E, 0x31, 0x39, 0xFA, 0x99, 0x06, 0xC7, 0xE0, 0xCA, 0xE4, 0xFA, 0x4D, 0xF9, 0x04, 0xBF, 0xB0, 0x9E, 0xB4, 0xB6, 0x1F, 0xB0, 0xA6, 0x4A, 0x84, 0x4A, 0xE6, 0xF2, 0x5B, 0xA2, 0x7A, 0x5E, 0x94, 0x4A, 0xA5, 0x21, 0x43, 0xA8, 0x5D, 0xBE, 0xD9, 0xFA, 0xC3, 0xEA, 0x29, 0x83, 0x79, 0xBC, 0x73, 0x20, 0xCB, 0xA2, 0x08, 0xE0, 0x60, 0xEB, 0x14, 0x5E, 0x54, 0x36, 0x2A, 0x02, 0x69, 0xEB, 0x13, 0x59, 0x1F, 0x55, 0x09, 0x8F, 0x09, 0x8D, 0x90, 0xFA, 0x7D, 0x8E, 0x9F, 0x23, 0x04, 0x25, 0xC4, 0xA6, 0xDB, 0x6F, 0x7E, 0x1C, 0x20, 0x54, 0x0E, 0x82, 0xE8, 0x90, 0xAC, 0x5A, 0x15, 0x77, 0x04, 0xCD, 0xA6, 0xBA, 0xBA, 0x4D, 0x51, 0x33, 0x31, 0x18, 0xC6, 0x68, 0xCC, 0x73, 0x78, 0x61, 0x33, 0xCC, 0xC4, 0xC6, 0x94, 0x21, 0x96
};

int main(void){

    //srand(time(NULL));
    uint8_t ch = 0;
    uint8_t id_A[4U] = {0U};
    uint8_t id_B[4U] = {0U};
    uint8_t key[FE_KEY_SIZE] = {0U};
    uint8_t ninitA[NONCE_SIZE] = {0U};
    uint8_t nonceA[NONCE_SIZE] = {0U};
    uint8_t nonceSA[NONCE_SIZE] = {0U};
    uint8_t nonceSB[NONCE_SIZE] = {0U};
    uint8_t nonceAB[NONCE_SIZE] = {0U};

    uint8_t msg_B[26] = {0U};
    uint8_t msg_A[MSG_A_SIZE] = {0U};
    uint8_t msg_SA[MSG_SA_SIZE] = {0U};
    uint8_t msg_BA[MSG_BA_SIZE] = {0U};

    uint8_t PRK_A[FE_KEY_SIZE] = {0U};
  
    uint8_t old_digest[DIGEST_SIZE] = {0U};
    uint8_t new_digest[DIGEST_SIZE] = {0U};
    uint8_t tempMac[100] = {0U};

    // Initialize Timer/Counter for millis
    initMillisTimer();
    //Initialize random seed
    initRandomSeed();
    
    for (;;){
        /* read a character from the console */
        ch = usart_rx_ch();

        /* IF you type 'p' you'll get the puf */
        if ('r' == ch){
            	usart_tx_buffer(helper_data, PUF_SIZE);
    		usart_rx_buffer(id_A, 4U);
        }
        if ('p' == ch){
            	usart_tx_buffer(bss_puf, PUF_SIZE);
        }
        if ('k' == ch) {
        	 // genero la chiave ki
    		puf_extract(bss_puf, helper_data, key);
    		usart_tx_buffer(key, FE_KEY_SIZE);
        }
        if ('n' == ch) {
            	// genero il nonce di inizializzazione ninitA
            	gen(ninitA,NONCE_SIZE);
    		usart_tx_buffer(ninitA, NONCE_SIZE);

		// generazione PRK
		hkdf(SHA256, NULL, 0, key, FE_KEY_SIZE, ninitA, NONCE_SIZE, PRK_A, FE_KEY_SIZE);
        }
        if ('v' == ch) {
            usart_rx_buffer(msg_A, MSG_A_SIZE);
            usart_rx_buffer(old_digest, DIGEST_SIZE);

            Decrypt(msg_A, PRK_A);

            memcpy(id_B, msg_A +4,4);
            memcpy(nonceSA, msg_A + 14,6);
            memcpy(nonceSB, msg_A + 20,6);
            memcpy(msg_B, msg_A + 26,26);

            // cifratura del messaggio msg_a con la chiave del primo device
            Encrypt(msg_A, PRK_A);
            
            // Concatenazioni informazioni msg_A, ki e Ns-a per creare il  nuovo mac
            memcpy(tempMac, msg_A, MSG_A_SIZE);
            memcpy(tempMac + MSG_A_SIZE, key, FE_KEY_SIZE);
            memcpy(tempMac + MSG_A_SIZE + FE_KEY_SIZE, nonceSA, NONCE_SIZE);
            
            hmac(SHA256, tempMac, MSG_A_CONC_SIZE, PRK_A, FE_KEY_SIZE, new_digest);

            // Compara mac
            for (uint8_t i = 0U; (i < 32U); i++){
                if(new_digest[i] != old_digest[i]) {
                    //usart_tx_string("I mac sono  diversi\r\n");
                    usart_tx_string("mac diversi\r\n");
                    exit(EXIT_FAILURE);
                }
            }

            free(new_digest);
            free(old_digest);
            free(tempMac);
            
            gen(nonceA,NONCE_SIZE);

            // Generazione messaggio SA e cifratura con PRK_A
            memcpy(msg_SA, id_A, ID_SIZE);
            memcpy(msg_SA + ID_SIZE, nonceSA, NONCE_SIZE);
            memcpy(msg_SA + ID_SIZE + NONCE_SIZE, nonceA, NONCE_SIZE);
            Encrypt(msg_SA, PRK_A);
            
            // Sezione Server Verifica il MAC
            memcpy(tempMac, msg_SA, MSG_SA_SIZE);
            memcpy(tempMac + MSG_SA_SIZE, nonceA, NONCE_SIZE);
            
            // Lunghezza digest 32 per sha256
            hmac(SHA256, tempMac, MSG_SA_CONC_SIZE, PRK_A, FE_KEY_SIZE, new_digest);

            usart_tx_buffer(msg_SA, MSG_SA_SIZE);
            usart_tx_buffer(new_digest, DIGEST_SIZE);
            usart_tx_buffer(msg_B, MSG_B_SIZE);
            
            free(msg_SA);
            free(tempMac);
            free(new_digest);
        }
    
        if ('b' == ch) {
             // Sezione creazione mac msg_B || NSB
                memcpy(tempMac, msg_B, MSG_B_SIZE);
                memcpy(tempMac + MSG_B_SIZE, nonceSB, NONCE_SIZE);

                // Lunghezza digest 32 per sha256
                hmac(SHA256, tempMac, MSG_B_CONC_SIZE, nonceSB, NONCE_SIZE, old_digest);
            
                usart_tx_buffer(old_digest, DIGEST_SIZE);

                free(old_digest);

                // Generazione messaggio BA e cifratura con N_SB
                gen(nonceAB,NONCE_SIZE);

                memcpy(msg_BA, id_A, ID_SIZE);
                memcpy(msg_BA + ID_SIZE, nonceA, NONCE_SIZE);
                memcpy(msg_BA + ID_SIZE + NONCE_SIZE, nonceAB, NONCE_SIZE);

                Encrypt(msg_BA, PRK_A);

                // Sezione creazione mac msg_BA || NAB
                memcpy(tempMac, msg_BA, MSG_BA_SIZE);
                memcpy(tempMac + MSG_BA_SIZE, nonceAB, NONCE_SIZE);

                // Lunghezza digest 32 per sha256
                hmac(SHA256, tempMac, MSG_BA_CONC_SIZE, nonceSB, NONCE_SIZE, old_digest);

                usart_tx_buffer(msg_BA, MSG_BA_SIZE);
                usart_tx_buffer(old_digest, DIGEST_SIZE);

                free(msg_BA);
                free(old_digest);
        }
        if ('c' == ch) {
            usart_rx_buffer(old_digest, DIGEST_SIZE);

            memcpy(tempMac, id_A, ID_SIZE);
            memcpy(tempMac + ID_SIZE, id_B, ID_SIZE);
            memcpy(tempMac + ID_SIZE + ID_SIZE, nonceAB, NONCE_SIZE);
        
            hmac(SHA256, tempMac, CONF_AUT_SIZE, nonceSB, NONCE_SIZE, new_digest);
            
            // Compara mac
            for (uint8_t i = 0U; (i < 32U); i++){
                if(new_digest[i] != old_digest[i]) {
                    usart_tx_string("I mac sono  diversi.\r\n");
                    exit(EXIT_FAILURE);
                }
            }

            free(old_digest);
            free(new_digest);
            free(tempMac);

            usart_tx_string("successo\r\n");
        }
        if('q' == ch) {
            usart_tx_string("Autenticazione fallita\r\n");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

void __init0(void){
    /* Reset __zero_reg__ (r1) */
    asm volatile("clr __zero_reg__");

    // setta lo stack pointer a RAMEND (0x8FF)
    asm volatile(
        "ldi r16, 0x08\n\t"
        "out __SP_H__,r16\n\t"
        "ldi r16, 0xff\n\t"
        "out __SP_L__,r16\n\t"
    );

    usart_init((unsigned int)BAUD_SETTING);

    usart_tx_buffer(bss_puf, PUF_SIZE);
}

void __init1(void){
   
}

void __start(void){
    __asm volatile("jmp __vectors");
}
