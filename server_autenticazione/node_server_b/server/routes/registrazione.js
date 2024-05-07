import express from "express";
const router = express.Router();

import { salvaStringaSuIPFS } from './../scripts/ipfs_function.js'
import { registraDispositivo } from './../scripts/interazione_contratto.js'

import {executeCProgram} from './../scripts/c_adapter.js';
import { chiave_cifratura, cifra } from './../scripts/diffieHelman.js'
import { generateDeviceID } from './../scripts/utils.js'
import { idMap } from './../scripts/myMapp.js'


// Endpoint per l'avvio del contratto e registrazione del dispositivo IoT
router.post('/registraDispositivoIoT', async (req, res) => {

  const { address_dispositivo, dump, helper_data} = req.body;

  //const {stdout} = await executeCProgram("cd server/c_code/ && ./genera_helper_data",`${dump}`);
  
  //let responseData = JSON.parse(stdout);

  //let helper_data = responseData.Helper_Data;


  let id = generateDeviceID(idMap);
  
  // Cifro il dump
  const encryptedData  = cifra(chiave_cifratura, dump);
  
  // Salvo il il dump cifrato su IPFS e ottengo il suo CID
  const CID = await salvaStringaSuIPFS(encryptedData);

  if (CID !== false){
  
    
    // Salvo il dispositivo sulla blockchain in caso di successo ottengo un suo riferimento
    const { success, message} = await registraDispositivo(address_dispositivo, id, CID, helper_data);
    
    if(success === true){
      res.json({ success: success, riferimento: message, id: id, helper_data: helper_data  });
    }else 
      res.json({ success: success, message: message  });
  } else {
    res.json({ success: false, message: "Errore nella connessione al server IPFS" });
  }

});

export default router;
