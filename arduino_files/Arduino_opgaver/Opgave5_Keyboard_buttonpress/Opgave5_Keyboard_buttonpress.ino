const int buttonPin = 2;
const int ledPin = 13;

int buttonState = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Input pullup reversed the button logic, because the button becomes connected to the 5v while not pressed. So high is the default, and low
  // happens when you press the button. Which is why, I needed to reverse ledPin now, to react to the button being high(unpressed) and low(pressed).
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    digitalWrite(ledPin, LOW);
  } else {
    digitalWrite(ledPin, HIGH);
  }
    delay(10);
}
