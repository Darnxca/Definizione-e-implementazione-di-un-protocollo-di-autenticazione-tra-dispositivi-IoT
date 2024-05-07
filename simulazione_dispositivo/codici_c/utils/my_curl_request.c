#include "my_curl_request.h"

// Inizializzazione variabili contenente l'url delle api
const char* apiRegistrazione = "https://127.0.0.1:3008/registrazione/registraDispositivoIoT";
const char* apiAutenticazione = "https://127.0.0.1:3008/autenticazione/avviaAutenticazione";
const char* apiVerificoAvvioAutenticazione = "https://127.0.0.1:3008/autenticazione/checkAvvioAutenticazione"; // Corretto per includere "http://"
const char* apiVerificaMacServerA = "https://127.0.0.1:3007/autenticazione/verificaMacDispositivoA"; // Corretto per includere "http://"
const char* apiOttieniNonceAB = "https://127.0.0.1:3008/autenticazione/ottieniNonceAB";
const char* apiConfermaAutenticazioneB = "https://127.0.0.1:3008/autenticazione/confermaAutenticazioneDispositivoB";
const char* apiConfermaAutenticazioneA = "https://127.0.0.1:3007/autenticazione/completaAutenticazioneA";
const char* apiVerificaAvvenutaAutenticazione = "https://127.0.0.1:3008/autenticazione/autenticazioneStabilita";

// Porte su cui sono ostati i due server
int portServerA = 3007;
int portServerB = 3008;

char* findSubstring(const char* input) {
    char* substring = strchr(input, '{');
    if (substring != NULL) {
        return substring;
    }
    return NULL;
}

void extractKeysAndValues(const char* json, KeyValuePair pairs[], int* count) {
    char temp[MAX_BUFFER_SIZE];
    strcpy(temp, json);

    char* token = strtok(temp, "{}\":,");
    char* key = NULL;
    char* value = NULL;
    int isKey = 1; // Flag per indicare se il token corrente è una chiave

    while (token != NULL) {
        if (isKey) {
            key = token;
        } else {
            value = token;
            if (key != NULL && value != NULL) {
                strncpy(pairs[*count].key, key, MAX_KEY_SIZE - 1);
                pairs[*count].key[MAX_KEY_SIZE - 1] = '\0';
                strncpy(pairs[*count].value, value, MAX_VALUE_SIZE - 1);
                pairs[*count].value[MAX_VALUE_SIZE - 1] = '\0';
                (*count)++;
                // Resetta chiave e valore
                key = NULL;
                value = NULL;
            }
        }
        // Inverti lo stato di isKey
        isKey = !isKey;
        token = strtok(NULL, "{}\":,");
    }
}

size_t write_callback(char *ptr, size_t size, size_t nmemb, char **response) {
    size_t response_size = size * nmemb;
    *response = realloc(*response, response_size + 1);
    if (*response) {
        memcpy(*response, ptr, response_size);
        (*response)[response_size] = '\0';
    }
    return response_size;
}

char* performCurlRequest(const char* host, int port, const char* body) {
    struct curl_slist *headers = NULL;

    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Errore nell'inizializzazione di cURL.\n");
        return NULL;
    }

    char *response = NULL;

    curl_easy_setopt(curl, CURLOPT_URL, host);
    curl_easy_setopt(curl, CURLOPT_PORT, port);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);

    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Ignora la verifica del certificato SSL
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "Errore durante l'invio della richiesta cURL: %s\n", curl_easy_strerror(res));
        free(response);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return NULL;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return response;
}

KeyValuePair* registraDispositivo(char * wallet_address_dispositivo, uint8_t * puf_response){
    // Costruzione del corpo JSON
    char body[MAX_BUFFER_SIZE];
    snprintf(body, MAX_BUFFER_SIZE, "{\"address_dispositivo\": \"%s\", \"dump\": \"%s\"}", wallet_address_dispositivo, uint8_t_to_hex_string(puf_response, PUF_SIZE));

    // Effettua la richiesta CURL e ottieni la risposta
    char* response = performCurlRequest(apiRegistrazione, portServerB, body);
    if (response == NULL) {
        printf("Errore durante l'esecuzione della richiesta CURL.\n");
        exit(1);
    }
    
    
    // Allocazione dinamica della memoria per pairs
    KeyValuePair* pairs = malloc(MAX_PAIRS * sizeof(KeyValuePair));
    if (pairs == NULL) {
        fprintf(stderr, "Errore durante l'allocazione della memoria.\n");
        exit(EXIT_FAILURE);
    }

    // Estrazione dei parametri dal JSON e stampa
    int count = 0;
    char* json = findSubstring(response); // Trova il JSON nella risposta
    if (json != NULL) {
        extractKeysAndValues(json, pairs, &count);
        if(strcmp(pairs[0].value, "false") == 0)
            exit(1);
    } else {
        printf("JSON non trovato nella risposta.\n");
        exit(1);
    }

    return pairs;
}

void avviaAutenticazione(char * wallet_address_dispositivo, uint8_t * id_dispositivoA, uint8_t * helper_dataA, uint8_t * nInitA, uint8_t * id_dispositivoB, uint8_t * helper_dataB, u_int8_t * nInitB){
     // Costruzione del corpo JSON
    char body[MAX_BUFFER_SIZE];
    snprintf(body, MAX_BUFFER_SIZE, "{\"address_dispositivo\": \"%s\", \"id_dispositivoA\": \"%s\", \"helper_dataA\": \"%s\", \"nonceA\": \"%s\", \"id_dispositivoB\": \"%s\", \"helper_dataB\": \"%s\", \"nonceB\": \"%s\"}", 
    wallet_address_dispositivo, uint8_t_to_hex_string(id_dispositivoA, ID_SIZE), uint8_t_to_hex_string(helper_dataA, PUF_SIZE),uint8_t_to_hex_string(nInitA,NONCE_SIZE), uint8_t_to_hex_string(id_dispositivoB, ID_SIZE), uint8_t_to_hex_string(helper_dataB, PUF_SIZE),uint8_t_to_hex_string(nInitB,NONCE_SIZE));
    
    char* response = performCurlRequest(apiAutenticazione, portServerB, body);
    if (response == NULL) {
        printf("Errore durante l'esecuzione della richiesta CURL.\n");
        exit(1);
    }

    // Allocazione dinamica della memoria per pairs
    KeyValuePair* pairs = malloc(MAX_PAIRS * sizeof(KeyValuePair));
    if (pairs == NULL) {
        fprintf(stderr, "Errore durante l'allocazione della memoria.\n");
        exit(EXIT_FAILURE);
    }

    // Estrazione dei parametri dal JSON e stampa
    int count = 0;
    char* json = findSubstring(response); // Trova il JSON nella risposta
    if (json != NULL) {
        extractKeysAndValues(json, pairs, &count);
        if(strcmp(pairs[0].value, "false") == 0){
            printf("%s",pairs[1].value);
            exit(1);
        }
    } else {
        printf("JSON non trovato nella risposta.\n");
        exit(1);
    }

    free(pairs);
}

KeyValuePair* verificaAvvioAutenticazione(uint8_t * nonceA){
   
    // Costruzione del corpo JSON
    char body[MAX_BUFFER_SIZE];
    snprintf(body, MAX_BUFFER_SIZE, "{\"nonce\": \"%s\"}", uint8_t_to_hex_string(nonceA,NONCE_SIZE));

    char* response = performCurlRequest(apiVerificoAvvioAutenticazione, portServerB, body);
    if (response == NULL) {
        printf("Errore durante l'esecuzione della richiesta CURL.\n");
        exit(1);
    }

    // Allocazione dinamica della memoria per pairs
    KeyValuePair* pairs = malloc(MAX_PAIRS * sizeof(KeyValuePair));
    if (pairs == NULL) {
        fprintf(stderr, "Errore durante l'allocazione della memoria.\n");
        exit(EXIT_FAILURE);
    }

    // Estrazione dei parametri dal JSON e stampa
    int count = 0;
    char* json = findSubstring(response); // Trova il JSON nella risposta
    if (json != NULL) {
        extractKeysAndValues(json, pairs, &count);      
    } else {
        printf("JSON non trovato nella risposta.\n");
        exit(1);
    }

    return pairs;
}

void serverAVerificaMac(char * wallet_address_dispositivo, uint8_t * msg_SA, uint8_t * server_digest, uint8_t * id_dispositivoA, uint8_t * helper_dataA, uint8_t * msg_B){
   
    // Costruzione del corpo JSON
    char body[MAX_BUFFER_SIZE];
    snprintf(body, MAX_BUFFER_SIZE, "{\"address_dispositivo\": \"%s\", \"M_SA\": \"%s\", \"digest\": \"%s\", \"id\": \"%s\", \"helper_data\": \"%s\", \"rif\": \"%s\"}", 
    wallet_address_dispositivo,uint8_t_to_hex_string(msg_SA, MSG_SA_SIZE), uint8_t_to_hex_string(server_digest, DIGEST_SIZE),uint8_t_to_hex_string(id_dispositivoA, ID_SIZE), uint8_t_to_hex_string(helper_dataA, PUF_SIZE), uint8_t_to_hex_string(msg_B, MSG_B_SIZE));

    char* response = performCurlRequest(apiVerificaMacServerA, portServerA, body);
    if (response == NULL) {
        printf("Errore durante l'esecuzione della richiesta CURL.\n");
        exit(1);
    }

    // Allocazione dinamica della memoria per pairs
    KeyValuePair* pairs = malloc(MAX_PAIRS * sizeof(KeyValuePair));
    if (pairs == NULL) {
        fprintf(stderr, "Errore durante l'allocazione della memoria.\n");
        exit(EXIT_FAILURE);
    }

    int count = 0;
    char* json = findSubstring(response); // Trova il JSON nella risposta
    if (json != NULL) {
        extractKeysAndValues(json, pairs, &count);
        if (strcmp(pairs[0].value, "false") == 0){
            printf("Errore nella verifica del mac da parte del serverA\n");
            exit(1);
        }
    } else {
        printf("JSON non trovato nella risposta.\n");
        exit(1);
    }

    free(pairs);
}

KeyValuePair* ottenimentoNonceAB(uint8_t *  msg_BA, uint8_t *  digest_msg_ba, uint8_t *  id_dispositivoA, uint8_t *  id_dispositivoB){
    // Costruzione del corpo JSON
    char body[MAX_BUFFER_SIZE];
    snprintf(body, MAX_BUFFER_SIZE, "{\"msg_BA\": \"%s\", \"digest\": \"%s\", \"id_dispositivoA\": \"%s\", \"id_dispositivoB\": \"%s\"}",     
    uint8_t_to_hex_string(msg_BA, MSG_SB_SIZE), uint8_t_to_hex_string(digest_msg_ba, DIGEST_SIZE),uint8_t_to_hex_string(id_dispositivoA, ID_SIZE), uint8_t_to_hex_string(id_dispositivoB, ID_SIZE));
    
    char* response = performCurlRequest(apiOttieniNonceAB, portServerB, body);
    if (response == NULL) {
        printf("Errore durante l'esecuzione della richiesta CURL.\n");
        exit(1);
    }

    // Allocazione dinamica della memoria per pairs
    KeyValuePair* pairs = malloc(MAX_PAIRS * sizeof(KeyValuePair));
    if (pairs == NULL) {
        fprintf(stderr, "Errore durante l'allocazione della memoria.\n");
        exit(EXIT_FAILURE);
    }
 
    int count = 0;
    char* json = findSubstring(response); // Trova il JSON nella risposta
    if (json != NULL) {
        extractKeysAndValues(json, pairs, &count);
    } else {
        printf("JSON non trovato nella risposta.\n");
        exit(1);
    }

    return pairs;
}

void dispositivoBConfermaAutenticazione(char * wallet_address_dispositivo,uint8_t *  msg_serverB, uint8_t *  digest_msg_server_b, uint8_t *  id_dispositivoB, uint8_t *  msg_B){
    // Costruzione del corpo JSON
    char body[MAX_BUFFER_SIZE];
    snprintf(body, MAX_BUFFER_SIZE, "{\"address_dispositivo\": \"%s\", \"M_SB\": \"%s\", \"digest\": \"%s\", \"id\": \"%s\", \"rif\": \"%s\"}",     
    wallet_address_dispositivo,uint8_t_to_hex_string(msg_serverB, MSG_SB_SIZE), uint8_t_to_hex_string(digest_msg_server_b, DIGEST_SIZE),uint8_t_to_hex_string(id_dispositivoB, ID_SIZE), uint8_t_to_hex_string(msg_B, MSG_B_SIZE));
    
    char* response = performCurlRequest(apiConfermaAutenticazioneB, portServerB, body);
    if (response == NULL) {
        printf("Errore durante l'esecuzione della richiesta CURL.\n");
        exit(1);
    }

    // Allocazione dinamica della memoria per pairs
    KeyValuePair* pairs = malloc(MAX_PAIRS * sizeof(KeyValuePair));
    if (pairs == NULL) {
        fprintf(stderr, "Errore durante l'allocazione della memoria.\n");
        exit(EXIT_FAILURE);
    }
 
    int count = 0;
    char* json = findSubstring(response); // Trova il JSON nella risposta
    if (json != NULL) {
        extractKeysAndValues(json, pairs, &count);
        if (strcmp(pairs[0].value, "false") == 0){
            printf("%s",pairs[1].value);
            exit(2);
        }
    } else {
        printf("JSON non trovato nella risposta.\n");
        exit(1);
    }

    free(pairs);
}

void dispositivoAConfermaAutenticazione(char * wallet_address_dispositivo, uint8_t *  id_dispositivoA, uint8_t *  riferimento){
    // Costruzione del corpo JSON
    char body[MAX_BUFFER_SIZE];
    snprintf(body, MAX_BUFFER_SIZE, "{\"address_dispositivo\": \"%s\", \"id\": \"%s\",  \"rif\": \"%s\"}",     
    wallet_address_dispositivo, uint8_t_to_hex_string(id_dispositivoA, ID_SIZE), uint8_t_to_hex_string(riferimento, MSG_B_SIZE));
    
    char* response = performCurlRequest(apiConfermaAutenticazioneA, portServerA, body);
    if (response == NULL) {
        printf("Errore durante l'esecuzione della richiesta CURL.\n");
        exit(1);
    }

    // Allocazione dinamica della memoria per pairs
    KeyValuePair* pairs = malloc(MAX_PAIRS * sizeof(KeyValuePair));
    if (pairs == NULL) {
        fprintf(stderr, "Errore durante l'allocazione della memoria.\n");
        exit(EXIT_FAILURE);
    }
 
    int count = 0;
    char* json = findSubstring(response); // Trova il JSON nella risposta
    if (json != NULL) {
        extractKeysAndValues(json, pairs, &count);
        if (strcmp(pairs[0].value, "false") == 0){
            printf("%s",pairs[1].value);
            exit(2);
        }
    } else {
        printf("JSON non trovato nella risposta.\n");
        exit(1);
    }

    free(pairs);
}

void verificaAvvenutaAutenticazione(char * wallet_address_dispositivo, uint8_t *  id_dispositivoA, uint8_t *  riferimento){
    
    char body[MAX_BUFFER_SIZE];
    snprintf(body, MAX_BUFFER_SIZE, "{\"address_dispositivo\": \"%s\", \"id\": \"%s\",  \"rif\": \"%s\"}",     
    wallet_address_dispositivo, uint8_t_to_hex_string(id_dispositivoA, ID_SIZE), uint8_t_to_hex_string(riferimento, MSG_B_SIZE));
    
    char* response = performCurlRequest(apiVerificaAvvenutaAutenticazione, portServerB, body);
    if (response == NULL) {
        printf("Errore durante l'esecuzione della richiesta CURL.\n");
        exit(1);
    }

    // Allocazione dinamica della memoria per pairs
    KeyValuePair* pairs = malloc(MAX_PAIRS * sizeof(KeyValuePair));
    if (pairs == NULL) {
        fprintf(stderr, "Errore durante l'allocazione della memoria.\n");
        exit(EXIT_FAILURE);
    }
 
    int count = 0;
    char* json = findSubstring(response); // Trova il JSON nella risposta
    if (json != NULL) {
        extractKeysAndValues(json, pairs, &count);
        if (strcmp(pairs[0].value, "false") == 0){
            printf("%s",pairs[1].value);
            exit(2);
        }
        if (strcmp(pairs[1].value, "false") == 0){
            printf("Autenticazione non ancora stabilità");
            exit(2);
        }
    } else {
        printf("JSON non trovato nella risposta.\n");
        exit(1);
    }

    free(pairs);
}

