import express from "express";
const router = express.Router();

import { ottieniInformazioniDispositivo, ottieniCIDsDispositivi } from './../scripts/interazione_contratto.js'

// Endpoint per l'avvio del contratto e registrazione del dispositivo IoT
router.post('/getInfoDispositivoIot', async (req, res) => {
    const { address_dispositivo, id } = req.body;
    
    const {success, message} = await ottieniInformazioniDispositivo(address_dispositivo, id);

    res.json({ success: success, message: message });
});

  // Endpoint per l'avvio del contratto e registrazione del dispositivo IoT
router.post('/getCIDs', async (req, res) => {
  const { address_dispositivo, id_1, id_2 } = req.body;
  
  const {success, message} = await ottieniCIDsDispositivi(address_dispositivo, id_1, id_2);

  res.json({ success: success, message: message });
  
});

export default router;