#include "epd4in2.h"
#include "epdpaint.h"
/*

  TCL IOT script
  Tim Waizenegger (c) 2018
  ESP8266 on arduino, pusub mqtt client, eink display module

https://github.com/timwaizenegger/e-ink-display-esp8266-mqtt-openwhisk

E-Ink display. I used a Waveshare 4.2in display with SPI connection. https://www.ebay.com/itm/4-2inch-E-Ink-Display-400x300-E-paper-Module-SPI-for-Raspberry-Pi-Arduino-STM32 Different brands and different conections may not be compatible with the Software 

*/


// NOTE: we need to increse the max packet size for our mqtt lib. its too small for these image messages
//   have to define this in /home/tim/Arduino/libraries/PubSubClient/src/PubSubClient.h
//   We need to figure out how to set these vars here...
//   this is the max length of our pixel data

// #define MQTT_MAX_PACKET_SIZE 15100 


#include <ESP8266WiFi.h>
#include <PubSubClient.h>
extern "C" {
#include "user_interface.h"
}

#define COLORED     0
#define UNCOLORED   1



#define SLEEPTIME 50000
unsigned long previousMillis = 0;


// And here we have triple-WIFI support :)
const char* ssid1 = "wifi name 1";
const char* password1 = "pw";

const char* ssid2 = "a";
const char* password2 = "a";

const char* ssid3 = "a-a-a";
const char* password3 = "a";

const char* mq_id = "mqttid";
const char* mq_authtoken = "password or token";
const char* mq_serverUrl = "example.com";


// topic is in multiple places in the code...
// const char* mq_topic = "displaychannel-eink2/byte-image-data"


WiFiClient espClient;
PubSubClient client(mq_serverUrl, 1883, espClient);



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// ARDUINO API


void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  Serial.println("Begin e-Paper display");

  //draw_status_display("Start...", "mqtt esp8266 eink display");

  draw_status_display("starting", "connecting to wifi network");
  setup_wifi();
  client.setServer(mq_serverUrl, 1883);
  client.setCallback(callback);



  Serial.println("starting ...");

}

void loop() {


  if (!client.connected()) {
    reconnect();
  }
  client.loop();




  // update the display regularly
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= SLEEPTIME) {
    previousMillis = currentMillis;

    Serial.println("ping");

  }

}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// MQTT WIFI

void setup_wifi() {


  //draw_status_display("WIFI", "connecting to WIFI");

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");

  while (WiFi.status() != WL_CONNECTED) {

    Serial.println(ssid1);
    WiFi.begin(ssid1, password1);
    for (int i = 0; i < 200; i++) {
      delay(100);
      Serial.print(".");
      Serial.print(WiFi.status());
      if (WiFi.status() == WL_CONNECTED) {
        break;
      }
    }
    if (WiFi.status() == WL_CONNECTED) {
      break;
    }
    Serial.println(ssid2);
    WiFi.begin(ssid2, password2);
    for (int i = 0; i < 200; i++) {
      delay(100);
      Serial.print(".");
      Serial.print(WiFi.status());
      if (WiFi.status() == WL_CONNECTED) {
        break;
      }
    }
    if (WiFi.status() == WL_CONNECTED) {
      break;
    }
    Serial.println(ssid3);
    WiFi.begin(ssid3, password3);
    for (int i = 0; i < 200; i++) {
      delay(100);
      Serial.print(".");
      Serial.print(WiFi.status());
      if (WiFi.status() == WL_CONNECTED) {
        break;
      }
    }



  }
  delay(500);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  draw_status_display("wifi connection", "online");
}




void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  if (strcmp(topic, "displaychannel-eink2/byte-image-data") == 0) {

    const int buffer_len = 15100;

    int copy_length = length;
    if (length > buffer_len) copy_length = buffer_len;

    Serial.println();
    Serial.println("Copying byte image data. Size of msg:");
    Serial.println(length);
    Serial.println("buffer length is:");
    Serial.println(buffer_len);

    unsigned char *image = new unsigned char[buffer_len];

    for (int i = 0; i < copy_length; i++) {
      //Serial.print((char)payload[i]);
      image[i] = payload[i];

    }
    Serial.println("Sending bytes to eink display");
    draw_data(image);

  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    draw_status_display("MQTT broker", "connecting ...");
    // Attempt to connect
    if (client.connect(mq_id, mq_id, mq_authtoken)) {
      Serial.println("connected");

      draw_status_display("MQTT connected!", "waiting for MQTT pixel data...");
      // ... and resubscribe
      client.subscribe("displaychannel-eink2/byte-image-data");
    } else {
      draw_status_display("MQTT", "MQTT connection failed");
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}












////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// EINK DISPLAY
void draw_status_display(char* title, char* message) {
  Serial.println("drawing status display message");
  Serial.println(title);
  Serial.println(message);

  const int buffer_len = 15100;
  unsigned char *image = new unsigned char[buffer_len];

  Paint paint(image, 400, 300);
  paint.Clear(UNCOLORED);


  paint.DrawStringAt(5, 5, title, &Font24, COLORED);
  paint.DrawStringAt(0, 100, message, &Font16, COLORED);


  Epd epd;
  if (epd.Init() != 0) {
    Serial.println("e-Paper init failed");
    return;
  }
  epd.ClearFrame();
  epd.DisplayFrame(paint.GetImage());
  delete(image);
  epd.Sleep();


}




void draw_data(unsigned char *image) {
  Serial.println("drawing display from bytes");

  Epd epd;
  if (epd.Init() != 0) {
    Serial.println("e-Paper init failed");
    return;
  }
  epd.ClearFrame();
  epd.DisplayFrame(image);
  delete(image);
  epd.Sleep();
}


