#include "IOT.h"

AsyncMqttClient _mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;
Method _connectedCb;

void connectToWifi() {
  Serial.print("WiFi - Connecting to SSID: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void mqttConnect() {
  Serial.print("MQTT - Connecting to server: ");
  Serial.println(MQTT_HOST);

  _mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) {
  Serial.printf("[WiFi-event] event: %d\n", event);

  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Serial.println("==============================");
      Serial.println("WiFi connected.");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      Serial.println("==============================");

      // connect to mqtt
      mqttConnect();
      break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");

      // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
      xTimerStop(mqttReconnectTimer, 0);
      xTimerStart(wifiReconnectTimer, 0);
      break;
    }
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  Serial.println("==============================");

  // notify others
  _connectedCb.callback();
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  /*
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
  */
}

void onMqttUnsubscribe(uint16_t packetId) {
  /*
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  */
}

void onMqttMessage(
  char* topic,
  char* payload,
  AsyncMqttClientMessageProperties properties,
  size_t len,
  size_t index,
  size_t total
) {
  /*
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
  */
}

void onMqttPublish(uint16_t packetId) {
  /*
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  */
}

IOT::IOT() {
}

void IOT::begin(Method callback) {
  _connectedCb = callback;

  // setup timers
  mqttReconnectTimer = xTimerCreate(
    "mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0,
    reinterpret_cast<TimerCallbackFunction_t>(mqttConnect)
  );
  wifiReconnectTimer = xTimerCreate(
    "wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0,
    reinterpret_cast<TimerCallbackFunction_t>(connectToWifi)
  );

  // setup wifi
  WiFi.onEvent(WiFiEvent);

  // setup mqtt
  _mqttClient.onConnect(onMqttConnect);
  _mqttClient.onDisconnect(onMqttDisconnect);
  _mqttClient.onSubscribe(onMqttSubscribe);
  _mqttClient.onUnsubscribe(onMqttUnsubscribe);
  _mqttClient.onMessage(onMqttMessage);
  _mqttClient.onPublish(onMqttPublish);
  _mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  _mqttClient.setCredentials(MQTT_USER, MQTT_PASSWORD);

  // start connection
  connectToWifi();
}

void IOT::disconnectMqtt() {
  // stop timer
  xTimerStop(mqttReconnectTimer, 0);
}

void IOT::publish(const char* topic, double value) {
  uint16_t packetIdPub1 = _mqttClient.publish(topic, 1, true, String(value).c_str());

  bool debug = false;
  if (debug) {
    Serial.printf("Publishing on topic %s at QoS 1, packetId: ", topic);
    Serial.println(packetIdPub1);
    Serial.printf("Message: %.2f \n", value);
  }
}
