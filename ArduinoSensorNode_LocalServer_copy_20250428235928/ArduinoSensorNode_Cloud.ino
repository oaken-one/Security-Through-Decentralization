// Arduino sketch: IoT Cloud Model
// This sketch connects the MKR WiFi 1010 + MKR ENV Shield to the Arduino IoT Cloud,
// and periodically sends sensor data (e.g., temperature) to the cloud dashboard.
// Requires the ArduinoIoTCloud library and a configured Thing with a Device ID and Secret key.

#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include <Arduino_MKRENV.h>

// Network credentials
const char SSID[]     = "YOUR_WIFI_SSID";       // replace with your WiFi network name
const char PASS[]     = "YOUR_WIFI_PASSWORD";   // replace with your WiFi password

// Arduino IoT Cloud credentials (from your IoT Cloud Thing setup)
const char THING_ID[]   = "YOUR_THING_ID";      // replace with your Arduino IoT Cloud Thing ID
const char DEVICE_KEY[] = "YOUR_DEVICE_KEY";    // replace with your Device Secret Key

// Cloud variable to send (e.g., temperature value). Additional variables can be added similarly.
float temperature;

// Initialize WiFi connection handler for Arduino IoT Cloud
WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);

// Set up the IoT Cloud properties (linking the `temperature` variable to the cloud)
void initProperties() {
  ArduinoCloud.setThingId(THING_ID);
  // Register the temperature variable as a READ-only cloud property updated every 10 seconds
  ArduinoCloud.addProperty(temperature, READ, 10 * SECONDS, NULL);
}

void setup() {
  Serial.begin(9600);
  // Initialize sensors on the ENV shield
  if (!ENV.begin()) {
    Serial.println("Failed to initialize MKR ENV shield!");
    while (1);
  }
  // Connect to Arduino IoT Cloud with the device credentials
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection, DEVICE_KEY);
  // Optional: wait for cloud connection
  Serial.print("Connecting to Arduino IoT Cloud...");
  while (!ArduinoCloud.connected()) {
    ArduinoCloud.update();
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to Arduino IoT Cloud.");
}

void loop() {
  // Regularly call ArduinoCloud.update() to maintain cloud connection and push data
  ArduinoCloud.update();
  
  // Read sensor and update cloud-bound variable
  float tempC = ENV.readTemperature();
  temperature = tempC;
  
  Serial.print("IoT Cloud update - Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  
  delay(1000); // Small delay to avoid tight loop (cloud library internally manages update rate)
}
