// Arduino sketch – Blockchain-Integrated Sensor Node
// This sketch connects to an Ethereum blockchain (via an Infura node or local node JSON-RPC) 
// and monitors the FirmwareUpdates smart contract for firmware update commands. 
// It switches between reading temperature and humidity based on the blockchain command, 
// simulating a remote firmware update.
// Requires: WiFiNINA library, Arduino_MKRENV, and a Web3 library for Arduino (e.g., kopanitsa/web3-arduino).

#include <WiFiNINA.h>
#include <Arduino_MKRENV.h>
#include <Web3.h>
#include <Contract.h>

// WiFi network credentials
const char* WIFI_SSID     = "YOUR_WIFI_SSID";        // replace with your WiFi network name
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";    // replace with your WiFi password

// Ethereum node (Infura or private blockchain RPC endpoint)
#define ETH_NODE_HOST   "rinkeby.infura.io"          // Example: Infura Rinkeby endpoint host
#define ETH_NODE_PATH   "/v3/YOUR_INFURA_PROJECT_ID" // Example path (replace with your project ID or use private node IP/path)

// Smart contract details
const char* CONTRACT_ADDRESS = "0xYOUR_CONTRACT_ADDRESS"; // address of deployed FirmwareUpdates contract
const char* OWNER_ADDRESS    = "0xYOUR_WALLET_ADDRESS";   // Address from which contract calls are made (owner)

// Initialize Web3 (Ethereum RPC) and Contract interface
Web3 web3(ETH_NODE_HOST, ETH_NODE_PATH);
Contract contract(&web3, CONTRACT_ADDRESS);

// Track last known firmware mode to detect changes
uint8_t lastMode = 0; // 0 = Temperature, 1 = Humidity

// Function to check contract for current mode
uint8_t getCurrentModeFromBlockchain() {
  // Prepare the function call to the contract: calling the `currentMode()` getter (no arguments)
  String functionCall = "currentMode()";
  String callData = contract.SetupContractData(&functionCall);  
  String result = contract.Call(&callData);  // Perform eth_call (no gas, read-only)
  
  Serial.print("Raw contract response: ");
  Serial.println(result);
  if (result.length() < 3) {
    Serial.println("Error: Empty response from blockchain.");
    return lastMode; // if fail, keep last known mode
  }
  // The result is a hex string (0x...); parse the last hex digit as the Mode (0 or 1)
  char lastHexChar = result.charAt(result.length() - 1);
  uint8_t modeValue;
  if (lastHexChar >= '0' && lastHexChar <= '9') {
    modeValue = lastHexChar - '0';
  } else if (lastHexChar >= 'a' && lastHexChar <= 'f') {
    modeValue = 10 + (lastHexChar - 'a'); 
  } else if (lastHexChar >= 'A' && lastHexChar <= 'F') {
    modeValue = 10 + (lastHexChar - 'A');
  } else {
    modeValue = 0;
  }
  return modeValue;
}

void setup() {
  Serial.begin(9600);
  // Initialize sensor shield
  if (!ENV.begin()) {
    Serial.println("Failed to initialize MKR ENV shield!");
    while (1);
  }
  // Connect to WiFi network
  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  
  // Set up Ethereum contract options (from address, gas settings)
  strcpy(contract.options.from, OWNER_ADDRESS);       // set the from address (not used for eth_call, but good to set)
  strcpy(contract.options.gasPrice, "10000000000");    // set gas price (not used for calls)
  contract.options.gas = 3000000;
  
  // Initial check of blockchain state
  lastMode = getCurrentModeFromBlockchain();
  if (lastMode == 0) {
    Serial.println("Initial Mode from blockchain: Temperature");
  } else {
    Serial.println("Initial Mode from blockchain: Humidity");
  }
}

void loop() {
  // Poll the blockchain for the current firmware mode
  uint8_t currentMode = getCurrentModeFromBlockchain();
  if (currentMode != lastMode) {
    // Mode changed due to a blockchain firmware update event
    lastMode = currentMode;
    if (currentMode == 0) {
      Serial.println("** Firmware Update Event: Switch to TEMPERATURE mode **");
    } else {
      Serial.println("** Firmware Update Event: Switch to HUMIDITY mode **");
    }
  }
  
  // Read the sensor corresponding to the current mode
  if (lastMode == 0) {
    // Mode 0: read temperature
    float tempC = ENV.readTemperature();
    Serial.print("Sending Temperature: ");
    Serial.print(tempC);
    Serial.println(" °C");
    // (In a real deployment, you might send this data to a server or cloud. For simplicity, we log it.)
    // Optionally, send to the local Flask server:
    // sendDataToServer("Temperature", tempC);
  } else {
    // Mode 1: read humidity
    float hum = ENV.readHumidity();
    Serial.print("Sending Humidity: ");
    Serial.print(hum);
    Serial.println(" %");
    // (Could send this data to server or cloud as needed, here just logging.)
    // Optionally, send to the local Flask server:
    // sendDataToServer("Humidity", hum);
  }
  
  delay(5000); // Wait before next poll and sensor send
}

// (Optional) Helper to send data to local server, showing how payload includes sensor type.
// Not called by default, but can be used if integration with local server is desired.
void sendDataToServer(const char* type, float value) {
  WiFiClient client;
  if (client.connect(SERVER_HOST, SERVER_PORT)) {
    String jsonPayload = String("{\"sensorType\": \"") + type + "\", \"value\": " + value + "}";
    client.println(String("POST /sensor-data HTTP/1.1"));
    client.println(String("Host: ") + SERVER_HOST);
    client.println("Content-Type: application/json");
    client.print("Content-Length: "); client.println(jsonPayload.length());
    client.println();
    client.println(jsonPayload);
    client.stop();
  }
}
