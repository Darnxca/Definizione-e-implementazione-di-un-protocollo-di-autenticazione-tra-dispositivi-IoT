#ifndef PC_UTILS_H
#define PC_UTILS_H

#include <inttypes.h>
#include <stdlib.h>


void print_buffer(const uint8_t* msg, size_t len);
void print_codeword(int16_t* cw, size_t len);
uint8_t ascii_to_byte(char c);
void load_buffer_from_file(const char* filename, uint8_t* buff, size_t len);
void write_buffer_to_file(const char* filename, const uint8_t* buff, size_t len);
void write_string_to_file(const char* filename, const char* string);
void read_string_from_file(const char* filename, char* string_buffer);

#endif // PC_UTILS_H