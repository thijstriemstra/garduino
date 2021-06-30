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
      Log.info(F("WiFi - Lost connection." CR));

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
  // notify others
  _disconnectedCb.callback();
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {}
void onMqttUnsubscribe(uint16_t packetId) {}

void onMqttMessage(
  char* topic,
  char* payload,
  AsyncMqttClientMessageProperties properties,
  size_t len,
  size_t index,
  size_t total
) {}

void onMqttPublish(uint16_t packetId) {
  _lastPacketIdPubAck = packetId;

  //Log.info(F("MQTT - Publish acknowledged for packet %d" CR),
  //  _lastPacketIdPubAck
  //);

  if (_lastPacketIdPubAck == _totalReadings) {
    Log.warning(F("MQTT - Published %d messages." CR), _totalReadings);

    // notify others
    _publishReadyCb.callback();
  }
}

bool IOT::publishReady() {
  return _lastPacketIdPubAck == _totalReadings;
}

void IOT::exit() {
  // notify others
  _publishReadyCb.callback();
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

  // set initial so publishReady() is correct
  _lastPacketIdPubAck = _totalReadings;

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

  Log.info(F("MQTT - Disconnecting..." CR));
  _mqttClient.disconnect();

  Log.info(F("WiFi - Disconnecting..." CR));
  WiFi.disconnect(true);
}

void IOT::publish(const char* sub_topic, double value) {
  // check for connection
  if (_mqttClient.connected()) {
    char mainTopic[80];
    sprintf(mainTopic, "%s%s", MQTT_BASE_TOPIC, sub_topic);

    // publish
    bool retain = true;
    _lastPacketIdPubSent = _mqttClient.publish(
      mainTopic,
      _qos,
      retain,
      String(value).c_str()
    );

    bool debug = false;
    if (debug) {
      Log.info(F("Publishing on topic %S at QoS %d, packetId: %d" CR),
        mainTopic, _qos, _lastPacketIdPubSent
      );
      Log.info(F("Message: %D" CR), value);
    }
  } else {
    // no connection
    Log.warning(F("MQTT - Cannot publish message: not connected" CR));
  }
}
