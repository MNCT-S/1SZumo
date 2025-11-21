#include <esp_now.h>
#include <WiFi.h>
#include <math.h>

////////////////////////////////////////////////////
// 先に受信側(Zumo)の電源を入れておくこと
////////////////////////////////////////////////////

// Analog input port
const int adX = 32;   // 向きを変更
const int adY = 33;

// Zumoに接続する UNIT-CAM の MAC アドレス
uint8_t targetAddress[] = {0xe0, 0xe2, 0xe6, 0x9b, 0x8b, 0xac};
esp_now_peer_info_t peerInfo;

/*
void OnDataSend(const uint8_t* mac_addr, esp_now_send_status_t status)
{
  if ( status != ESP_NOW_SEND_SUCCESS ) {
    Serial.println("[Error] send status");
  }
}
*/
////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);

  // Setup JOY-Stick
  pinMode(adX, ANALOG);
  pinMode(adY, ANALOG);
  analogSetAttenuation(ADC_11db);

  // ESP-NOW
  WiFi.mode(WIFI_STA);
  if ( esp_now_init() != ESP_OK ) {
    Serial.println("[Error] Initializing ESP-NOW");
    return;
  }
  memcpy(peerInfo.peer_addr, targetAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if ( esp_now_add_peer(&peerInfo) != ESP_OK ) {
    Serial.println("[Error] Failed to add peer");
    return;
  }

//  esp_now_register_send_cb(OnDataSend);
}

void loop() {
  // Dabbleアプリの挙動をエミュレート
  // 12bit -> 4bit 補正
  int x = analogRead(adX) >> 8;
  int y = analogRead(adY) >> 8;
  // center
  float x2 = x - 7.5;
  float y2 = y - 7.5;
  if ( fabs(x2) <= 1.5 ) x2 = 0;
  if ( fabs(y2) <= 1.5 ) y2 = 0;

  // GamePad.getRadius()
  int r = sqrt(x2*x2+y2*y2);
  // GamePad.getAngle()
  float a = -atan2(y2, x2)*180.0/M_PI;
  if ( a < 0 ) a+=360.0;
  int a30 = a / 30;

  uint8_t send = (r<<4) | (a30&0x0f);
  esp_now_send(targetAddress, &send, 1);  // send to 1byte

  delay(100);
}
