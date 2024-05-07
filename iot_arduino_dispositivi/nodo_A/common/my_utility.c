#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "my_utility.h"

// Converte una stringa esadecimale in un array di byte
// Restituisce 0 se la conversione ha successo, -1 altrimenti
int hex_string_to_bytes(const char *hex_string, uint8_t *byte_array, size_t array_size) {
    size_t hex_len = strlen(hex_string);
    
    // Controllo se la lunghezza della stringa esadecimale è valida
    if (hex_len % 2 != 0) {
        printf("La lunghezza della stringa esadecimale non è valida.\n");
        return -1;
    }
    
    // Controllo se la lunghezza della stringa esadecimale corrisponde alla dimensione dell'array
    if (hex_len / 2 != array_size) {
        printf("La lunghezza della stringa esadecimale non corrisponde alla dimensione dell'array.\n");
        return -1;
    }
    
    // Conversione di ciascun byte della stringa esadecimale
    for (size_t i = 0; i < hex_len; i += 2) {
        char byte_str[3] = { hex_string[i], hex_string[i + 1], '\0' };
        char *endptr;
        long byte_value = strtol(byte_str, &endptr, 16);

        // Controllo se la stringa contiene caratteri non validi
        if (*endptr != '\0') {
            printf("Caratteri non validi nella stringa esadecimale.\n");
            return -1;
        }

        // Controllo se il valore convertito è nel range valido
        if (byte_value < 0 || byte_value > 255) {
            printf("Valore non valido nella stringa esadecimale.\n");
            return -1;
        }

        byte_array[i / 2] = (uint8_t) byte_value;
    }

  
    return 0;
}
// Converte un array di byte in una stringa esadecimale
// Restituisce 0 se la conversione ha successo, -1 altrimenti
int bytes_to_hex_string(const uint8_t *byte_array, size_t array_size, char *hex_string, size_t hex_string_size) {
    // Conversione di ciascun byte dell'array
    for (size_t i = 0; i < array_size; ++i) {
        snprintf(hex_string + (i * 2), hex_string_size - (i * 2), "%02X", byte_array[i]);
    }
    return 0;
}

// Stampa i buffer come stringhe esadecimali in formato JSON.
// Questa funzione accetta un elenco variabile di buffer insieme ai loro nomi e le dimensioni dei buffer.
// I buffer vengono stampati come stringhe esadecimali all'interno di un oggetto JSON formattato.
// Il formato è: {"nome_buffer": "valore_esadecimale", ...}
void print_buffer_as_json(const char *format, ...) {
    va_list args;
    va_start(args, format); // Inizializza l'elenco di argomenti variabili

    printf("{\n"); // Inizio dell'oggetto JSON
    const char *name = format; // Ottiene il nome del primo buffer

    // Itera attraverso gli argomenti variabili
    while (name != NULL) {
        uint8_t *buffer = va_arg(args, uint8_t *); // Ottiene il buffer
        if (buffer == NULL) {
            break;
        }

        size_t buffer_size = va_arg(args, size_t); // Ottiene la dimensione del buffer
        printf("\"%s\": \"", name); // Stampa il nome del buffer come chiave JSON

        // Stampa il buffer come stringa esadecimale
        for (size_t i = 0; i < buffer_size; i++) {
            printf("%02X", buffer[i]); // Stampa ciascun byte del buffer come due caratteri esadecimali
        }
        printf("\""); // Chiude il valore esadecimale

        name = va_arg(args, const char *); // Ottiene il nome del prossimo buffer
        if (name != NULL) {
            printf(",\n"); // Aggiunge una virgola se ci sono più buffer da stampare
        }
    }

    printf("\n}\n"); // Fine dell'oggetto JSON
    va_end(args); // Pulizia dell'elenco di argomenti variabili
}

void stampa(uint8_t block[], int size){
        for (int i = 0; i < size; i++)
        {
            printf("%02hhX",block[i]);
        }
    printf("\n");
}

void genera(uint8_t block[],int size){
    // Generate an array of 16 random uint8_t values
    for (int i = 0; i < size; i++) {
        block[i] = rand() % 256;
    }
}

char* uint8_t_to_hex_string(uint8_t* array, size_t size) {
    char* hex_string = malloc(size * 2 + 1); // Ogni byte occupa 2 caratteri esadecimali + terminatore nullo
    if (hex_string == NULL) {
        fprintf(stderr, "Errore: impossibile allocare memoria.\n");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < size; i++) {
        sprintf(hex_string + i * 2, "%02X", array[i]);
    }
    hex_string[size * 2] = '\0'; // Aggiungi il terminatore nullo

    return hex_string;
}