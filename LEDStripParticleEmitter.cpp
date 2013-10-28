/*
Arduino library to simulate particle emission with an RGB LED strip.
Copyright (C) Stumpware
MIT license
*/

#include "LEDStripParticleEmitter.h"
#define MAX_COLOR 255

/*****************************************************************************/

ParticleEmitter::ParticleEmitter(uint16_t n) {
    numPixels = n;
    numParticles = MAX_PARTICLES;
    maxVelocity = (random(100) / 100.0 * 0.028) + 0.004; //float(numParticles / 5000.0);
    stripPosition = 0.5;
    
    for (int i=0; i < MAX_PARTICLES; i++) {
        particles[i] = newParticle();
    }
}

particle ParticleEmitter::newParticle() {
    particle p;
    
    int8_t direction = (random(2) == 0 ? 1 : -1);
    uint8_t maxColor = MAX_COLOR * (1.0 - (random(50) / 100));
    maxColor = (random(10) > 4) ? maxColor : 0.0;

    p.velocity = ((random(89) + 10) / 100.0) * direction;    
    p.redColor = random(maxColor);
    p.greenColor = random(maxColor*0.25);
    p.blueColor = random(maxColor*0.75);    
    p.startTime = millis();
    p.startStripPosition = stripPosition;
    p.currentStripPosition = p.startStripPosition;
    p.dimmed = 0;

    return p;
}

void ParticleEmitter::begin(void) {
}

particle ParticleEmitter::updateParticle(uint16_t i, boolean respawn) {
    particle *p = &particles[i];
    
    p->currentStripPosition = p->currentStripPosition +
                              (maxVelocity * p->velocity);  // *
                              //float(millis() - p->startTime);

    p->dimmed = (random(3) == 0 ? 1 : 0);

    if (respawn) {
        if (respawnOnOtherSide) {        
            if (p->currentStripPosition < 0.0) {
                p->currentStripPosition = 1.0;            
            }
            else if (p->currentStripPosition > 1.0) {
                p->currentStripPosition = 0.0;            
            }
        }
        else {
            if (p->currentStripPosition < -1.0 || p->currentStripPosition > 2.0) {
                *p = newParticle();            
            }
        }
    }

    return *p;
}



