#include <Adafruit_NeoPixel.h>
#include "LEDStripParticleEmitter.h"

#define LED_STRIP_PIN 11
#define PIXEL_COUNT 32
#define PIXELS_PER_METER 30
#define PARTICLE_COUNT 9
#define MAX_COLOR 255

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, LED_STRIP_PIN, NEO_RGB + NEO_KHZ800);
LEDStripParticleEmitter emitter = LEDStripParticleEmitter(PIXEL_COUNT, PIXELS_PER_METER, PARTICLE_COUNT);

void setup() {
  Serial.begin(9600);
  strip.begin();
  // The speed of each particle varies by plus or minus half of the range value.
  emitter.particleSpeedMetersPerSec = emitter.particleSpeedRange = 1.0;
  emitter.maxColor = MAX_COLOR;
  emitter.threed = false;
  
  // Enter up to 3 colors to sequence, 
  // or let the emitter choose random colors 
//  emitter.colorSequence[0] = strip.Color(255, 0, 0);
//  emitter.colorSequence[1] = strip.Color(0, 255, 0);
//  emitter.colorSequence[2] = strip.Color(255, 255, 255);
}

void loop() {  
  emitter.updateStrip(strip);
}

