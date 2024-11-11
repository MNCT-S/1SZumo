#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>  // 先に書かないとArduinoIDEがハングアップ
#include <WiFi.h>
#include <Wire.h>
#include "esp_camera.h"

//#define _DEBUG
//#define _DEBUG_I2C

// Standalone
//#define SOFT_AP
//#define _SSID   "M5Camera99"
//#define _IP     199
//#define _BLE    _SSID
#ifdef SOFT_AP
//M5camera SoftAP Configration
const IPAddress ip(192,168,_IP,1);
const IPAddress subnet(255,255,255,0);
#endif

// Machine Name
#define _BLE    "M5Camera01"
// Wifi Router
#define _SSID   "Mechatro-01"
const int   channel = 11;

// AccessPoint
const char* ssid = _SSID;
const char* password = _SSID;
// I2C Setting
const int     PIN_SDA = 4;
const int     PIN_SCL = 13;
const int     PIN_LED = 2;
const uint8_t I2C_ADDRESS = 0x10;

//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
//#define CAMERA_MODEL_ESP_EYE // Has PSRAM
#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM (TimerCAM)
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
//#define CAMERA_MODEL_M5STACK_UNITCAM // No PSRAM
//#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM
#include "camera_pins.h"

void startCameraServer(); // app_httpd.cpp

#ifdef SOFT_AP
void setupWifiSoftAp()
{
  WiFi.softAP(ssid, password, channel);
  WiFi.setSleep(false);
  delay(100);
  WiFi.softAPConfig(ip,ip,subnet);
  IPAddress myIP = WiFi.softAPIP();
#ifdef _DEBUG
  Serial.println("M5camera SoftAP Mode start.");
  Serial.print("SSID:");
  Serial.println(ssid);
  Serial.print("Password:");
  Serial.println(password);
#endif
}
#endif

void setupWifiRouter()
{
  WiFi.begin(ssid, password, channel);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#ifdef _DEBUG
    Serial.print(".");
#endif    
  }
}

bool setupCamera()
{
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
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
#ifdef _DEBUG    
    Serial.printf("Camera init failed with error 0x%x", err);
#endif    
    return false;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
//    s->set_vflip(s, 1); // flip it back
    s->set_hmirror(s, 1);
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#ifdef SOFT_AP
  setupWifiSoftAp();
#else
  setupWifiRouter();
#endif

  startCameraServer();
#ifdef _DEBUG
  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
#endif

  return true;
}

void dataSend(byte s)
{
  Wire.beginTransmission(I2C_ADDRESS);
  Wire.write(s);
#ifdef _DEBUG_I2C
  byte r = Wire.endTransmission();
  Serial.print("sendData=");  Serial.print(s, HEX);  Serial.print(" ");
  Serial.print("I2C TransCode="); Serial.println(r);
#else          
  Wire.endTransmission();
#endif
}

///////////////////////////////////////////////////////////

void setup() {
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);    // power on

#ifdef _DEBUG
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
#endif

  // Setup Bluetooth (for iPad)
  Dabble.begin(_BLE);

  // Setup I2C
  Wire.begin(PIN_SDA, PIN_SCL);  // master

  // Setup Camera
  if ( !setupCamera() ) return;

  digitalWrite(PIN_LED, HIGH);   // initial check ok
}

///////////////////////////////////////////////////////////

void loop() {
  Dabble.processInput();
  // ボタンの場合
  // 下位4ビットに前(1)後(2)左(3)右(4)の4パターン|0x80
  // アナログスティックの場合
  // 上位4ビットにモータパワー（0-7）
  // 下位4ビットに進行方向15度ずつ24パターン -> 30度ずつ12パターンに
  byte s = 0;
  if ( GamePad.isUpPressed() || GamePad.isTrianglePressed() ) {
    s  = 1;
    s |= 0x80;
  }
  else if ( GamePad.isDownPressed() || GamePad.isCrossPressed() ) {
    s  = 2;
    s |= 0x80;
  }
  else if ( GamePad.isLeftPressed() || GamePad.isSquarePressed() ) {
    s  = 3;
    s |= 0x80;
  }
  else if ( GamePad.isRightPressed() || GamePad.isCirclePressed() ) {
    s  = 4;
    s |= 0x80;
  }
  else {
    int p = GamePad.getRadius(),  // 0-6 マニュアルでは7までだが7まで出ない角度があった
        a = GamePad.getAngle();   // 0-345 step15
    s = (p<<4) | ((a/30)&0x0f);
  }
  dataSend(s);
  delay(100);
}
