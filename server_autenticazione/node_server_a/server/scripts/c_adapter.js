import { exec } from 'child_process';

// Funzione che esegue il programma C come processo separato
export function executeCProgram(programName, args) {
    return new Promise((resolve, reject) => {

        exec(`${programName} ${args}`, (error, stdout, stderr) => {
            if (error) {
                console.error(`Errore durante l'esecuzione del programma: ${error}`);
                reject(error);
                return;
            }
            resolve({ stdout });
        });

    });
}