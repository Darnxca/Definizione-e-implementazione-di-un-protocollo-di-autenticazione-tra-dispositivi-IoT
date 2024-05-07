import crypto from 'crypto';
import aes256 from 'aes256';
import { createHash, randomBytes}  from 'crypto'

let serverKeyStore = null;
let chiave_cifratura = null;

function genera_chiavi(){
    return crypto.createECDH('secp256k1');
}

export function ottieni_chiave_pubblica(keysStore){
    return keysStore.getPublicKey().toString('base64');
};

export function ottieni_chiave_condivisa(keysStore, other_public_key64){
    return keysStore.computeSecret(other_public_key64, 'base64', 'hex')
}

// Funzione per generare la chiave univoca
function generateUniqueKey() {
    const key = randomBytes(32); // Genera una chiave casuale di 32 byte
    chiave_cifratura = createHash('sha512').update(key.toString('base64')).digest('hex');
    
}

export function cifra(key, messaggio){
    return aes256.encrypt(key, messaggio);
}

export function decifra(public_key, messaggio_cifrato){
    return aes256.decrypt(public_key, messaggio_cifrato);
}

// Funzione per generare la chiave pubblica e privata tramite diffiehellman
export async function generaChiaveDiffieHellman() {
    if (!serverKeyStore) {
        serverKeyStore = genera_chiavi();
        serverKeyStore.generateKeys();
        generateUniqueKey();
    }
}

export { serverKeyStore, chiave_cifratura};