/*  Copyright (c) 2020-2021, Collab
 *  All rights reserved
*/

#include "IOT.h"

AsyncMqttClient _mqttClient;
uint16_t _lastPacketIdPubAck;
bool _stopReconnect = false;
int _totalConnectionAttempts = 0;
int _totalReadings;
int _completedReadings = 0;

// callbacks
Method _connectedCb;
Method _disconnectedCb;
Method _publishReadyCb;
Method _failedConnectionCb;

void mqttTimeOut(void * parameter) {
  for (;;) {
    // pause the task
    vTaskDelay((MQTT_TIMEOUT * 1000) / portTICK_PERIOD_MS);

    if (!_mqttClient.connected()) {
      Log.warning(CR);
      Log.warning(F("MQTT - Connection timeout!" CR));
      Log.warning(F("==========================" CR));

      // give up
      _failedConnectionCb.callbackIntArg(1);

      // cleanup task
      vTaskDelete(NULL);
    }
  }
}

void connectToWifi() {
  ++_totalConnectionAttempts;

  Log.info(F("WiFi - SSID: %S" CR), WIFI_SSID);
  Log.info(F("WiFi - Connecting..." CR));

  // connect to WIFI
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void mqttConnect() {
  Log.info(F("MQTT - Timeout set to %d seconds" CR), MQTT_TIMEOUT);
  Log.info(F("MQTT - Connecting to %S" CR), MQTT_HOST);

  // connect to MQTT
  // Note that this will hang forever if there is no MQTT broker running
  _mqttClient.connect();

  // add timeout
  xTaskCreate(
    mqttTimeOut,               /* Task function. */
    "mqttTimeOutTask",         /* String with name of task. */
    10000,                     /* Stack size in words. */
    NULL,                      /* Parameter passed as input of the task */
    1,                         /* Priority of the task. */
    NULL                       /* Task handle. */
  );
}

void WiFiEvent(WiFiEvent_t event) {
  //Log.info(F("[WiFi-event] event: %d" CR), event);

  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      Log.info(F("WiFi - Connected." CR));
      Log.info(F("IP address: %p" CR), WiFi.localIP());
      Log.info(F("================================" CR));

      // connect to mqtt
      mqttConnect();
      break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
      Log.info(F("WiFi - Lost connection." CR));

      WiFi.disconnect(true);

      // notify others
      _failedConnectionCb.callbackIntArg(0);
      break;
    }
}

void onMqttConnect(bool sessionPresent) {
  Log.info(F("Connected to MQTT." CR));
  Log.info(F("================================" CR));

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
  _completedReadings++;

  //Log.verbose(F("MQTT - Publish acknowledged for packet %d" CR),
  //  _lastPacketIdPubAck
  //);

  if (_completedReadings >= _totalReadings) {
    Log.warning(F("MQTT - Published %d messages." CR), _totalReadings);

    // notify others
    _publishReadyCb.callback();

    // reset counter
    _completedReadings = 0;
  }
}

bool IOT::publishReady() {
  return _completedReadings == _totalReadings;
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

  // setup wifi
  WiFi.onEvent(WiFiEvent);

  // setup mqtt
  _mqttClient.onConnect(onMqttConnect);
  _mqttClient.onDisconnect(onMqttDisconnect);
  _mqttClient.onSubscribe(onMqttSubscribe);
  _mqttClient.onUnsubscribe(onMqttUnsubscribe);
  _mqttClient.onMessage(onMqttMessage);
  _mqttClient.onPublish(onMqttPublish);
  _mqttClient.setClientId(MQTT_CLIENT_ID);
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
