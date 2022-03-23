#include <Wire.h>

float ext_temp = 0.0; //ambient temp
float ext_hum = 0.0; //ambient humidity

void setup() {
 //init wire
 wire.begin();
 //init serial, baud 115200
 Serial.begin(115200);
 //print
 Serial.println("SHT30 sensor demo code");
}

void loop() {
  //get temp and humidity
  getExtTempHum();
}

inline void getExtTempHum() {
  uint8_t data[6] = { 0 };
  uint8_t config[2] = {0x2c, 0x06};
  uint8_t x = 0;

  int temp = 0;

  Wire.beginTransmission(sht30_addr);
  Wire.write(config[0]);
  Wire.write(config[1]);
  //Wire.endTransmission();
  delay(2);
  Wire.requestFrom(sht30_addr, 6);    // request 6 byte
  while (Wire.available()) {
    data[x++] = (uint8_t)Wire.read();
  }
  Wire.endTransmission();

  temp = (data[0] * 256 + data[1]);
  ext_temp = double((-45.0 + (175.0 * (temp / 65535.0))) - 1.0);
  ext_hum = double(100.0 * ((data[3] * 256.0 + data[4]) / 65535.0));
}
