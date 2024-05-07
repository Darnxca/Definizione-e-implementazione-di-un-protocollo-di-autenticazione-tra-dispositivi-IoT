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
    0x03, 0x46, 0x02, 0x48, 0xDB, 0x57, 0xF7, 0xE6, 0x28, 0x0E, 0x30, 0x64, 0xD7, 0xBA, 0xFD, 0x57, 0xDB, 0x55, 0x1C, 0x74, 0xBF, 0x81, 0xA1, 0xD8, 0x55, 0xD4, 0x78, 0x95, 0xA4, 0xD0, 0xAF, 0x0E, 0x37, 0x2A, 0x53, 0x72, 0xFC, 0xCC, 0x18, 0xC8, 0xAB, 0x36, 0x11, 0xA3, 0xAE, 0xEC, 0xCE, 0x2C, 0x03, 0x66, 0xCC, 0x02, 0x1C, 0x09, 0x9B, 0x05, 0x65, 0x6E, 0xE4, 0x62, 0x19, 0x88, 0xE9, 0x95, 0x6F, 0x87, 0x0F, 0x03, 0xF4, 0x70, 0xFC, 0x4B, 0x8F, 0x2A, 0x3A, 0x8F, 0xF0, 0xD1, 0xB1, 0x71, 0xFE, 0x5B, 0x1A, 0x65, 0x3C, 0xE5, 0x09, 0xA6, 0x59, 0x50, 0x26, 0x11, 0x92, 0x61, 0xAD, 0xE4, 0xBF, 0x7D, 0x76, 0x7C, 0xC1, 0xD2, 0x27, 0x52, 0xFC, 0x1D, 0x7C, 0x7D, 0x47, 0xC2, 0x43, 0xCA, 0x21, 0x69, 0x4A, 0x6B, 0x17, 0x96, 0x0E, 0x96, 0x79, 0x7B, 0x10, 0x69, 0xF6, 0x81, 0xD6, 0x94, 0x61, 0xFF, 0x08, 0x7F, 0xC0, 0x00, 0xCF, 0x1C, 0x00, 0xB2, 0x00, 0x2F, 0x7F, 0xA8, 0x77, 0x00, 0x5D, 0xCC, 0x5D, 0xAA, 0xEC, 0x11, 0xAB, 0x51, 0xD4, 0x2A, 0x12, 0xCB, 0xB2, 0x55, 0xA9, 0xD1, 0xA4, 0xDD, 0x32, 0x6E, 0x88, 0x33, 0xE9, 0x33, 0x03, 0x21, 0x12, 0xC6, 0x54, 0x73, 0xCC, 0xBA, 0xD6, 0x69, 0x62, 0x68, 0x90, 0x60, 0x99, 0xE6, 0x7B, 0x9C, 0x64, 0x91, 0x89, 0x25, 0xB8, 0x7E, 0x2E, 0x4A, 0x07, 0xC0, 0xB2, 0x0B, 0xF2, 0x0E, 0x1E, 0xF0, 0x4F, 0xD1, 0xB0, 0xAF, 0xF0, 0x0F, 0xF9, 0xA3, 0x6C, 0xB5, 0xA5, 0x5F, 0x25, 0x5A, 0x4E, 0xA3, 0x50, 0x91, 0xA4, 0x4D, 0xA1, 0x3A, 0x38, 0x42, 0x38, 0xC3, 0x47, 0x9A, 0xA2, 0x36, 0x28, 0xC0, 0xBA, 0x83, 0xE3, 0xFE, 0xD3, 0x7C, 0x4B, 0xB6, 0x7D, 0xB4, 0x86, 0x79, 0x32, 0x2C, 0x6F, 0x9A, 0xE1, 0x96, 0x96, 0x68, 0x82, 0x71, 0x20, 0xB0, 0xFB, 0xFE, 0x73, 0x8A, 0x20, 0x00, 0x80, 0x3C, 0x96, 0xD7, 0xBE, 0x97, 0x20, 0x10, 0x4B, 0x0D, 0xA2, 0x87, 0xCC, 0x54, 0x1C, 0x35, 0x3D, 0x56, 0xAA, 0x3A, 0x6A, 0xEA, 0x51, 0x0F, 0x46, 0x32, 0x67, 0xDF, 0xA7, 0x89, 0x11, 0x05, 0x75, 0x99, 0xDC, 0x85, 0x98, 0xD8, 0x08, 0xFB
};


int main(void){

    //srand(time(NULL));
    uint8_t ch = 0;
    uint8_t id_B[4U] = {0U};
    uint8_t id_A[4U] = {0U};

    uint8_t key[FE_KEY_SIZE] = {0U};

    uint8_t ninitB[NONCE_SIZE] = {0U};
    uint8_t nonceSB[NONCE_SIZE] = {0U};
    uint8_t nonceAB[NONCE_SIZE] = {0U};
    uint8_t nonceB[NONCE_SIZE] = {0U};

    uint8_t PRK_B[FE_KEY_SIZE] = {0U};

    uint8_t msg_B[26] = {0U};
    uint8_t msg_SA[MSG_SA_SIZE] = {0U};
    uint8_t msg_BA[MSG_BA_SIZE] = {0U};
    uint8_t msg_nonceAB[MSG_BA_SIZE] = {0U};
    uint8_t msg_serverB[MSG_SB_SIZE] = {0U};

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
    		usart_rx_buffer(id_B, 4U);
        }
        if ('p' == ch){
            	usart_tx_buffer(bss_puf, PUF_SIZE);
        }
        if ('k' == ch) {
        	 // genero la chiave ki
    		puf_extract(bss_puf, helper_data, key);
    		usart_tx_buffer(key, FE_KEY_SIZE);
        }
        if('n' == ch) {
            	// genero il nonce di inizializzazione ninitB
            	gen(ninitB,NONCE_SIZE);
    		    usart_tx_buffer(ninitB, NONCE_SIZE);
    		
    		// generazione PRK
		hkdf(SHA256, NULL, 0, key, FE_KEY_SIZE, ninitB, NONCE_SIZE, PRK_B, FE_KEY_SIZE);
        }
        if('b' == ch){
            usart_rx_buffer(msg_B, 26);
            usart_rx_buffer(old_digest, DIGEST_SIZE);

            Decrypt(msg_B, PRK_B);

            memcpy(nonceSB, msg_B + 20,6);
            
            // Verifico ora il digest di msg_b
            Encrypt(msg_B, PRK_B);

            uint8_t tempMac[MSG_B_CONC_SIZE] = {0U};
            memcpy(tempMac, msg_B, MSG_B_SIZE);
            memcpy(tempMac + MSG_B_SIZE, nonceSB, NONCE_SIZE);

            
            hmac(SHA256, tempMac, MSG_B_CONC_SIZE, nonceSB, NONCE_SIZE, new_digest);

            // Compara mac
            for (uint8_t i = 0U; (i < 32U); i++){
                if(new_digest[i] != old_digest[i]) {
                    usart_tx_string("I mac sono  diversi.\r\n");
                    exit(EXIT_FAILURE);
                }
            }

            free(msg_B);
            free(old_digest);
            free(new_digest);
            free(tempMac);

            usart_tx_string("successo\r\n");
        }
        if ('c' == ch){
            usart_rx_buffer(msg_BA, MSG_BA_SIZE);
            usart_rx_buffer(id_A, 4U);
            usart_rx_buffer(msg_nonceAB, MSG_BA_SIZE);
            usart_rx_buffer(old_digest, DIGEST_SIZE);
            
            Decrypt(msg_nonceAB, PRK_B);

            memcpy(nonceAB, msg_nonceAB + 10,6);
            
            Encrypt(msg_nonceAB, PRK_B);
            
            // Sezione creazione mac msg_BA || NAB
            memcpy(tempMac, msg_nonceAB, MSG_BA_SIZE);
            memcpy(tempMac + MSG_BA_SIZE, nonceAB, NONCE_SIZE);

            // Lunghezza digest 32 per sha256
            hmac(SHA256, tempMac, MSG_BA_CONC_SIZE, nonceSB, NONCE_SIZE, new_digest);

            // Compara mac
            for (uint8_t i = 0U; (i < 32U); i++){
                if(new_digest[i] != old_digest[i]) {
                    usart_tx_string("I mac sono 2 diversi.\n");
                    exit(EXIT_FAILURE);
                }
            }

            free(msg_BA);
            free(old_digest);
            free(new_digest);
            free(msg_nonceAB);
            free(tempMac);

            gen(nonceB,NONCE_SIZE);
            // Generazione messaggio server B
            memcpy(msg_serverB, id_B, ID_SIZE);
            memcpy(msg_serverB + ID_SIZE, nonceSB, NONCE_SIZE);
            memcpy(msg_serverB + ID_SIZE + NONCE_SIZE, nonceB, NONCE_SIZE);
            
            Encrypt(msg_serverB, PRK_B);

            // Sezione creazione mac msg_BA || NAB
            memcpy(tempMac, msg_serverB, MSG_SB_SIZE);
            memcpy(tempMac + MSG_SB_SIZE, nonceB, NONCE_SIZE);

            // Lunghezza digest 32 per sha256
            hmac(SHA256, tempMac, MSG_SB_CONC_SIZE, nonceB, NONCE_SIZE, new_digest);
            
            usart_tx_buffer(msg_serverB, MSG_SB_SIZE);
            usart_tx_buffer(new_digest, DIGEST_SIZE);

            free(tempMac);
            free(msg_serverB);
            free(new_digest);
        }
        if('s' == ch) {
            memcpy(tempMac, id_A, ID_SIZE);
            memcpy(tempMac + ID_SIZE, id_B, ID_SIZE);
            memcpy(tempMac + ID_SIZE + ID_SIZE, nonceAB, NONCE_SIZE);

            hmac(SHA256, tempMac, CONF_AUT_SIZE, nonceSB, NONCE_SIZE, new_digest);

            usart_tx_buffer(new_digest, DIGEST_SIZE);
            
            free(tempMac);
            free(new_digest);
                   
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
