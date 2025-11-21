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

> [!CAUTION]
> esp32 のバージョン 2.0.3 でしか動きません

- [Dabble-ESP32 library](https://thestempedia.com/download/28488/) （直リンク）  
Arduino IDE で zip形式のライブラリをインストール．zipのままArduinoへライブラリインポート

|Router|Channel|所属番号|
|----|:--:|----|
|Mechatro-01|11|01, 02, 03, 04, 05, 06, 07, 08|
|Mechatro-02|06|09, 10, 11, 12, 13, 14, 15, 16|
|Mechatro-03|01|17, 18, 19, 20, 21, 22, 23|

チャンネルはルータ側で手動設定し，それに合わせている．  
IPアドレスは 192.168.11.(100+番号) とする．

## MarioSounds

国民的ゲーム音を鳴らします（小ネタ）  
ButtonA：始まりの音  
ButtonC：終わりの音  


## PixyPet

新旧どちらでも使えます．  
参考URL https://k-magara.github.io/zumo/32u4_de_pixypet.html  
![イメージ図](/img/PixyPet.jpg)


## PS2RC

PixyPet先導用．Zumoに書き込むだけでVS-C1とリンクできます．

- PS2RC 左スティック上下で前進後退，右スティック左右で方向
- PS2RC_LR 左右の上下で左右モータの前進後退

参考URL https://k-magara.github.io/zumo/vsc1.html  
[Zumo32U4でサーボの制御](https://k-magara.github.io/zumo/servo.html) をしておかないとコンパイルできません．  
![イメージ図](/img/VSC1.jpg)


## ZumoXBee (SendXBee + RecvXBee)

PixyPet先導用．

- SendXBeeは，ボードを Arduino UNO に変更して書き込み
- Ver2.0 アナログスティック化
- Ver3.0 MakerUNO (Sound)

参考URL https://k-magara.github.io/zumo/xbee.html  
![イメージ図](/img/ZumoXBee.jpg)


## BLEserial3 (BLEjoy)

PixyPet先導用．RaspberryPI 3B+ と PS4コントローラでBluetooth通信  

- Python 3.9.2  
- pygame 1.9.6  
- bleak 0.19.5  

で動作確認．

### RaspberryPI側の事前設定
- RaspberryPI/BLEjoy.pyを適当なフォルダに保存  
- RaspberryPI/pswatch.pyを自動起動に設定  
  たとえば ~/.config/lxsession/LXDE-pi/autostart の末尾に `@python /home/pi/python/pswatch.py` を追記  

### 動かし方
- ZumoにBLEjoy.inoを書き込み
- RaspberryPI起動（電源接続のみで良いがOSが起動する頃合いまで待つ）
- Zumoの電源が入っている状態で PS4コントローラのHOMEボタンを押す（RaspberryPIと接続，PS4のLEDが薄青点灯）
- 自動的にBLEjoy.pyが起動されZumoと接続される（BLEserial3の赤LEDが点滅から点灯に変わる）
- PS4コントローラの OPTIONS+X ボタンでRaspberryPIがシャットダウンするように書いてあるので，
RaspberryPIへのリモート接続やキーボード・モニタ等の設備は不要．
SDアクセスLEDが消灯したら電源コンセントを抜けばよい．
（電源ぶち抜きするとSDカードが壊れます）

Windowsでも動いていたが，ライブラリのアップデートで動かなくなった

参考URL https://k-magara.github.io/zumo/ble.html , https://k-magara.github.io/zumo/PS4.html  
![イメージ図](/img/BLEserial3.jpg)


## ESP-Now (ESPNOWsend + ESPNOWrecv + ZumoMario_iPad)

> [!CAUTION]
> esp32 のバージョン 3.3.3 でコードを書いたので，上の M5mario_OV3660_iPad(esp 2.0.3) とは別環境でコンパイルする必要があります．

コントローラアプリ Dabble をエミュレートして送信プログラムを書いたので，
Zumoへの書き込みは ZumoMario_iPad でOK.  
注意点として，送信側から受信側のMacアドレスに決め打ちで接続しに行くので，受信側のMacアドレスを事前に調べておいてください．

![イメージ図](/img/ESP32NOW.jpg)


## （番外）倒立振子とライントレース (BalancingKAI, LineFollowerW[S])

### BalancingKAI（写真右）

- Zumoスケッチ例 Balancing の改良版．LCDコネクタをむしり取られたZumo筐体の活用として．
- 水平状態で電源ON，黄色LED消灯後車体を垂直に起こしてAボタン押すと倒立状態を維持．少しバランス悪いです．

### LineFollowerW[S]（写真左）

- Zumoスケッチ例 LineFollower の黒地に白線版．ウチのLEGO用コースに最適化．
- Sなし：Zumo32U4版，Sあり：ZumoShield版（ボードはArduinoLeonardo）

![イメージ図](/img/bangai.jpg)
