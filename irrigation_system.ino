#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

const char* ssid = "pandawa";
const char* password = "Werkud4r@";
const char* MQTT_SERVER = "192.168.1.105";
const char* MQTT_CONTROL_TOPIC = "irrigation/esp/control";
const char* MQTT_BRIGHTNESS_TOPIC = "irrigation/esp/brightness";

const char* MQTT_CLIENT_ID = "ESP32Client";
const char* MQTT_USERNAME = "esp32";
const char* MQTT_PASSWORD = "admin";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// Pin Setup
#define lamp_relay 18
#define pump_relay 19

#define num_pix     60
#define pin_strip   13
Adafruit_NeoPixel pixel(num_pix, pin_strip, NEO_GRB + NEO_KHZ800);

void setup_wifi() {
  delay(10);
  // Connect to Wifi Network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  // Serial Baud Rate
  Serial.begin(115200);
  setup_wifi();

  // MQTT Server Setup
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);

  pinMode(lamp_relay, OUTPUT);
  pinMode(pump_relay, OUTPUT);
  digitalWrite(lamp_relay, HIGH);
  digitalWrite(pump_relay, HIGH);

  pixel.begin();
  pixel.show();
}

void callback(char *topic, byte * message, unsigned int length) {

  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (String(topic) == MQTT_CONTROL_TOPIC) {
    Serial.print("Changing output to ");
    if (messageTemp == "off1") {
      Serial.println("off lamp");
      digitalWrite(lamp_relay, HIGH);
    }
    else if (messageTemp == "on1") {
      Serial.println("on lamp");
      digitalWrite(lamp_relay, LOW);
    }
    if (messageTemp == "off2") {
      Serial.println("off pump");
      digitalWrite(pump_relay, HIGH);
    }
    else if (messageTemp == "on2") {
      Serial.println("on pump");
      digitalWrite(pump_relay, LOW);
    }
    if (messageTemp == "off3") {
      //colorWipe(pixel.Color(R,B,G),Brightness);
      colorWipe(pixel.Color(0, 0, 0), 100);
    }
    else if (messageTemp == "on3") {
      //colorWipe(pixel.Color(R,B,G),Brightness);
      colorWipe(pixel.Color(0, 255, 0), 100);

    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("connected");
      // Subscribe
      client.subscribe(MQTT_CONTROL_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < pixel.numPixels(); i++) {
    pixel.setPixelColor(i, c);
    pixel.show();
    delay(wait);
  }
}

void loop() {
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}
