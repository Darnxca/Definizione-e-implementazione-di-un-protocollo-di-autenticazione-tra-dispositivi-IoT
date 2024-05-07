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

#define S_MAC 32

unsigned const int __attribute__((section(".version"))) optiboot_version = 2049U;
uint8_t __attribute__((section(".noinit"))) bss_puf[PUF_SIZE];

/** Functions Prototypes */
void __init0(void) __attribute__((naked)) __attribute__((section(".init0")));
void __init1(void) __attribute__((naked)) __attribute__((section(".init1")));
void __start(void)  __attribute__((section(".startup")));



int main(void)
{
    //srand(time(NULL));
    uint8_t ch = 0;
    uint8_t key[16U] = {0U};
    uint8_t id[4U] = {0U};
    id[3] = 1U;
    uint8_t prk[16U];
    // Da generare randomicamente
    uint8_t ninit[6] = {0U};
    int result;
    
    char errore[]="H err\n";
    
    // Initialize Timer/Counter for millis
    		initMillisTimer();

    		// Initialize random seed
    		initRandomSeed();

    for (;;)
    {
        /* read a character from the console */
        ch = usart_rx_ch();

        /* IF you type 'p' you'll get the puf */
        if ('p' == ch)
        {
            usart_tx_buffer(bss_puf, PUF_SIZE);
        }
        else if (ch == 'k')
        {
            usart_tx_buffer(key, 16U);
        }
        else if (ch == 'f')
        {
            //esegui il FE
            //usart_tx_string("eseguo FE\n\r");
            puf_extract(bss_puf, key);
            //usart_tx_string("fine del FE\n\r");
            usart_tx_buffer(key, 16U);
        }
        else if (ch == 'i')
        {
        	usart_tx_string("A-B?");
        	ch = usart_rx_ch();
        	if (ch == 'b')
        		id[3] = 2U;
           	usart_tx_string("ID:");
    		usart_tx_buffer(id, sizeof(id));
        }
        /////////////////////////////////////////////////////////////////////////////////////////Nodo A
    	else if (ch == 'h' && id[3] == 1U)
    	{
    		gen(ninit,sizeof(ninit));
    		
    		//Da inviare al nodo B
    		usart_tx_string("Ninit_A: ");
    		usart_tx_buffer(ninit,sizeof(ninit));
    		
    		// Calcolo PRK
    		result = hkdf(SHA256, NULL, 0, key, FE_KEY_SIZE, ninit, sizeof(ninit), prk, FE_KEY_SIZE);
    		// Check for errors
    		if (result != 0)
    		{
    		    usart_tx_string(errore);
    		    return 1;
    		}
    		//usart_tx_string("INFO:PRK: ");
    		usart_tx_buffer(prk,sizeof(prk));
    		
    		//Inserisci messaggio e mac
    		uint8_t *mA = (uint8_t *)malloc(96 * sizeof(uint8_t));
    		//uint8_t mA[96] = {0U};
    		usart_tx_string("Inserisci M_A \n");
    		usart_rx_buffer(mA,96);
    		//usart_tx_string("M_A inserito: ");
    		usart_tx_buffer(mA,96);

    		//uint8_t oldmacA[32U];
    		uint8_t *oldmacA = (uint8_t *)malloc(S_MAC * sizeof(uint8_t));
    		usart_tx_string("Inserisci Mac_A:\n");
    		usart_rx_buffer(oldmacA,S_MAC);
    		//usart_tx_string("Mac inserito: ");
    		usart_tx_buffer(oldmacA,S_MAC);
    		
    		//uint8_t oldmacB[32U];
    		uint8_t *oldmacB = (uint8_t *)malloc(S_MAC * sizeof(uint8_t));
    		usart_tx_string("Inserisci Mac_B:\n");
    		usart_rx_buffer(oldmacB,S_MAC);
    		//usart_tx_string("Mac inserito: ");
    		usart_tx_buffer(oldmacB,S_MAC);
    		
    		
    		uint8_t nsA[6] = {0U};
    		uint8_t nsB[16] = {0U};
    		uint8_t *mB = (uint8_t *)malloc(48 * sizeof(uint8_t));
    		//uint8_t mB[48] = {0U};
    		
    		//Decifra messaggio
    		DecryptN(mA,96, prk);
    		//usart_tx_string("Decripted M_A: ");
    		usart_tx_buffer(mA,96);
    		
    		uint8_t offset=0;
    		//Check per ID e Ninit
    		for (int i = 0; i < sizeof(id); i++)
    		{
     			if(mA[i]!=id[i]){
    		        usart_tx_string("ERR ID_A\n");
 //           		return -1;
            		}
    		}
    		offset+=sizeof(id);
    		uint8_t idB[4] = {0U};
    		//Estraggo idB
    		for (int i = 0; i < sizeof(idB); i++)
    		{
        		idB[i] = mA[i + offset];
    		}
    		offset+= sizeof(idB);
    		for (int i = 0; i < sizeof(ninit); i++)
    		{
        		if(mA[i+offset]!=ninit[i]){
            		usart_tx_string("ERR Ninit_A\n");
//            		return -1;
            		}
    		}
    		offset+= sizeof(ninit);
    		//Estraggo nsA
    		for (int i = 0; i < sizeof(nsA); i++)
    		{
        		nsA[i] = mA[i + offset];
    		}
    		offset+= sizeof(nsA);
    		//Estraggo nsB
    		for (int i = 0; i < sizeof(nsB); i++)
    		{
        		nsB[i] = mA[i + offset];
    		}
    		offset+= sizeof(nsB);
    		//Estraggo mB
    		for (int i = 0; i < 48; i++)
    		{
        		mB[i] = mA[i + offset];
    		}
    		
    		
    		
    		
    // MAC A
    // Creare un nuovo mac con gli stessi dati e compararlo (cifrare prima i dati del messaggio)
    EncryptN(mA,96,prk);
    
    uint8_t *pmacA= (uint8_t *)malloc((sizeof(id) + 96 + sizeof(key) + sizeof(nsA)) * sizeof(uint8_t));
    //uint8_t pmacA[sizeof(id) + 96 + sizeof(key) + sizeof(nsA)];
    offset = 0;
    memcpy(pmacA + offset, id, sizeof(id));
    offset+=sizeof(id);
    memcpy(pmacA + offset, mA, 96);
    offset+=96;
    memcpy(pmacA + offset, key, sizeof(key));
    offset+=sizeof(key);
    memcpy(pmacA + offset, nsA, sizeof(nsA));

    //usart_tx_string("INFO:pmacA\n");
    usart_tx_buffer(pmacA, sizeof(id) + 96 + sizeof(key) + sizeof(nsA));

    // Lunghezza digest 32 per sha256
    //uint8_t macA[32U];
    uint8_t *macA= (uint8_t *)malloc(32 * sizeof(uint8_t));
    result = hmac(SHA256, pmacA, sizeof(id) + 96 + sizeof(key) + sizeof(nsA), prk, sizeof(prk), macA);
    // Check for errors
    if (result != 0)
    {
        usart_tx_string(errore);
        return 1;
    }
    usart_tx_string("Mac_A\n");
    usart_tx_buffer(macA, 32);
    		

    // Compara mac
    result = 1;
    for (uint8_t i = 0U; (i < 32) && result; i++)
    {
        result = (macA[i] == oldmacA[i]);
    }
    if (result == 1)
    {
        usart_tx_string("Mac verificato\n");
    }
    else
    {
        usart_tx_string("Errore Mac \n");
    }
	
	free(mA);
	free(oldmacA);
	free(oldmacB);
	free(pmacA);
	free(macA);



    //Prepara messaggi e mac
    uint8_t nA[6] = {0U};
    gen(nA,sizeof(nA));
    //usart_tx_string("INFO:N_A:");
    usart_tx_buffer(nA, sizeof(nA));
    uint8_t nAB[12] = {0U};
    gen(nAB,sizeof(nAB));
    //usart_tx_string("INFO:N_AB:");
    usart_tx_buffer(nAB, sizeof(nAB));

    // mSA (id,nsA,nA)
    /*
    uint8_t mSA[sizeof(id) + sizeof(nsA) + sizeof(nA)] = {0U};
    for (int i = 0; i < sizeof(id); i++)
    {
        mSA[i] = id[i];
    }
    for (int i = 0; i < sizeof(nsA); i++)
    {
        mSA[i + sizeof(id)] = nsA[i];
    }
    for (int i = 0; i < sizeof(nA); i++)
    {
        mSA[i + sizeof(nsA) + sizeof(id)] = nA[i];
    }
    Encrypt(mSA,prk);
    usart_tx_string("Messaggio mSA cifrato: ");
    usart_tx_buffer(mSA,sizeof(mSA));
    */
    
    uint8_t mSA[sizeof(id) + sizeof(nsA) + sizeof(nA)];
    offset = 0;
    memcpy(mSA + offset, id, sizeof(id));
    offset+=sizeof(id);
    memcpy(mSA + offset, nsA, sizeof(nsA));
    offset+=sizeof(nsA);
    memcpy(mSA + offset, nA, sizeof(nA));
    
    Encrypt(mSA,prk);
    usart_tx_string("mSA cifrato:");
    usart_tx_buffer(mSA,sizeof(mSA));
    
    
    
    // mBA (id,nAB)
    uint8_t mBA[sizeof(id) + sizeof(nAB)] = {0U};
    for (int i = 0; i < sizeof(id); i++)
    {
        mBA[i] = id[i];
    }
    for (int i = 0; i < sizeof(nAB); i++)
    {
        mBA[i + sizeof(id)] = nAB[i];
    }
    Encrypt(mBA,prk);
    usart_tx_string("mBA cifrato: ");
    usart_tx_buffer(mBA,sizeof(mBA));
    
   
    
    
    
    //1
    //uint8_t pmac1[sizeof(mSA) + sizeof(nA)] = {0U};
    // Allocazione dinamica di memoria per un array di uint8_t
    uint8_t *pmac1 = (uint8_t *)malloc((sizeof(mSA) + sizeof(nA)) * sizeof(uint8_t));
    for (int i = 0; i < sizeof(mSA); i++)
    {
        pmac1[i] = mSA[i];
    }
    for (int i = 0; i < sizeof(nA); i++)
    {
        pmac1[i+sizeof(mSA)] = nA[i];
    }
    //usart_tx_string("pmac1\n");
    usart_tx_buffer(pmac1,sizeof(mSA) + sizeof(nA));

    // Lunghezza digest 32 per sha256
    //uint8_t mac1[32U];
    // Allocazione dinamica di memoria per un array di uint8_t
    uint8_t *mac1 = (uint8_t *)malloc(32 * sizeof(uint8_t));
    result = hmac(SHA256, pmac1, sizeof(mSA) + sizeof(nA), prk, sizeof(prk), mac1);
    // Check for errors
    if (result != 0)
    {
        usart_tx_string(errore);
        return 1;
    }
    // mac(mSA||nA)
    usart_tx_string("MAC{mS-A||nA}\n");
    usart_tx_buffer(mac1,32);
   
    // Liberazione della memoria allocata dinamicamente
    free(pmac1);
    free(mac1);
/*    
    //2
    //uint8_t pmac2[48 + sizeof(nsB)] = {0U};
    // Allocazione dinamica di memoria per un array di uint8_t
    uint8_t *pmac2 = (uint8_t *)malloc((48 + sizeof(nsB)) * sizeof(uint8_t));
    for (int i = 0; i < 48; i++)
    {
        pmac2[i] = mB[i];
    }
    for (int i = 0; i < sizeof(nsB); i++)
    {
        pmac2[i+48] = nsB[i];
    }
    //usart_tx_string("pmac2\n");
    usart_tx_buffer(pmac2,48 + sizeof(nsB));

    // Lunghezza digest 32 per sha256
    //uint8_t mac2[32U];
    uint8_t *mac2 = (uint8_t *)malloc(32 * sizeof(uint8_t));
    result = hmac(SHA256, pmac2, 48 + sizeof(nsB), nsB, sizeof(nsB), mac2);
    // Check for errors
    if (result != 0)
    {
        usart_tx_string(errore);
        return 1;
    }
    // mac(mB||nSB)
    usart_tx_string("MAC{mB||nSB}\n");
    usart_tx_buffer(mac2,32);
    
    // Liberazione della memoria allocata dinamicamente
    free(mB);
    free(pmac2);
    free(mac2);
    
*/
    
    //3
    //uint8_t pmac3[sizeof(mBA) + sizeof(nAB)] = {0U};
    // Allocazione dinamica di memoria per un array di uint8_t
    uint8_t *pmac3 = (uint8_t *)malloc((sizeof(mBA) + sizeof(nAB)) * sizeof(uint8_t));
    for (int i = 0; i < sizeof(mBA); i++)
    {
        pmac3[i] = mBA[i];
    }
    for (int i = 0; i < sizeof(nAB); i++)
    {
        pmac3[i+sizeof(mBA)] = nAB[i];
    }
    //usart_tx_string("pmac3\n");
    usart_tx_buffer(pmac3,sizeof(mBA) + sizeof(nAB));

    // Lunghezza digest 32 per sha256
    //uint8_t mac3[32U];
    uint8_t *mac3 = (uint8_t *)malloc(32 * sizeof(uint8_t));
    result = hmac(SHA256, pmac3, sizeof(mBA) + sizeof(nAB), nsB, sizeof(nsB), mac3);
    // Check for errors
    if (result != 0)
    {
        usart_tx_string(errore);
        return 1;
    }
    // mac(mBA||nAB)
    usart_tx_string("MAC{mBA||nAB}\n");
    usart_tx_buffer(mac3,32);
    
    // Liberazione della memoria allocata dinamicamente
    free(pmac3);
    free(mac3);
    
  
    
/*    
    //MAC{ID_A||ID_B ||NA-B}
    uint8_t oldmacAA[32U];
    usart_tx_string("Inserisci Mac_AA:\n");
    usart_rx_buffer(oldmacAA,sizeof(oldmacAA));
    usart_tx_string("Mac inserito: ");
    usart_tx_buffer(oldmacAA,sizeof(oldmacAA));
    
    
    uint8_t pmacAA[sizeof(id) + sizeof(idB) + sizeof(nAB)] = {0U};
    for (int i = 0; i < sizeof(id); i++)
    {
        pmacAA[i] = id[i];
    }
    for (int i = 0; i < sizeof(idB); i++)
    {
        pmacAA[i+sizeof(id)] = idB[i];
    }
    for (int i = 0; i < sizeof(nAB); i++)
    {
        pmacAA[i+sizeof(id)+sizeof(idB)] = nAB[i];
    }
    usart_tx_string("Messaggio prima del pmacAA\n");
    usart_tx_buffer(pmacAA,sizeof(pmacAA));

    // Lunghezza digest 32 per sha256
    uint8_t macAA[32U];
    result = hmac(SHA256, pmacAA, sizeof(pmacAA), nsB, sizeof(nsB), macAA);
    // Check for errors
    if (result != 0)
    {
        usart_tx_string("HMAC failed with error \n");
        return 1;
    }
    usart_tx_string("MAC{ID_A||ID_B ||NA-B}\n");
    usart_tx_buffer(macAA,sizeof(macAA));
    
    // Compara mac
    result = 1;
    for (uint8_t i = 0U; (i < 32) && result; i++)
    {
        result = (macAA[i] == oldmacAA[i]);
    }
    if (result == 1)
    {
        usart_tx_string("Mac verificato, autenticazione completata con il nodo B\n");
    }
    else
    {
        usart_tx_string("Mac non autenticato\n");
    }
    
    
    return 0;
    
*/    
    	}
    	///////////////////////////////////////////////////////////////////////////////////////////////Nodo B
    	else if (ch == 'h' && id[3] == 2U)
    	{
    		gen(ninit,sizeof(ninit));
    		
    		//Da inviare al server insieme alle info del nodo A
    		usart_tx_string("Ninit_B: ");
    		usart_tx_buffer(ninit,sizeof(ninit));
    		
    		
    		uint8_t idA[4U] = {0U};
    		usart_tx_string("Inserisci id nodo A \n");
    		usart_rx_buffer(idA,sizeof(idA));
    		
    		
    		// Calcolo PRK
    		result = hkdf(SHA256, NULL, 0, key, FE_KEY_SIZE, ninit, sizeof(ninit), prk, FE_KEY_SIZE);
    		// Check for errors
    		if (result != 0)
    		{
    		    usart_tx_string("HKDF failed with error\n");
    		    return 1;
    		}
    		usart_tx_string("INFO:PRK: ");
    		usart_tx_buffer(prk,sizeof(prk));
    		
    		
    		
    		//Inserisci messaggi e mac
    		uint8_t mB[48] = {0U};
    		usart_tx_string("Inserisci M_B \n");
    		usart_rx_buffer(mB,sizeof(mB));
    		usart_tx_string("Messaggio inserito: ");
    		usart_tx_buffer(mB,sizeof(mB));
    		
    		uint8_t mBA[16] = {0U};
    		usart_tx_string("Inserisci M_BA \n");
    		usart_rx_buffer(mBA,sizeof(mBA));
    		usart_tx_string("Messaggio inserito: ");
    		usart_tx_buffer(mBA,sizeof(mBA));

    		uint8_t oldmac2[32U];
    		usart_tx_string("Inserisci Mac_{M_B||NS-B}:\n");
    		usart_rx_buffer(oldmac2,sizeof(oldmac2));
    		usart_tx_string("Mac inserito: ");
    		usart_tx_buffer(oldmac2,sizeof(oldmac2));
    		
    		uint8_t oldmac3[32U];
    		usart_tx_string("Inserisci Mac_{MB-A||NA-B}:\n");
    		usart_rx_buffer(oldmac3,sizeof(oldmac3));
    		usart_tx_string("Mac inserito: ");
    		usart_tx_buffer(oldmac3,sizeof(oldmac3));
    		
    		
    		
    		uint8_t nsA[6] = {0U};
    		uint8_t nsB[16] = {0U};
    		
    		//Decifra messaggio MB
    		DecryptN(mB,sizeof(mB), prk);
    		usart_tx_string("Decripted M_B: ");
    		usart_tx_buffer(mB,sizeof(mB));
/*    		
    		uint8_t offset=0;
    		//Check per ID e Ninit
    		for (int i = 0; i < sizeof(idA); i++)
    		{
     			if(mB[i]!=idA[i]){
    		        usart_tx_string("Errore ID_A non corrisponde\n");
            		return -1;
            		}
    		}
    		offset+=sizeof(idA);
    		for (int i = 0; i < sizeof(id); i++)
    		{
     			if(mB[i]!=id[i]){
    		        usart_tx_string("Errore ID_B non corrisponde\n");
            		return -1;
            		}
    		}
    		offset+=sizeof(id);
    		for (int i = 0; i < sizeof(ninit); i++)
    		{
        		if(mB[i+offset]!=ninit[i]){
            		usart_tx_string("Errore Ninit_A non corrisponde\n");
            		return -1;
            		}
    		}
    		offset+= sizeof(ninit);
    		//Estraggo nsA
    		for (int i = 0; i < sizeof(nsA); i++)
    		{
        		nsA[i] = mB[i + offset];
    		}
    		offset+= sizeof(nsA);
    		//Estraggo nsB
    		for (int i = 0; i < sizeof(nsB); i++)
    		{
        		nsB[i] = mB[i + offset];
    		}
    		
    		
    		//Decifra messaggio M_BA
    		DecryptN(mBA,sizeof(mBA), nsB);
    		usart_tx_string("Decripted M_BA: ");
    		usart_tx_buffer(mBA,sizeof(mBA));
    		
    		uint8_t nAB[12] = {0U};
    		offset=0;
    		//Check per ID e Ninit
    		for (int i = 0; i < sizeof(idA); i++)
    		{
     			if(mBA[i]!=idA[i]){
    		        usart_tx_string("Errore ID_A non corrisponde\n");
            		return -1;
            		}
    		}
    		offset+=sizeof(idA);
    		//Estraggo nAB
    		for (int i = 0; i < sizeof(nAB); i++)
    		{
        		nAB[i] = mBA[i + offset];
    		}
    		
    		
    		//VERIFICA MAC
    // MAC 2
    EncryptN(mB,sizeof(mB),prk);
    //2
    uint8_t pmac2[sizeof(mB) + sizeof(nsB)] = {0U};
    for (int i = 0; i < sizeof(mB); i++)
    {
        pmac2[i] = mB[i];
    }
    for (int i = 0; i < sizeof(nsB); i++)
    {
        pmac2[i+sizeof(mB)] = nsB[i];
    }
    usart_tx_string("Messaggio prima del pmac2\n");
    usart_tx_buffer(pmac2,sizeof(pmac2));

    // Lunghezza digest 32 per sha256
    uint8_t mac2[32U];
    result = hmac(SHA256, pmac2, sizeof(pmac2), nsB, sizeof(nsB), mac2);
    // Check for errors
    if (result != 0)
    {
        usart_tx_string("HMAC failed with error \n");
        return 1;
    }
    // mac(mB||nSB)
    usart_tx_string("MAC{mB||nSB}\n");
    usart_tx_buffer(mac2,sizeof(mac2));
    
    // Compara mac
    result = 1;
    for (uint8_t i = 0U; (i < 32) && result; i++)
    {
        result = (mac2[i] == oldmac2[i]);
    }
    if (result == 1)
    {
        usart_tx_string("Mac2 verificato\n");
    }
    else
    {
        usart_tx_string("Mac2 non autenticato\n");
    }
    
    //3
    uint8_t pmac3[sizeof(mBA) + sizeof(nAB)] = {0U};
    for (int i = 0; i < sizeof(mBA); i++)
    {
        pmac3[i] = mBA[i];
    }
    for (int i = 0; i < sizeof(nAB); i++)
    {
        pmac3[i+sizeof(mBA)] = nAB[i];
    }
    usart_tx_string("Messaggio prima del pmac3\n");
    usart_tx_buffer(pmac3,sizeof(pmac3));

    // Lunghezza digest 32 per sha256
    uint8_t mac3[32U];
    result = hmac(SHA256, pmac3, sizeof(pmac3), nsB, sizeof(nsB), mac3);
    // Check for errors
    if (result != 0)
    {
        usart_tx_string("HMAC failed with error \n");
        return 1;
    }
    // mac(mBA||nAB)
    usart_tx_string("MAC{mBA||nAB}\n");
    usart_tx_buffer(mac3,sizeof(mac3));
    
    // Compara mac
    result = 1;
    for (uint8_t i = 0U; (i < 32) && result; i++)
    {
        result = (mac3[i] == oldmac3[i]);
    }
    if (result == 1)
    {
        usart_tx_string("Mac3 verificato\n");
    }
    else
    {
        usart_tx_string("Mac3 non autenticato\n");
    }
    
    //Prepara messaggi e mac
    uint8_t nB[6] = {0U};
    gen(nB,sizeof(nB));

    // mSB (id,nsB,nB)
    //lungh 26
    uint8_t mSB[32] = {0U};
    for (int i = 0; i < sizeof(id); i++)
    {
        mSB[i] = id[i];
    }
    for (int i = 0; i < sizeof(nsB); i++)
    {
        mSB[i + sizeof(id)] = nsB[i];
    }
    for (int i = 0; i < sizeof(nB); i++)
    {
        mSB[i + sizeof(nsB) + sizeof(id)] = nB[i];
    }
    EncryptN(mSB,sizeof(mSB),prk);
    usart_tx_string("Messaggio mSB cifrato: ");
    usart_tx_buffer(mSB,sizeof(mSB));
    
    
    //MAC_S{MS-B||NB}
    uint8_t pmacS[sizeof(mSB) + sizeof(nB)] = {0U};
    for (int i = 0; i < sizeof(mSB); i++)
    {
        pmacS[i] = mSB[i];
    }
    for (int i = 0; i < sizeof(nB); i++)
    {
        pmacS[i+sizeof(mSB)] = nB[i];
    }
    usart_tx_string("Messaggio prima del pmacS\n");
    usart_tx_buffer(pmacS,sizeof(pmacS));

    // Lunghezza digest 32 per sha256
    uint8_t macS[32U];
    result = hmac(SHA256, pmacS, sizeof(pmacS), prk, sizeof(prk), macS);
    // Check for errors
    if (result != 0)
    {
        usart_tx_string("HMAC failed with error \n");
        return 1;
    }
    // mac(mSB||nB)
    usart_tx_string("MAC{mS-B||nB}\n");
    usart_tx_buffer(macS,sizeof(macS));
    
    
    //MAC{ID_A||ID_B ||NA-B}
    uint8_t pmacA[sizeof(idA) + sizeof(id) + sizeof(nAB)] = {0U};
    for (int i = 0; i < sizeof(idA); i++)
    {
        pmacA[i] = idA[i];
    }
    for (int i = 0; i < sizeof(id); i++)
    {
        pmacA[i+sizeof(idA)] = id[i];
    }
    for (int i = 0; i < sizeof(nAB); i++)
    {
        pmacA[i+sizeof(idA)+sizeof(id)] = nAB[i];
    }
    usart_tx_string("Messaggio prima del pmacA\n");
    usart_tx_buffer(pmacA,sizeof(pmacA));

    // Lunghezza digest 32 per sha256
    uint8_t macA[32U];
    result = hmac(SHA256, pmacA, sizeof(pmacA), nsB, sizeof(nsB), macA);
    // Check for errors
    if (result != 0)
    {
        usart_tx_string("HMAC failed with error \n");
        return 1;
    }
    usart_tx_string("MAC{ID_A||ID_B ||NA-B}\n");
    usart_tx_buffer(macA,sizeof(macA));
    
    return 0;
*/    
        }
    }

    return 0;
}

void __init0(void)
{
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

    usart_tx_string("puf init0:\n\r");
    usart_tx_buffer(bss_puf, PUF_SIZE);
}

void __init1(void)
{
    usart_tx_string("puf init1:\n\r");
    usart_tx_buffer(bss_puf, PUF_SIZE);
}

void __start(void)
{
    __asm volatile("jmp __vectors");
}
