#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>

// Define OLED pins for ESP32 (My Board - WROVER-E)
#define OLED_DC  16
#define OLED_CS  5
#define OLED_RST 17

U8G2_SSD1306_128X64_NONAME_F_HW_SPI u8g2(U8G2_R0, OLED_CS, OLED_DC, OLED_RST);

void setup(void) {
  Serial.begin(115200);          // Initialize serial communication
  while (!Serial) {              // Wait for the serial port to connect
    delay(10);
  }

  Serial.println("Enter text to display on the OLED:");

  u8g2.begin();
  u8g2.setFontPosTop();
  u8g2.setFont(u8g2_font_ncenB08_tr); // Choose a font
}

void loop(void) {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n'); // Read input from the serial monitor

    u8g2.clearBuffer();              
    u8g2.drawStr(0, 10, input.c_str());
    u8g2.sendBuffer();                

    Serial.print("Displayed: ");
    Serial.println(input);            
  }

  delay(100); 
}
