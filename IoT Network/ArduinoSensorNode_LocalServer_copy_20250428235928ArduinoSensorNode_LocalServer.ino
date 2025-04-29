// Arduino sketch: IoT Sensor Node for Local Server Model
// This sketch reads temperature from the MKR ENV Shield and POSTs it to the Flask server.
// Hardware: Arduino MKR WiFi 1010 + MKR ENV Shield. Uses WiFiNINA for Wi-Fi connectivity.

#include <WiFiNINA.h>
#include <Arduino_MKRENV.h>

const char* WIFI_SSID = "YOUR_WIFI_SSID";        // replace with your WiFi network name
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD"; // replace with your WiFi password
const char* SERVER_HOST = "192.168.1.100";       // replace with Flask server IP or hostname
const int   SERVER_PORT = 5000;                 // Flask server port (5000 as defined in server.py)

WiFiClient client;

void setup() {
  Serial.begin(9600);
  // Initialize the ENV Shield (which has temperature, humidity, etc.)
  if (!ENV.begin()) {
    Serial.println("Failed to initialize MKR ENV shield!");
    while (1); // Halt if sensor shield not detected
  }
  // Connect to WiFi network
  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("Device IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Read sensor data (temperature in Celsius as example for firmware functionality)
  float temperature = ENV.readTemperature();  
  Serial.print("Temperature reading: ");
  Serial.print(temperature);
  Serial.println(" °C");
  
  // Construct JSON payload string with the sensor data
  // Here we only send temperature. In JSON: {"temperature": <value>}
  String jsonPayload = String("{\"temperature\": ") + temperature + String("}");
  
  // Connect to the server and send HTTP POST request
  if (client.connect(SERVER_HOST, SERVER_PORT)) {
    Serial.println("Connected to server, sending data...");
    // Send HTTP request headers
    client.println("POST /sensor-data HTTP/1.1");
    client.print("Host: "); client.println(SERVER_HOST);
    client.println("Content-Type: application/json");
    client.print("Content-Length: "); client.println(jsonPayload.length());
    client.println();  // blank line to end headers
    // Send the JSON body
    client.println(jsonPayload);
    
    // Read server response (optional, for debugging)
    while (client.connected() && !client.available()) {
      delay(10); // wait for response
    }
    if (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.print("Server response: ");
      Serial.println(line);
    }
    // Disconnect
    client.stop();
  } else {
    Serial.println("ERROR: Could not connect to server.");
    client.stop();
  }
  
  delay(5000);  // Wait 5 seconds before sending next reading (adjust as needed)
}
