#include "cloud.h"

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
}