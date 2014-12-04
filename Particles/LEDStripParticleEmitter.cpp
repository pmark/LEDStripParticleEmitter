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

template <typename T,unsigned S>
inline unsigned arraysize(const T (&v)[S]) { return S; }

LEDStripParticleEmitter::LEDStripParticleEmitter(uint16_t _pixelCount, uint8_t _ppm, uint8_t _particleCount) {
    pixelCount = _pixelCount;
    ppm = _ppm;
    particleSpeedMetersPerSec = 0.5;
    particleSpeedRange = 0.0;
    stripPosition = 0.5;
    zDeltaDirection = 1.0;
    threed = false;
    flicker = false;
    frameLastUpdatedAt = 0;
    particlesLastUpdatedAt = 0;
    maxColor = MAX_COLOR;
    
    numParticles = _particleCount;
    if (numParticles > MAX_PARTICLES) {
      numParticles = MAX_PARTICLES;
    }

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


    // compute particle speed
    particleSpeedMetersPerSec = fmax(0.001, particleSpeedMetersPerSec);
    ppm = (ppm > 0 ? ppm : 1);
    float stripLengthMeters = float(pixelCount) / float(ppm);
    stripLengthMeters = (stripLengthMeters > 0.0 ? stripLengthMeters : 1.0);

    float velocityUnitsPerSec = (particleSpeedMetersPerSec / float(stripLengthMeters));
    velocityUnitsPerSec = (velocityUnitsPerSec - (particleSpeedRange / 2.0)) + (random(100.0) / 100.0 * particleSpeedRange);
    velocityUnitsPerSec = fmax(0.0001, velocityUnitsPerSec);

    p.speed.x = (velocityUnitsPerSec * direction) / 1000.0;
    p.speed.y = 0.0;

    direction = (random(2) == 0 ? 1 : -1);
    p.speed.z = 0.0; //(M_PI/45); //(M_PI / 180.0 * random(90) * direction);

    maxColor = fmin(MAX_COLOR, fmax(0, maxColor));
    uint8_t prtMaxColor = maxColor * (1.0 - (random(66) / 100.0));

    p.redColor = random(prtMaxColor);
    p.greenColor = random(prtMaxColor);
    p.blueColor = random(prtMaxColor);

    p.dimmed = false;        
    return p;
}

Particle LEDStripParticleEmitter::updateParticle(uint16_t i) {
    Particle *p = &particles[i];
    
    if (flicker) {
        p->dimmed = (random(3) == 0 ? 1 : 0);
    }

    // Z speed acts as theta
    p->coord.z = (threed ? 0.9*sin(p->speed.z) : 0.0);
    p->speed.z += (M_PI/500.0);

    if (p->speed.z >= M_PI) {
        p->speed.z = 0.0;
    }

    float zScale = (1.0 - (p->coord.z * 0.9));
    unsigned long now = millis();
    unsigned long millisSinceLastUpdate = (now - particlesLastUpdatedAt);
    p->coord.x += (float(millisSinceLastUpdate) * p->speed.x) * zScale;
    p->coord.y += (millisSinceLastUpdate * p->speed.y) * zScale;

    float extra = 0.25;
    
    if (p->coord.x < 0.0-extra || p->coord.x > 1.0+extra ||
        p->coord.y < 0.0-extra || p->coord.y > 1.0+extra) {
//        delete p;
        *p = newParticle();            
    }

    return *p;
}

void LEDStripParticleEmitter::updateStrip(Adafruit_NeoPixel& strip) {
    if (frameLastUpdatedAt == 0) {
        frameLastUpdatedAt = millis();
    }

    if (particlesLastUpdatedAt == 0) {
        particlesLastUpdatedAt = millis();
    }

    // Draw each particle
    for (int i=0; i < numParticles; i++) {

        // Update this particle's position
        Particle prt = updateParticle(i);
        
        float zScale = (1.0 - prt.coord.z);
        float tailLength = 1 + abs(prt.speed.x * 5000) * zScale;
        int16_t startSlot = pixelCount * prt.coord.x;
        int16_t currentSlot = startSlot;
        int16_t oldSlot = currentSlot;

        // Draw the particle and its tail
        // High speed particles have longer tails
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
            currentSlot = startSlot + ((ii+1) * (prt.speed.x > 0 ? -1 : 1));
        }

        //Terminate the tail
        strip.setPixelColor(oldSlot, strip.Color(0, 0, 0));
    }

    particlesLastUpdatedAt = millis();
    delay(1);
    
    if ((millis() - frameLastUpdatedAt) >= MILLIS_PER_FRAME) {
        strip.show();
        frameLastUpdatedAt = millis();        
    }
}

