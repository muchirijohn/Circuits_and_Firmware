/*
   LED Acrylic Light firmware

   Hardware: ESP8266 + ADXL345 + WS2812B LEDs

   @Author Muchirijohn
*/

#include <Wire.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

//pin connected to the ws2812/neopixel leds
#define PIN            13

//number of pixels connected
#define NUMPIXELS      2
//accel unique id
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
//init neopixels
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

uint8_t new_state = 0;
uint8_t old_state = 0;
uint8_t show_type = 0;
uint8_t rb_cycles = 0;

//display sensor details
void displaySensorDetails(void)
{
  sensor_t sensor;
  accel.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" m/s^2");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" m/s^2");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" m/s^2");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

//setup
void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  pixels.begin();
  //init accel sensor
  if (!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while (1) {
      delay(1);
    }
  }
  //set accel range
  accel.setRange(ADXL345_RANGE_16_G);
  //display sensro details
  displaySensorDetails();
  //init
  startShow(show_type);
  old_state = 1;
}

//loop
void loop() {
  int x = accel.getX();
  int y = accel.getY();
  /* Display x and y values */
  //Serial.printf("X: %d , Y: %d\n", x, y);

  if (x <=  -150 && (new_state != old_state)) {
    show_type++;
    if (show_type > 7)
      show_type = 0;
    new_state = old_state;
  }

  if (x >= 0 && (new_state == old_state)) {
    new_state = !old_state;
  }

  startShow(show_type);
  delay(100);
}

void startShow(int i) {
  switch (i) {
    case 0: colorWipe(pixels.Color(0, 0, 0), 50);    // Black/off
      break;
    case 1: colorWipe(pixels.Color(255, 0, 0), 50);  // Red
      break;
    case 2: colorWipe(pixels.Color(0, 255, 0), 50);  // Green
      break;
    case 3: colorWipe(pixels.Color(0, 0, 255), 50);  // Blue
      break;
    case 4: colorWipe(pixels.Color(255, 255, 0), 50);  // Blue
      break;
    case 5: colorWipe(pixels.Color(255, 0, 255), 50); // Red
      break;
    case 6: colorWipe(pixels.Color(0, 255, 255), 50); // Blue
      break;
    case 7: rainbowCycle(20);
      break;
    default:
      break;
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, c);
    pixels.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    int x = accel.getX();
    if (x < -100 && new_state == old_state) {
      if (rb_cycles >= 3) {
        new_state = !old_state;
        rb_cycles = 0;
        break;
      }
      rb_cycles++;
      break;
    }
    delay(wait);
  }
}
