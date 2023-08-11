#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

SoftwareSerial sim800l(2, 3); // RX, TX for SIM800L
const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";
const char* gptApiKey = "YourChatGPTAPIKey";

void setup() {
  Serial.begin(115200);
  sim800l.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  sim800l.println("AT+CMGF=1"); // Set SMS mode to text
  delay(100);
}

void loop() {
  if (sim800l.available()) {
    String sms = sim800l.readString();
    if (sms.indexOf("CHATGPT") != -1) { // Check for a keyword in the SMS to trigger processing
      // Extract sender's number and query
      String senderNumber = // Extract sender number from SMS
      String query = // Extract query text from SMS
      
      // Process query with ChatGPT
      String response = processQueryWithChatGPT(query);
      
      // Send SMS response back using SIM800L
      sendSMS(senderNumber, response);
    }
  }
}

String processQueryWithChatGPT(String query) {
  HTTPClient http;
  http.begin("https://api.openai.com/v1/chat/completions");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(gptApiKey));
  
  // Prepare the request payload
  String payload = "{\"model\":\"gpt-3.5-turbo\",\"messages\":[{\"role\":\"system\",\"content\":\"You are a helpful assistant.\"},{\"role\":\"user\",\"content\":\"" + query + "\"}]}";
  
  // Send the POST request and parse the response
  int httpCode = http.POST(payload);
  String response = "";
  if (httpCode == HTTP_CODE_OK) {
    String responseBody = http.getString();
    DynamicJsonDocument jsonDoc(1024);
    deserializeJson(jsonDoc, responseBody);
    JsonArray choices = jsonDoc["choices"];
    response = choices[0]["message"]["content"].as<String>();
  }
  
  http.end();
  return response;
}

void sendSMS(String recipient, String message) {
  sim800l.print("AT+CMGS=\"");
  sim800l.print(recipient);
  sim800l.println("\"");
  delay(100);
  sim800l.print(message);
  delay(100);
  sim800l.write(26); // Ctrl+Z to send the SMS
  delay(1000);
}
