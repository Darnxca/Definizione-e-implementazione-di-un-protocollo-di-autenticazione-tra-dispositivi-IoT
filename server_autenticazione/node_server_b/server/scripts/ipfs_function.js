import { create } from 'kubo-rpc-client'
import { logErrori } from './utils.js';
// Libreria per leggere file
import fs from 'fs';

let ipfs_client;
// Connessione ad ipfs
async function InizializzaIPFS(){
    let ipfs_temp;
    if (!ipfs_client) {
        ipfs_temp = create('/ip4/127.0.0.1/tcp/5001');
        ipfs_client = ipfs_temp;
    } 
    return { ipfs_client: ipfs_client };
};

// Funzione per salvare su IPFS una stringa e ottenre il suo CID
export const salvaStringaSuIPFS = async (stringa) => {

    try {
        const { ipfs_client } = await InizializzaIPFS();

        // Aggiunta della stringa a IPFS e ottenimento del CID
        const { cid } = await ipfs_client.add(stringa);
        const valore_CID = cid.toString(); // Converti il CID in una stringa
        return valore_CID;
    } catch(error){
        const errore_stringa = JSON.stringify(error);
        logErrori(errore_stringa);

        return false;
    }
};

// Funzione per ottenere da IPFSuna stringa salvata tramite il suo CID
export const ottieniStringaDalCID = async (CID) => {

    try{
        const { ipfs_client } = await InizializzaIPFS();

        // Ottieni i byte del contenuto della stringa utilizzando la funzione cat()
        const contentBytes = [];
        for await (const chunk of ipfs_client.cat(CID)) {
            contentBytes.push(chunk);
        }
        // Concatena i byte del contenuto in una stringa
        const stringa = Buffer.concat(contentBytes).toString('utf-8');
    
        return stringa;
    } catch(error){
        const errore_stringa = JSON.stringify(error);
        logErrori(errore_stringa);

        return false;
    }
};

// Funzione per salvare su IPFS il contenuto di un file e ottenere il suo CID
export const salvaFileSuIPFS = async (file) => {

    try{
        const { ipfs_client } = await InizializzaIPFS();

        // Aggiungi il file al nodo IPFS
        const { cid } = await ipfs_client.add(fs.readFileSync(file.path));

        // Rimuovi il file temporaneo
        fs.unlinkSync(file.path);
    
        const valore_CID = cid.toString(); // Converti il CID in una stringa
        return valore_CID;
    } catch(error){
        const errore_stringa = JSON.stringify(error);
        logErrori(errore_stringa);

        return false;
    }
};

// Funzione per ottenere da IPFS il contenuto di un file salvato tramite il suo CID
export const ottieniFileDalCID = async (cid) => {

    try{
        const { ipfs_client } = await InizializzaIPFS();

        // Ottieni i byte del contenuto del file utilizzando la funzione cat()
        const contentBytes = [];
        for await (const chunk of ipfs_client.cat(cid)) {
            contentBytes.push(chunk);
        }

        // Concatena i byte del contenuto in una stringa
        const contenutoFile = Buffer.concat(contentBytes).toString('utf-8');
    
        // Ritorno il contenuto del file
        return contenutoFile;
    } catch(error){
        const errore_stringa = JSON.stringify(error);
        logErrori(errore_stringa);

        return false;
    }
};
