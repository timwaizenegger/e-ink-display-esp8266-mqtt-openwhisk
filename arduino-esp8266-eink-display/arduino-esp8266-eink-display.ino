#include <SPI.h>
#include "epd4in2.h"
#include "imagedata.h"
#include "epdpaint.h"
/*

  TCL IOT script
  Tim Waizenegger (c) 2017
  ESP8266 on arduino, pusub mqtt client, eink display module

*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
extern "C" {
#include "user_interface.h"
}

#define COLORED     0
#define UNCOLORED   1

#define SLEEPTIME 50000
unsigned long previousMillis = 0;

const char* ssid = "hive_iot";
const char* password = "xxx";
const char* mq_id = "eink-display-2";
const char* mq_authtoken = "xxx";
const char* mq_serverUrl = "timsrv.de";


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


  setup_wifi();
  client.setServer(mq_serverUrl, 1883);
  client.setCallback(callback);



  Serial.println("update e-Paper display");
  //draw_status_display("Ready :)", "mqtt esp8266 eink display");

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
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  delay(500);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  draw_status_display("WIFI", "Success! Connected to WIFI");
}




void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  if (strcmp(topic, "eink2/byte-image-data") == 0) {
    
    const int buffer_len = 15100;

    int copy_length = length;
    if(length>buffer_len) copy_length = buffer_len;
    
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

    draw_status_display("MQTT", "Attempting connection to MQTT broker");
    // Attempt to connect
    if (client.connect(mq_id, mq_id, mq_authtoken)) {
      Serial.println("connected");

      draw_status_display("Start...", "Success! Connected to MQTT broker");
      // ... and resubscribe
      client.subscribe("eink2/byte-image-data");
    } else {
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

