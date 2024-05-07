import express from "express";
const router = express.Router();

import { avviaAutenticazione, confermaAutenticazioneB } from './../scripts/comunicazione_arduino.js';
import { dispositivoA, dispositivoB} from './../scripts/dispositivo.js';
// Endpoint per l'avvio del contratto e registrazione del dispositivo IoT
router.post('/OttieniDatidaA', async (req, res) => {

    const { id, helper_data, ninit} = req.body;
    
    dispositivoA.id = id;
    dispositivoA.helper_data = helper_data;
    dispositivoA.ninit = ninit
    
    const success = await avviaAutenticazione();

    res.json({ success: success});
    
  
  });


router.post('/avviaConfermaAutenticazione', async (req, res) => {

    const { ref, digestMsgB, messageBA, digestMsgBA} = req.body;
    
    const digestAut  = await confermaAutenticazioneB(ref, digestMsgB, messageBA, digestMsgBA)

    res.json({ success: true, digestAut: digestAut});
    
  
});
  
export default router;