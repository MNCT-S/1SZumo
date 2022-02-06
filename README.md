# 1S メカトロニクス演習 II

## ESP32 + OV2640 について

- ESP32 は，`PSRAM` 搭載の物が必要
- Arduino を使用
- Websocket を使用

### 使用する（外部）ライブラリ

- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)

- [AsyncTCP](https://github.com/me-no-dev/AsyncTCP) : ESPAsyncWebServer が使用

### SPIFFS の利用

- [arduino-esp32fs-plugin](https://github.com/me-no-dev/arduino-esp32fs-plugin) : `data` ディレクトリをアップロード

## 参考

- [AsyncWebCamera.cpp](https://gist.github.com/me-no-dev/d34fba51a8f059ac559bf62002e61aa3)

- [bobboteck/JoyStick](https://github.com/bobboteck/JoyStick) : Javascript の JoyStick ライブラリ
