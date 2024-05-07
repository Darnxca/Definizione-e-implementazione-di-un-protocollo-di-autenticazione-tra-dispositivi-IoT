import express from "express";
const router = express.Router();

import { createHash } from 'crypto'

import { salvaInformazioniAutenticazione, completaAutenticazioneServer, ottieniDatiAutenticazione, autenticazioneIsStabilita } from '../scripts/interazione_contratto.js'
import { ottieniInformazioniDispositivo, ottieniCIDsDispositivi } from '../scripts/interazione_contratto.js'
import { ottieniStringaDalCID } from '../scripts/ipfs_function.js'
import { chiave_cifratura, cifra, decifra } from '../scripts/diffieHelman.js'

import {executeCProgram} from '../scripts/c_adapter.js';
import { authMap }  from '../scripts/myMapp.js';

import {avviaScambio } from '../scripts/utils.js'

const chiaveA = "B72371F7950CAA06DA22215A17BE277C"
const chiaveB = "741DC332B656A395D16FF4ADA22320B9"

// Endpoint per l'avvio del contratto e registrazione del dispositivo IoT
router.post('/avviaAutenticazione', async (req, res) => {
    const { address_dispositivo ,id_dispositivoA, helper_dataA, nonceA, id_dispositivoB, helper_dataB, nonceB} = req.body;
    
    const {success, message} = await ottieniCIDsDispositivi(address_dispositivo, id_dispositivoA, id_dispositivoB);

    const CIDA = message[0];
    const CIDB = message[1];

    // Ottengo i dump cifrati su IPFS
    const encryptedDumpA = await ottieniStringaDalCID(CIDA);
    const encryptedDumpB = await ottieniStringaDalCID(CIDB);

    // Controllo la presenza dei due CID
    if (encryptedDumpA !== false && encryptedDumpB !== false){

        
        const decryptedDumpA = decifra(chiave_cifratura, encryptedDumpA)
        const decryptedDumpB = decifra(chiave_cifratura, encryptedDumpB)
     
        // da inviare al file c i due dump, i due id, ninita e ninitb
        const args = `${decryptedDumpA} ${decryptedDumpB} ${helper_dataA} ${helper_dataB} ${nonceA} ${nonceB} ${id_dispositivoA} ${id_dispositivoB}`;
        const {stdout} = await executeCProgram("cd server/c_code/ && ./genera_M_e_PRK",`${args}`);

        let responseData = JSON.parse(stdout);

        const indirizzo_serverB = "127.0.0.1:3007";
        let NinitA = responseData["NinitA"];
        let Ns_a = responseData["Ns_a"];
        let Ns_b = responseData["Ns_b"];
        let MA = responseData["MA"];
        let MB = responseData["MB"];
        let digest = responseData["digest"];

        let riferimentoAutenticazione = createHash('sha512').update("rif" + MB).digest('hex');
        
        let NinitEncrypt = cifra(chiave_cifratura,NinitA);
        let Ns_aEncrypt = cifra(chiave_cifratura,Ns_a);
        let Ns_bEncrypt = cifra(chiave_cifratura,Ns_b);

        const {success, message} = await salvaInformazioniAutenticazione(address_dispositivo ,id_dispositivoA, riferimentoAutenticazione, indirizzo_serverB, NinitEncrypt, Ns_aEncrypt, Ns_bEncrypt);

        if(success === true){
            const hashedValue = createHash('sha512').update("Auth" + nonceA).digest('hex');
            authMap.set(hashedValue, [{ MA, digest }])
            
            res.json({ success: success});
        }
        else{
            res.json({ success: success, message: message});
        }
    } else {
        res.json({ success: false, message: "CID non presenti"});
    }
});

router.post('/checkAvvioAutenticazione', async (req, res) => {
    const { nonce } = req.body;

    // Trova il valore corrispondente nel tuo map
    const hashedNonce = createHash('sha512').update("Auth" + nonce).digest('hex');
    const values = authMap.get(hashedNonce);

    // Verifica se il valore è presente
    if (values) {
        // Distruggi l'elemento dalla mappa dopo averlo ottenuto
        authMap.delete(hashedNonce);

        // Estrai i valori richiesti
        const { MA, digest } = values[0];
        
        // Restituisci i valori
        res.json({ success: true, MA, digest });
    } else {
        res.json({ success: false, message: "Nonce non valido o scaduto" });
    }
});

// Endpoint per verificare l'inizio della connessione da parte del server A per il dispositivo A
router.post('/verificaMacDispositivoA', async (req, res) => {
    const { address_dispositivo, M_SA, digest, id, helper_data, rif } = req.body;
    
    const rifAuth = createHash('sha512').update("rif" + rif).digest('hex');

    const { success, message} = await ottieniDatiAutenticazione(address_dispositivo, id, rifAuth);
    
    if (success) {
        
        const address_serverB = message[0];
        const encryptNInitA = message[1];
        const encryptNSA = message[2];
        const encryptNSB = message[3];
        const CID = message[4];

        if (success){
            const { find , key } = await avviaScambio(address_serverB);
            
            if (find) {
                const NInita = decifra(key,encryptNInitA);
                const NSA = decifra(key,encryptNSA);
                const NSB = decifra(key,encryptNSB);

                const encryptedDump = await ottieniStringaDalCID(CID);

                // Controllo la presenza dei due CID
                if (encryptedDump !== false){

                    const decryptedDump = decifra(key, encryptedDump)

                    // da inviare al file c i due dump, i due id, ninita e ninitb
                    const args = `${decryptedDump} ${helper_data} ${id} ${NInita} ${NSA} ${NSB} ${M_SA} ${digest} ${chiaveA}`;
                    const {stdout} = await executeCProgram("cd server/c_code/ && ./serverA_verifica_mac",`${args}`);
                    console.log(stdout)
                    res.json({ success: stdout});

                } else {
                    res.json({ success: false, message: "CID non trovato"});
                }
                
            } else {
                res.json({ success: false, message: key});
            }
        } else {
            res.json({ success: false, message: message});
        }
        
    } else {
        res.json({ success: false, message: message});
    }
});

// Endpoint per verificare l'inizio della connessione da parte del server A per il dispositivo A
router.post('/ottieniNonceAB', async (req, res) => {

    const {msg_BA, digest, id_dispositivoA, id_dispositivoB } = req.body;


    const args = `${msg_BA} ${digest} ${id_dispositivoA} ${id_dispositivoB}`;
    const {stdout} = await executeCProgram("cd server/c_code/ && ./ottenimento_nonceAB",`${args}`);
    let responseData = JSON.parse(stdout);
    res.json({ success: true, nonceAB :  responseData.nonceAB, digest: responseData.digest });
});

// Endpoint per il completamento dell'autenticazione
router.post('/confermaAutenticazioneDispositivoB', async (req, res) => {
    const { address_dispositivo, M_SB, digest, id, rif } = req.body;
   
    // da inviare al file c i due dump, i due id, ninita e ninitb
    const args = `${id} ${M_SB} ${digest}`;
    const {stdout} = await executeCProgram("cd server/c_code/ && ./serverB_verifica_mac",`${args}`);
            
    if(stdout.trim() === "true"){

        // il server B conferma la'utenticazione
        const server = true;

        let riferimentoAutenticazione = createHash('sha512').update("rif" + rif).digest('hex');
        
        // Avvio il procesos di autenticazione e salvo le informazioni del dispositivo B
        const {success, message} = await completaAutenticazioneServer(address_dispositivo ,id, riferimentoAutenticazione, server);

        res.json({ success: success, message: message });   
    }else {
        res.json({ success: "false"});
    }
});

// Endpoint per il completamento dell'autenticazione
router.post('/completaAutenticazioneA', async (req, res) => {
    const { address_dispositivo, id, rif} = req.body;

    const server = false;
    let riferimentoAutenticazione = createHash('sha512').update("rif" + rif).digest('hex');
    
    // Avvio il procesos di autenticazione e salvo le informazioni del dispositivo B
    const {success, message} = await completaAutenticazioneServer(address_dispositivo ,id, riferimentoAutenticazione, server);

    res.json({ success: success, message: message });
});


// Endpoint per l'avvio del contratto e registrazione del dispositivo IoT
router.post('/autenticazioneStabilita', async (req, res) => {
    const { address_dispositivo, id, rif} = req.body;
  
    let riferimentoAutenticazione = createHash('sha512').update("rif" + rif).digest('hex');
    
    const { success, message} = await autenticazioneIsStabilita(address_dispositivo, id, riferimentoAutenticazione);

    res.json({ success: success, message: message });    
});

export default router;