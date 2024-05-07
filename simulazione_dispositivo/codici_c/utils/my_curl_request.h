#ifndef MY_CURL_REQUEST_H
#define MY_CURL_REQUEST_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include "my_utility.h"
#include "puf_parameters.h"

#define MAX_BUFFER_SIZE 10000
#define MAX_KEY_SIZE 50
#define MAX_VALUE_SIZE 10000
#define MAX_PAIRS 10

typedef struct {
    char key[MAX_KEY_SIZE];
    char value[MAX_VALUE_SIZE];
} KeyValuePair;

// Funzione per cercare e restituire la sottostringa che inizia con '{' e finisce con '}'
char* findSubstring(const char* input);

// Funzione per estrarre le chiavi e i valori dal JSON
void extractKeysAndValues(const char* json, KeyValuePair pairs[], int* count);

// Funzione callback per gestire i dati ricevuti dal server
size_t write_callback(char *ptr, size_t size, size_t nmemb, char **response);

// Funzione per effettuare una richiesta CURL e restituire la risposta
char* performCurlRequest(const char* host, int port, const char* body);

KeyValuePair* registraDispositivo(char * wallet_address_dispositivo, uint8_t * puf_response);
void avviaAutenticazione(char * wallet_address_dispositivo, uint8_t * id_dispositivoA, uint8_t * helper_dataA, uint8_t * nInitA, uint8_t * id_dispositivoB, uint8_t * helper_dataB, u_int8_t * nInitB);
KeyValuePair* verificaAvvioAutenticazione(uint8_t * nonceA);
void serverAVerificaMac(char * wallet_address_dispositivo, uint8_t * msg_SA, uint8_t * server_digest, uint8_t * id_dispositivoA, uint8_t * helper_dataA, uint8_t * msg_B);
KeyValuePair* ottenimentoNonceAB(uint8_t *  msg_BA, uint8_t *  digest_msg_ba, uint8_t *  id_dispositivoA, uint8_t *  id_dispositivoB);
void dispositivoBConfermaAutenticazione(char * wallet_address_dispositivo,uint8_t *  msg_serverB, uint8_t *  digest_msg_server_b, uint8_t *  id_dispositivoA, uint8_t *  msg_B);
void dispositivoAConfermaAutenticazione(char * wallet_address_dispositivo, uint8_t *  id_dispositivoA, uint8_t *  riferimento);
void verificaAvvenutaAutenticazione(char * wallet_address_dispositivo, uint8_t *  id_dispositivoA, uint8_t *  riferimento);

#endif /* MY_CURL_REQUEST_H */
