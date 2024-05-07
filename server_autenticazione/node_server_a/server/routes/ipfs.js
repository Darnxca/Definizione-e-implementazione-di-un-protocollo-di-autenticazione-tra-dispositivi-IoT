import express from "express";
const router = express.Router();
// Libreria per leggere file
import fs from 'fs';
// Importa le funzioni necessarie da altri moduli
import { configureUpload } from './../scripts/utils.js'; 
import { salvaStringaSuIPFS, ottieniStringaDalCID, salvaFileSuIPFS, ottieniFileDalCID } from './../scripts/ipfs_function.js'

// Endpoint per salvare una stringa sul client ipfs
router.get('/salvaStringa/:stringa', async (req, res) => {
    // Stringa da salvare su ipfs
    const str = req.params.stringa;

    // Aggiunta della stringa a ipfs e ottenimento del cid
    const cid  = await salvaStringaSuIPFS(str);
 
    // Invia il CID come risposta JSON
    res.json({ success: true, message: cid });
});

// Endpoint per ottenere una stringa dal suo CID
router.get('/ottieniStringa/:CID', async (req, res) => {
    // Cid per ottenere la stringa
    const cid = req.params.CID;

    // Ottengo la stringa tramite il suo CID
    const stringa = await ottieniStringaDalCID(cid);

    // Invia la stringa come risposta JSON
    res.json({ success: true, message: stringa });
   
});


const upload = configureUpload();

// Endpoint per l'upload del contentuo file e aggiunta del file su ipfs
router.post('/salvaFile', upload.single('file'), async (req, res) => {
    // File da salvare
    const { file } = req;

    // Aggiungi il file al nodo IPFS e ottengo il suo CID
    const cid  = await salvaFileSuIPFS(file);

    // Invio il CID come risposta JSON
    res.json({ success: true, message: cid});
});


// Endpoint per ottenere il contenuto di un file da IPFS
router.get('/ottieniContenutoFile/:cid', async (req, res) => {  
    // CID del file da ottenere
    const cid = req.params.cid;

    // Concatena i byte del contenuto in una stringa
    const contenutoFile = await ottieniFileDalCID(cid);
 
    // Invia il contenuto del file come risposta JSON
    res.json({ success: true, message: contenutoFile });
});

export default router;