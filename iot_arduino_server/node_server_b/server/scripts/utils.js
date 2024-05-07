// Libreria per leggere file
import fs from 'fs';
// Libreria per upload del file
import multer from 'multer';
// Libreria per la gestione del tempo
import moment from 'moment';

import axios from 'axios';
import https from 'https'; 

import { serverKeyStore, ottieni_chiave_pubblica, ottieni_chiave_condivisa, decifra } from './diffieHelman.js'

export const configureUpload = () => {
  const uploadDirectory = './server/temp_uploads/';
  if (!fs.existsSync(uploadDirectory)) {
      fs.mkdirSync(uploadDirectory);
  }
  
  const upload = multer({ dest: './server/temp_uploads/' });

  return upload;
};

// Funzione per registrare gli errori su file
export const logErrori = (errore) => {
    const timestamp = moment().format('YYYY-MM-DD HH:mm:ss');
    const errorMessage = `[${timestamp}] ${errore}\n`;

    // Specifica il percorso del file di log degli errori
    const logFilePath = './server/log/log_error.txt';

    fs.promises.appendFile(logFilePath, errorMessage)
        .catch(err => console.error('Errore durante il salvataggio dell\'errore:', err));
};

export const generateDeviceID  = (idMap) => {
  // Lunghezza dell'ID del dispositivo (in byte)
  const idLength = 4;

  let id;
  do {
      // Genera un array di byte casuali
      const deviceID = Array.from({ length: idLength }, () => Math.floor(Math.random() * 256));

      // Converte l'array di byte in un array di stringhe esadecimali
      const deviceIDHex = deviceID.map(value => value.toString(16).toUpperCase().padStart(2, '0'));

      // Crea l'ID del dispositivo come stringa esadecimale
      id = deviceIDHex.join('');
  } while (idMap.has(id)); // Continua a generare ID fino a quando non trovi uno unico

  // Ritorna l'ID del dispositivo come stringa esadecimale
  return id;
};

export const avviaScambio = async (host) => {
  const pubkey = ottieni_chiave_pubblica(serverKeyStore);

  try {
    const response = await axios.post(`https://${host}/keyexchange/avviaScambio`, { pubkey }, {
      httpsAgent: new https.Agent({ rejectUnauthorized: false })  }); // httpsAgent usato perchè le chiavi sono autogenerate

    if (response.data.success === 'success') {
        
        const chiave_pubblica_serverB = response.data.pubkey;
        const encryptText = response.data.encryptedText;

        const sharedKey = ottieni_chiave_condivisa(serverKeyStore,chiave_pubblica_serverB);
        const chiave_cifratura = decifra(sharedKey, encryptText);

        return { find: true, key: chiave_cifratura }; 

        // Puoi fare ulteriori azioni qui in base alla risposta dell'API
    } else {
        return { find: false, key: 'Errore durante l\'avvio dello scambio:'+ response.data.message }; 
    }
  } catch (error) {
      console.error('Si è verificato un errore durante la chiamata API:', error);
      return { find: false , key :""};
  }
}