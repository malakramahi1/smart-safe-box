// puzzle3_ir_math.ino
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.hpp>

LiquidCrystal_I2C lcd(0x27, 16, 2);

char topBuf[16];
char botBuf[16];
unsigned long lastUpdated = 0;

const byte IR_PIN = 2;            // IR receiver pin
const byte rxPin = 0;             // SoftwareSerial RX
const byte txPin = 1;             // SoftwareSerial TX
SoftwareSerial link(rxPin, txPin);

bool newProblem = true;
int numA = 0, numB = 0, guess = 0;

unsigned long lastRead = 0;       // IR debounce
int state = 0;                    // 0=wait, 1=active, 2=done
int progress = 0;

void setup() {
  link.begin(9600);
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
  randomSeed(analogRead(A0));
}

void loop() {
  if (state == 0) {
    readSerial();
  } else if (state == 1) {
    generateMath();
    if (progress == 3) {                 // 3 correct in a row → open
      link.write(1);                      // signal Arduino 1
      lcd.clear();
      snprintf(topBuf, sizeof(topBuf), "OPENING SAFE");
      lcd.setCursor(0, 0);
      lcd.print(topBuf);
      state = 2;
    }
  } else if (state == 2) {
    // idle
  }
}

void readSerial() {
  int data = 0;
  if (link.available()) data = link.read();

  if (data == 1) {
    state = 1;
  } else if (data == 2) { // Simon correct
    lcd.clear();
    snprintf(topBuf, sizeof(topBuf), "GOOD JOB");
    lcd.setCursor(0, 0);
    lcd.print(topBuf);
    lastUpdated = millis();
  } else if (data == 3) { // Simon wrong
    lcd.clear();
    snprintf(topBuf, sizeof(topBuf), "TRY AGAIN");
    lcd.setCursor(0, 0);
    lcd.print(topBuf);
    lastUpdated = millis();
  }

  if (millis() - lastUpdated > 2000UL) {
    lcd.clear();
  }
}

static inline bool checkAnswer(int g, int a, int b) {
  return (a + b == g);
}

void generateMath() {
  if (newProblem) {
    numA = random(500);
    numB = random(500);
    snprintf(topBuf, sizeof(topBuf), "%d + %d = ?", numA, numB);
    lcd.clear();
    newProblem = false;
  }

  unsigned long now = millis();
  if (IrReceiver.decode() && (now - lastRead > 250UL)) {
    lastRead = now;
    uint8_t cmd = IrReceiver.decodedIRData.command;
    IrReceiver.resume();

    switch (cmd) {
      case 22: guess = guess * 10 + 0; break;
      case 12: guess = guess * 10 + 1; break;
      case 24: guess = guess * 10 + 2; break;
      case 94: guess = guess * 10 + 3; break;
      case 8:  guess = guess * 10 + 4; break;
      case 28: guess = guess * 10 + 5; break;
      case 90: guess = guess * 10 + 6; break;
      case 66: guess = guess * 10 + 7; break;
      case 82: guess = guess * 10 + 8; break;
      case 74: guess = guess * 10 + 9; break;

      case 68: // clear
        guess = 0;
        lcd.clear();
        break;

      case 7:  // submit
        if (checkAnswer(guess, numA, numB)) {
          progress++;
          newProblem = true;
        }
        guess = 0;
        lcd.clear();
        break;

      default:
        // Serial.println(cmd);
        break;
    }
    snprintf(botBuf, sizeof(botBuf), "%d", guess);
  }

  lcd.setCursor(0, 0);
  lcd.print(topBuf);
  lcd.setCursor(0, 1);
  lcd.print(botBuf);
}
