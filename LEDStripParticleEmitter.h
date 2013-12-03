#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

#define MAX_PARTICLES 40 // Watch out for

typedef struct {
  float x;
  float y; 
  float z;
} Coord3D;

typedef struct {
    Coord3D velocity;
    byte redColor;
    byte greenColor;
    byte blueColor;
    boolean dimmed;
    Coord3D coord;
} Particle;

class ParticleEmitter {

 public:

  ParticleEmitter(uint16_t numPixels, uint8_t maxColor);
  ParticleEmitter(void);
  void
    begin(void);
  Particle
    updateParticle(uint16_t i, boolean respawn),
    newParticle();
  float
    stripPosition,
    maxVelocity;    
  uint16_t
    numPixels,    // Number of RGB LEDs in strip
    numParticles;
  uint8_t
    maxColor;
  bool
    respawnOnOtherSide;

 private:

  Particle
    particles[MAX_PARTICLES];
  float
    masterVelocityDeltaDirection;
};
