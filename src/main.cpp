#include <WiFi.h>
#include "SinricPro.h"
#include "SinricProContactsensor.h"

#define WIFI_SSID "DaleWeb"        // Replace with your Wi-Fi SSID
#define WIFI_PASS "letmein2"    // Replace with your Wi-Fi Password

#define APP_KEY           "5c66607b-7a14-48e2-8c24-433d6b1ca80f"
#define APP_SECRET        "8a947e10-ff71-4935-8fee-fca82f87fbd4-b9badf28-71a2-4a8b-b839-3998331b952f"
#define CONTACT_ID        "678c18c9f25540068f826677" 

#define BUTTON_PIN 4  // GPIO connected to the button
#define LED_PIN 2     // GPIO connected to the LED
#define DEBOUNCE_DELAY 50 // Debounce delay in milliseconds

unsigned long lastDebounceTime = 0; // Tracks the last button press
bool lastButtonState = HIGH;       // Last button state
bool ledState = HIGH;               // LED state (LOW means off for active-low LEDs)
bool isCountingDown = false;


void handleContactsensor(bool switchState) {
  if (SinricPro.isConnected() == false) {
    Serial.printf("Not connected to Sinric Pro...!\r\n");
    return; 
  }

  SinricProContactsensor &myContact = SinricPro[CONTACT_ID]; // get contact sensor device
  Serial.printf("Switching light to %s\r\n", switchState ? "OFF" : "ON");
  myContact.sendContactEvent(switchState);      // send event with the passed state
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Use internal pull-up resistor
    pinMode(LED_PIN, OUTPUT);          // Set LED as output
    digitalWrite(LED_PIN, ledState);   // Start with LED off

    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("WiFi connected!");

      // add device to SinricPro
  SinricProContactsensor& myContact = SinricPro[CONTACT_ID];

  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); });
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
  SinricPro.begin(APP_KEY, APP_SECRET);
}



void testHardware() {
  // Read the current button state
  int currentButtonState = digitalRead(BUTTON_PIN);

  // Check for state changes (debounce)
  if (currentButtonState != lastButtonState) {
    lastDebounceTime = millis(); // Update the debounce timer
  }

  

      if (currentButtonState == LOW) {
        // Button is pressed
        Serial.println("Button Pressed!");
        ledState = !ledState; // Turn LED on
        digitalWrite(LED_PIN, ledState);
      } else {
        // Button is released
        Serial.println("Button Released!");
     
      }
}

void sendAlexa(){
    SinricPro.handle();  // Keep SinricPro connection alive

    // Read the current button state
    int currentButtonState = digitalRead(BUTTON_PIN);
    //Serial.println("Button State has Changed to - "+String(currentButtonState));  
   
    // Detect if the button state has changed
    if ((currentButtonState != lastButtonState) && isCountingDown == false) {
        lastDebounceTime = millis(); // Update debounce timer
        //Serial.println("Button State has Changed to - "+String(currentButtonState));
        isCountingDown = true;
        
    }

   // Serial.println("the delay is: " + String(millis() - lastDebounceTime));
    // Check if debounce delay has passed
    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
     
        // Only process if the state is different
        if (currentButtonState != lastButtonState) {
           isCountingDown = false;
            lastButtonState = currentButtonState; // Update last known state

            if (currentButtonState == LOW) {
                // Button is pressed
                Serial.println("Turned to " +String(ledState));
                ledState = !ledState;  // Toggle LED state
                digitalWrite(LED_PIN, ledState);  // Change LED state
               handleContactsensor(ledState);
            } else {
                // Button is released
                Serial.println("Button Released!");
              
               
            }
        }
    }
}

void loop() {
  //testHardware();
  sendAlexa();
}