import { SerialPort } from 'serialport';
import { RegexParser} from '@serialport/parser-regex';

import { registraDispositivoRequest, inviaDatiaB, checkVerificaAvvioAutenticazione, verificaMacServerA, bconfermaAutenticazione, confermaAutenticazione } from './postRequestServer.js'
import { dispositivoA} from './dispositivo.js';
import { input_console } from './input_console.js';

let messaggeA = "";
let digestMsgA = "";

let messageSA ='';
let digestMSA ='';
let rif = '';

let digestMsgB = '';
let messageBA = ''
let digestMsgBA = '';

let digestAut = '';

let carattere = 'p'
let countk = 0;

// Funzione per avviare la connessione con il dispositivo sulla porta /dev/ttyACM0
// La funzione è completamente automatizzata per renderla interattiva decommentare
// carattere = await input_console("inserisci k "); e mettere in port.write() la variabile carattere
export function avviaConnessioneArduino(){
  const port = new SerialPort({path:'/dev/ttyACM0',baudRate:115200});
  const parser = port.pipe(new RegexParser({ regex: /[\r\n]+/ }))
  
  port.on('open', () => {
    console.log('Serial Port Opened');
    console.time('start')
    console.time("puf")
  });  
  
  port.on('close', () => {
    console.log('Serial Port closed');
  });  

  // Parser che permette di analizzare i dati messi in output da dispositvo sulla porta UART
  parser.on('data', async (data) =>{
    let dataNew = data.trim().replace(/[\r\n]/g, '');

    switch(carattere){
      case 'p':
        carattere = '';
        dispositivoA.puf = dataNew;
        console.timeEnd("puf")
        //carattere = await input_console("inserisci k ");
        countk += 1;
        carattere = 'k';
        port.write('k');
        break;
      case 'k':
        carattere = '';
        //console.log(dataNew)
        if(countk == 1){
          //carattere = await input_console("inserisci k ");
          carattere = 'k';
          port.write('k')
          console.time('chiave')
          countk +=1
        } else {
          //carattere = await input_console("inserisci r ");
          console.timeEnd('chiave')
          carattere = 'r';
          port.write('r')
        }
        break;
      case 'r':
        carattere = '';
        const helper_data = dataNew;
        console.time('Registrazione')
        const {id} = await registraDispositivoRequest(dispositivoA.walletAddress, dispositivoA.puf, helper_data );
        console.timeEnd('Registrazione')
        dispositivoA.id = id;
        dispositivoA.helper_data = helper_data
        port.write(id);
        //carattere = await input_console("inserisci n ");
        carattere = 'n';
        port.write('n')
        //port.write('n') // genero nonce
        break;
      case 'n':
        carattere = ''
        // salvo il nonce di inizializzazione (farlo anche su B)
        dispositivoA.ninit = dataNew;
        console.time('AvvioAutenticazione')
        const success = await inviaDatiaB(dispositivoA.id, dispositivoA.helper_data, dispositivoA.ninit)
        console.timeEnd('AvvioAutenticazione')
       
        // se tutto va bene inizia il controlla per l'avvenuta autenticazione
        if (success == true){
          let loop = false;
          
          // loop per controllare l'avvenuta autenticazione
          while(loop === false){
            let { successAvvio, MA, digest} = await checkVerificaAvvioAutenticazione(dispositivoA.ninit);
            loop = successAvvio;
            messaggeA = MA;
            digestMsgA = digest;
          }
          //console.log(loop)

          //carattere = await input_console("inserisci v ");
          carattere = 'v';
          // avvio generazione del messaggio MSA
          port.write('v', (err) =>{
            if (err){
              console.log("errore")
            }
            port.write(messaggeA, (err) =>{
              if (err){
                console.log("errore")
              }
              port.write(digestMsgA, (err) => {
                if (err){
                  console.log("errore")
                }
                carattere = 'msa'
                console.time('Generazione messaggio ServerA')
              })
            })
          })
        }
        break;
      case 'msa':
        console.timeEnd('Generazione messaggio ServerA')
        carattere ='dmsa'
        messageSA = dataNew
        break;
      case 'dmsa':
        carattere ='rif'
        digestMSA =dataNew
        break;
      case 'rif':
        carattere =''
        rif = dataNew
        // verifica de messaggio MSA sul server di autenticazione A
        console.time('Verifica mac del Server A')
        let { macVerificatoA } = await verificaMacServerA(dispositivoA.walletAddress, messageSA, digestMSA, dispositivoA.id, dispositivoA.helper_data, rif);
        console.timeEnd('Verifica mac del Server A')
        console.log(macVerificatoA)
        if(macVerificatoA === true) {
          // inizio generazione del messaggio BA
          port.write('b', (err) =>{
            if (err){
              console.log("errore")
            }
            carattere = 'dmsb'
            console.time('Generazione messaggio e mac BA')
          })
        }else {
          console.error("Mac non valido")
        }
        break;
      case 'dmsb':
        console.timeEnd('Generazione messaggio e mac BA')
        carattere = 'msba'
        digestMsgB = dataNew
        break;
      case 'msba':
        carattere = 'dmsba'
        messageBA = dataNew
        break;
      case 'dmsba':
        carattere = ''
        digestMsgBA = dataNew
        //console.log(dataNew)
        // invio dati msgB e msgBA
        let { successAutenticazione, digestAut} = await bconfermaAutenticazione(rif, digestMsgB, messageBA, digestMsgBA);
        console.log()
        if (successAutenticazione === false){
          console.log("errore");      
        } else {
          console.log("");
          carattere = 'c';

          port.write('c', (err) =>{
            if (err){
              console.log("errore")
            }
            port.write(digestAut);
            console.time('Verifica messaggio mandato dal server B')
          })
          
        }
        break;
      case 'c':
        console.timeEnd('Verifica messaggio mandato dal server B')
        carattere = '';
        console.log("");
        if(dataNew === 'successo'){
          console.time('conferma Autenticazione A')
          // controllo per verificare che tutto il processo di autenticazione è stato completato
          const {conferma, message} = await confermaAutenticazione(dispositivoA.walletAddress, dispositivoA.id, rif);
          console.timeEnd('conferma Autenticazione A')
          if (conferma === true){
            console.log(message);
            console.timeEnd('start')
          }
        }
    }
  }) 
}
