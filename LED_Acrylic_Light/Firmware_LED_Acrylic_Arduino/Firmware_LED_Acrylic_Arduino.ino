#include <Wire.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            13

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      2
//accel unique id
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
//init neopixels
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

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


uint8_t new_state = 0;
uint8_t old_state = 0;
uint8_t show_type = 0;
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
  /* Display some basic information on this sensor */
  Serial.printf("X: %d , Y: %d\n", x, y);

  if (x <=  -150 && (new_state != old_state)) {
    show_type++;
    if (show_type > 9)
      show_type = 0;
    //startShow(show_type);
    new_state = old_state;
  }

  if(x >= 0 && (new_state == old_state)){
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
    case 4: theaterChase(pixels.Color(127, 127, 127), 50); // White
      break;
    case 5: theaterChase(pixels.Color(127,   0,   0), 50); // Red
      break;
    case 6: theaterChase(pixels.Color(  0,   0, 127), 50); // Blue
      break;
    case 7: rainbow(20);
      break;
    case 8: rainbowCycle(20);
      break;
    case 9: theaterChaseRainbow(50);
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

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i + j) & 255));
    }
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
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < pixels.numPixels(); i = i + 3) {
        pixels.setPixelColor(i + q, c);  //turn every third pixel on
      }
      pixels.show();

      delay(wait);

      for (int i = 0; i < pixels.numPixels(); i = i + 3) {
        pixels.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < pixels.numPixels(); i = i + 3) {
        pixels.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      pixels.show();

      delay(wait);

      for (int i = 0; i < pixels.numPixels(); i = i + 3) {
        pixels.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
