#include "pc_utils.h"
#include <stdio.h>
#include <string.h>

void print_buffer(const uint8_t* msg, size_t len){
    for(uint32_t i = 0; i < len; i++)
    {
        printf("%02X", msg[i]);
    }
    printf("\n\n");
}

uint8_t ascii_to_byte(char c){
    uint8_t rtVal = 0;

    if(c >= '0' && c <= '9')
    {
        rtVal = c - '0';
    }
    else
    {
        rtVal = c - 'a' + 10;
    }

    return rtVal;
}

void print_codeword(int16_t* cw, size_t len){
    for(uint32_t i = 0; i < len; i++)
    {
        printf("%d", cw[i]);
    }
    printf("\n\n");
}

void load_buffer_from_file(const char* filename, uint8_t* buff, size_t len){
    FILE* fp1 = fopen(filename,"r");

    for (uint16_t i = 0U; i < len; i++)
    {
        buff[i] = ((ascii_to_byte(fgetc(fp1)) & 0xF) << 4 | (ascii_to_byte(fgetc(fp1)) & 0xF));
        if( feof(fp1) )
        {
            printf("\n\nError while reading file %s!\n\n", filename);
            break ;
        }
    }
    fclose(fp1);
}

void write_buffer_to_file(const char* filename, const uint8_t* buff, size_t len) {
    FILE* fp = fopen(filename, "wb+"); // Apriamo il file in modalità di scrittura

    if (fp == NULL) {
        printf("Errore nell'apertura del file %s\n", filename);
        return;
    }

    // Scrivi il contenuto del buffer come una sequenza di byte esadecimali nel file
    for (size_t i = 0; i < len; i++) {
        fprintf(fp, "%02X", buff[i]); // Formatta il byte in esadecimale e scrivilo nel file
    }

    fclose(fp); // Chiudiamo il file
}

void write_string_to_file(const char* filename, const char* string) {
    FILE* fp = fopen(filename, "wb+"); // Apriamo il file in modalità di scrittura

    if (fp == NULL) {
        printf("Errore nell'apertura del file %s\n", filename);
        return;
    }

    // Scrivi il contenuto del buffer direttamente nel file
    fwrite(string, sizeof(char), strlen(string), fp);

    fclose(fp); // Chiudiamo il file
}

void read_string_from_file(const char* filename, char* string_buffer) {
    FILE* fp = fopen(filename, "r"); // Apri il file in modalità di lettura

    if (fp == NULL) {
        printf("Errore nell'apertura del file %s\n", filename);
        return;
    }

    // Leggi la stringa dal file
    if (fscanf(fp, "%63s", string_buffer) != 1) { // Leggi la stringa nel formato specificato
        printf("Errore nella lettura della stringa dal file %s\n", filename);
        fclose(fp);
        return;
    }

    fclose(fp); // Chiudi il file
}