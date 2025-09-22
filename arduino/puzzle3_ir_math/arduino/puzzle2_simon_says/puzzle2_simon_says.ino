// puzzle2_simon_says.ino
#include <SoftwareSerial.h>

const byte rxPin = 0, txPin = 1;
SoftwareSerial link(rxPin, txPin);

const byte REDLIGHT_PIN   = 5;   // ensure PWM capable
const byte BLUELIGHT_PIN  = 6;   // ensure PWM capable
const byte GREENLIGHT_PIN = 9;   // moved to a PWM pin (e.g., 9)

const byte REDBUTTON_PIN   = 11;
const byte BLUEBUTTON_PIN  = 10;
const byte GREENBUTTON_PIN = 7;

unsigned long redDebounceMs = 0, blueDebounceMs = 0, greenDebounceMs = 0;
const unsigned long debounceTime = 50;

int redState = LOW, blueState = LOW, greenState = LOW;
int lastRed = LOW, lastBlue = LOW, lastGreen = LOW;

int order[6];
int levelInputs[6];
int progress = 1;            // levels go 1..6
int lightDelay = 1000;
unsigned long lastLightMs = 0;
int displayNumber = 0;
int levelProgress = 0;

int data = 0;
int state = 0;               // 0=wait, 1=play, 2=done

void setup() {
  pinMode(REDLIGHT_PIN, OUTPUT);
  pinMode(BLUELIGHT_PIN, OUTPUT);
  pinMode(GREENLIGHT_PIN, OUTPUT);

  pinMode(REDBUTTON_PIN, INPUT);
  pinMode(BLUEBUTTON_PIN, INPUT);
  pinMode(GREENBUTTON_PIN, INPUT);

  randomSeed(analogRead(A0));
  randomOrder();

  link.begin(9600);
  Serial.begin(9600);
}

void loop() {
  if (state == 0) {
    readSerial();
  } else if (state == 1) {
    simonSays();
    if (progress == 7) {
      link.write(1);     // tell next puzzle to start
      state = 2;
      lightDelay = 100;
      progress = 1;
    }
  } else if (state == 2) {
    // idle
  }
}

void readSerial() {
  if (link.available()) data = link.read();
  if (data == 1) state = 1;
}

void randomOrder() {
  for (int i = 0; i < 6; i++) {
    order[i] = random(3);    // 0=R, 1=B, 2=G
  }
}

void simonSays() {
  unsigned long now = millis();

  // show pattern
  if (displayNumber < progress && (now - lastLightMs > lightDelay)) {
    setRGB(order[displayNumber]);
    lastLightMs = now;
    displayNumber++;
  } else if (displayNumber == progress && (now - lastLightMs > (lightDelay / 2))) {
    setRGB(3); // off
  }

  int guess = -1;

  int r = digitalRead(REDBUTTON_PIN);
  int b = digitalRead(BLUEBUTTON_PIN);
  int g = digitalRead(GREENBUTTON_PIN);

  if (r != lastRed)  redDebounceMs = now;
  if (b != lastBlue) blueDebounceMs = now;
  if (g != lastGreen) greenDebounceMs = now;

  if ((now - redDebounceMs) > debounceTime && r != redState) {
    redState = r;
    if (redState == HIGH) guess = 0;
  }
  if ((now - blueDebounceMs) > debounceTime && b != blueState) {
    blueState = b;
    if (blueState == HIGH) guess = 1;
  }
  if ((now - greenDebounceMs) > debounceTime && g != greenState) {
    greenState = g;
    if (greenState == HIGH) guess = 2;
  }

  if (guess != -1 && guess == order[levelProgress]) {
    levelProgress++;
    if (levelProgress == progress) {
      progress++;
      link.write(2);                 // tell math LCD: GOOD JOB
      levelProgress = 0;
      displayNumber = 0;
      lightDelay = max(150, lightDelay - 100);
    }
  } else if (guess != -1) {
    levelProgress = 0;
    displayNumber = 0;
    link.write(3);                   // tell math LCD: TRY AGAIN
  }

  lastRed = r; lastBlue = b; lastGreen = g;
}

void setRGB(int c) {
  if (c == 0)      setColor(200, 0, 0);
  else if (c == 1) setColor(0, 0, 200);
  else if (c == 2) setColor(0, 200, 0);
  else             setColor(0, 0, 0);
}

void setColor(int r, int b, int g) {
  analogWrite(REDLIGHT_PIN,   r);
  analogWrite(BLUELIGHT_PIN,  b);
  analogWrite(GREENLIGHT_PIN, g);
}
