#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ── Konfigurasi ──────────────────────────────
#define MY_ADDR   0x09
#define LCD_ADDR  0x3F

// Bus 1 untuk slave ke STM32
TwoWire I2C_SLAVE = TwoWire(1);

LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);

volatile byte receivedVal = 0;
volatile bool newData     = false;
byte responseData         = 0;

// ── Callback ─────────────────────────────────
void receiveEvent(int bytes) {
  while (I2C_SLAVE.available()) {
    receivedVal = I2C_SLAVE.read();
  }
  responseData = receivedVal + 0x80;
  newData = true;
}

void requestEvent() {
  I2C_SLAVE.write(responseData);
}

// ── Setup ─────────────────────────────────────
void setup() {
  Serial.begin(115200);

  // Bus 0 - LCD pakai Wire default
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("ESP32  Slave");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();

  // Bus 1 - Slave ke STM32
  I2C_SLAVE.begin(MY_ADDR, 33, 32, 50000);
  I2C_SLAVE.onReceive(receiveEvent);
  I2C_SLAVE.onRequest(requestEvent);

  Serial.println("=== ESP32 I2C Slave ===");
  Serial.print("Listening at addr: 0x");
  Serial.println(MY_ADDR, HEX);
}

// ── Loop ──────────────────────────────────────
// ── Loop ──────────────────────────────────────
void loop() {
  if (newData) {
    newData = false;

    // Serial Monitor
    Serial.println("──────────────────────");
    Serial.print("Diterima dari STM32: ");
    Serial.println(receivedVal);
    Serial.print("Dikirim balik      : 0x");
    Serial.println(responseData, HEX);

    // LCD update hanya saat ada data baru
    lcd.setCursor(0, 0);
    lcd.print("RX:");
    lcd.print(receivedVal);
    lcd.print(" STM32 OK   ");

    lcd.setCursor(0, 1);
    lcd.print("TX:0x");
    lcd.print(responseData, HEX);
    lcd.print("          ");
  }

  // HAPUS bagian else — jangan tulis apapun ke LCD kalau tidak ada data baru

  delay(1000);
}