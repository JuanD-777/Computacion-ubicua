const int PIN_BOTON = 2; 
const int PIN_LED   = 3;

//cambie el int por bool, ya que "encaja" mejor
bool estadoLed = LOW;

void setup() {
  pinMode(PIN_BOTON, INPUT);   // botón como entrada
  pinMode(PIN_LED, OUTPUT);    // LED como salida
}

void loop() {

  // esperar a que se presione el botón
  while (digitalRead(PIN_BOTON) == LOW) {}

  // cambia el estado del LED
  estadoLed = digitalRead(PIN_LED);
  digitalWrite(PIN_LED, !estadoLed);

  // esperar a que se suelte 
  while (digitalRead(PIN_BOTON) == HIGH) {}
}