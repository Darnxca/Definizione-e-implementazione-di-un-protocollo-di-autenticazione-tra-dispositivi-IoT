# Blockchain README

## Panoramica
Benvenuto nell README del mio progetto blockchain! Questo progetto mostra una configurazione di base utilizzando Hardhat, un ambiente di sviluppo per gli smart contract di Ethereum. Di seguito, troverai istruzioni su come iniziare con questo progetto, spiegazioni dei comandi forniti e alcune informazioni aggiuntive per aiutarti a orientarti.

## Per Iniziare
Prima di immergerti nel progetto, assicurati di avere Node.js e npm installati sul tuo sistema. Quindi, segui questi passaggi per configurare il progetto:

1. Clona questo repository sul tuo computer locale.
2. Naviga nella directory del progetto nel tuo terminale.
3. Esegui `npm install` per installare tutte le dipendenze del progetto.

## Comandi
Una volta configurato il progetto, puoi utilizzare i seguenti comandi per eseguire varie attività:

1. **Comando help:** Questo comando mostra il menu di aiuto per Hardhat, fornendo informazioni su comandi disponibili e opzioni.
```shell
    npx hardhat help
```

2. **Comando test:** Questo comando esegue i test per i contratti intelligenti nel progetto. Il flag `--network hardhat` specifica che i test devono essere eseguiti sulla rete locale di Hardhat.
```shell
    npx hardhat test --network hardhat
```

3. **Comando compile:** Questo comando viene utilizzato per compilare gli smart contract presenti nel progetto.
```shell
    npx hardhat compile
```
4. **Comando per avviare la blockchain:** Questo comando avvia un nodo Ethereum locale utilizzando Hardhat, consentendoti di interagire con la blockchain in locale per scopi di testing e sviluppo.
```shell
    npx hardhat node
```

5. **Deploy di un contratto:** Questo comando esegue lo script di deploy situato in `scripts/deploy.js`. Questo script è responsabile per il deploy dei contratti intelligenti sulla blockchain.
```shell
    npx hardhat run scripts/deploy.js
```

6. **Comando clean:** Questo comando viene utilizzato per pulire il progetto Hardhat rimuovendo tutti i file generati durante il processo di compilazione e di test, ripristina il progetto allo stato iniziale e garantisce un ambiente di sviluppo pulito. Questo comando è particolarmente utile quando si desidera ripulire il progetto prima di fare un nuovo deploy dei contratti o prima di eseguire una nuova serie di test.
```shell
    npx hardhat clean
```

## Strutture delle directory
- **Artifacts:** Gli artifacts sono i file generati durante la compilazione dei contratti intelligenti. Questi file contengono informazioni sui contratti, come ABI (Application Binary Interface), indirizzi dei contratti deployati e altre informazioni utili per l'interazione con i contratti dalla tua interfaccia utente o da altri contratti. Gli artifacts sono spesso utilizzati nelle applicazioni web decentralizzate (DApp) per consentire loro di comunicare con i contratti intelligenti sulla blockchain.
- **Contratti Intelligenti:** Il progetto include un contratto intelligente di esempio situato nella directory `contracts/`. Puoi esplorare e modificare questo contratto secondo necessità.
- **Test:** I file di test si trovano nella directory `test/`. Puoi aggiungere ulteriori test mentre sviluppi il tuo progetto per garantire la funzionalità dei tuoi contratti intelligenti.
- **Script di Deploy:** Lo script di deploy è situato in `scripts/deploy.js`. Puoi personalizzare questo script per effettuare il deploy dei tuoi contratti con configurazioni specifiche se necessario.
- **Script verifa costi:** Lo script per controllare i costi della blockchain è situato in `scripts/costi_funzioni.js`.
 
## Cosa contiene questo progetto
- **Smart contract:** Lo smart contracr presente in questo progetto è un contratto di registrazione e autenticazione per dispositivi IoT. Consente di registrare dispositivi e avviare processi di autenticazione tra di essi.
- **Registrazione Dispositivi IoT:** La funzione `registrazioneDispositivoIoT` consente di registrare un nuovo dispositivo IoT.
- **Avvio Autenticazione:** La funzione `avviaAutenticazione` avvia la fase di autenticazione tra due dispositivi.
- **Conferma Autenticazione:** La funzione `completaAutenticazioneServer` permette a un server di confermare il completamento dell'autenticazione.
- **Ottenere Informazioni:** Le funzioni `getInfoDispositivoIoT`, `getCIDs`, `ottieniDatiAutenticazione` forniscono informazioni e dati relativi ai dispositivi e agli processi di autenticazione.
- **Stato Autenticazione:** La funzione `autenticazioneStabilita` permette di controllare se entrambi i server hanno confermato l'avvenuta autenticazione.

## Inizializzare un Progetto da Zero

Se desideri iniziare un nuovo progetto da zero, segui questi passaggi:

1. **Creare una Nuova Cartella:** Crea una nuova cartella sul tuo computer per il tuo progetto blockchain.

2. **Inizializzare il Progetto npm:** Apri il terminale nella nuova cartella e esegui il comando:
```shell
   npm init -y
```
Questo comando creerà un file package.json predefinito per il tuo progetto.

3. **Installare Hardhat:** Esegui il comando seguente per installare Hardhat come dipendenza di sviluppo:
```shell
   npm install --save-dev hardhat
```

4. **Installare Hardhat Toolbox:** Installa Hardhat Toolbox eseguendo il comando:
```shell
    npm install @nomicfoundation/hardhat-toolbox
```
5. **Inizializzare il Progetto Hardhat:** Esegui il comando seguente per inizializzare il progetto Hardhat:
```shell
npx hardhat init
```
Segui le istruzioni per scegliere la configurazione del progetto. Di solito, si seleziona "Simple project" per un progetto JavaScript semplice.
Una volta completati questi passaggi, avrai un nuovo progetto Hardhat configurato e pronto per essere modificato.
