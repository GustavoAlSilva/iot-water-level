#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

const int WATER_SENSOR_PIN = 32;
const char* SSID = "<seu_ssid>";
const char* PASSWORD = "<sua_senha>";

const char* MQTT_SERVER = "2eb50930f18143728c49e987c49a249f.s1.eu.hivemq.cloud";
const int MQTT_PORT = 8883;
const char* MQTT_TOPIC_ESP32_IP = "sensor/water/esp32_ip";
const char* MQTT_TOPIC_BROKER_PORT = "sensor/water/broker_port";
const char* MQTT_TOPIC_CLIENT_PORT = "sensor/water/client_port";
const char* MQTT_TOPIC_STATUS = "sensor/water/status";
const char* MQTT_USERNAME = "Publisher";
const char* MQTT_PASSWORD = "Publisher123";

const char* WATER_DETECTED_STATUS = "ALTO";
const char* WATER_LOW_STATUS = "NORMAL";

WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

void setupWifi() {

  Serial.println();
  Serial.print("Conectando-se à rede Wi-Fi ");
  Serial.println(SSID);

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {

    delay(1000);

    Serial.print(".");
  }

  Serial.println();
  Serial.println("Conectado ao Wi-Fi!");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void connectToMQTT() {

  while (!mqttClient.connected()) {

    Serial.print("Conectando-se ao broker MQTT HiveMQ... ");

    if (mqttClient.connect("ESP32Client", MQTT_USERNAME, MQTT_PASSWORD)) {

      Serial.println("Conectado!");
      Serial.println();
    } else {

      Serial.println("Falha na conexão.");
      Serial.print("Código: ");
      Serial.println(mqttClient.state());

      delay(1000);
    }
  }
}

void publish(const char* status) {

  Serial.println(status);

  mqttClient.publish(MQTT_TOPIC_STATUS, status);
  mqttClient.publish(MQTT_TOPIC_ESP32_IP, WiFi.localIP().toString().c_str());
  mqttClient.publish(MQTT_TOPIC_BROKER_PORT, String(MQTT_PORT).c_str());
  mqttClient.publish(MQTT_TOPIC_CLIENT_PORT, String(espClient.localPort()).c_str());
}

void setup() {

  Serial.begin(115200);

  delay(1000);

  setupWifi();

  espClient.setInsecure();
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  connectToMQTT();

  pinMode(WATER_SENSOR_PIN, INPUT_PULLUP);
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {

    setupWifi();
  }

  connectToMQTT();

  mqttClient.loop();

  int sensorState = digitalRead(WATER_SENSOR_PIN);

  if (sensorState == LOW) {

    publish(WATER_DETECTED_STATUS);
  } else {

    publish(WATER_LOW_STATUS);
  }

  delay(5000);
}
