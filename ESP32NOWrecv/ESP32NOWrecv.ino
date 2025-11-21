#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>   // I2C

const int PIN_LED = 4;
const int PIN_SDA = 17;
const int PIN_SCL = 16;
const uint8_t I2C_ADDRESS = 0x10;

void sendI2C(uint8_t s)
{
  Wire.beginTransmission(I2C_ADDRESS);
  Wire.write(s);
  Wire.endTransmission();
}

void OnDataRecv(const esp_now_recv_info_t* esp_now_info, const uint8_t* recvData, int dataLen)
{
  sendI2C(*recvData);
}

////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);  // 消灯

  // I2C master (to Zumo)
  Wire.begin(PIN_SDA, PIN_SCL);

  // ESP-NOW
  WiFi.mode(WIFI_STA);
  if ( esp_now_init() != ESP_OK ) {
    Serial.println("[Error] Initializing ESP-NOW");
    return;
  }
  if ( esp_now_register_recv_cb(OnDataRecv) != ESP_OK ) {
    Serial.println("[Error] Failed to esp_now_register_recv_cb()");
    return;
  }
  Serial.println("[OK] ESP-NOW Start!");

  digitalWrite(PIN_LED, LOW); // 点灯
}

void loop() {
  // put your main code here, to run repeatedly:
}
