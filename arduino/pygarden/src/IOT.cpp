#include "IOT.h"

AsyncMqttClient _mqttClient;
TimerHandle_t _mqttReconnectTimer;
TimerHandle_t _wifiReconnectTimer;
uint16_t _lastPacketIdPubAck;
int _totalReadings;
// callbacks
Method _connectedCb;
Method _disconnectedCb;
Method _publishReadyCb;

void connectToWifi() {
  Serial.print("WiFi - Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void mqttConnect() {
  Serial.print("MQTT - Connecting to ");
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
      xTimerStop(_mqttReconnectTimer, 0);
      xTimerStart(_wifiReconnectTimer, 0);
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
  //Serial.println("Disconnected from MQTT.");

  // notify others
  _disconnectedCb.callback();
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
  _lastPacketIdPubAck = packetId;

  //Serial.print("Publish acknowledged: ");
  //Serial.println(_lastPacketIdPubAck);

  if (_lastPacketIdPubAck == _totalReadings) {
    // notify others
    _publishReadyCb.callback();
  }
}

IOT::IOT() {
}

void IOT::begin(
  int totalReadings,
  Method connected_callback,
  Method disconnected_callback,
  Method publishReady_callback
) {
  _totalReadings = totalReadings;
  _disconnectedCb = disconnected_callback;
  _connectedCb = connected_callback;
  _publishReadyCb = publishReady_callback;

  // setup timers
  _mqttReconnectTimer = xTimerCreate(
    "mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0,
    reinterpret_cast<TimerCallbackFunction_t>(mqttConnect)
  );
  _wifiReconnectTimer = xTimerCreate(
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

void IOT::disconnect() {
  xTimerStop(_mqttReconnectTimer, 0);

  _mqttClient.disconnect();
}

void IOT::publish(const char* topic, double value) {
  _lastPacketIdPubSent = _mqttClient.publish(topic, 1, true, String(value).c_str());

  bool debug = false;
  if (debug) {
    Serial.printf("Publishing on topic %s at QoS 1, packetId: ", topic);
    Serial.println(_lastPacketIdPubSent);
    Serial.printf("Message: %.2f \n", value);
  }
}
