#include <Adafruit_NeoPixel.h>
#include "LEDStripParticleEmitter.h"

#define PIXEL_COUNT 60
#define PARTICLE_COUNT 12

#define FPS 30
#define LED_STRIP_PIN 11
#define MAX_COLOR 255   // max 255
#define MIN_COLOR 3
#define MAX_VELOCITY 0.015
#define MILLIS_PER_FRAME (1000 / FPS)

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, LED_STRIP_PIN, NEO_RGB + NEO_KHZ800);
ParticleEmitter emitter = ParticleEmitter(PIXEL_COUNT, MAX_COLOR);

unsigned long frameStartMillis = 0;
uint8_t randomRedColor = 0;
uint8_t randomGreenColor = 0;
uint8_t randomBlueColor = 0;

void setCoordColor(Coord3D coord, uint32_t color);

void setup() {
  strip.begin();
  strip.show();
  
  emitter.respawnOnOtherSide = false;
  emitter.threed = true;
  emitter.numParticles = PARTICLE_COUNT;
  emitter.maxVelocity = MAX_VELOCITY;
  
  emitterTransitStartMillis = millis();
}

void loop() {  
  particles();
}

void particles() {
    unsigned long frameStartMillis = millis();
    emitter.stripPosition = 0.0;

    // Draw each particle
    for (int i=0; i < emitter.numParticles; i++) {

        // Update this particle's position
        Particle prt = emitter.updateParticle(i);

        float zScale = (1.0 - prt.coord.z);
        uint8_t tailLength = 1 + abs(prt.velocity.x * 15) * zScale;
        int16_t startSlot = emitter.numPixels * prt.coord.x;
        int16_t currentSlot = startSlot;
        int16_t oldSlot = currentSlot;

        // Draw the particle and its tail
        // High velocity particles have longer tails
        for (int ii=0; ii < tailLength; ii++) {

            // Taper the tail fade
            float colorScale = ((tailLength - (ii * GOLDEN_RATIO)) / tailLength);

            if (ii == 0 && prt.dimmed) {
            // Flicker the first particle
                colorScale *= (random(50) / 100) + 0.05;
            }      

            if (emitter.threed) {
                colorScale *= zScale;
            }

            if (colorScale < 0.05) {
                colorScale = 0.0;
            }

            // Draw particle
            strip.setPixelColor(currentSlot, 
                                strip.Color(prt.redColor * colorScale, 
                                            prt.greenColor * colorScale, 
                                            prt.blueColor * colorScale));

            oldSlot = currentSlot;
            currentSlot = startSlot + ((ii+1) * (prt.velocity.x > 0 ? -1 : 1));
        }

        //Terminate the tail
        strip.setPixelColor(oldSlot, strip.Color(0, 0, 0));
    }

    uint16_t frameElapsedMillis = millis() - frameStartMillis;
    uint16_t frameDelayMillis = (MILLIS_PER_FRAME - frameElapsedMillis);

    if (frameDelayMillis > 0.0) {
        Serial.println(frameDelayMillis);
        delay(frameDelayMillis);
        strip.show();
    }
}

void setCoordColor(Coord3D coord, uint32_t color) {
    strip.setPixelColor(coord.x * emitter.numPixels, color); 
}
