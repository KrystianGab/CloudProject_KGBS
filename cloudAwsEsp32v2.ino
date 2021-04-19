#include "SPIFFS.h"
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <PubSubClient.h>
#include "topSecret.h"

const char* ssid = "vodafone-9C33"; //Provide your SSID
const char* password = "GQ8FU4JCLMMB64RR"; // Provide Password

const char* mqtt_server = "a3ddoytnj05dne-ats.iot.us-east-1.amazonaws.com"; // Relace with your MQTT END point
const int mqtt_port = 8883;


char *ourTopic = "testTopic";
char testString[30] = "Test message!";

int hallMonitorVal = 0;
int touchSensorVal=50;
int hallMonitorPrevVal = -1; 
int touchSensorPrevVal=0;//set previous value check for false-positive cases;
int touchSensorFlag = 0;
int hallMonitorFlag = 0;

char *touchSensorStatus = "TOUCH";
char *hallMonitorStatus = "HALL";

WiFiClientSecure espClient;
PubSubClient client(espClient);

void ourCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(ourTopic, "hello world");
      // ... and resubscribe
      client.subscribe(ourTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void hallMonitor(){
  hallMonitorVal = hallRead();  //read Hall sensor

  //Send data only once per flip; discard false-positives
  //Hall monitor
  if((hallMonitorVal<=0)&&(hallMonitorPrevVal<=0)){
    Serial.println(hallMonitorVal);
    if (hallMonitorFlag == 1){
      char payloadHall[30] = "Hall Sensor Tripped";
      client.publish(hallMonitorStatus, payloadHall);
      delay(1000);
    }
    hallMonitorFlag = 0;
  } 
  else if((hallMonitorVal>0)&&(hallMonitorPrevVal>0)){
    Serial.println(hallMonitorVal);
    if (hallMonitorFlag == 0){
      char payloadHall[30] = "Hall Sensor Set";
      client.publish(hallMonitorStatus, payloadHall);
      delay(1000);
    }
    hallMonitorFlag = 1;
  }

  hallMonitorPrevVal = hallMonitorVal;
  
}

void touchSensor(){
  touchSensorVal = touchRead(T0); //read T0 pin touch sensor

  //Send data only once per flip; discard false-positives
  //TouchSensor
  if((touchSensorVal <35) && (touchSensorPrevVal<35)){
    Serial.println(touchSensorVal);
    if (touchSensorFlag == 0){
      char payloadTouch[30] = "Touch Sensor Set";
      client.publish(touchSensorStatus, payloadTouch);
      delay(1000); 
    }
    touchSensorFlag = 1;
  } 
  else if ((touchSensorVal >35) && (touchSensorPrevVal>35)){
    Serial.println(touchSensorVal);
    if (touchSensorFlag == 1){
      char payloadTouch[30] = "Touch Sensor Tripped";
      client.publish(touchSensorStatus, payloadTouch);
      delay(1000); 
    }
    touchSensorFlag = 0;
  }

  touchSensorPrevVal = touchSensorVal;
  
}
  


void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  espClient.setCACert(AmazonRootCA1);
  espClient.setCertificate(certificatePem);
  espClient.setPrivateKey(privatePem);

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(ourCallback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  hallMonitor();

  touchSensor();
  
  delay(1000); 
}
