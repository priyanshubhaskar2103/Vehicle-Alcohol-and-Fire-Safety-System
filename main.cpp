#include <SoftwareSerial.h>

SoftwareSerial sim(9, 10); // RX, TX

// Pin definitions
const int mq3Pin = A0;
const int irPin = A1;
const int buzzerPin = 13;
const int relayPin = 12;

// Threshold values
int alcoholThreshold = 850;   // Adjust after calibration
int fireThreshold = 200;      // IR detection threshold

// Predefined phone numbers
String num1 = "+916207066158";
String num2 = "+919039220855";
String num3 = "+910112233445";

void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(irPin, INPUT);

  digitalWrite(buzzerPin, LOW);

  // Motor ON when Arduino is ON
  // Relay HIGH = motor ON (using NO terminal)
  digitalWrite(relayPin, HIGH);

  Serial.begin(9600);
  sim.begin(9600);

  delay(2000);
}

void loop() {

  int alcoholVal = analogRead(mq3Pin);
  int fireVal = analogRead(irPin);

  Serial.print("Alcohol: "); Serial.print(alcoholVal);
  Serial.print("  Fire: "); Serial.println(fireVal);

  if (alcoholVal > alcoholThreshold || fireVal < fireThreshold) 
  {
    triggerAlert();
    delay(10000); // Wait to avoid multiple triggers
  }

  delay(500);
}

void triggerAlert() {
  Serial.println("⚠ ALERT TRIGGERED!");

  // Stop motor
  // Relay LOW = motor OFF (NO terminal)
  digitalWrite(relayPin, LOW);

  // Buzzer ON
  digitalWrite(buzzerPin, HIGH);
  delay(5000);
  digitalWrite(buzzerPin, LOW);

  sendSMS("ALERT: Alcohol or Fire Detected! Vehicle Disabled.");
  makeCall(num1);
  makeCall(num2);
  makeCall(num3);

  delay(1000);
}

void sendSMS(String msg) {
  sim.println("AT+CMGF=1");
  delay(500);
  
  sendSMSNumber(num1, msg);
  sendSMSNumber(num2, msg);
  sendSMSNumber(num3, msg);
}

void sendSMSNumber(String number, String msg) {
  sim.print("AT+CMGS=\"");
  sim.print(number);
  sim.println("\"");
  delay(500);
  sim.println(msg);
  delay(500);
  sim.write(26); // CTRL+Z
  delay(3000);
}

void makeCall(String number) {
  sim.print("ATD");
  sim.print(number);
  sim.println(";");
  delay(10000);  // Ring 10 sec
  sim.println("ATH");  // Hang up
  delay(2000);
}
