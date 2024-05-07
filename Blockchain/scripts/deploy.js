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
}

// We recommend this pattern to be able to use async/await everywhere
// and properly handle errors.
main().catch((error) => {
  console.error(error);
  process.exitCode = 1;
});
