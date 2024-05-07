// server/index.js
import express from "express";
import cors from 'cors';
import https from 'https'; // Importa il modulo HTTPS
import { fileURLToPath } from 'url';
import fs from 'fs';
import path from 'path';
import bodyParser from 'body-parser';


import autenticazione from './routes/autenticazione_dispositivi.js';
import { avviaConnessioneArduino } from './scripts/comunicazione_arduino.js';

const PORT = process.env.PORT || 3012;
const HOST = '127.0.0.1';
const app = express();

// Ottieni il percorso del file corrente
const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

app.use(cors());
app.use(bodyParser.json());

app.use('/autenticazione', autenticazione);

// Configurazione del server HTTPS
const options = {
  key: fs.readFileSync(path.resolve(__dirname,'https_file/server_keyB.pem')),
  cert: fs.readFileSync(path.resolve(__dirname,'https_file/server_certB.pem'))
};

const server = https.createServer(options, app); // Usa HTTPS invece di app
server.listen(PORT, HOST, function () {
  const address = server.address().address;
  const port = server.address().port;

  console.log("Listening on " + address + ":" + port);
});

// Esempio di utilizzo
avviaConnessioneArduino();

export default app; 
