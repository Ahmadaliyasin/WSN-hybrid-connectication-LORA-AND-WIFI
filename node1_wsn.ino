#include <BH1750.h>
#include <DHT.h>
#include "painlessMesh.h"
#include <Wire.h>

#define MESH_PREFIX "mesh_network"
#define MESH_PASSWORD "password"
#define MESH_PORT 5555
Scheduler userScheduler;
painlessMesh mesh;
DHT dht(4, DHT22);
BH1750 lightMeter;
String pesan;
boolean led = 2;
String namaNode = "node2-2"; // Nama node

void sendMessage();

Task taskSendMessage(TASK_SECOND *10, TASK_FOREVER, &sendMessage);

void sendMessage()
{
  mesh.sendBroadcast( pesan );
  taskSendMessage.setInterval(TASK_SECOND * 10);
}

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  dht.begin();
  Serial.println("Initializing...");
  mesh.setDebugMsgTypes(ERROR | STARTUP);
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
  lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);

  Serial.println(F("BH1750 One-Time Test"));
}

void loop()
{
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(hic);
  Serial.print(F("°C "));
  

  while (!lightMeter.measurementReady())
  {
    yield();
  }

  float lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx"); 
  lightMeter.configure(BH1750::ONE_TIME_HIGH_RES_MODE);

  Serial.print(" kirim data");
    pesan = (namaNode + " => lux :" + String(lux) + ", suhu:" + String(t) + "C" +", kelembapan:"+ String(h) + " %");
  Serial.println();
  mesh.update();
}
