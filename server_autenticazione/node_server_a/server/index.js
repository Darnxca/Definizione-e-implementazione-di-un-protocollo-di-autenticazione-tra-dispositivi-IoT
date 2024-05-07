// server/index.js
import express from "express";
import cors from 'cors';
import https from 'https'; // Importa il modulo HTTPS
import { fileURLToPath } from 'url';
import fs from 'fs';
import path from 'path';
import bodyParser from 'body-parser';

import registrazione from './routes/registrazione.js';
import ottieniInformazioni from './routes/ottieniInformazioni.js';
import autenticazione from './routes/autenticazione.js';
import ipfs from './routes/ipfs.js';
import keyexchange from './routes/key_exchange.js';

import { generaChiaveDiffieHellman } from "./scripts/diffieHelman.js";

const PORT = process.env.PORT || 3007;
const HOST = '127.0.0.1';
const app = express();

// Ottieni il percorso del file corrente
const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

app.use(cors());
app.use(bodyParser.json());

generaChiaveDiffieHellman();

app.use('/registrazione', registrazione);
app.use('/ottieniInformazioni', ottieniInformazioni);
app.use('/autenticazione', autenticazione);
app.use('/ipfs/',ipfs);
app.use('/keyexchange/',keyexchange)

// Configurazione del server HTTPS
const options = {
  key: fs.readFileSync(path.resolve(__dirname,'https_file/server_keyA.pem')),
  cert: fs.readFileSync(path.resolve(__dirname,'https_file/server_certA.pem'))
};

const server = https.createServer(options, app); // Usa HTTPS invece di app
server.listen(PORT, HOST, function () {
  const address = server.address().address;
  const port = server.address().port;

  console.log("Listening on " + address + ":" + port);
});

export default app; 
