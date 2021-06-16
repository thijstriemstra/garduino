/*  Copyright (c) 2020-2021, Collab
 *  All rights reserved
*/

#include "IOT.h"

AsyncMqttClient _mqttClient;
uint16_t _lastPacketIdPubAck;
bool _stopReconnect = false;
int _totalConnectionAttempts = 0;
int _totalReadings;

// callbacks
Method _connectedCb;
Method _disconnectedCb;
Method _publishReadyCb;
Method _failedConnectionCb;

void connectToWifi() {
  ++_totalConnectionAttempts;

  Log.info(F("WiFi - SSID: %S" CR), WIFI_SSID);
  Log.info(F("WiFi - Connecting..." CR));

  //WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void mqttConnect() {
  Log.info(F("MQTT - Connecting to %S" CR), MQTT_HOST);

  _mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) {
  //Log.info(F("[WiFi-event] event: %d" CR), event);

  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Log.info(F("WiFi - Connected." CR));
      Log.info(F("IP address: %p" CR), WiFi.localIP());
      Log.info(F("==============================" CR));

      // connect to mqtt
      mqttConnect();
      break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
      Log.info(F("WiFi - lost connection." CR));

      WiFi.disconnect(true);

      // notify others
      _failedConnectionCb.callback();
      break;
    }
}

void onMqttConnect(bool sessionPresent) {
  Log.info(F("Connected to MQTT." CR));
  Log.info(F("==============================" CR));

  // notify others
  _connectedCb.callback();
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  //Log.info(F("Disconnected from MQTT."));

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
  Method publishReady_callback,
  Method connectionFailed_callback
) {
  _totalReadings = totalReadings;
  _disconnectedCb = disconnected_callback;
  _connectedCb = connected_callback;
  _failedConnectionCb = connectionFailed_callback;
  _publishReadyCb = publishReady_callback;

  // setup wifi
  WiFi.onEvent(WiFiEvent);

  // setup mqtt
  _mqttClient.onConnect(onMqttConnect);
  _mqttClient.onDisconnect(onMqttDisconnect);
  _mqttClient.onSubscribe(onMqttSubscribe);
  _mqttClient.onUnsubscribe(onMqttUnsubscribe);
  _mqttClient.onMessage(onMqttMessage);
  _mqttClient.onPublish(onMqttPublish);
  _mqttClient.setKeepAlive(MQTT_TIMEOUT);
  _mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  _mqttClient.setCredentials(MQTT_USER, MQTT_PASSWORD);

  connect();
}

void IOT::connect() {
  // start connection
  connectToWifi();
}

bool IOT::connected() {
  return _mqttClient.connected();
}

void IOT::disconnect() {
  _stopReconnect = true;

  WiFi.disconnect(true);
  _mqttClient.disconnect();
}

void IOT::publish(const char* sub_topic, double value) {
  // check for connection
  if (_mqttClient.connected()) {
    char mainTopic[80];
    sprintf(mainTopic, "%s%s", MQTT_BASE_TOPIC, sub_topic);

    // publish
    _lastPacketIdPubSent = _mqttClient.publish(mainTopic, 1, true, String(value).c_str());
    bool debug = false;
    if (debug) {
      Log.info(F("Publishing on topic %S at QoS 1, packetId: %d" CR),
        mainTopic, _lastPacketIdPubSent
      );
      Log.info(F("Message: %D" CR), value);
    }
  } else {
    // no connection
    Log.warning(F("Cannot publish message: not connected to MQTT" CR));
  }
}
