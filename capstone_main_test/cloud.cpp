#include "cloud.h"
#include <PubSubClient.h>
#//include "WiFiEsp.h"
#include <WiFiEspAT.h>
#include "Adafruit_VL53L0X.h"

#ifndef HAVE_HWSERIAL1
#include <SoftwareSerial.h>
static SoftwareSerial Serial1(18, 19); // RX, TX
#endif

// ======== MQTT + WiFi Config ========
static const char *brokerAddress = "test.mosquitto.org";
static const char *mqtt_topic    = "4ID3_Group3/dispense";
static uint16_t    addressPort   = 1883;
static const char *clientID      = "ESP01";
static const char *mqtt_username = "";
static const char *mqtt_password = "";

static char ssid[] = "Learning Factory";
static char pass[] = "Factory2";

static int status           = WL_IDLE_STATUS;
static float totalDistance  = 150.0f;
static bool systemStatus    = false;

// ======== Hardware Objects ========
static WiFiClient wifiClient;
static PubSubClient  client(wifiClient);
static Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// ======== Internal Helpers ========
void checkClient(){
  client.loop();
}

bool Connect()
{
  // Connect to MQTT Server and subscribe to the topic
  if (client.connect(clientID, mqtt_username, mqtt_password))
  {
    client.subscribe(mqtt_topic);
    return true;
  }
  else
  {
    return false;
  }
}

static uint16_t getMeasurement()
{
  VL53L0X_RangingMeasurementData_t measure;

  Serial.print("Reading a measurement... ");
  lox.rangingTest(&measure, false); // false = no debug

  if (measure.RangeStatus != 4)
  {
    Serial.print("Distance (mm): ");
    Serial.println(measure.RangeMilliMeter);
    return measure.RangeMilliMeter;
  }
  else
  {
    Serial.println(" out of range ");
    return 0;
  }
}

static uint16_t getBatterLevel()
{
  if (getMeasurement()>totalDistance){
    return 0;
  }
  return (uint16_t)(totalDistance - getMeasurement());
}

static bool cloudConnectOnce()
{
  if (client.connect(clientID, mqtt_username, mqtt_password))
  {
    client.subscribe(mqtt_topic);
    return true;
  }
  return false;
}

void cloudReconnect() {
  //While the client remains unconnected from the MQTT broker, attempt to reconnect every 2 seconds
  //Also, print diagnostic information
  while (!client.connected()) {
    Serial.print("\nAttempting MQTT connection...");
  
    if (client.connect("ESP8266Client")) {
      Serial.println("Connected to MQTT server");  
      client.subscribe("testTopic");
    } else {
      Serial.print("\nFailed to connect to MQTT server, rc = ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

// static void cloudReconnect()
// {
//   while (!client.connected())
//   {
//     Serial.print("\nAttempting MQTT connection...");
//     if (client.connect("ESP8266Client"))
//     {
//       Serial.println("Connected to MQTT server");
//       client.subscribe("testTopic");
//     }
//     else
//     {
//       Serial.print("\nFailed to connect to MQTT server, rc = ");
//       Serial.print(client.state());
//       delay(2000);
//     }
//   }
// }

// ======== Public API ========
void Cloud_init()
{
  // initialize ESP module
  WiFi.init(&Serial1);
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true)
      ;
  }
  // attempt to connect to WiFi network
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  // Once connected, print the local IP address
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // you're connected now, so print out the data
  Serial.println("You're connected to the network");
  Serial.println();

  while (!Serial)
  {
    delay(1);
  }

  Serial.println("Adafruit VL53L0X test");
  if (!lox.begin())
  {
    Serial.println(F("Failed to boot VL53L0X"));
    while (1)
      ;
  }

  // power
  Serial.println(F("VL53L0X API Simple Ranging example\n\n"));

  Serial.println("You're connected to the network");
  Serial.println();

  client.setServer(brokerAddress, addressPort);
  if (Connect())
  {
    Serial.println("Connected Successfully to MQTT Broker!");
  }
  else
  {
    Serial.println("Connection Failed!");
  }

}

void Cloud_sendData()
{
  uint16_t batterLevel = getBatterLevel();
  float batterLevelPercent = (batterLevel / totalDistance) * 100;
  Serial.print("Level Percent: ");
  Serial.println(batterLevelPercent);

  if (!client.connected())
  {
    cloudReconnect(); 
  }

  // 3. Publish only if connected
  if (client.connected())
  {
    client.publish("Capstone/BatterLevel", String(batterLevelPercent).c_str()); 
    Serial.println("Published data.");
  }

  // if (!client.connected())
  // {
  //   cloudReconnect();
  // }
  // if (!client.loop())
  //   client.connect("wifiClient");
  // // Publish the data to the associated topics

  // if (WiFi.status() != WL_CONNECTED) {
  //   Serial.println("WiFi NOT connected!");
  // } 

  // client.publish("Capstone/BatterLevel", String(batterLevelPercent).c_str());
  // Serial.println("Published data.");
}

// void Cloud_sendData()
// {
//   systemStatus = !systemStatus;
//   uint16_t batterLevel       = getBatterLevel();
//   float batterLevelPercent   = (batterLevel / totalDistance) * 100.0f;

//   Serial.print("Level Percent: ");
//   Serial.println(batterLevelPercent);

//   if (!client.connected())
//   {
//     cloudReconnect();
//   }

//   // if (!client.loop())
//   // {
//   //   client.connect("ESP8266Client");
//   // }


// while(!client.loop()){
//    client.connect("ESP8266Client");
//   if (client.loop())
//   {
//     client.publish("Capstone/BatterLevel", String(batterLevelPercent).c_str());
//     client.publish("Capstone/SystemStatus", String(systemStatus).c_str());
//     Serial.println("Published data.");
//     return;
//   }
// }
//   Serial.println("DONE AND SENT");
// }