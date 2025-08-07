# Hands 2.0 README

Treasure hunting project 2025 in collaboration with Matsuhisa Lab, to make invisible hand-wearable device for thumb wrestling

ONGOING

## 📁 Software

Each script runs indivisually

- `standalone_thumbwrestling.ino`– Arduino sketch for thumb wrestling.
- `M5_capacitance.ino`– Arduino sketch to measure a capasitance value of Matsuhisa Lab's original pressure sensor
- `wired_thumbwrestlingjugde.ino`- Arduino sketch for the first prototype to judge a thumb wrestling game using a Arduino as a main brain. we already 進化させた the concept to having two stand-alone device for the game.

## ⚙️ Hardware

- M5 Atom S3 for `standalone_thumbwrestling.ino` and `M5_capacitance.ino`
- Arduino UNO for `wired_thumbwrestlingjugde.ino`

## Wiring と 動かし方

### standalone_thumbwrestling.ino

以下のようにつないでください
![SA_wiring](SA_wiring.jpg)

**Operation**
1. 各プレイヤーが手首につけているタッチセンサのキャパシタンスの変化によってプレイヤー同士が手を組んだことを表示します. ゲーム中にプレイヤーが手を離せばゲームはリセットされます。
2. 親指の爪の上の圧力センサが押されているかどうか、親指の腹のタッチせんさ（今後圧力センサに変更予定）が触れているかどうかをディスプレイに表示する
3. 上記a,b,cの情報を相手に送り、受け取った情報を元に勝敗を判定する。

**Threshold config**
declared at the beginning
- 手首のタッチセンサ
- 圧力センサ
- 親指のタッチセンサ
- 勝つためのカウント: 5s

### M5_capacitance.ino

以下のようにつないでください
![cap_wiring](cap_wiring.jpg)

resiterの選び方

### wired_thumbwrestlingjugde.ino

以下のようにつないでください
![wired_wiring](wired_wiring.jpg)


**Operation**
1. 各プレイヤーが手首につけているタッチセンサのキャパシタンスの変化によってプレイヤー同士が手を組んだことを表示します. ゲーム中にプレイヤーが手を離せばゲームはリセットされます。
2. 親指の爪の上の圧力センサが押されているかどうか、親指の腹のタッチせんさ（今後圧力センサに変更予定）が触れているかどうかを判定する
3. 一つのarduinoが、読み取った情報を元に勝敗を判定する。
