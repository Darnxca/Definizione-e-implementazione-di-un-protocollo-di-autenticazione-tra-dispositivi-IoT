import { SerialPort } from 'serialport';
import { RegexParser} from '@serialport/parser-regex';

import { registraDispositivoRequest, avviaProcessoAutenticazioneRequest, ottieniNonceAB, confermaAutenticazione } from './postRequestServer.js'
import { dispositivoA, dispositivoB} from './dispositivo.js';

import { input_console } from './input_console.js';

const port = new SerialPort({path:'/dev/ttyACM1',baudRate:115200});
const parser = port.pipe(new RegexParser({ regex: /[\r\n]+/ }))

let riferimento = '';
let messageSB = '';
let digestMsgSB = '';

let carattere = 'p'
let countk = 0;

port.on('open', () => {
  console.log('Serial Port Opened');
  console.time("start")
  console.time("puf")
});  


port.on('close', () => {
  console.log('Serial Port closed');
});  

// Funzione per avviare la connessione con il dispositivo sulla porta /dev/ttyACM1
// La funzione è completamente automatizzata per renderla interattiva decommentare
// carattere = await input_console("inserisci k "); e mettere in port.write() la variabile carattere
export function avviaConnessioneArduino(){
  

  const onDataReceived = async (data) => {
    let dataNew = data.trim().replace(/[\r\n]/g, '');
   
    // Parser che permette di analizzare i dati messi in output da dispositvo sulla porta UART
    switch(carattere){
      case 'p':
        carattere = '';
        dispositivoB.puf = dataNew;
        console.timeEnd("puf")
        //carattere = await input_console("inserisci k ");
        countk += 1;
        carattere = 'k';
        port.write('k');
        break;
      case 'k':
        carattere = '';
        console.log(dataNew)
        if(countk == 1){
          //carattere = await input_console("inserisci k ");
          carattere = 'k';
          port.write('k')
          console.time('chiave')
          countk +=1
        } else {
          console.timeEnd('chiave')
          //carattere = await input_console("inserisci r ");
          carattere = 'r';
          port.write('r')
        }
        break;
      case 'r':
        carattere = '';
        const helper_data = dataNew;
        console.time('Registrazione')
        const {id} = await registraDispositivoRequest(dispositivoB.walletAddress, dispositivoB.puf, helper_data );
        console.timeEnd('Registrazione')
        dispositivoB.id = id;
        dispositivoB.helper_data = helper_data
        port.write(id)
        parser.off('data', onDataReceived);
        console.log("true")
        break;
    }
  }

  parser.on('data', onDataReceived);
}

// funzione per avviare il processo di autenticazione
export const avviaAutenticazione = () =>{
  return new Promise((resolve, reject) => {
    port.write('n')
    carattere = 'n'
    
    const onDataReceived = async (data) => {
      let dataNew = data.trim().replace(/[\r\n]/g, '');
      
      switch(carattere){
        case 'n':
          carattere = '';
          dispositivoB.ninit = dataNew;
          console.time('AvvioAutenticazione')
          const success  = await avviaProcessoAutenticazioneRequest(dispositivoB.walletAddress, 
            dispositivoA.id, dispositivoA.helper_data, dispositivoA.ninit,
            dispositivoB.id, dispositivoB.helper_data, dispositivoB.ninit)
          console.timeEnd('AvvioAutenticazione')
            parser.off('data', onDataReceived);
             
            console.log(true)
            resolve(success);
            break;
      }
    }
    
  
    parser.on('data', onDataReceived);

  });
}

// funzione per confermare il processo di autenticazione
export const confermaAutenticazioneB = (ref, digestMsgB, messageBA, digestMsgBA)=> {
  riferimento = ref;
  return new Promise((resolve, reject) => {
    port.write('b', (err) =>{
      if (err){
        console.log("errore")
      }
      port.write(ref, (err) =>{
        if (err){
          console.log("errore")
        }
        port.write(digestMsgB, (err) =>{
          if (err){
            console.log("errore")
          }
          carattere = 'confDigestb'
          console.time("Verifica Mac B")
        })
      })
    })
    
    const onDataReceived = async (data) => {
      let dataNew = data.trim().replace(/[\r\n]/g, '');
      
      console.log("")
      switch(carattere){
        case 'confDigestb':
          console.timeEnd("Verifica Mac B")
          if(dataNew === 'successo'){
            console.log("")
            carattere = 'msgSB'
            console.time("Ottenimento nonce AB")
            const {nonceAB, digest} = await ottieniNonceAB(messageBA, digestMsgBA, dispositivoA.id, dispositivoB.id);
            console.timeEnd("Ottenimento nonce AB")
            port.write('c', (err) =>{
              if (err){
                console.log("errore")
              }
              port.write(messageBA, (err) =>{
                if (err){
                  console.log("errore")
                }
                port.write(dispositivoA.id, (err) =>{
                  if (err){
                    console.log("errore")
                  }
                  port.write(nonceAB, (err) =>{
                    if (err){
                      console.log("errore")
                    }
                    port.write(digest);
                    console.time("Verifica mac messaggio BA")
                  })
                })
              })
            })
          }
          break;
        case 'msgSB':
          console.timeEnd("Verifica mac messaggio BA")
          carattere = 'dmsgSB';
          messageSB = dataNew;
          break;
        case 'dmsgSB':
          carattere ='s';
          digestMsgSB = dataNew;
          console.time("Autenticazione B")
          const {conferma} = await confermaAutenticazione(dispositivoB.walletAddress, messageSB, digestMsgSB, dispositivoB.id, riferimento); 
          console.timeEnd("Autenticazione B")
          if(conferma === true){
            console.log("")
            port.write('s');
            console.timeEnd("start")
          }
          break;
        case 's':
          console.log("")
          parser.off('data', onDataReceived);
          resolve(dataNew);
          break;
      }
    }

    parser.on('data', onDataReceived);
  });
}