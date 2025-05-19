#define TRIG_PIN 3
#define ECHO_PIN 4
#define PIR_PIN 2
#define WATER_SENSOR_PIN A0

#define RED_LED 7
#define YELLOW_LED 6
#define BUZZER 8

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
}

long getUltrasonicDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  long distance = duration * 0.034 / 2;
  if (distance == 0) distance = 999;  // Filter out bad reads
  return distance;
}

void playBeepPattern(int onTime, int offTime) {
  tone(BUZZER, 1000);  // 1kHz tone
  delay(onTime);
  noTone(BUZZER);
  delay(offTime);
}

void playAlarmSound() {
  for (int i = 0; i < 3; i++) {
    tone(BUZZER, 2000); // Higher pitch alarm
    delay(150);
    noTone(BUZZER);
    delay(100);
  }
}

void loop() {
  long distance = getUltrasonicDistance();
  int waterLevel = analogRead(WATER_SENSOR_PIN);
  int pirState = digitalRead(PIR_PIN);

  Serial.print("Distance: "); Serial.print(distance); Serial.print(" cm | ");
  Serial.print("Water: "); Serial.print(waterLevel); Serial.print(" | ");
  Serial.print("PIR: "); Serial.println(pirState);

  bool waterDry = waterLevel < 300;
  bool waterLow = waterLevel >= 300 && waterLevel < 600;
  bool waterHigh = waterLevel >= 600;

  // MUTE MODE CHECK: All sensors quiet
  if (pirState == LOW && waterDry && distance > 30) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    noTone(BUZZER);
    return;
  }

  // PRIORITY: PIR motion first
  if (pirState == HIGH) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    playAlarmSound(); // Different warning sound
    return;
  }

  // Handle Water Sensor
  if (waterHigh) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(YELLOW_LED, HIGH);
    tone(BUZZER, 1000);
    delay(1000); // Long tone
    noTone(BUZZER);
    delay(500);
    return;
  }
  else if (waterLow) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    for (int i = 0; i < 3; i++) {
      tone(BUZZER, 1500);
      delay(100);
      noTone(BUZZER);
      delay(100);
    }
    return;
  }

  // Handle Ultrasonic Sensor
  if (distance < 10) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(YELLOW_LED, HIGH);
    playBeepPattern(500, 100); // Long beep
  } 
  else if (distance >= 10 && distance <= 20) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    playBeepPattern(200, 200); // Medium beeps
  } 
  else {
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    playBeepPattern(100, 300); // Short beeps
  }
}
