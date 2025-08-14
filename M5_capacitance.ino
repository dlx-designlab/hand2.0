/*

 A code for measuring the Matsuhisa Lab original capacitance pressure sensor on  M5 atom S3
  
*/


const int chargePin = G5;       // On/Off output pin
const int sensePin = G6;        // Analog input pin for voltage reading
//const int dischargePin = G7;

const float threshold_voltage = 1.1;         // Threshold voltage
const float source_voltage = 3.3;            // Output voltage of ESP32 (M5 Atom)
const float resistor_value = 51000.0;       // 51k ohm or 68k ohm (Set the value to match the actual resistance.)

void setup() {
  Serial.begin(9600);
  pinMode(chargePin, OUTPUT);
  digitalWrite(chargePin, LOW);
}

void loop() {
  pinMode(sensePin, OUTPUT); // Discharge the capacitor and return to GND before each INPUT read.
  digitalWrite(sensePin, LOW);
  delay(200);
  pinMode(sensePin, INPUT);

  digitalWrite(chargePin, HIGH);
  unsigned long startTime = micros();

  while (true) {
    int adc = analogRead(sensePin);
    float voltage = adc*(3.3 / 4095.0);  // ADC conversion

    if (voltage >= threshold_voltage) break;
    if (micros() - startTime > 1000000) {  // Timeout：1 sec
      Serial.println("Timeout");
      return;
    }
  }
  unsigned long elapsed = micros() - startTime;
  digitalWrite(chargePin, LOW);  // Charging finished

 // Capacitance calculation
  float ln_part = log(1.0 - (threshold_voltage / source_voltage));
  float capacitance = -(float)elapsed / (resistor_value * ln_part);  // Farad単位
  Serial.println(capacitance * 1e5);
  delay(10);
}
