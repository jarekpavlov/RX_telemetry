#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
unsigned long timer = 0;

const byte pipe[3] = "ch2";         // NOTE: The same as in the receiver
RF24 radio(9,10);                       // select CE,CSN pin
      
struct ResponseSignal {
  byte voltage;
  unsigned int altitude;
};
ResponseSignal responseData;
void setup()
{
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();

                                      //Configure the NRF24 module
  radio.begin();
  radio.openReadingPipe(1,pipe);
  radio.setChannel(100);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);    // The lowest data rate value for more stable communication
  radio.setPALevel(RF24_PA_MAX);      // Output power is set for maximum
  radio.startListening();
  delay(50);
  timer = millis();
}

void loop()
{
  display.clearDisplay();
  while (radio.available()) {
    radio.read(&responseData, sizeof(ResponseSignal));// Receive the data
  }
  if (millis() - timer > 500) {
    timer = millis();
    display.setTextSize(1); 
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0,0);
    display.print(F("Volt. V: "));
    display.print((0.699 + (2.301/255) * responseData.voltage) * 5.72);
    display.setCursor(0,10);
    display.print(F("Alt. m:  "));
    display.print(responseData.altitude);
    display.display();
  }
}