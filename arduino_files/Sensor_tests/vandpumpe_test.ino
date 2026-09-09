const int IN1 = 2;
const int IN2 = 3;


void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  // Set pump direction
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

}



void loop() {
  // Nothing happens here
}
