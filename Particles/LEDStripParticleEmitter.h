/*
Library to simulate particle emission with an RGB LED strip.
Copyright (C) P. Mark Anderson
MIT license
*/

#ifndef LEDStripParticleEmitter_h
#define LEDStripParticleEmitter_h

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

#include <Adafruit_NeoPixel.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#define MAX_PARTICLES 33
#define GOLDEN_RATIO 1.618

typedef struct {
  float x;
  float y; 
  float z;
} Coord3D;

typedef struct {
    Coord3D speed;
    uint8_t redColor;
    uint8_t greenColor;
    uint8_t blueColor;
    bool dimmed;
    Coord3D coord;
    unsigned long lastUpdatedAt;
} Particle;

class LEDStripParticleEmitter {
 public:
  LEDStripParticleEmitter(uint16_t pixelCount, uint8_t ppm, uint8_t particleCount);
  void
    updateStrip(Adafruit_NeoPixel& strip);
  Particle
    updateParticle(uint16_t i),
    newParticle();
  float
    metersToPixels(float meters),
    stripPosition,
    particleSpeedMetersPerSec,
    particleSpeedRange;
  uint16_t
    pixelCount,    // Number of RGB LEDs in strip
    particleCount;
  uint8_t
    ppm,
    maxColor;
  bool
    flicker,
    useColorSequence,
    threed;
  uint32_t
    colorSequence[3];
  uint8_t
    currentColorSequenceIndex;

 private:
  Particle
    particles[MAX_PARTICLES];
  float
    zDeltaDirection;
  unsigned long
    particlesLastUpdatedAt,
    frameLastUpdatedAt;
    
};


#endif
