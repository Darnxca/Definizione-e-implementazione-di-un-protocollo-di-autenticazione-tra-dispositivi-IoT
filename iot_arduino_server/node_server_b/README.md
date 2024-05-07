# Back-end node.js README.md 

Server back-end scritto in Node.js progettato per far comunicare il dispositivo arduino (Nodo B)tramite la porta seriale UART con i server di autenticazione.

## Prerequisiti

- Node.js installato sul tuo sistema
- Gestore di pacchetti npm


## Installazione

1. **Installa le dipendenze:**

```shell
   npm install
```

## Uso
1. **Connessione del dispositivo**
Come prima operazione connettere il dispositivo arduino e assicurarsi che sia sulla porta "/dev/ttyACM1" altrimenti verrà restituito un errore.

2. **Avvia il server:**
```shell
    npm start
```

## Struttura dei due progetti:

I due progetti hanno la seguente struttura delle directory:

- **https_file**: Contiene i certificati generati da openSSL per poter simulare una connessione https.
- **routes**: contiene delle API che permettono al dispositivo di ricevere dati dal dispositivo A.
- **scripts**: Questa directory contiene gli script ausiliari utilizzati per varie operazioni come la compilazione del contratto, l'avvio del server, ecc. Lo script più importante è "comunicazione_arduino.js" che contiene il codice per comunicare con arduino utilizzando il canale USART. Lo script "postRequestServer.js" contiene tutte le funzione che effettuano chiamate post sia al server di autenticazione sia al dispositivo A.