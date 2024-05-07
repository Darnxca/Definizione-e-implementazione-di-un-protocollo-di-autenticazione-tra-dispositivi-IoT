import * as readline from 'node:readline/promises';
import { stdin as input, stdout as output } from 'node:process';



export async function input_console(str){
    const rl = readline.createInterface({ input, output });

    const answer = await rl.question(str);
    
    rl.close();

    return answer;
}
