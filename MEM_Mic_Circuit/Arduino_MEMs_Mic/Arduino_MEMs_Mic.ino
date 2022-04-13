/* 
 * Analog mems mic demo
 * 
 * @author muchirijohn
*/


#define sample_time 50
#define mic_pin A0

uint16_t mic_adc = 0;

unsigned long int prev_t = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Analog MEMs Demo");
}

void loop() {
  findPtp();
  delay(100);
}

void findPtp() {
  uint16_t mic_max = 0;
  uint16_t mic_min = 1023;
  uint16_t mic_ptp = 0;

  for (uint8_t x = 0; x < 4; x++) {
    prev_t = millis();
    mic_ptp = 0;
    while (millis() - prev_t < sample_time) {
      mic_adc = analogRead(mic_pin);
      if (mic_adc > mic_max) {
        mic_max = mic_adc;
      } else if (mic_adc < mic_min) {
        mic_min = mic_adc;
      }
    }
    mic_ptp = mic_max - mic_min;
  }
  Serial.println("MEMS MIC: ");
  Serial.print(mic_ptp);
  Serial.println();
}
