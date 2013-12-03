/*
Arduino library to simulate particle emission with an RGB LED strip.
Copyright (C) Stumpware
MIT license
*/

#include "LEDStripParticleEmitter.h"
#define MAX_COLOR 255

/*****************************************************************************/

ParticleEmitter::ParticleEmitter(uint16_t _numPixels, uint8_t _maxColor) {
    maxColor = fmin(MAX_COLOR, fmax(0, _maxColor));
    numPixels = _numPixels;
    numParticles = MAX_PARTICLES;
    maxVelocity = (random(100) / 100.0 * 0.028) + 0.004; //float(numParticles / 5000.0);
    stripPosition = 0.5;
    masterVelocityDeltaDirection = 1;
    
    for (int i=0; i < numParticles; i++) {
        particles[i] = newParticle();
    }
}

Particle ParticleEmitter::newParticle() {
    Particle p;
    
    p.coord.x = random(20) / 100.0 + 0.4;
    p.coord.y = random(20) / 100.0 + 0.4;
    p.coord.z = 0;

    int8_t direction = p.coord.x > 0.5 ? 1 : -1;
    // int8_t direction = (random(2) == 0 ? 1 : -1);    
    // p.velocity.x = ((random(89) + 10) / 100.0) * direction;
    p.velocity.x = ((random(89) + 10) / 100.0) * direction;
    direction = (random(2) == 0 ? 1 : -1);    
    p.velocity.y = ((random(89) + 10) / 100.0) * direction;

    uint8_t prtMaxColor = maxColor * (1.0 - (random(50) / 100.0));
    // prtMaxColor = (random(10) > 4) ? prtMaxColor : 0.0;
    // uint8_t prtMaxColor = 0;
    p.redColor = random(prtMaxColor);
    p.greenColor = random(prtMaxColor);
    p.blueColor = random(prtMaxColor);    
    p.dimmed = false;

    masterVelocityDeltaDirection = !masterVelocityDeltaDirection;
    return p;
}

void ParticleEmitter::begin(void) {
}

Particle ParticleEmitter::updateParticle(uint16_t i, boolean respawn) {
    Particle *p = &particles[i];    
    p->coord.x = p->coord.x + (maxVelocity * p->velocity.x);
    p->coord.y = p->coord.y + (maxVelocity * p->velocity.y);
    p->dimmed = (random(3) == 0 ? 1 : 0);

    p->velocity.x += maxVelocity  * masterVelocityDeltaDirection; // * (random(10.0) / 10.0 + 0.1);
    p->velocity.y += maxVelocity  * masterVelocityDeltaDirection; // * (random(10.0) / 10.0 + 0.1);

    if (fabs(p->velocity.x) > 3.0 || 
        fabs(p->velocity.y) > 3.0) {
        masterVelocityDeltaDirection *= -1.0;
    }

    if (respawn) {
        if (respawnOnOtherSide) {        
            if (p->coord.x < 0.0) {
                p->coord.x = 1.0;            
            }
            else if (p->coord.x > 1.0) {
                p->coord.x = 0.0;            
            }

            if (p->coord.y < 0.0) {
                p->coord.y = 1.0;            
            }
            else if (p->coord.y > 1.0) {
                p->coord.y = 0.0;            
            }
        }
        else {
            if (p->coord.x < 0.0 || p->coord.x > 1.0 ||
                p->coord.y < 0.0 || p->coord.y > 1.0) {
                *p = newParticle();            
            }
        }
    }

    return *p;
}



