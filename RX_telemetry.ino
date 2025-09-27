#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);
bool buttonWasPressed = false;
const byte pipe[][3] = {"ch1"};         // NOTE: The same as in the receiver
RF24 radio(9,10);                       // select CE,CSN pin
      
struct ResponseSignal {
  byte voltage;
  unsigned int altitude;
};
ResponseSignal responseData;
void setup()
{
  lcd.init();                     
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Voltage:");
  lcd.setCursor(0, 1);
  lcd.print("Altitude:");
  lcd.setCursor(0, 2);
  lcd.print("Width:");
  lcd.setCursor(0, 3);
  lcd.print("Height:");
                                      //Configure the NRF24 module
  radio.begin();
  radio.openReadingPipe(1,pipe[0]);
  radio.setChannel(100);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);    // The lowest data rate value for more stable communication
  radio.setPALevel(RF24_PA_MAX);      // Output power is set for maximum
  radio.startListening();
  delay(50);
}

void loop()
{
while (radio.available()) {
  radio.read(&responseData, sizeof(ResponseSignal));// Receive the data
}
  lcd.setCursor(14, 0);
  lcd.print((0.875 + (2.125/255) * responseData.voltage) * 5.71);
  lcd.setCursor(14, 1);
  lcd.print(responseData.altitude);
}