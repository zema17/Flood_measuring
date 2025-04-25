#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// ========== CẤU HÌNH WIFI ========== //
const char* ssid = "vinhphu";
const char* password = "01012002";

// ========== CẤU HÌNH SERVER ========== //
const char* serverUrl = "http://localhost/flood.php";  // Thay bằng URL API của bạn
const int serverInterval = 5000;  // Gửi dữ liệu mỗi 5 giây

// ========== CẤU HÌNH TELEGRAM ========== //
const char* telegramToken = "123456789:ABCdefGHIJKlmNoPQRsTuVWXYz"; //khong public dc heheee
const char* chatId = "123456789";
const float FLOOD_THRESHOLD = 50.0;  // Ngưỡng cảnh báo lũ (cm)

// ========== BIẾN TOÀN CỤC ========== //
float lastWaterLevel = 0.0;
bool isFloodAlertSent = false;

// ========== HÀM GỬI CẢNH BÁO TELEGRAM ========== //
void sendTelegramAlert(float waterLevel) {
  String message = "⚠️ **CẢNH BÁO LŨ!** ⚠️\n";
  message += "📍 **Mực nước hiện tại:** " + String(waterLevel) + " cm\n";
  message += "❌ **Vượt ngưỡng an toàn** (" + String(FLOOD_THRESHOLD) + " cm)!";

  HTTPClient http;
  String url = "https://api.telegram.org/bot" + String(telegramToken) + 
               "/sendMessage?chat_id=" + String(chatId) + 
               "&text=" + URLEncode(message) + 
               "&parse_mode=Markdown";
  
  http.begin(url);
  int httpCode = http.GET();
  
  if (httpCode == HTTP_CODE_OK) {
    Serial.println("[Telegram] Cảnh báo đã gửi!");
    isFloodAlertSent = true;
  } else {
    Serial.printf("[Telegram] Lỗi: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

// ========== HÀM GỬI DỮ LIỆU LÊN SERVER ========== //
void sendToServer(float waterLevel) {
  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

  String jsonData = "{\"water_level\":" + String(waterLevel) + "}";
  int httpCode = http.POST(jsonData);

  if (httpCode == HTTP_CODE_OK) {
    Serial.println("[Server] Dữ liệu đã gửi thành công!");
  } else {
    Serial.printf("[Server] Lỗi: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

// ========== SETUP ========== //
void setup() {
  Serial.begin(115200);  // UART với STM32
  WiFi.begin(ssid, password);

  Serial.println("\nĐang kết nối WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi đã kết nối!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ========== LOOP ========== //
void loop() {
  if (Serial.available()) {
    String jsonData = Serial.readStringUntil('\n');
    StaticJsonDocument<200> doc;
    deserializeJson(doc, jsonData);
    lastWaterLevel = doc["water_level"];

    // Gửi dữ liệu lên server
    sendToServer(lastWaterLevel);

    // Kiểm tra ngưỡng lũ và gửi cảnh báo
    if (lastWaterLevel > FLOOD_THRESHOLD && !isFloodAlertSent) {
      sendTelegramAlert(lastWaterLevel);
    } else if (lastWaterLevel <= FLOOD_THRESHOLD) {
      isFloodAlertSent = false;  // Reset cảnh báo nếu mực nước giảm
    }
  }

  delay(100);  // Giảm CPU usage
}