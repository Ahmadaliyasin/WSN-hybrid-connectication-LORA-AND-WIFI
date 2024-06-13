#include <LoRa.h>
#include <painlessMesh.h>

#define MESH_PREFIX "mesh_network"
#define MESH_PASSWORD "password"
#define MESH_PORT 5555
#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 2

const int maxNode = 100;
int node[maxNode] = {0};
String pesan[maxNode];

painlessMesh mesh;

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received message from Node %u: %s\n", from, msg.c_str());

  for (int i = 0; i < maxNode; i++) {
    if (node[i] == from || node[i] == 0) {
      node[i] = from;
      pesan[i] = msg;
      break;
    }
  }

  Serial.print("All received messages: ");
  for (int i = 0; i < maxNode && node[i] != 0; i++) {
    Serial.print(pesan[i]);
    if (node[i + 1] != 0) {
      Serial.print("; ");
    }
  }
  Serial.println("");
}

void setup() {
  Serial.begin(115200);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  while (!LoRa.begin(477E3)) {
    Serial.println("LoRa initialization failed. Check your connections.");
  }
  LoRa.setSpreadingFactor(12);  

  mesh.setDebugMsgTypes(ERROR);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT);
  mesh.onReceive(&receivedCallback);
}

unsigned long lastMessageTime = millis();  

void loop() {
  for (int i = 0; i < maxNode && node[i] != 0; i++) {
    Serial.print("Sending message via LoRa: ");
    Serial.println(pesan[i]);  

    sendLoRaMessage(pesan[i]);
    delay(1000); 
    
    
    lastMessageTime = millis();
  }

  mesh.update();
}

void sendLoRaMessage(String message) {
  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();
}

