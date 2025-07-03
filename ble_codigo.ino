#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define PWM_PIN 10  




BLEServer* pServer = NULL; //servidor de BLE
BLECharacteristic* pSensorCharacteristic = NULL; //caracteristics para mandar datos (notificar valores)
BLECharacteristic* pLedCharacteristic = NULL; //caracteristics para recibir datos (prende y apaga led)
bool deviceConnected = false; //para ver si hay un cliente conectado a BLE
bool oldDeviceConnected = false; //para detectar cambios en la conexión
uint32_t value = 0; //contador que se manda por BLE

const int ledPin = 2; //pin del led

// See the following for generating UUIDs: tengo que generar los míos
// https://www.uuidgenerator.net/
#define SERVICE_UUID        "11a028b4-c028-4b4d-ac25-d6d262ff6ba5"
#define SENSOR_CHARACTERISTIC_UUID "11a028b4-c028-4b4d-ac25-d6d262ff6ba5"
#define LED_CHARACTERISTIC_UUID "11a028b4-c028-4b4d-ac25-d6d262ff6ba5"

//cuando un cliente se conecta o desconecta
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

//se ejecuta cuando el cliente BLE escribe en caracteristic del LED (si recibe 1 se prende el led)
class MyCharacteristicCallbacks : public BLECharacteristicCallbacks { 
  void onWrite(BLECharacteristic* pLedCharacteristic) {
    String rxValue = String(pLedCharacteristic->getValue().c_str());
 
    if (rxValue.length() > 0) {
      Serial.print("Characteristic event, written: ");
      Serial.println(static_cast<int>(rxValue[0])); // Print the integer value

      int receivedValue = static_cast<int>(rxValue[0]);
      if (receivedValue == 1) {
        digitalWrite(ledPin, HIGH);
      } else {
        digitalWrite(ledPin, LOW);
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(PWM_PIN, 0);
  // inicializa el BLE con el nombre ESP32
  BLEDevice::init("ESP32");

  //crea el servidor BLE y configura los callbacks
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  //crea el servicio BLE con un UUID específico
  BLEService *pService = pServer->createService(SERVICE_UUID);

  //puede leerse, escribirse y enviar notificaciones
  pSensorCharacteristic = pService->createCharacteristic(
                      SENSOR_CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  //solo puede escribirse
  pLedCharacteristic = pService->createCharacteristic(
                      LED_CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_WRITE
                    );

  //asocia el callback a la caracteristic del led
  pLedCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // agrega caracteristics que le permiten a los clientes habilitar notificaciones
  pSensorCharacteristic->addDescriptor(new BLE2902());
  pLedCharacteristic->addDescriptor(new BLE2902());

  //inicia el servicio
  pService->start();

  //emite la señal para que otros dispositivos puedan encontrar al ESP32 por BLE
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  //si hay alguien conectado manda una notificación y el value se incrementa 
  if (deviceConnected) {
    pSensorCharacteristic->setValue(String(value).c_str());
    pSensorCharacteristic->notify();
    value++;
    Serial.print("New value notified: ");
    Serial.println(value);
    delay(3000); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
  }
  //si el dispositivo se deconecta se reinicia el servidor para permitir nuevas conexiones
  if (!deviceConnected && oldDeviceConnected) {
    Serial.println("Device disconnected.");
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("Start advertising");
    oldDeviceConnected = deviceConnected;
  }
  //si se conectó un cliente se muestra por el puerto serie
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
    Serial.println("Device Connected");
  }
}

/*void regularVolumen(volumen){
  int volumen_recibido;
  int pwmValor = map()
}

void identificarDatos() {
  if ()
}*/












