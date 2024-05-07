export async function dispositivoEsiste(contract, id) {
    try {
        await contract.methods.getInfoDispositivoIoT(id).call();
        return true; // Il dispositivo esiste
    } catch (error) {
        return false; // Il dispositivo non esiste
    }
}

export async function autenticazioneEsiste(contract, id) {
    try {
        await contract.methods.ottieniDatiAutenticazione(id).call();
        return true; // L'autenticazione esiste
    } catch (error) {
        return false; // L'autenticazione non esiste
    }
}
