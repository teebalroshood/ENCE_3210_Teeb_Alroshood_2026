
#include "images.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* OLED MACROS */
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BUTTON_1 2
#define BUTTON_2 3

void setup() {
  // Start Serial Port
  Serial.begin(115200);

  // Init Buttons
  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_2, INPUT);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Set OLED Text Color
  display.setTextColor(SSD1306_WHITE);

  // Clear the buffer
  display.clearDisplay();
}

void loop() {
  // Show analog values on screen
  if(!digitalRead(BUTTON_1))
    screen_TextExample();
  else
    screen_ImageExample();

  // Set screen rotation
  if(digitalRead(BUTTON_2))
    display.setRotation(0);
  else
    display.setRotation(2);
}

void screen_TextExample(){
  // Render OLED images
  display.clearDisplay();

  // Render OLED Temperature Value
  display.setTextSize(2); // Draw 2X-scale text
  display.setCursor(10, 10);
  display.print(analogRead(A0));

  display.setCursor(70, 10);
  display.print(analogRead(A1));

  // Display OLED images
  display.display();
}

void screen_ImageExample(){
  
  for(int i=0; i<4; i++){
    // Render OLED images
    display.clearDisplay();
    
    display.drawBitmap(0, 0, images[i], SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);

    // Display OLED images
    display.display();

    delay(2000);
  }

}
