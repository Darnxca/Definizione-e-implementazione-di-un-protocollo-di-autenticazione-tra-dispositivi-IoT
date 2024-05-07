// We require the Hardhat Runtime Environment explicitly here. This is optional
// but useful for running the script in a standalone fashion through `node <script>`.
//
// You can also run a script with `npx hardhat run <script>`. If you do that, Hardhat
// will compile your contracts, add the Hardhat Runtime Environment's members to the
// global scope, and execute the script.
const hre = require("hardhat");

async function main() {
  const Contratto_Registrazione_Autenticazione = await hre.ethers.deployContract("Contratto_Registrazione_Autenticazione");

  await Contratto_Registrazione_Autenticazione.waitForDeployment();

  console.log("deployed to ", await Contratto_Registrazione_Autenticazione.getAddress());

  const idA = 'aabbccdd' // in byte ha grandezza 4, sulla blockchain è una stringa con lunghezza 8
  const idB = 'ddccbbaa' // in byte ha grandezza 4, sulla blockchain è una stringa con lunghezza 8
  const CIDA = 'QmPK1s3pNYLi9ERiq3BDxKa4XosgWwFRQUydHUtz4YgpqA'
  const CIDB = 'QmPK1s3pNYLi9ERiq3BDxKa4XosgWwFRQUydHUtz4YgpqB'
  const helper_data_esempio = '1EA2EF05C8544AC73084B9700FBFD4DE2817637F0DBAAEA9F134472F7A990C2A77712DB04BACD7505005961BEF9FF06522AA8444B5CDD529B847041025399975E10F868EC182BCA837604E6AC4F520D53BE05D518DADD56D513F564CE4ACA0A77C35351404C6D694CC22710ABDB04CAA417AC84C3A909AA0F37146C0CE02569B9EEFE296294DEB08613983F5F3133D3A67D05C6F0F16B4D7210646EF1613B35726D89204548344B7AF874094D894C02269430CCDCF88857EAEB988C90E601047EBFC8303F21AAA0377750F7F0E49D1F24B8968C2A544EC5BFA855B6F05CEAD712D852ED3CF1CDB080CF334CEC33CD41F7217B7B48748D698FD306BDECE0596E95C09F7E9DEB900884A2A4FEFFB6718005484ABB26CAA99D55554E2784CB841B93A5BDD456CCF23722D21E8B1EDD69701' // in byte ha grandezza 304 in stringa 608
  const nonce_esempio = 'CSWBMs9ozHO01Z9JxDJXcYb4q3OH20VENs3I8Q==' // in byte ha grandezza 6, sulla blockchain è crittato con aes256 e ha dimensione 40
  const rif ='c88828f91926cb8c6b97d37a566fa67f1b96c80bee0708c8fa6466de53859f97dcfa2e8f0e3792701f7036a2fe53d6638bc53ae77d430690f0be78d4856505a4'
  const indirizzo_serverB = "127.0.0.1:3007";

  // registro A
  await Contratto_Registrazione_Autenticazione.registrazioneDispositivoIoT(idA, CIDA, helper_data_esempio);
  // registro B
  await Contratto_Registrazione_Autenticazione.registrazioneDispositivoIoT(idB, CIDB, helper_data_esempio);

  // informazioni dispositivo A
  await Contratto_Registrazione_Autenticazione.getInfoDispositivoIoT(idA);
  // informazioni dispositivo B
  await Contratto_Registrazione_Autenticazione.getInfoDispositivoIoT(idB);

  // get cids
  await Contratto_Registrazione_Autenticazione.getCIDs(idA, idB);

  // Autenticazione
  await Contratto_Registrazione_Autenticazione.avviaAutenticazione(idA, rif, indirizzo_serverB, nonce_esempio, nonce_esempio, nonce_esempio);

  // Ottieni dati autenticazione
  await Contratto_Registrazione_Autenticazione.ottieniDatiAutenticazione(idA, rif);
  // b conferma
  await Contratto_Registrazione_Autenticazione.completaAutenticazioneServer(idA, rif, true);
  // a conferma
  await Contratto_Registrazione_Autenticazione.completaAutenticazioneServer(idA, rif, false);

  // verifica avvenuta autenticazione
  await Contratto_Registrazione_Autenticazione.autenticazioneStabilita(idA, rif);
}

// We recommend this pattern to be able to use async/await everywhere
// and properly handle errors.
main().catch((error) => {
  console.error(error);
  process.exitCode = 1;
});