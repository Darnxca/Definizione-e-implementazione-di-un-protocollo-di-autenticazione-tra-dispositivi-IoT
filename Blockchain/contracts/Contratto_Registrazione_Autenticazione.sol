// SPDX-License-Identifier: UNLICENSED
pragma solidity ^0.8.19;

import "hardhat/console.sol";

// Dichiarazione del contratto Registrazione/Autenticazione
contract Contratto_Registrazione_Autenticazione {

    // Struttura che rappresenta un dispositivo IoT
    struct DispositivoIoT {
        string CID;
        string helperData;
    }
    // Struttura che rappresenta le informazioni utili all'autenticazione
    struct Autenticazione {
        string addressB;
        string NInitA;
        string NSA;
        string NSB;
        bool confirmA;
        bool confirmB;
    }

    // Mappa che associa un valore hash (id del dispositivo) alle sue informazioni
    mapping(bytes32 => DispositivoIoT) private dispositiviIoT;

    // Mappa che associa il valore hash (Usare Mb come chiave chiedere) ad una richiesta di autenticazione
    mapping(bytes32 => Autenticazione) private autenticazioneDispositivi;

    // Evento per catturare la registrazione del dispositivo
    event RegistrazioneDispositivoIoTEvent(bytes32 riferimento);
    // Evento per catturare l' inizio della fase di autenticazione
    event avviaAutenticazioneEvent(bytes32 riferimento);
    // Evento per confermare l'autenticazione
    event confermaAutenticazione(string str);

    // Funzione per calcolare un riferimento per un dispositivo IoT
    function calcolaHash(string memory str) private pure returns (bytes32) {
        // Utilizza keccak256 per ottenere il valore hash dei dati
        return keccak256(abi.encodePacked(str));
    }

    // Funzione per registrare un nuovo dispositivo e restituire il suo riferimento nella mappa
    function registrazioneDispositivoIoT(string memory _id, string memory _CID, string memory _helperData) public {
        // Calcolo del riferimento
        bytes32 riferimento = calcolaHash(_id);

        // Controllo per verificare che il dispositivo non sia già registrato
        require(bytes(dispositiviIoT[riferimento].CID).length == 0, "Dispositivo registrato precedentemente.");
        
        // Controllo per verificare che i campi non siano vuoti
        require(bytes(_id).length > 0, "Il campo id non puo' essere vuoto");
        require(bytes(_CID).length > 0, "Il campo CID non puo' essere vuoto");
        require(bytes(_helperData).length > 0, "Il campo helperData non puo' essere vuoto");

        // Creazione di una nuova istanza del dispositivo IoT
        DispositivoIoT memory newDevice = DispositivoIoT({
            CID: _CID,
            helperData: _helperData
        });
        
        // Salvataggio del dispositivo nella mappa
        dispositiviIoT[riferimento] = newDevice;

        emit RegistrazioneDispositivoIoTEvent(riferimento);
    }

    // Funzione per avviare la fase di autenticazione fra i due dispositivi (salvataggio indirizzo del server B)
    function avviaAutenticazione(string memory _idA, string memory riferimentoAutenticazione, string memory _addressB, string memory _NInitA,
                                    string memory _NSA, string memory _NSB) public {

        bytes32 riferimento_idA = calcolaHash(_idA);
        bytes32 riferimento_autenticazione = calcolaHash(riferimentoAutenticazione);

        // Controllo per verificare che il dispositivo sia già registrato
        require(bytes(dispositiviIoT[riferimento_idA].CID).length > 0, "Dispositivo non registrato precedentemente!");
        // Controllo per verificare che l'autenticazione non sia già stata avviata verso il dispositivo con idA
        require(bytes(autenticazioneDispositivi[riferimento_autenticazione].addressB).length == 0, "Autenticazione gia avviata con questo dispositivo.");

        // Controllo per verificare che i campi non siano vuoti
        require(bytes(_addressB).length > 0, "L'indirizzo non puo' essere vuoto");
        require(bytes(_NInitA).length > 0, "Il campo NInitA non puo' essere vuoto");
        require(bytes(_NSA).length > 0, "Il campo NSA non puo' essere vuoto");
        require(bytes(_NSB).length > 0, "Il campo NSB non puo' essere vuoto");

        Autenticazione memory nuovaAutenticazione = Autenticazione ({
            addressB : _addressB,
            NInitA: _NInitA,
            NSA: _NSA,
            NSB: _NSB,
            confirmA: false,
            confirmB: false
        });
    
        autenticazioneDispositivi[riferimento_autenticazione] = nuovaAutenticazione;
        emit avviaAutenticazioneEvent(riferimento_autenticazione);        
    }

    // Funzione per confermare il completamento dell'autenticazione da parte di un server
    function completaAutenticazioneServer(string memory _idA, string memory riferimentoAutenticazione, bool server) public {
        
        bytes32 riferimento_idA = calcolaHash(_idA);
        bytes32 riferimento_autenticazione = calcolaHash(riferimentoAutenticazione);

        // Controllo per verificare che il dispositivo sia già registrato
        require(bytes(dispositiviIoT[riferimento_idA].CID).length > 0, "Dispositivo non registrato precedentemente.");
        // Controllo per verificare che il dispositivo sia già registrato
        require(bytes(autenticazioneDispositivi[riferimento_autenticazione].addressB).length > 0, "Autenticazione non avviata.");
      
        string memory str;

        // Se è true conferma il server B se è false il server A
        if(server){
            // Controllo se uno dei due dispositivi non ha già confermato l'autenticazione
            require(autenticazioneDispositivi[riferimento_autenticazione].confirmB == false,"Il dispositivo B ha gia' confermato l'autenticazione!");
            autenticazioneDispositivi[riferimento_autenticazione].confirmB = true;
            str = "Dispositivo B ha confermato l'autenticazione";
        } else {
            // Controllo se uno dei due dispositivi non ha già confermato l'autenticazione
            require(autenticazioneDispositivi[riferimento_autenticazione].confirmA == false,"Il dispositivo A ha gia' confermato l'autenticazione!");
            autenticazioneDispositivi[riferimento_autenticazione].confirmA = true;
            str = "Dispositivo A ha confermato l'autenticazione";
        }
    
        emit confermaAutenticazione(str);
    }

    // Funzione per ottenere le informazioni di un dispositivo tramite il suo riferimento
    function getInfoDispositivoIoT(string memory _id) public view returns (string memory, string memory, string memory) {  
        bytes32 riferimento = calcolaHash(_id);
        // Verifico che il dispositivo sia registrato
        require(bytes(dispositiviIoT[riferimento].CID).length > 0, "Dispositivo non registrato!");
        // Restituisco le informazioni del dispositivo associato al riferimento fornito
        return (
            _id,
            dispositiviIoT[riferimento].CID,
            dispositiviIoT[riferimento].helperData
        );
    }

    // Funzione per ottenere i CIDs dei due dispositivi che si vogliono autenticare
    function getCIDs(string memory _idA, string memory _idB) public view returns (string memory, string memory){
        bytes32 riferimento_idA = calcolaHash(_idA);
        bytes32 riferimento_idB = calcolaHash(_idB);

        // Verifico che i due dispositivi siano registrati
        require(bytes(dispositiviIoT[riferimento_idA].CID).length > 0, "Dispositivo A non registrato!");
        require(bytes(dispositiviIoT[riferimento_idB].CID).length > 0, "Dispositivo B non registrato!");

        // Restituisco i CIDs collegati agli id
        return(
            dispositiviIoT[riferimento_idA].CID,
            dispositiviIoT[riferimento_idB].CID
        );
    }

    // Funzione per ottenere i dati per stabilire la connessione con il server B
    function ottieniDatiAutenticazione(string memory _idA, string memory riferimentoAutenticazione) public view returns (string memory, string memory, string memory, string memory, string memory){
        bytes32 riferimento_idA = calcolaHash(_idA);
        bytes32 riferimento_autenticazione = calcolaHash(riferimentoAutenticazione);

        // Controllo per verificare che il dispositivo sia già registrato
        require(bytes(dispositiviIoT[riferimento_idA].CID).length > 0, "Dispositivo non registrato precedentemente.");
        // Controllo per verificare che il dispositivo sia già registrato
        require(bytes(autenticazioneDispositivi[riferimento_autenticazione].addressB).length > 0, "Autenticazione non avviata.");
    
        // Restituisco le informazioni del server che vuole avviare l'autenticazione
        return(
            autenticazioneDispositivi[riferimento_autenticazione].addressB,
            autenticazioneDispositivi[riferimento_autenticazione].NInitA,
            autenticazioneDispositivi[riferimento_autenticazione].NSA,
            autenticazioneDispositivi[riferimento_autenticazione].NSB,
            dispositiviIoT[riferimento_idA].CID
        );
    }

    // Funzione per controllare se entrambi i server hanno confermato l'avvenuta autenticazione
    function autenticazioneStabilita(string memory _idA, string memory riferimentoAutenticazione) public view returns (bool) {
        bytes32 riferimento_idA = calcolaHash(_idA);
        bytes32 riferimento_autenticazione = calcolaHash(riferimentoAutenticazione);

        // Controllo per verificare che il dispositivo sia già registrato
        require(bytes(dispositiviIoT[riferimento_idA].CID).length > 0, "Dispositivo non registrato precedentemente.");
        // Controllo per verificare che il dispositivo sia già registrato
        require(bytes(autenticazioneDispositivi[riferimento_autenticazione].addressB).length > 0, "Autenticazione non avviata.");
    
        // Se entrambi i server hanno confermato ritorna true
        if(autenticazioneDispositivi[riferimento_autenticazione].confirmB && autenticazioneDispositivi[riferimento_autenticazione].confirmA){
            return true;
        } else {
            return false;
        }
    }
}
