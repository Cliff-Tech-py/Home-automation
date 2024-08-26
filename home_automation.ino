#include <WiFi.h>                                // ESP32 WiFi library
#include <IOXhop_FirebaseESP32.h>                // Firebase library

// WiFi credentials
#define WIFI_SSID "V5SJ4FS"                      // Input your WiFi name
#define WIFI_PASSWORD "012345678Q"               // Input your WiFi password

// Firebase credentials
#define FIREBASE_HOST "home-automation-44d86-default-rtdb.firebaseio.com"  // Firebase project host address
#define FIREBASE_AUTH "dmx9iUPN5jWt25TjlajZNUes2Y7TLWS0N3z6zQaj"            // Firebase secret key

// LED pins
const int ledPinWiFi = 18; // LED for WiFi connection status
const int ledPinControl = 5; // LED controlled via Firebase

// Button pins
const int buttonOnPin = 2;    // the number of the pushbutton pin for turning the LED on
const int buttonOffPin = 4;   // the number of the pushbutton pin for turning the LED off

String fireStatus = ""; // LED status received from Firebase
bool ledState = false;  // Local LED status variable

void setup() {
  // Initialize the LED pins as outputs
  pinMode(ledPinWiFi, OUTPUT);
  pinMode(ledPinControl, OUTPUT);

  // Initialize the pushbutton pins as inputs
  pinMode(buttonOnPin, INPUT);
  pinMode(buttonOffPin, INPUT);

  // Initialize the Serial Monitor
  Serial.begin(115200);

  // Connect to WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Wait for the WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    // Blink WiFi LED while connecting
    digitalWrite(ledPinWiFi, LOW);
    delay(1000);
    digitalWrite(ledPinWiFi, HIGH);
    delay(1000);
    Serial.print(".");
  }

  // WiFi connected
  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); // Print local IP address
  digitalWrite(ledPinWiFi, HIGH); // Turn on WiFi LED

  // Connect to Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  // Retrieve initial LED status from Firebase
  fireStatus = Firebase.getString("LED_STATUS");

  // Set initial LED state based on Firebase data
  if (fireStatus == "1") {
    ledState = true;
    digitalWrite(ledPinControl, HIGH); // Turn on the control LED
  } else {
    ledState = false;
    digitalWrite(ledPinControl, LOW); // Turn off the control LED
  }

  // Send initial LED status to Firebase
  Firebase.setString("LED_STATUS", ledState ? "1" : "0");
}

void loop() {
  // Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(ledPinWiFi, HIGH); // Turn on WiFi LED if connected
  } else {
    digitalWrite(ledPinWiFi, LOW); // Turn off WiFi LED if not connected
  }

  // Read the state of the pushbuttons
  int buttonOnState = digitalRead(buttonOnPin);
  int buttonOffState = digitalRead(buttonOffPin);

  // Check if the button for turning the LED on is pressed
  if (buttonOnState == HIGH) {
    // Turn LED on
    ledState = true;
    digitalWrite(ledPinControl, HIGH);
    // Update Firebase with LED status
    Firebase.setString("LED_STATUS", "1");
  }

  // Check if the button for turning the LED off is pressed
  if (buttonOffState == HIGH) {
    // Turn LED off
    ledState = false;
    digitalWrite(ledPinControl, LOW);
    // Update Firebase with LED status
    Firebase.setString("LED_STATUS", "0");
  }

  // Get the LED status from Firebase
  fireStatus = Firebase.getString("LED_STATUS");

  // Handle Firebase LED status
  if (fireStatus == "1") {
    ledState = true;
    digitalWrite(ledPinControl, HIGH); // Turn on the control LED
  } else if (fireStatus == "0") {
    ledState = false;
    digitalWrite(ledPinControl, LOW); // Turn off the control LED
  }

  // Print LED status to Serial Monitor
  Serial.print("Local LED Status: ");
  Serial.println(ledState);

  delay(1000); // Add a delay to avoid rapid polling
}
