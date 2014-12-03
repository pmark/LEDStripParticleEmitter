/*
Arduino library to simulate particle emission with an RGB LED strip.
Copyright (C) P. Mark Anderson
MIT license
*/

#include "LEDStripParticleEmitter.h"
#include <stdlib.h>
#include <math.h>

#define MAX_COLOR 255
#define MAX_VELOCITY 0.015  // TODO: Set this indirectly by specifying max seconds to transit entire strip.
#define FPS 30.0
#define MILLIS_PER_FRAME (1000 / FPS)

LEDStripParticleEmitter::LEDStripParticleEmitter(uint16_t _numPixels, uint8_t _ppm, float _minTransitTimeSec, uint8_t _maxColor) {
    maxColor = fmin(MAX_COLOR, fmax(0, _maxColor));
    numPixels = _numPixels;
    ppm = _ppm;
    minTransitTimeSec = _minTransitTimeSec;
    numParticles = 12;
    
    stripPosition = 0.5;
    zDeltaDirection = 1.0;
    threed = false;
    flicker = false;
    respawnOnOtherSide = false;

    _minTransitTimeSec = fmax(0.1, _minTransitTimeSec);
    uint8_t stripLengthMeters = (_numPixels / _ppm);
    float metersPerSec = (stripLengthMeters  / _minTransitTimeSec);
    
    maxVelocity = (metersPerSec / FPS);
    maxVelocity = fmax(0.01, maxVelocity);
    maxVelocity = fmin(0.02, maxVelocity);
    
    for (int i=0; i < numParticles; i++) {
        particles[i] = newParticle();
    }
}

Particle LEDStripParticleEmitter::newParticle() {
    Particle p;
 
    p.coord.x = (random(2) == 0 ? 0.0 : 1.0); //random(67) / 100.0 + 0.33;
    p.coord.y = 0.0;
    p.coord.z = (threed ? random(100) / 100.0 : 0.0);

    int8_t direction = (p.coord.x > 0.5 ? -1 : 1);
    p.velocity.x = 1.0; //((random(77) + 33) / 100.0) * direction;
    p.velocity.y = 0.0;

    direction = (random(2) == 0 ? 1 : -1);
    p.velocity.z = (M_PI/45); //(M_PI / 180.0 * random(90) * direction);

    uint8_t prtMaxColor = maxColor * (1.0 - (random(50) / 100.0));

    p.redColor = random(prtMaxColor);
    p.greenColor = random(prtMaxColor);
    p.blueColor = random(prtMaxColor);

    p.dimmed = false;        
    return p;
}

void LEDStripParticleEmitter::begin(void) {
}

Particle LEDStripParticleEmitter::updateParticle(uint16_t i) {
  
    Particle *p = &particles[i];
    
    if (flicker) {
        p->dimmed = (random(3) == 0 ? 1 : 0);
    }

    float zScale = (1.0 - (p->coord.z * 0.9));
    p->coord.x += (maxVelocity * p->velocity.x) * zScale;
    p->coord.y += (maxVelocity * p->velocity.y) * zScale;

    // Z velocity acts as theta
    p->coord.z =1.0; // min(0.9, sin(p->velocity.z));
    p->velocity.z += M_PI/200.0 * zDeltaDirection;

    if (p->velocity.z >= M_PI/2 || p->velocity.z <= M_PI/90) {
        zDeltaDirection *= -1.0;
    }

    float maxHighVelocity = maxVelocity * GOLDEN_RATIO * 10.0; 
    float extra = 0.0; //0.5;
    
    if (respawnOnOtherSide && p->velocity.x < maxHighVelocity && p->velocity.y < maxHighVelocity) {        
        if (p->coord.x < 0.0-extra) {
            p->coord.x = 1.0;
            p->velocity.x *= GOLDEN_RATIO;
        }
        else if (p->coord.x > 1.0+extra) {
            p->coord.x = 0.0;            
            p->velocity.x *= GOLDEN_RATIO;
        }

        if (p->coord.y < 0.0-extra) {
            p->coord.y = 1.0;            
            p->velocity.y *= GOLDEN_RATIO;
        }
        else if (p->coord.y > 1.0+extra) {
            p->coord.y = 0.0;            
            p->velocity.y *= GOLDEN_RATIO;
        }
    }
    else {
        if (p->coord.x < 0.0-extra || p->coord.x > 1.0+extra ||
            p->coord.y < 0.0-extra || p->coord.y > 1.0+extra) {
            *p = newParticle();            
        }
    }

    return *p;
}


void LEDStripParticleEmitter::updateStrip(Adafruit_NeoPixel& strip) {
    unsigned long frameStartMillis = millis();

    // Draw each particle
    for (int i=0; i < numParticles; i++) {

        // Update this particle's position
        Particle prt = updateParticle(i);
  Serial.println(prt.coord.x);

        float zScale = (1.0 - prt.coord.z);
        float tailLength = 2; // 1 + abs(prt.velocity.x * 15) * zScale;
        int16_t startSlot = numPixels * prt.coord.x;
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

            if (threed) {
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
        delay(frameDelayMillis);
        strip.show();
    }
}

