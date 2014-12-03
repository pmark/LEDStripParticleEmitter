#include <Adafruit_NeoPixel.h>
#include "LEDStripParticleEmitter.h"

#define PIXEL_COUNT 32

#define LED_STRIP_PIN 11
#define MAX_COLOR 255   // max 255
#define PIXELS_PER_METER 30
#define MIN_TRANSIT_TIME_SEC 10.0

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, LED_STRIP_PIN, NEO_RGB + NEO_KHZ800);
LEDStripParticleEmitter emitter = LEDStripParticleEmitter(PIXEL_COUNT, PIXELS_PER_METER, MIN_TRANSIT_TIME_SEC, MAX_COLOR);

void setup() {
  Serial.begin(9600);
  
  strip.begin();
  strip.show();
  
  emitter.numParticles = 1;
//  emitter.threed = true;
//  emitter.respawnOnOtherSide = false;
//  emitter.flicker = false;

//  TODO: Make max particle velocity be time in seconds to transit entire strip
//  emitter.maxVelocity = 0.015;   // UNO: 0.015 

}

void loop() {  
  emitter.updateStrip(strip);
}

