// puzzle1_rfid_lock.ino
#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>

// *** Use 3.3V for MFRC522 ***

#define FORWARD  1
#define BACKWARD 2
#define STOP     0

const byte rxPin = 0, txPin = 1;
SoftwareSerial link(rxPin, txPin);

// MFRC522 wiring (default)
const byte SS_PIN  = 10;
const byte RST_PIN = 9;
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Motor pins
const byte MOTOR1_PIN = 3;
const byte MOTOR2_PIN = 4;
const byte EN1        = 2;

int state = 0;
int data  = 0;
bool unlocking = false;

void setup() {
  pinMode(MOTOR1_PIN, OUTPUT);
  pinMode(MOTOR2_PIN, OUTPUT);
  pinMode(EN1, OUTPUT);

  link.begin(9600);
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
}

void loop() {
  if (state == 0) {
    rfidSensor();
  } else if (state == 1) {
    readSerial();
  } else if (state == 2) {
    openLock();
  }
}

void readSerial() {
  if (link.available()) data = link.read();
  if (data == 1) state = 2;
}

void rfidSensor() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial())   return;

  String msg = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    msg += (mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    msg += String(mfrc522.uid.uidByte[i], HEX);
  }
  msg.toUpperCase();

  // Replace with your actual UID
  if (msg.substring(1) == "53 B1 21 14") {
    Serial.println("Correct RFID Read");
    link.write(1);    // start Simon
    state = 1;
  } else {
    Serial.println("Wrong ID");
  }
}

void setMotor(int dir, int speed) {
  if (dir == FORWARD) {
    digitalWrite(MOTOR1_PIN, HIGH);
    digitalWrite(MOTOR2_PIN, LOW);
  } else if (dir == BACKWARD) {
    digitalWrite(MOTOR1_PIN, LOW);
    digitalWrite(MOTOR2_PIN, HIGH);
  } else {
    digitalWrite(MOTOR1_PIN, LOW);
    digitalWrite(MOTOR2_PIN, LOW);
  }
  analogWrite(EN1, speed);
}

void openLock() {
  static unsigned long startMs = 0;

  if (!unlocking) {
    unlocking = true;
    startMs = millis();
    setMotor(FORWARD, 100);
  } else if (millis() - startMs > 2000UL) {
    setMotor(STOP, 0);
    unlocking = false;
    state = 99; // done
  }
}
