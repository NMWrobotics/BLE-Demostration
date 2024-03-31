#include "BluetoothSerial.h"
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
 
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

char* temperatureChar;
 
// Bluetooth Serial Object (Handle)
BluetoothSerial SerialBT;
 
// ESP32 Bluetooth (Slave) Device Information
const char *pin = "1234"; // Change this to more secure PIN.
String device_name = "ESP32-BT-Slave";
 
// Bluetooth Received Byte & Message Buffer Array
String RxBuffer = "";
char RxByte;
 
void setup() {
    //OLED display setup
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE,0);
  display.setCursor(0,25);
  display.print("Bl Client");
  display.display();

  Serial.begin(115200);

  SerialBT.begin(device_name); //Bluetooth device name

  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());

  SerialBT.setPin(pin);
  
  Serial.println("Using PIN");
}
 
void loop() {
  // Read The Received Bytes & Add Them To Message (RxBuffer) Array
  if (SerialBT.available()){
    RxByte = SerialBT.read();
    if (RxByte != '\n'){
      RxBuffer += String(RxByte);
    }
    else{
      RxBuffer = "";
    }
    
  }
  Serial.println(RxBuffer);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE,0);
  display.setCursor(0,25);
  display.print(RxBuffer);
  display.display();
  delay(3000);
}