import os
from my_script import c_exec

def crea_cartelle():
    # Definisci i percorsi delle cartelle
    percorsi_cartelle = [
        '../file_dispositivi',
        '../file_dispositivi/A_invia_B',
        '../file_dispositivi/B_invia_A',
        '../file_dispositivi/dispositivo_1',
        '../file_dispositivi/dispositivo_2'
    ]

    # Controlla e crea le cartelle se non esistono
    for cartella in percorsi_cartelle:
        if not os.path.exists(cartella):
            os.makedirs(cartella)
            print(f"Cartella creata in: {cartella}")

    # Percorsi dei file all'interno delle ultime due cartelle dell'array
    nomi_file = ['contract_address.txt','puf_response_dispositivo_2.puf',
                 'contract_address.txt', 'puf_response_dispositivo_1.puf']
    valori_prefissati = ['0x90F79bf6EB2c4f870365E785982E1f101E93b906', 
                            'EE7CCEEDEEC975CDD62CFEFB2FBC4AFF3E3FF4EFADDE616FFF7F7BBF7F83B77CCDFF796FFF7576D61F1F1F01C6F5F5FD6FBB571FBBDEAAABE7EF45ED95B4E7D9D7ED37696F2BC7D70C64BA277CB230D6673FA3FC77BFFDE96EFEEB57D3B9FFE097FB27F7A6FEE7EAFFFBA7DE5309CBAD870BBEEF99CAAFAC716F1FAEF3FEBE18EFAAAE1FDFCCBEE5647FFD78F8B625A6B9B6FD9D6FF6DD1EC6B7EF9DDB06EBA6FEF9DBC4DD8FAF6F327D6D6EC8B1B2F9DF5DCEFBF7AF7ABBEEEF79FB650597DE571BABDFF8FD5FECFF7A6BFF8AFFF0536B1D2AEABEDCEFFE762A73FFB367FEEC28338DAD19EBAA532FC6BC6B247FEE1BFF7387F3B6BBBFEBFF7A7A07D0EF6EB6FE7FBD7FC7FEFA24BF2FD89CEFCF9E99B6E22EF7F3BCBE5FBF8FDFCBFF761D95E5D33BBABDE569FBD2DF3B88BC315BBF',
                            '0x9965507D1a55bcC2695C58ba16FB37d819B0A4dc', 'EE7CCEEDEEC975CD562CFEFB2FBC4AFF3E3FF4EFADDE616FFF7F7FBF7F83B77CCDFF396FFF7576D61F1F1F03C6F5F5FD6FBB571FBBDEAAABE3EF45ED95B4E7D9D7EDBF6D6F2BC7D70C64BA277CA231D6673FA3FCE7BFFDE96EFEEB57D3B9FFE097FB27F7A6FEE7EAFF7BA7FE5309CBAD870BBEEF89EAAF8C716F1FAEF3FEBE18EFAAAE1BDFCCAEE5647FFD78F8B625A6B9B6FD9D6FF6DD0EC6B7EF9DDB06EBA4FEF9DB44DDCFAF6F327D6D6EC8B1B2F9DF5DCEFBF3AF72BBEEEF78FA650597DE571BABDFF8F55FECFF7A6BFB0AFFF0536B1D2AEABEDC6EFE763A73FBB367FEEC29338DAD19EBAA532FC6BC6B247FAE1BFF7387F396BBBFEBFF7A7A07D0EF6EB6FE7FBD7FC7FEFA24972FD89CEFCF9E99B6E22EF7F3BCBE5FBF8FDF5BFF361D95E5D33BBABDE569FBD2DF3B88BC315ABF']

    # Itera sulle coppie di nomi di file e valori prefissati
    for i in range(2):
        path_cartella = percorsi_cartelle[-(i + 1)]
        for nome_file, valore_prefissato in zip(nomi_file[i * 2:(i + 1) * 2], valori_prefissati[i * 2:(i + 1) * 2]):
            path_file = os.path.join(path_cartella, nome_file)
            if not os.path.exists(path_file):
                with open(path_file, 'w') as file:
                    file.write(valore_prefissato)
                print(f"File creato in: {path_file}")

if __name__ == "__main__":
    crea_cartelle()

    c_exec.registra_dispositivi()
    print("Dispositivi A e B registrati")
    c_exec.generaNonce()
    print("Nonce generati")
    c_exec.avviaAutenticazione()
    print("B avvia il processo autenticazione avviato")
    c_exec.a_controlla_autenticazione()
    print("A inizia il processo di verifica")
    c_exec.b_conferma_autenticazione()
    print("B conferma l'autenticazione")
    c_exec.a_conferma_autenticazione()
    print("A conferma l'autenticazione")
    c_exec.verifica_avvenuta_autenticazione()
    print("Autenticazione stabilità")
