#include <Arduino.h>
#include <HardwareSerial.h>

// Khai báo chân cảm biến siêu âm
#define TRIG_PIN PA1
#define ECHO_PIN PA0

// Khai báo UART để giao tiếp với ESP8266
HardwareSerial Serial2(USART2);  // PA3 (RX), PA2 (TX)

void setup() {
  Serial2.begin(115200);  // UART với ESP8266
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

float readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;  // Tính khoảng cách (cm)
  return distance;
}

void loop() {
  float waterLevel = readUltrasonic();
  
  // Gửi dữ liệu dạng JSON qua UART
  Serial2.print("{\"water_level\":");
  Serial2.print(waterLevel);
  Serial2.println("}");

  delay(5000);  // Gửi mỗi 5 giây
}