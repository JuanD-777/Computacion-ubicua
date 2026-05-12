#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define DHT_PIN 2
#define DHT_TYPE DHT11

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHT_PIN, DHT_TYPE);

int   SENSOR;
float TEMPERATURA;
float SUMA;

void setup() {
  lcd.init();
  lcd.backlight();
  dht.begin();

  lcd.setCursor(0, 0);
  lcd.print("  Termometro  ");
  lcd.setCursor(0, 1);
  lcd.print(" LM35 + DHT11 ");
  delay(2000);
  lcd.clear();
}

void loop() {
  // ── LM35: promedio de 5 lecturas ─────────────────────────
  SUMA = 0;
  for (int i = 0; i < 5; i++) {
    SENSOR = analogRead(A0);
    TEMPERATURA = ((SENSOR * 5000.0) / 1023.0) / 10.0;
    SUMA += TEMPERATURA;
    delay(500);
  }
  float promedioLM35 = SUMA / 5.0;

  // ── DHT11: temperatura ────────────────────────────────────
  float tempDHT = dht.readTemperature();

  // ── Mostrar en LCD ────────────────────────────────────────
  lcd.setCursor(0, 0);
  lcd.print("LM35: ");
  lcd.print(promedioLM35, 1);
  lcd.print(" C   ");

  lcd.setCursor(0, 1);
  lcd.print("DHT : ");
  if (isnan(tempDHT)) {
    lcd.print("Error   ");
  } else {
    lcd.print(tempDHT, 1);
    lcd.print(" C   ");
  }
}