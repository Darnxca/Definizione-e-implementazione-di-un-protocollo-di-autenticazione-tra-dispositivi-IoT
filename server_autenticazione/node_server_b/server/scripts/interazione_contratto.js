// Importa le funzioni necessarie da altri moduli
import { getWeb3, getContract, logErrori } from './utils.js';

let web3;
let contract;

// Middleware per ottenere web3 e il contratto
async function InizializzaContratto(){
    let web3_temp, contract_temp;
    if (!web3 || !contract) {
        web3_temp = await getWeb3();
        //const accounts = await web3.eth.getAccounts();
        contract_temp = await getContract(web3_temp);
        web3 = web3_temp;
        contract = contract_temp
    }
    return { web3: web3, contract: contract };
};

// Funzione per registrare un dispositivo sulla blockchain
export const registraDispositivo = async (address_dispositivo, id_dispositivo, CID, helper_data) => {

    const { contract } = await InizializzaContratto();
    
    try {
        // Esegui la registrazione del dispositivo IoT
        const output = await contract.methods.registrazioneDispositivoIoT(id_dispositivo, CID, helper_data).send({ from: address_dispositivo });
        // Log dell'evento restituito dalla transazione
        const riferimento = output.events.RegistrazioneDispositivoIoTEvent.returnValues.riferimento;

        return { success: true, message: riferimento };
    } catch (error) {
        const errore_stringa = JSON.stringify(error);

        logErrori(errore_stringa);

        switch(true) {
            case errore_stringa.includes("Il campo id non puo' essere vuoto"):
                return { success: false, message: 'Il campo id non può essere vuoto' };
            case errore_stringa.includes("Il campo CID non puo' essere vuoto"):
                return { success: false, message: 'Il campo CID non può essere vuoto' };
            case errore_stringa.includes("Il campo helperData non puo' essere vuoto"):
                return { success: false, message: 'Il campo helper data non può essere vuoto' };
            case errore_stringa.includes("Dispositivo registrato precedentemente."):
                return { success: false, message: 'Dispositivo gia\' registrato!' };
            default:
                return { success: false, message: 'Errore sconosciuto!' };
        }
    }
}

// Funzione per salvare le informazioni per raggiungere il server sulla blockchain
export const salvaInformazioniAutenticazione = async (address_dispositivo, id_dispositivo, riferimentoAutenticazione, indirizzo_serverB, InitA, NSA, NSB) => {

    const { contract } = await InizializzaContratto();

    try {
        // Esegui l'avvio dell'autenticazione
        const output = await contract.methods.avviaAutenticazione(id_dispositivo, riferimentoAutenticazione, indirizzo_serverB, InitA, NSA, NSB).send({ from: address_dispositivo });
        
        // Log dell'evento restituito dalla transazione
        const riferimento = output.events.avviaAutenticazioneEvent.returnValues.riferimento;
    
        return { success: true, message: riferimento};

      } catch (error) {

        const errore_stringa = JSON.stringify(error);

        logErrori(errore_stringa);

        switch(true) {
            case errore_stringa.includes("L'indirizzo non puo' essere vuoto"):
                return { success: false, message: 'L\'indirizzo del server non può essere vuoto!' };
            case errore_stringa.includes("Il campo NInitA non puo' essere vuoto"):
                return { success: false, message: 'Il campo NInitA non può essere vuoto!' };
            case errore_stringa.includes("Il campo NSA non puo' essere vuoto"):
                return { success: false, message: 'Il campo NSA non può essere vuoto' };
            case errore_stringa.includes("Il campo NSB non puo' essere vuoto"):
                return { success: false, message: 'Il campo NSB non può essere vuoto' };
            case errore_stringa.includes("Dispositivo non registrato precedentemente!"):
                return { success: false, message: `Dispositivo ${id_dispositivo} non registrato!` };
            case errore_stringa.includes("Autenticazione gia avviata con questo dispositivo."):
                return { success: false, message: `Processo di autenticazione già avviato con il dispositivo ${id_dispositivo}!` };
            default:
                return { success: false, message: 'Errore sconosciuto!' };
        }
    }
}

// Funzione per registrare il completamento della fase di autenticazione da parte di un dispositivo
export const completaAutenticazioneServer = async (address_dispositivo, id_dispositivo, riferimentoAutenticazione, server) => {

    const { contract } = await InizializzaContratto();

    try {
      
        const output = await contract.methods.completaAutenticazioneServer(id_dispositivo, riferimentoAutenticazione, server).send({ from: address_dispositivo });
  
        const messaggio = output.events.confermaAutenticazione.returnValues.str;

        return { success: true, message: messaggio };
      } catch (error) {

        const errore_stringa = JSON.stringify(error);
        
        logErrori(errore_stringa);

        switch(true) {
            case errore_stringa.includes("Dispositivo non registrato precedentemente."):
                return { success: false, message: `Dispositivo ${id_dispositivo} non registrato!` };
            case errore_stringa.includes("Autenticazione non avviata."):
                return { success: false, message: 'Autenticazione non avviata!' };
            case errore_stringa.includes("Il dispositivo B ha gia' confermato l'autenticazione!"):
                return { success: false, message: `Il dispositivo ${id_dispositivo} ha già confermato l\'autenticazione!` };
            case errore_stringa.includes("Il dispositivo A ha gia' confermato l'autenticazione!"):
                return { success: false, message: `Il dispositivo ${id_dispositivo} ha già confermato l\'autenticazione!` };
            default:
                return { success: false, message: 'Errore sconosciuto!' };
        }
    }
}

//
export const ottieniDatiAutenticazione = async (address_dispositivo, id, riferimentoAutenticazione) => {
    const { contract } = await InizializzaContratto();

    try {
        // Esegui la registrazione del dispositivo IoT
        const informazioniAutenticazione = await contract.methods.ottieniDatiAutenticazione(id, riferimentoAutenticazione).call({ from: address_dispositivo });
        return { success: true, message: informazioniAutenticazione };

      } catch (error) {

        const errore_stringa = JSON.stringify(error);
        
        logErrori(errore_stringa);

        switch(true) {
            case errore_stringa.includes("Dispositivo non registrato precedentemente."):
                return { success: false, message: `Dispositivo ${id} non registrato!` };
            case errore_stringa.includes("Autenticazione non avviata."):
                return { success: false, message: 'Autenticazione non avviata!' };
            default:
                return { success: false, message: 'Errore sconosciuto!' };
        }
    }
}

//
export const autenticazioneIsStabilita = async (address_dispositivo, id, riferimentoAutenticazione) => {
    const { contract } = await InizializzaContratto();

    try {
        // Esegui la registrazione del dispositivo IoT
        const isStabilita = await contract.methods.autenticazioneStabilita(id, riferimentoAutenticazione).call({ from: address_dispositivo });
        
        return { success: true, message: isStabilita };

      } catch (error) {

        const errore_stringa = JSON.stringify(error);
        
        logErrori(errore_stringa);

        switch(true) {
            case errore_stringa.includes("Dispositivo non registrato precedentemente."):
                return { success: false, message: `Dispositivo ${id} non registrato!` };
            case errore_stringa.includes("Autenticazione non avviata."):
                return { success: false, message: 'Autenticazione non avviata!' };
            default:
                return { success: false, message: 'Errore sconosciuto!' };
        }
    }
}
// Funzione per ottenere le informazioni di un dispositivo dalla blockchain
export const ottieniInformazioniDispositivo = async (address_dispositivo, id_dispositivo) => {

    const { contract } = await InizializzaContratto();

    try {
        // Ottengo le informazioni del dispositivo dalla blockchain
        const infoDispositivo = await contract.methods.getInfoDispositivoIoT(id_dispositivo).call({ from: address_dispositivo });

        return { success: true, message: infoDispositivo };

    } catch (error) {
        const errore_stringa = JSON.stringify(error);

        logErrori(errore_stringa);

        switch(true) {
            case errore_stringa.includes("Dispositivo non registrato!"):
                return { success: false, message: `Dispositivo con id: ${id_dispositivo} non registrato`};
            default:
                return { success: false, message: 'Errore sconosciuto!' };
        }
    }
}

// Funzione per ottenere i CID di due dispositivi dalla blockchain
export const ottieniCIDsDispositivi = async (address_dispositivo, id_dispositivo1, id_dispositivo2) => {

    const { contract } = await InizializzaContratto();

    try {
        // Esegui la registrazione del dispositivo IoT
        const CIDs = await contract.methods.getCIDs(id_dispositivo1, id_dispositivo2).call({ from: address_dispositivo });
        return { success: true, message: CIDs };
    } catch (error) {
        const errore_stringa = JSON.stringify(error);

        logErrori(errore_stringa);

        switch(true) {
            case errore_stringa.includes("Dispositivo A non registrato!"):
                return { success: false, message: `Dispositivo con id: ${id_dispositivo1} non registrato`};
            case errore_stringa.includes("Dispositivo B non registrato!"):
                return { success: false, message: `Dispositivo con id: ${id_dispositivo2} non registrato`};
            default:
                return { success: false, message: 'Errore sconosciuto!' };
        }   
    }
}