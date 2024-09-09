#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int flowSensorPin = 5;   // GPIO 5 for flow rate sensor input

volatile unsigned long pulseCount = 0;     // Cumulative pulse count
volatile unsigned long lastPulseCount = 0; // Pulse count for the last second
unsigned long lastPulseTime = 0;           // Time of the last pulse
unsigned long pulsesPerSecond = 0;         // Pulse count per second

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void pulseCounter() {
  pulseCount++;
}

void setup() {
  Serial.begin(9600);

  // Initialize I2C with specific pins
  Wire.begin(9, 8); // SDA on pin 6, SCL on pin 7

  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  //display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  
  pinMode(flowSensorPin, INPUT_PULLUP); // INPUT_PULLUP to use internal pull-up resistor

  // Attach interrupt to handle flow sensor pulses
  attachInterrupt(digitalPinToInterrupt(flowSensorPin), pulseCounter, RISING);
}

void loop() {
  unsigned long now = millis();
  
  // Calculate pulses per second
  if (now - lastPulseTime >= 1000) {
    detachInterrupt(digitalPinToInterrupt(flowSensorPin)); // Disable interrupt to safely read pulseCount
    pulsesPerSecond = pulseCount - lastPulseCount;
    lastPulseCount = pulseCount;
    lastPulseTime = now;
    attachInterrupt(digitalPinToInterrupt(flowSensorPin), pulseCounter, RISING); // Re-enable interrupt
  }

  // Display cumulative pulse count and pulses per second on OLED
  //display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Cumulative Pulses: ");
  display.println(pulseCount);
  display.print("Pulses per Second: ");
  display.println(pulsesPerSecond);
  display.display();
  
  // Print to Serial Monitor for debugging or additional output
  Serial.print("Cumulative Pulses: ");
  Serial.print(pulseCount);
  Serial.print(", Pulses per Second: ");
  Serial.println(pulsesPerSecond);
  
  delay(1000); // Update every second
}
