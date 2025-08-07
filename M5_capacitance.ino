// M5atomで自作コンデンサの静電容量を測る

const int chargePin = G5;       // 充電用onoff出力ピン
const int sensePin = G6;        // 電圧読み取り用アナログ入力ピン
//const int dischargePin = G7;

const float threshold_voltage = 1.1;         // しきい値電圧（例：1.1V）?
const float source_voltage = 3.3;            // ESP32の出力電圧
const float resistor_value = 51000.0;       // 10kΩ

void setup() {
  Serial.begin(9600);
  pinMode(chargePin, OUTPUT);
  digitalWrite(chargePin, LOW);
}

void loop() {
  pinMode(sensePin, OUTPUT); //毎回放電してGNDに戻してからINPUT読み取る
  digitalWrite(sensePin, LOW);
  delay(200);
  pinMode(sensePin, INPUT);

  digitalWrite(chargePin, HIGH);
  unsigned long startTime = micros();

  while (true) {
    int adc = analogRead(sensePin);
    float voltage = adc*(3.3 / 4095.0);  // ESP32 ADC換算 * 

    if (voltage >= threshold_voltage) break;
    if (micros() - startTime > 1000000) {  // タイムアウト：1秒
      Serial.println("Timeout");
      return;
    }
  }
  unsigned long elapsed = micros() - startTime;
  digitalWrite(chargePin, LOW);  // 充電終了

  // 静電容量計算
  float ln_part = log(1.0 - (threshold_voltage / source_voltage));
  float capacitance = -(float)elapsed / (resistor_value * ln_part);  // Farad単位
  Serial.println(capacitance * 1e5);
  delay(10);
}
