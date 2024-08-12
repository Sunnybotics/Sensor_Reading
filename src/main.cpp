#include <Arduino.h>         // Arduino core library for general functions
#include <WiFi.h>           // Library for WiFi connectivity
#include <HTTPClient.h>     // Library for handling HTTP requests
#include "ws2812.h"         // Custom library for WS2812 LED control (if used)
#include "system.h"         // Custom header file containing system variables
#include "multitask.h"      // Custom header file for multitasking support

// Network credentials
const char* ssid = "WIFI";           // The SSID (name) of the WiFi network to connect to
const char* password = "INNOVATION"; // The password for the WiFi network

// CSRF token for HTTP requests (currently unused in this code)
String csrf_token = ""; 

// Sensor pin definitions for two groups of sensors
const int group1Pins[4] = {19, 18, 12, 14}; // Array of pins for the first group of sensors
const int group2Pins[4] = {26, 32, 33, 35}; // Array of pins for the second group of sensors

// Relay and buzzer pin definitions
const int relay1 = RL1;    // Pin connected to relay 1
const int relay2 = RL2;    // Pin connected to relay 2
const int buzzer = BUZZER; // Pin connected to the buzzer

// Variables to keep track of the number of active sensors in each group
int group1Count = 0; // Count of active sensors in group 1
int group2Count = 0; // Count of active sensors in group 2

// Timing variables for sensor updates
unsigned long lastSensorUpdateTime = 0; // Last time sensors were updated
const unsigned long sensorUpdateInterval = 2; // Interval (in milliseconds) for updating sensors

/**
 * Updates the sensor readings and controls the relays and buzzer based on sensor counts.
 * - Reads the state of each sensor in the two groups.
 * - Activates relays if all sensors in a group are active.
 * - Controls the buzzer based on the state of the relays.
 */
void updateSensors() {
  group1Count = 0; // Reset the count for group 1 sensors
  group2Count = 0; // Reset the count for group 2 sensors

  // Iterate through each sensor pin in both groups and update the counts
  for (int i = 0; i < 4; i++) {
    group1Count += digitalRead(group1Pins[i]) == HIGH; // Increment count if sensor is HIGH
    group2Count += digitalRead(group2Pins[i]) == HIGH; // Increment count if sensor is HIGH
  }

  // Determine if relays should be activated based on sensor counts
  bool relay1Active = (group1Count >= 4); // Relay 1 is active if all sensors in group 1 are HIGH
  bool relay2Active = (group2Count >= 4); // Relay 2 is active if all sensors in group 2 are HIGH

  // Set relay states based on the above determination
  digitalWrite(relay1, relay1Active ? HIGH : LOW); // Set relay 1 state
  digitalWrite(relay2, relay2Active ? LOW : HIGH); // Set relay 2 state

  // Control the buzzer based on the state of the relays
  if (relay1Active || relay2Active) {
    digitalWrite(buzzer, LOW); // Buzzer OFF if any relay is active
  } else {
    digitalWrite(buzzer, HIGH); // Buzzer ON if no relay is active
  }
  
  // Uncomment the following line for debugging purposes to indicate data transmission
  // printf("Data sent\n");
}

/**
 * Task function to periodically update sensor readings.
 * Runs in an infinite loop, checking the time interval to update sensor data.
 * - Calls the updateSensors function every `sensorUpdateInterval` milliseconds.
 */
void sensorTask(void *arg) {
  unsigned long previousMillis = 0; // Last time the sensors were updated
  const unsigned long sensorUpdateInterval = 10; // Interval for updating sensors in milliseconds

  for (;;) { // Infinite loop for the task
    unsigned long currentMillis = millis(); // Get the current time

    // Check if it's time to update the sensors
    if (currentMillis - previousMillis >= sensorUpdateInterval) {
      updateSensors(); // Update sensor readings and control relays/buzzer
      previousMillis = currentMillis; // Update the previousMillis variable
    }
  }
}

/**
 * Task function to periodically handle Modbus transmission.
 * Runs in an infinite loop, checking the time interval to send data to the server.
 * - Sends sensor data to a remote server via an HTTP POST request every `modbusInterval` milliseconds.
 */
void modbusTask(void *arg) {
  unsigned long previousMillis = 0; // Last time data was sent to the server
  const unsigned long modbusInterval = 5000; // Interval for sending data in milliseconds

  for (;;) { // Infinite loop for the task
    unsigned long currentMillis = millis(); // Get the current time

    // Check if it's time to send data
    if (currentMillis - previousMillis >= modbusInterval) {
      if (WiFi.status() == WL_CONNECTED) { // Ensure WiFi is connected
        HTTPClient http; // Create an HTTP client object

        // Construct the data string to be sent
        String dataToSend = "sen1del=" + (String)digitalRead(group1Pins[0]) + 
                            "&sen2del=" + (String)digitalRead(group1Pins[1]) + 
                            "&sen3del=" + (String)digitalRead(group1Pins[2]) + 
                            "&sen4del=" + (String)digitalRead(group1Pins[3]) + 
                            "&sen1atras=" + (String)digitalRead(group2Pins[0]) + 
                            "&sen2atras=" + (String)digitalRead(group2Pins[1]) + 
                            "&sen3atras=" + (String)digitalRead(group2Pins[2]) + 
                            "&sen4atras=" + (String)digitalRead(group2Pins[3]);

        // Specify the URL of the server endpoint
        http.begin("https://sunnyesp32.up.railway.app/add_register_esp32/");
        http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // Set content type for the POST request

        // Send the POST request and retrieve the response code
        int responseCode = http.POST(dataToSend);

        // Check the response code from the server
        if (responseCode > 0) {
          Serial.println("HTTP Response Code ► " + String(responseCode)); // Print the HTTP response code

          if (responseCode == 200) { // If response code is 200 (OK)
            String responseBody = http.getString(); // Get the response body from the server
            Serial.println("Server Response ▼ ");
            Serial.println(responseBody); // Print the server response
          }
        } else {
          Serial.print("Error sending POST, code: ");
          Serial.println(responseCode); // Print error code if the POST request failed
        }

        http.end(); // End the HTTP request and free resources
        previousMillis = currentMillis; // Update the previousMillis variable
      } else {
        Serial.println("WiFi connection error"); // Print an error message if WiFi is not connected
      }
    }
  }
}

/**
 * Initialization function for the setup phase.
 * Configures the hardware and establishes WiFi connection.
 */
void setup() {
  // Initialize multitasking (assuming MultitaskInit is defined elsewhere)
  MultitaskInit();

  // Initialize serial communication for debugging and information
  Serial.begin(115200);

  // Configure relay and buzzer pins as output
  pinMode(relay1, OUTPUT);
  digitalWrite(relay1, LOW); // Initially set relay1 to LOW

  pinMode(relay2, OUTPUT);
  digitalWrite(relay2, HIGH); // Initially set relay2 to HIGH

  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, HIGH); // Initially set buzzer to HIGH

  // Configure sensor pins as input
  for (int i = 0; i < 4; i++) {
    pinMode(group1Pins[i], INPUT); // Set each pin in group 1 as input
    pinMode(group2Pins[i], INPUT); // Set each pin in group 2 as input
  }

  // Initialize WiFi connection
  WiFi.mode(WIFI_STA); // Set WiFi mode to Station (client)
  WiFi.begin(ssid, password); // Start WiFi connection with the specified SSID and password

  // Wait until WiFi is connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // Wait 500 milliseconds before checking again
    Serial.print("."); // Print a dot to indicate progress
  }

  // Print connection information once connected
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid); // Print the SSID of the connected network
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); // Print the local IP address assigned by the WiFi network
}

/**
 * Main loop function (empty in this case).
 * The loop function is left empty as tasks are handled by the multitasking system.
 */
void loop() {
  // The loop function is empty because multitasking handles periodic tasks
}
