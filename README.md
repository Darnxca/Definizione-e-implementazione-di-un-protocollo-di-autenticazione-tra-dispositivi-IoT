# Progetto di Autenticazione IoT con IPFS e Blockchain

Questo progetto si propone di facilitare l'autenticazione tra dispositivi IoT utilizzando tecnologie decentralizzate come IPFS (InterPlanetary File System) e la blockchain. Il processo prevede l'archiviazione di informazioni sensibili su IPFS per ottenere un CID (Content Identifier), che viene quindi utilizzato per memorizzare e validare le informazioni sulla blockchain.

## Motivazioni del Progetto

- **Decentralizzazione:** Utilizzando IPFS e la blockchain, si elimina la necessità di un'autorità centrale per l'autenticazione, migliorando la sicurezza e la resistenza ai singoli punti di fallimento.
- **Immutabilità dei Dati:** Le informazioni memorizzate sulla blockchain sono immutabili, garantendo un registro affidabile e sicuro delle transazioni e delle interazioni tra dispositivi.
- **Tracciabilità e Verifica:** La blockchain consente di tracciare e verificare le interazioni tra dispositivi IoT in modo trasparente e affidabile.

## Struttura del Progetto

Il progetto è organizzato in tre principali componenti:
1. **blockchain:** Questa cartella contiene il codice relativo alla gestione della blockchain. È responsabile della memorizzazione e della validazione delle informazioni tra dispositivi IoT. 
Per maggiori dettagli, consultare il README.md al suo interno.
2. **iot_arduino_dispositivi:** Questa cartella contiene il codice che verrà installato sui dispositivi arduino, consultare il README.md al suo interno per capire quali dipendenze installare e come verificare il corretot funzionamento.
3. **iot_arduino_server:** All'interno sono prenseti due cartelle che rappresentano rispettivamente dei server nodejs collegati ai due dispositivi arduino attraverso la porta UART per potere comunicare sia tra di loro che con i server di autenticazione.
Per maggiori dettagli, consultare il README.md al loro interno.
4. **server autenticazione:** All'interno sono prenseti due cartelle che rappresentano rispettivamente i server di autenticazione A e B. Per maggiori dettagli, consultare il README.md al loro interno.
5. **simulazione dispositivo:**  Sono contenuto alcuni codici c per simulare il funzionamento di un dispositivo iot. I codici c usati sul server sono contenuti nelle cartelle:
- `5)generazione_M_e_PRK`
- `7)serverA_verifica_mac`
- `9)server_b_controlla_mac`
- `11)server_ottiene_nonceAB`
6. **Tempi e dati:** Al suo interno è presente l'excel che contiene i tempi di esecuzione, i costi delle transazioni e del contratto.
7. **Risultati scyther**: Contiene il protocollo implementato nel linguaggio scyther e le varie immagini che contengono i risultati dell'analisi formale.



