/* 
 * my_utility.h
 *
 *  Created on: 21 feb 2024
 *      Author: Carmine D'Angelo
 */

#ifndef MY_UTILITY_H_  // Verifica se il file è stato incluso precedentemente
#define MY_UTILITY_H_  // Se non è stato incluso, definisci il simbolo di inclusione condizionale

#define DISPOSITIVO_1_PATH "../../file_dispositivi/dispositivo_1/"
#define DISPOSITIVO_2_PATH "../../file_dispositivi/dispositivo_2/"
#define CONTRACT_SIZE 64

#include <stdio.h>
#include <stdint.h>

int hex_string_to_bytes(const char *hex_string, uint8_t *byte_array, size_t array_size);
int bytes_to_hex_string(const uint8_t *byte_array, size_t array_size, char *hex_string, size_t hex_string_size);
void print_buffer_as_json(const char *format, ...);

void stampa(uint8_t block[], int size);

void genera(uint8_t block[],int size);

char* uint8_t_to_hex_string(uint8_t* array, size_t size);
#endif /* MY_UTILITY_H_ */
