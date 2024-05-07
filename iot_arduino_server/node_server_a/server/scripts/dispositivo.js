class Dispositivo {
	constructor() {
		this._walletAddress = '0x90F79bf6EB2c4f870365E785982E1f101E93b906';
		this._puf = '';
		this._id = '';
		this._helper_data = ''
		this._key = '';
		this._prk = '';
        this._init = '';
	} 
	
    get walletAddress() { return this._walletAddress; }
	get puf() { return this._puf; }
	get id() { return this._id; }
	get ninit() { return this._init; }
	get helper_data() { return this._helper_data; }

	set puf(new_puf) {this._puf = new_puf; }
	set id(new_id) {this._id = new_id; }
	set ninit(new_init) {this._init = new_init; }
	set helper_data(new_helper) { this._helper_data = new_helper }
}

let dispositivoA = new Dispositivo();
let dispositivoB = new Dispositivo();

export { dispositivoA, dispositivoB };