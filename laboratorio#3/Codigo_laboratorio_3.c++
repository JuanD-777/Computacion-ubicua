int LED = 3;				// LED en pin 3
int BRILLO;        // Variable que almacenará el valor de intensidad del LED (0 - 255)
int POT = 0;				// // Pin A0 donde está conectado el potenciómetro (entrada analógica)

void setup(){
	pinMode(LED, OUTPUT);		// se configura el pin 3 como salida para el led
	  // Nota: Las entradas analógicas (A0, A1, etc.) no necesitan pinMode()
     // porque el Arduino las reconoce automáticamente como entradas.
}

void loop(){
	BRILLO = analogRead(POT) / 4;	
	// analogRead(POT) lee un valor entre 0 y 1023 (resolución de 10 bits)
   // se divide entre 4 porque analogWrite trabaja con valores de 0 a 255 (8 bits)


	analogWrite(LED, BRILLO);	
	// analogWrite genera una señal PWM 
  // Esto simula un voltaje variable para controlar el brillo del LED
  // Si BRILLO = 0 → LED apagado
  // Si BRILLO = 255 → LED al máximo

}
