import axios from 'axios';
import https from 'https'; 

export const registraDispositivoRequest = async (address_dispositivo, dump, helper_data) => {  
    try {
      const response = await axios.post(`https://127.0.0.1:3008/registrazione/registraDispositivoIoT`, 
        { address_dispositivo, dump, helper_data }, {
        httpsAgent: new https.Agent({ rejectUnauthorized: false })  }); // httpsAgent usato perchè le chiavi sono autogenerate
      if (response.data.success === true) {
          const id = response.data.id;
          const helper_data = response.data.helper_data;
          
          return { id: id, helper_data: helper_data }; 
          // Puoi fare ulteriori azioni qui in base alla risposta dell'API
      } else {
          return { find: false, key: 'Errore durante l\'avvio dello scambio:'+ response.data.message }; 
      }
    } catch (error) {
        console.error('Si è verificato un errore durante la chiamata API:', error);
        return { find: false , key :""};
    }
  }


  export const avviaProcessoAutenticazioneRequest = async (address_dispositivo, id_dispositivoA, helper_dataA, nonceA, id_dispositivoB, helper_dataB, nonceB) => {
    try {
      const response = await axios.post(`https://127.0.0.1:3008/autenticazione/avviaAutenticazione`, 
        { address_dispositivo, id_dispositivoA, helper_dataA, nonceA, id_dispositivoB, helper_dataB, nonceB }, {
        httpsAgent: new https.Agent({ rejectUnauthorized: false })  }); // httpsAgent usato perchè le chiavi sono autogenerate
      if (response.data.success === true) {
          return true; 
          // Puoi fare ulteriori azioni qui in base alla risposta dell'API
      } else {
          return false; 
      }
    } catch (error) {
        console.error('Si è verificato un errore durante la chiamata API:', error);
        return false;
    }
  }


  export const ottieniNonceAB = async (msg_BA, digest, id_dispositivoA, id_dispositivoB) => {
    try {
      const response = await axios.post(`https://127.0.0.1:3008/autenticazione/ottieniNonceAB`, 
        { msg_BA, digest, id_dispositivoA, id_dispositivoB}, {
        httpsAgent: new https.Agent({ rejectUnauthorized: false })  }); // httpsAgent usato perchè le chiavi sono autogenerate
      if (response.data.success === true) {
          return ({nonceAB: response.data.nonceAB, digest: response.data.digest}); 
          // Puoi fare ulteriori azioni qui in base alla risposta dell'API
      } else {
          return false; 
      }
    } catch (error) {
        console.error('Si è verificato un errore durante la chiamata API:', error);
        return false;
    }
  }


  export const confermaAutenticazione = async (address_dispositivo, M_SB, digest, id, rif) => {
    try {
      const response = await axios.post(`https://127.0.0.1:3008/autenticazione/confermaAutenticazioneDispositivoB`, 
        { address_dispositivo, M_SB, digest, id, rif}, {
        httpsAgent: new https.Agent({ rejectUnauthorized: false })  }); // httpsAgent usato perchè le chiavi sono autogenerate
        if (response.data.success === true || response.data.success === "true") {
          return ({conferma: true, message: response.data.message}); 
          // Puoi fare ulteriori azioni qui in base alla risposta dell'API
      } else {
          return ({conferma: false, message: response.data.message}); 
      }
    } catch (error) {
        console.error('Si è verificato un errore durante la chiamata API:', error);
        return false;
    }
  }