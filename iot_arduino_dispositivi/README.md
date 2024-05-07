# Codice arduino README.md 

Server back-end scritto in Node.js progettato per far comunicare il dispositivo arduino (Nodo A)tramite la porta seriale UART con i server di autenticazione.

## Prerequisiti

- Installare gcc: versione utilizzata nel progetto "gcc version 12.3.0 (Ubuntu 12.3.0-1ubuntu1~23.04)"
- Installare scons: sudo apt install scons
- Installare gcc-avr: sudo apt-get install gcc-avr
- Installare avr-libc: sudo apt-get install avr-libc
- Installare minicom: sudo apt-get install minicom

## Uso
1. **Compilazione**
Per compilare il file del progetto andare nella cartella scripts ed eseguire:
```shell
    sudo bash ./compile_bootloader.sh
```
2. **Installazione**
Per installarlo su un dispositivo arduino, configuare in modo corretto un secondo arduino come ISP come segue collegando in modo crretto i cavi cone le corrispondenti porte:
| Dispositivo Programmatore| Target|
|---|---| 
| GND| GND |
| 5V | 5V |
| 10 | Reset |
| 11 | 11 |
| 12 | 12 |
| 13 | 13 |

Per ulteriori dettagli andare su: https://docs.arduino.cc/built-in-examples/arduino-isp/ArduinoISP/ 

Successivamente andare nella cartella scripts ed eseguire il seguendo comando:
```shell
    sudo bash ./flash_bootloader.sh  ../build/bootloader/bootloader.hex
```
3. **Verifica del funzionamento**
Per verificare il corretto funzionamento del programma collegarsi sulla porta UART tramite l'uso del comando minicom:
```shell
    minicom -D /dev/ttyACM0 -b 115200 -C ./test1.cap
```

## Spiegazione delle funzionalità associate ai caratteri per il dispositivo A
| Carattere| Funzionalità|
|---|---| 
| k | Genera la chiave del dispositvo e la stampa sulla console |
| p | Stampa il dump sulla console|
| r | Attende che sulla console siano scritti gli helper data e l'id |
| n | genera il nonce di inizializzazione e la prk e lo stampa sulla console |
| v | Fase di verifica del messaggio msg_a e generazione del messaggio SA |
| b| Fase di generazione del messaggio BA |
| c| Verifica il mac inviato dal dispositivo b (IDA , IDB ,NONCEAB) |
| q | Stampa messaggio errore |

## Spiegazione delle funzionalità associate ai caratteri per il dispositivo B
| Carattere| Funzionalità|
|---|---| 
| k | Genera la chiave del dispositvo e la stampa sulla console |
| p | Stampa il dump sulla console|
| r | Attende che sulla console siano scritti gli helper data e l'id |
| n | genera il nonce di inizializzazione e la prk e lo stampa sulla console |
| b| Verifica messaggio B e messaggio BA |
| c| Ottenimento nonceAB e generazione messaggio serverB |
| s | Generazione mac da inviare al dispositivo a (IDA , IDB ,NONCEAB)
| q | Stampa messaggio errore |