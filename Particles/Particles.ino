#include <Adafruit_NeoPixel.h>
#include "LEDStripParticleEmitter.h"

#define LED_STRIP_PIN 11
#define PIXEL_COUNT 32
#define PIXELS_PER_METER 30
#define PARTICLE_COUNT 3

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
  strip.show();
  
  emitter.particleSpeedMetersPerSec = 2.0;

  // particleSpeedRange is the range of allowed random values for a particle’s speed.
  // The default value is 0.0. If non-zero, the speed of each particle is randomly determined and may vary by plus or minus half of the range value.
  emitter.particleSpeedRange = 0.66;
  emitter.maxColor = 255;
  emitter.threed = false;

//  emitter.flicker = false;

}

void loop() {  
  emitter.updateStrip(strip);
}

