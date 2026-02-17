int LED = 3;				// LED en pin 3
int BRILLO;
int POT = 0;				// potenciometro en pin A0

void setup(){
	pinMode(LED, OUTPUT);		// usa el pin 3 como salida
	// las entradas analogicas no requieren inicializacion
}

void loop(){
	BRILLO = analogRead(POT) / 4;	// el valor es leido de entrada analogica divido por 4
	analogWrite(LED, BRILLO);	// brillo del LED es proporcional al giro del potenciometro
}
