// ======================================================
//      DISPENSADOR AUTOMÁTICO LCD + TOGGLE + VASO
// ======================================================

#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 10, 9, 8, 6);

const int TRIG = 3;
const int ECHO = 5;
const int relayPin = 7;
const int botonPin = 13;

long duracion;
int distancia;

int vasoVacio;
int vasoLleno;
bool llenadoCompleto = false;
const int diferenciaLlenado = 5;

int contadorPulsos = 0;
unsigned long ultimoPulso = 0;
bool botonAnterior = HIGH;
unsigned long tiempoPresionado = 0;
bool botonSostenido = false;

int modo = 0;

bool estadoBomba = false;
bool manoDetectada = false;

unsigned long ultimaLectura = 0;
const unsigned long INTERVALO = 300;

bool enTransicion = false;



void setup() {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(botonPin, INPUT_PULLUP);
  digitalWrite(relayPin, HIGH);
  Serial.begin(9600);
  lcd.begin(16, 2);
  pantallaMenu();
}


// LOOP PRINCIPAL
void loop() {
  if (enTransicion) return;

  leerBoton();

  if (contadorPulsos > 0 && millis() - ultimoPulso > 800) {
    if (modo == 0) {
      if (contadorPulsos == 1) activarToggle();
      else if (contadorPulsos == 2) activarVaso();
    }
    contadorPulsos = 0;
    botonAnterior = HIGH;
  }

  if (millis() - ultimaLectura >= INTERVALO) {
    ultimaLectura = millis();
    if (modo == 1) modoToggle();
    else if (modo == 2) modoVaso();
  }
}

// LEER BOTÓN

void leerBoton() {
  bool estadoBoton = digitalRead(botonPin);

  if (estadoBoton == LOW && botonAnterior == HIGH) {
    contadorPulsos++;
    ultimoPulso = millis();
    tiempoPresionado = millis();
    botonSostenido = false;
    Serial.println("BOTON PRESIONADO");
  }

  if (estadoBoton == LOW && !botonSostenido) {
    if (millis() - tiempoPresionado >= 3000) {
      botonSostenido = true;
      contadorPulsos = 0;
      Serial.println("BOTON 3 SEG - VOLVIENDO AL MENU");
      volverMenu();
    }
  }

  if (estadoBoton == HIGH && botonAnterior == LOW) {
    botonSostenido = false;
  }

  botonAnterior = estadoBoton;
}


// PANTALLA MENÚ
void pantallaMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1P:Toggle");
  lcd.setCursor(0, 1);
  lcd.print("2P:Vaso");
  Serial.println("====================");
  Serial.println("MENU: 1P=Toggle 2P=Vaso");
}

// VOLVER MENÚ
void volverMenu() {
  enTransicion = true;
  digitalWrite(relayPin, HIGH);

  modo = 0;
  llenadoCompleto = false;
  estadoBomba = false;
  manoDetectada = false;
  contadorPulsos = 0;
  tiempoPresionado = 0;
  botonSostenido = false;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Volviendo...");
  Serial.println("VOLVIENDO AL MENU");

  while (digitalRead(botonPin) == LOW) delay(10);
  delay(500);

  contadorPulsos = 0;
  botonAnterior = HIGH;
  ultimaLectura = millis();

  pantallaMenu();
  enTransicion = false;
}

// ACTIVAR MODO TOGGLE
void activarToggle() {
  modo = 1;
  estadoBomba = false;
  manoDetectada = false;
  digitalWrite(relayPin, HIGH);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MODO TOGGLE");
  lcd.setCursor(0, 1);
  lcd.print("Acerca la mano");
  Serial.println("MODO TOGGLE ACTIVO");

  delay(1000);
  lcd.clear();
}


// ACTIVAR MODO VASO
void activarVaso() {
  modo = 2;
  llenadoCompleto = false;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pon vaso vacio");
  lcd.setCursor(0, 1);
  lcd.print("Espera 5 seg...");
  Serial.println("Esperando 5 segundos...");

  for (int i = 5; i > 0; i--) {
    lcd.setCursor(14, 1);
    lcd.print(i);
    lcd.print(" ");
    unsigned long t = millis();
    while (millis() - t < 1000) {
      leerBoton();
      if (modo == 0) return;
      delay(10);
    }
  }

  Serial.println("Calibrando...");
  vasoVacio = medirDistancia();
  vasoLleno = vasoVacio - diferenciaLlenado;

  Serial.print("Vaso vacio: "); Serial.print(vasoVacio); Serial.println(" cm");
  Serial.print("Vaso lleno: "); Serial.print(vasoLleno); Serial.println(" cm");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Vacio:"); lcd.print(vasoVacio); lcd.print("cm");
  lcd.setCursor(0, 1);
  lcd.print("Lleno:"); lcd.print(vasoLleno); lcd.print("cm");

  unsigned long t2 = millis();
  while (millis() - t2 < 2000) {
    leerBoton();
    if (modo == 0) return;
    delay(10);
  }

  lcd.clear();
  Serial.println("Sistema iniciado");
}

// MODO TOGGLE
void modoToggle() {
  distancia = medirDistancia();

  lcd.setCursor(0, 0);
  lcd.print("D:"); lcd.print(distancia); lcd.print("cm      ");

  lcd.setCursor(0, 1);
  if (estadoBomba) lcd.print("Bomba: ON       ");
  else             lcd.print("Acerca la mano  ");

  if (distancia > 0 && distancia <= 5) {
    if (!manoDetectada) {
      manoDetectada = true;
      estadoBomba = !estadoBomba;
      if (estadoBomba) {
        digitalWrite(relayPin, LOW);
        Serial.println("BOMBA ON");
      } else {
        digitalWrite(relayPin, HIGH);
        Serial.println("BOMBA OFF");
      }
    }
  } else {
    manoDetectada = false;
  }
}


// MODO VASO
void modoVaso() {

  distancia = medirDistancia();

  Serial.print("Distancia: "); Serial.print(distancia); Serial.println(" cm");

  if (llenadoCompleto) {
    digitalWrite(relayPin, HIGH);
    enTransicion = true;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("   FINALIZADO   ");
    lcd.setCursor(0, 1);
    lcd.print("                ");

    Serial.println("LLENADO FINALIZADO");
    delay(3000);

    llenadoCompleto = false;
    modo = 0;
    contadorPulsos = 0;
    botonAnterior = HIGH;
    botonSostenido = false;
    tiempoPresionado = 0;
    ultimaLectura = millis();

    while (digitalRead(botonPin) == LOW) delay(10);
    delay(300);

    pantallaMenu();
    enTransicion = false;
    return;
  }

  // =====================================
  // ERROR SENSOR
  // =====================================
  if (distancia <= 0) {
    Serial.println("Sin lectura");
    return;
  }

  // =====================================
  // VASO LLENO
  // =====================================
  if (distancia <= vasoLleno) {
    digitalWrite(relayPin, HIGH);
    llenadoCompleto = true;
    Serial.println("VASO LLENO");
    return;
  }


  // LLENANDO
  else {
    digitalWrite(relayPin, LOW);

    lcd.setCursor(0, 0);
    lcd.print("   Llenando...  ");
    lcd.setCursor(0, 1);
    lcd.print("                ");

    Serial.println("LLENANDO...");
  }
}



// MEDIR DISTANCIA
int medirDistancia() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  duracion = pulseIn(ECHO, HIGH, 30000);
  if (duracion == 0) return -1;
  distancia = duracion * 0.034 / 2;
  return distancia;
}

// CONTROL BOMBA
void encenderBomba() {
  digitalWrite(relayPin, LOW);
  Serial.println("BOMBA ENCENDIDA");
}

void apagarBomba() {
  digitalWrite(relayPin, HIGH);
  Serial.println("BOMBA APAGADA");
}