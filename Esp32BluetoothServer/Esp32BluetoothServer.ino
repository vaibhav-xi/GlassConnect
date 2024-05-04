#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX   "6E400002-B5A3-F393-E0A9-E50E24DCCA9E" // RX characteristic UUID
#define CHARACTERISTIC_UUID_TX   "6E400003-B5A3-F393-E0A9-E50E24DCCA9E" // TX characteristic UUID

BLEServer *pServer;
BLECharacteristic *pCharacteristicRX;
BLECharacteristic *pCharacteristicTX;
bool deviceConnected = false;
bool oldDeviceConnected = false;
String receivedMessage = "";

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
    }

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
    }
};

class MyCharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() > 0) {
            Serial.println("Received value: " + String(rxValue.c_str()));
            receivedMessage = String(rxValue.c_str());
        }
    }
};

void setup() {
    Serial.begin(115200);

    BLEDevice::init("ESP32_BLE_Server");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *pService = pServer->createService(SERVICE_UUID);
    
    pCharacteristicRX = pService->createCharacteristic(
                           CHARACTERISTIC_UUID_RX,
                           BLECharacteristic::PROPERTY_WRITE
                         );
    pCharacteristicRX->setCallbacks(new MyCharacteristicCallbacks());

    pCharacteristicTX = pService->createCharacteristic(
                           CHARACTERISTIC_UUID_TX,
                           BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
                         );
    pCharacteristicTX->addDescriptor(new BLE2902());

    pService->start();
    pServer->getAdvertising()->start();

    Serial.println("Waiting for a connection...");
}

void loop() {
    if (deviceConnected) {
        if (!oldDeviceConnected) {
            Serial.println("Device connected");
            oldDeviceConnected = true;
        }

        if (receivedMessage != "") {
            Serial.println("Received message: " + receivedMessage);
            receivedMessage = ""; // Clear the received message
        }

        // Send data back to the mobile app
        if (pCharacteristicTX->getValue().length() > 0) {
            pCharacteristicTX->setValue("");
        }
    } else {
        if (oldDeviceConnected) {
            Serial.println("Device disconnected");
            oldDeviceConnected = false;
        }
    }

    delay(10);
}
