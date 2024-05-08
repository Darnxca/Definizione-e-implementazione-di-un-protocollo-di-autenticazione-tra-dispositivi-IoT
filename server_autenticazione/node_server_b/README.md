# Back-end node.js README.md 

Server back-end scritto in Node.js progettato per comunicare con uno smart contract.

## Prerequisiti

- Node.js installato sul tuo sistema
- Gestore di pacchetti npm
- Installazione di Kubo IPFS
- Installare gcc: versione utilizzata nel progetto "gcc version 12.3.0 (Ubuntu 12.3.0-1ubuntu1~23.04)"

## Installazione

1. **Installa le dipendenze:**

```shell
   npm install
```

2. **Installa Kubo IPFS**: Per l'installazione di Kubo IPFS seguire la guida al seguente link <https://docs.ipfs.tech/install/command-line/#system-requirements>

## Uso
1. **Avvia il server:**
```shell
    npm start
```
2. **Avviare il server IPFS localmente:**
```shell
    ipfs daemon
```
## Struttura del Progetto

Il progetto è organizzato nelle seguenti directory:

- **contracts**: Contiene il file ABI dello smart contract. Questo file è essenziale per interagire con lo smart contract dalla tua applicazione Node.js. Puoi trovare il file ABI nella cartella artifacts generata dalla compilazione del tuo smart contract, al percorso `artifacts/contracts/nome_dello_smart_contract.json`.

- **log**: Contiene un file per i log degli errori generati durante l'esecuzione del server.
- **routes**: Questa directory contiene i file delle route del server. Le route definiscono le varie API endpoint e gestiscono le richieste dei client.
- **scripts**: Questa directory contiene gli script ausiliari utilizzati per varie operazioni come la compilazione del contratto, l'avvio del server, ecc.
- **index.js**: Questo è il file principale del server. Viene eseguito per avviare il server e contiene la configurazione iniziale.

## API Endpoints
1. **Registrazione**
    - **/registraDispositivoIoT (POST)**: Questo endpoint registra un dispositivo IoT sul server. Richiede l'indirizzo del dispositivo, l'ID del dispositivo, il dump dei dati IoT, e altri dati ausiliari. Restituisce un messaggio che indica se l'operazione è riuscita.

2. **IPFS**
    - **/salvaStringa/:stringa (GET)**: Questo endpoint salva una stringa su IPFS e restituisce il CID generato per essa.
    - **/ottieniStringa/:CID (GET)**: Questo endpoint ottiene una stringa da IPFS utilizzando il CID fornito.
    - **/salvaFile (POST): Questo endpoint carica un file e lo salva su IPFS, restituendo il CID generato per il file.
    - **/ottieniContenutoFile/:cid (GET)**: Questo endpoint ottiene il contenuto di un file da IPFS utilizzando il CID fornito.

3. **Autenticazione**
    - **/avviaAutenticazione (POST)**: Questo endpoint avvia il processo di autenticazione per registrare un dispositivo IoT sul server. Richiede l'indirizzo del dispositivo, l'ID del dispositivo, l'indirizzo del server B, e altre informazioni necessarie per l'autenticazione. Restituisce un messaggio che indica se l'operazione è riuscita.
    - **/completaAutenticazione (POST)**: Questo endpoint completa il processo di autenticazione del dispositivo IoT. Richiede l'indirizzo del dispositivo, l'ID del dispositivo, e l'indirizzo del server. Restituisce un messaggio che indica se l'operazione è riuscita.
    - **/ottieniDatiAutenticazione (POST)**: Questo endpoint ottiene i dati di autenticazione per un dispositivo IoT. Richiede l'indirizzo del dispositivo e l'ID del dispositivo. Restituisce i dati di autenticazione.
    - **/autenticazioneStabilita (POST)**: Questo endpoint verifica se l'autenticazione è stata stabilita per un dispositivo IoT. Richiede l'indirizzo del dispositivo e l'ID del dispositivo. Restituisce un messaggio che indica se l'autenticazione è stata stabilita.

4. **Ottieni Informazioni**
    - **/getInfoDispositivoIot (POST)**: Questo endpoint ottiene informazioni su un dispositivo IoT specifico. Richiede l'indirizzo del dispositivo e l'ID del dispositivo. Restituisce le informazioni richieste.

    - **/getCIDs (POST)**: Questo endpoint ottiene i CID dei dispositivi specificati. Richiede l'indirizzo del dispositivo e gli ID dei due dispositivi. Restituisce i CID ottenuti.

5. **c_code**: Questa cartella contiene i codici sorgente C per eseguire operazioni di cifratura e generazioni delle chiavi.

**NOTA**: nel file autenticazione.js in chiaveA e chiaveB sono salvate la chiavi del disposito arduino A e dell'arduino B ottenute durante il loro enrollment.
