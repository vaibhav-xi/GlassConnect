#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

bool isConnectedToWifi() {
    return WiFi.status() == WL_CONNECTED;
}

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("Device connected via Bluetooth.");
  }

  void onDisconnect(BLEServer* pServer) {
    Serial.println("Device disconnected.");
  }
};

void sendResultToApp(BLECharacteristic* pCharacteristic, const char* message) {
    pCharacteristic->setValue(message);
    pCharacteristic->notify();
}

void onWrite(BLECharacteristic* pCharacteristic) {
  Serial.println("Received Something");

  std::string value = pCharacteristic->getValue();

  if (value.length() > 0) {
      Serial.println("Received value:");
      Serial.println(value.c_str()); // Print the received value

      // Extract WiFi credentials and attempt connection
      // Format: ssid,password
      String ssid, password;
      int commaIndex = value.find(",");
      if (commaIndex != -1) {
          ssid = value.substr(0, commaIndex).c_str();
          password = value.substr(commaIndex + 1).c_str();
          
          Serial.println("Attempting to connect to WiFi...");
          if (WiFi.begin(ssid.c_str(), password.c_str()) == WL_CONNECTED) {
              Serial.println("Connected to WiFi.");
              sendResultToApp(pCharacteristic, "WiFi connection successful.");
          } else {
              Serial.println("Failed to connect to WiFi.");
              sendResultToApp(pCharacteristic, "Unable to connect to WiFi.");
          }
      }
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize WiFi
  WiFi.begin();

  unsigned long startTime = millis(); // Start time
  const unsigned long timeout = 30000;

  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startTime >= timeout) {
      Serial.println("WiFi connection timeout.");
      break; // Exit loop if timeout is reached
    }

    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  if (!isConnectedToWifi()) {
    // Initialize BLE
    BLEDevice::init("GlassConnect"); // Set Bluetooth name here
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                            CHARACTERISTIC_UUID,
                                            BLECharacteristic::PROPERTY_READ |
                                            BLECharacteristic::PROPERTY_WRITE |
                                            BLECharacteristic::PROPERTY_NOTIFY |
                                            BLECharacteristic::PROPERTY_INDICATE
                                        );

    pService->start();

    // Advertise service
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->addServiceUUID(pService->getUUID());
    pAdvertising->start();

    Serial.println("Waiting for connections...");
  } else {
    Serial.println("Connected to WiFi.");
  }
}

void loop() {
    // Your loop code
}
