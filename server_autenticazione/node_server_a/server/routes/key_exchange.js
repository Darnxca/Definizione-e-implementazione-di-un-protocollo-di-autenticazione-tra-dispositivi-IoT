import express from "express";
const router = express.Router();

import { serverKeyStore, chiave_cifratura, ottieni_chiave_pubblica, ottieni_chiave_condivisa, cifra } from '../scripts/diffieHelman.js'


// Endpoint per l'avvio del contratto e registrazione del dispositivo IoT
router.post('/avviaScambio', async (req, res) => {
    const chiave_pubblica_serverA = req.body.pubkey;
    const mia_chiave_pubblica = ottieni_chiave_pubblica(serverKeyStore);
    
    const sharedKey = ottieni_chiave_condivisa(serverKeyStore,chiave_pubblica_serverA);

    const encrypted = cifra(sharedKey, chiave_cifratura);

    res.json({ success: "success", pubkey: mia_chiave_pubblica, encryptedText: encrypted });

});

export default router;