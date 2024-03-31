#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <DHTesp.h>


//BLE server name
#define bleServerName "DHT11_BLE"

#define DHT 26
DHTesp dhtSensor;

double DHT_temp;
//double DHT_humidity;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 3000;

bool deviceConnected = false;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID "91bad492-b950-4226-aa2b-4ede9fa42f59"

// Temperature Characteristic and Descriptor
BLECharacteristic DHT11TemperatureCharacteristics("cba1d466-344c-4be3-ab3f-189f80dd7518", BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor DHT11TemperatureDescriptor(BLEUUID((uint16_t)0x2902));


// Humidity Characteristic and Descriptor
//BLECharacteristic DHT11HumidityCharacteristics("ca73b3ba-39f6-4ab3-91ae-186dc9577d99", BLECharacteristic::PROPERTY_NOTIFY);
//BLEDescriptor DHT11HumidityDescriptor(BLEUUID((uint16_t)0x2903));

//Setup callbacks onConnect and onDisconnect
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};


void setup() {
  // Start serial communication 
  Serial.begin(115200);

  dhtSensor.setup(DHT, DHTesp::DHT11);

  // Create the BLE Device
  BLEDevice::init(bleServerName);

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *DHT11Service = pServer->createService(SERVICE_UUID);

  // Create BLE Characteristics and Create a BLE Descriptor
  // Temperature
    DHT11Service->addCharacteristic(&DHT11TemperatureCharacteristics);
    DHT11TemperatureDescriptor.setValue("DHT11 temperature Celsius");
    DHT11TemperatureCharacteristics.addDescriptor(new BLE2902());


  // Humidity
  //DHT11Service->addCharacteristic(&DHT11HumidityCharacteristics);
  //DHT11HumidityDescriptor.setValue("DHT11 humidity");
  //DHT11HumidityCharacteristics.addDescriptor(new BLE2902());
  
  // Start the service
  DHT11Service->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  if (deviceConnected) {
    
    if ((millis() - lastTime) > timerDelay) {
      TempAndHumidity data = dhtSensor.getTempAndHumidity();
  
      DHT_temp =  data.temperature;
      //DHT_humidity = data.humidity;
      
  
      //Notify temperature reading from BME sensor
      static char temperatureTemp[6];
      dtostrf(DHT_temp, 6, 2, temperatureTemp);
      //Set temperature Characteristic value and notify connected client
      DHT11TemperatureCharacteristics.setValue(temperatureTemp);
      DHT11TemperatureCharacteristics.notify();
      Serial.print("Temperature: ");
      Serial.print(temperatureTemp);
      Serial.print(" ºC");
      
      //Notify humidity reading from BME
      //static char humidityTemp[6];
      //dtostrf(DHT_humidity, 6, 2, humidityTemp);
      //Set humidity Characteristic value and notify connected client
      //DHT11HumidityCharacteristics.setValue(humidityTemp);
     // DHT11HumidityCharacteristics.notify();   
      //Serial.print(" - Humidity: ");
      //Serial.print(DHT_humidity);
      //Serial.println(" %");
      
      lastTime = millis();
    }
  }
}