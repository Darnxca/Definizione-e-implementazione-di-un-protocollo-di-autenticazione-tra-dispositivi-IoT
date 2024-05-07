import subprocess
import os

def registra_dispositivi():
    # Ottieni la directory corrente del file Python
    directory_corrente = os.path.dirname(os.path.abspath(__file__))
    
    # Imposta la directory di lavoro sulla cartella degli eseguibili
    directory_eseguibili = os.path.join(directory_corrente, '..', 'eseguibili_c')
    os.chdir(directory_eseguibili)

    # Costruisci il percorso completo dell'eseguibile
    dispositivo_1 = os.path.join(directory_eseguibili, "dispositivo_1")
    dispositivo_2 = os.path.join(directory_eseguibili, "dispositivo_2")

    # Esegui l'eseguibile
    subprocess.run([dispositivo_1])
    subprocess.run([dispositivo_2])

def generaNonce():
    # Ottieni la directory corrente del file Python
    directory_corrente = os.path.dirname(os.path.abspath(__file__))
    
    # Imposta la directory di lavoro sulla cartella degli eseguibili
    directory_eseguibili = os.path.join(directory_corrente, '..', 'eseguibili_c')
    os.chdir(directory_eseguibili)

    # Costruisci il percorso completo dell'eseguibile
    nonce1 = os.path.join(directory_eseguibili, "nonce1")
    nonce2 = os.path.join(directory_eseguibili, "nonce2")

    # Esegui l'eseguibile
    subprocess.run([nonce1])
    subprocess.run([nonce2])

def avviaAutenticazione():
    # Ottieni la directory corrente del file Python
    directory_corrente = os.path.dirname(os.path.abspath(__file__))
    
    # Imposta la directory di lavoro sulla cartella degli eseguibili
    directory_eseguibili = os.path.join(directory_corrente, '..', 'eseguibili_c')
    os.chdir(directory_eseguibili)

    # Costruisci il percorso completo dell'eseguibile
    avvia_autenticazione = os.path.join(directory_eseguibili, "avvia_autenticazione")

    # Esegui l'eseguibile
    subprocess.run([avvia_autenticazione])

def a_controlla_autenticazione():
    # Ottieni la directory corrente del file Python
    directory_corrente = os.path.dirname(os.path.abspath(__file__))
    
    # Imposta la directory di lavoro sulla cartella degli eseguibili
    directory_eseguibili = os.path.join(directory_corrente, '..', 'eseguibili_c')
    os.chdir(directory_eseguibili)

    # Costruisci il percorso completo dell'eseguibile
    disp_a_check_autenticazione = os.path.join(directory_eseguibili, "disp_a_check_autenticazione")

    # Esegui l'eseguibile
    subprocess.run([disp_a_check_autenticazione])

def b_conferma_autenticazione():
    # Ottieni la directory corrente del file Python
    directory_corrente = os.path.dirname(os.path.abspath(__file__))
    
    # Imposta la directory di lavoro sulla cartella degli eseguibili
    directory_eseguibili = os.path.join(directory_corrente, '..', 'eseguibili_c')
    os.chdir(directory_eseguibili)

    # Costruisci il percorso completo dell'eseguibile
    disp_b_check_autenticazione = os.path.join(directory_eseguibili, "disp_b_check_autenticazione")

    # Esegui l'eseguibile
    subprocess.run([disp_b_check_autenticazione])

def a_conferma_autenticazione():
    # Ottieni la directory corrente del file Python
    directory_corrente = os.path.dirname(os.path.abspath(__file__))
    
    # Imposta la directory di lavoro sulla cartella degli eseguibili
    directory_eseguibili = os.path.join(directory_corrente, '..', 'eseguibili_c')
    os.chdir(directory_eseguibili)

    # Costruisci il percorso completo dell'eseguibile
    disp_a_conferma_autenticazione = os.path.join(directory_eseguibili, "disp_a_conferma_autenticazione")

    # Esegui l'eseguibile
    subprocess.run([disp_a_conferma_autenticazione])

def verifica_avvenuta_autenticazione():
    # Ottieni la directory corrente del file Python
    directory_corrente = os.path.dirname(os.path.abspath(__file__))
    
    # Imposta la directory di lavoro sulla cartella degli eseguibili
    directory_eseguibili = os.path.join(directory_corrente, '..', 'eseguibili_c')
    os.chdir(directory_eseguibili)

    # Costruisci il percorso completo dell'eseguibile
    check_autenticazione = os.path.join(directory_eseguibili, "check_autenticazione")

    # Esegui l'eseguibile
    subprocess.run([check_autenticazione])