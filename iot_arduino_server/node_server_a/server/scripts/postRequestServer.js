import axios from 'axios';
import https from 'https'; 

export const registraDispositivoRequest = async (address_dispositivo, dump, helper_data) => {
    try {
      const response = await axios.post(`https://127.0.0.1:3008/registrazione/registraDispositivoIoT`, 
        { address_dispositivo, dump , helper_data}, {
        httpsAgent: new https.Agent({ rejectUnauthorized: false })  }); // httpsAgent usato perchè le chiavi sono autogenerate
      if (response.data.success === true) {
          const id = response.data.id;
          const helper_data = response.data.helper_data;
          
          return { id: id, helper_data: helper_data }; 
      } else {
          return { find: false, key: 'Errore durante l\'avvio dello scambio:'+ response.data.message }; 
      }
    } catch (error) {
        console.error('Si è verificato un errore durante la chiamata API:', error);
        return { find: false , key :""};
    }
}

export const inviaDatiaB = async (id, helper_data, ninit) => {
  try {
    const response = await axios.post(`https://127.0.0.1:3012/autenticazione/OttieniDatidaA`, 
      { id, helper_data, ninit }, {
      httpsAgent: new https.Agent({ rejectUnauthorized: false })  }); // httpsAgent usato perchè le chiavi sono autogenerate
    if (response.data.success === true) {
        
        return true; 
    } else {
        return false; 
    }
  } catch (error) {
      console.error('Si è verificato un errore durante la chiamata API:', error);
      return false;
  }
}

export const checkVerificaAvvioAutenticazione = async (nonce) => {
  try {
    const response = await axios.post(`https://127.0.0.1:3008/autenticazione/checkAvvioAutenticazione`, 
      { nonce}, {
      httpsAgent: new https.Agent({ rejectUnauthorized: false })  }); // httpsAgent usato perchè le chiavi sono autogenerate
      
      if (response.data.success === true) {
        
        return {successAvvio: true, MA: response.data.MA, digest: response.data.digest}; 
    } else {
        return { successAvvio: false }; 
    }
  } catch (error) {
      console.error('Si è verificato un errore durante la chiamata API:', error);
      return { successAvvio: false};
  }
}

export const verificaMacServerA = async (address_dispositivo, M_SA, digest, id, helper_data, rif) => {

  try {
    
    const response = await axios.post(`https://127.0.0.1:3007/autenticazione/verificaMacDispositivoA`, 
      { address_dispositivo,  M_SA, digest, id, helper_data, rif}, {
      httpsAgent: new https.Agent({ rejectUnauthorized: false })  }); // httpsAgent usato perchè le chiavi sono autogenerate
      if (response.data.success === 'true') {
        return {macVerificatoA: true}; 
        // Puoi fare ulteriori azioni qui in base alla risposta dell'API
    } else {
        return { macVerificatoA: false }; 
    }
  } catch (error) {
      console.error('Si è verificato un errore durante la chiamata API:', error);
      return { macVerificatoA: false};
  }
}

// funzione usata per comunicare al nodo B che può tentare di avviare la conferma di autenticazione
export const bconfermaAutenticazione = async (ref, digestMsgB, messageBA, digestMsgBA) => {
  try {
    const response = await axios.post(`https://127.0.0.1:3012/autenticazione/avviaConfermaAutenticazione`, 
      { ref, digestMsgB, messageBA, digestMsgBA}, {
      httpsAgent: new https.Agent({ rejectUnauthorized: false })  }); // httpsAgent usato perchè le chiavi sono autogenerate
      
      if (response.data.success === true) {
        
        return {successAutenticazione: true, digestAut: response.data.digestAut};
    } else {
        return { successAutenticazione: false }; 
    }
  } catch (error) {
      console.error('Si è verificato un errore durante la chiamata API:', error);
      return { successAvvio: false};
  }
}

export const confermaAutenticazione = async (address_dispositivo, id, rif) => {
  try {
    const response = await axios.post(`https://127.0.0.1:3007/autenticazione/completaAutenticazioneA`, 
      { address_dispositivo, id, rif}, {
      httpsAgent: new https.Agent({ rejectUnauthorized: false })  }); // httpsAgent usato perchè le chiavi sono autogenerate
      if (response.data.success === true) {
        return ({conferma: true, message: response.data.message}); 
    } else {
        return ({conferma: false, message: "Autenticazione fallita"}); 
    }
  } catch (error) {
      console.error('Si è verificato un errore durante la chiamata API:', error);
      return { macVerificatoA: false};
  }
}


