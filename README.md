# 1S メカトロニクス演習 II

## JoyCamera + ZumoJoyCamera

### ESP32 + OV2640 について

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
- [Robot Joystick to Differential Steering](https://www.impulseadventure.com/elec/robot-differential-steering.html)

## M5mario_OV3660 + ZumoMario + PythonMario.py

PCに接続したゲームコントローラでZumoを動かす一式

## M5mario_OV3660_iPad + ZumoMario_iPad

DabbleアプリでZumoを動かす一式

- [Dabble-ESP32 library](https://thestempedia.com/download/24469/)  
Arduino IDE で zip形式のライブラリをインストール


## MarioSounds

国民的ゲーム音を鳴らします（小ネタ）  
ButtonA：始まりの音  
ButtonC：終わりの音  