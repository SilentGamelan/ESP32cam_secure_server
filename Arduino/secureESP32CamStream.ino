#include "esp_camera.h"
#include <WiFi.h>
//#include <ArduinoWebsockets.h>
#include <Websockets2_Generic.h>
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

// !! SET WIFI CREDENTIALS HERE
const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* websocket_server_host = "wss://192.168.1.80";
// set this var if using a signed cert
const char* ssl_ca_cert = nullptr;
const int INITIAL_WIFI_CONNECTION_TIMEOUT = 10;
const bool USE_INSECURE_MODE = true;
const bool SERIAL_PRINT_MESSAGES = true;
//using namespace websockets;
using namespace websockets2_generic;

WebsocketsClient client;
bool isWebSocketConnected;

void onEventsCallback(WebsocketsEvent event, String data){
 /**
  if(event == WebsocketsEvent::ConnectionOpened){
    Serial.println("Connection Opened");
    isWebSocketConnected = true;
  }else if(event == WebsocketsEvent::ConnectionClosed){
    Serial.println("Connection Closed");
    isWebSocketConnected = false;
    webSocketConnect();
  }
**/
  switch (data)
  {
  case WebsocketsEvent::ConnectionOpened:
    Serial.println("Connection Opened");
    isWebSocketConnected = true;
    break;
  case WebsocketsEvent::ConnectionClosed:
    Serial.println("Connection Closed");
    isWebSocketConnected = false;
    break;
  case WebsSocketsEvent::GotPing:
    Serial.println("Ping!");
    break;
  case WebSocketsEvent::GotPong:
    Serial.println("Pong!");
    break;
  default:
    Serial.println("Unrecognised Websocket Event");
    break;
  }
}

void onMessageCallback(WebsocketsMessage msg) {
  Serial.print("Received Message of type: " + msg.type);
  
 
    switch (msg.type) {
      case MESSAGEType::Text:  
        Serial.print("Text Message");
        if(SERIAL_PRINT_MESSAGES) {
          Serial.print("---STARTOFMESSAGE--");
          Serial.print(msg.data());
          Serial.print("---ENDOFMESSAGE---");
        }
        break;
      case MESSAGEType::Binary:
        Serial.print("Binary Message");
        break;
      default:
        Serial.print("Not text or binary type");
        break;
    }
  } else {
    Serial.print("Message logging is DISABLED");
  }
}

void setup() {
  isWebSocketConnected = false;
  
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting ESP32Cam Websockets client (Insecure mode enabled");
  Serial.println("Detected board: " + String(ARDUINO_BOARD));
  Serial.println("WebSockets2_Generic library version: " + WEBSOCKETS_2_GENERIC_VERSION);
  Serial.println("\n");

  Serial.println("Initialising board pins...");
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_VGA;
  config.jpeg_quality = 40;
  config.fb_count = 2;

  // camera init
  Serial.println("Initalizing Camera");
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
 

 // consider using interrupts and sleep mode to save power - periodically wake back up to try again?    
  WiFi.begin(ssid, password);
  
  while(Wifi.status() != WL_CONNECTED) {
    Serial.println("Attempting to connect to WIFI");
    for(int i = 0; i < INITIAL_WIFI_CONNECTION_TIMEOUT && WIFi.status() != WL_CONNECTED; i++) {
    Serial.print(".");
    delay(1000);
    }
    if(i == INITIAL_WIFI_CONNECTION_TIMEOUT) {
      Serial.println("");
     // Serial.println("WiFi still not connected ")
    }
  } 
  
  Serial.println("WIFI CONNECTED!")
 /**
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("");
    Serial.println("!! Wifi Not Connected after 10 seconds")
  } else {
    Serial.println("");
    Serial.println("WiFi connected");
**/
    client.onEvent(onEventsCallback);
    client.onMessage(onMessageCallback);

  if(USE_INSECURE_MODE) {
    client.setInsecure();
  } else {
    if(!ssl_ca_cert) {
      Serial.print("!! SECURE MODE ERROR: SSL Certificate Not set!!")
    } else {
      client.setCACert(ssl_ca_cert)
    }
  }
    webSocketConnect();
  

  
}

void webSocketConnect(){
  int websocketConnectionAttempt = 0;
  Serial.println("Attempting to establish websocket connection");
   while(!client.connect(websocket_server_host)){
    delay(500);
    Serial.println(".");
    websocketConnectionAttempt++:
    if(websocke tConnectionAttempt == 10) {
      websocketConnectionAttempt = 0;
      Serial.print("Reattempting websocket Connection");
    }
  }
  Serial.println(" Websocket Connected!");
}

void loop() {

  if(client.available()){
    client.poll();
  }
  
  if(!isWebSocketConnected) return;
  
  camera_fb_t *fb = esp_camera_fb_get();
  if(!fb){
    Serial.println("Camera capture failed");
    esp_camera_fb_return(fb);
    return;
  }

  if(fb->format != PIXFORMAT_JPEG){
    Serial.println("Non-JPEG data not implemented");
    return;
  }
  
  // !! NEED TO MANUALLY SET THE CAM ID HERE
  fb->buf[12] = 0x01; //FIRST CAM
  //fb->buf[12] = 0x02; //SECOND CAM

  client.sendBinary((const char*) fb->buf, fb->len);
  esp_camera_fb_return(fb);
}
