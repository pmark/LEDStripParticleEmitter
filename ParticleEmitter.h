#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

#define MAX_PARTICLES 30

struct particle {
    float velocity;
    uint8_t redColor;
    uint8_t greenColor;
    uint8_t blueColor;
    uint8_t dimmed;
    unsigned long int startTime;
    float startStripPosition;
    float currentStripPosition;
};

class ParticleEmitter {

 public:

  ParticleEmitter(uint16_t n);
  ParticleEmitter(void);
  void
    begin(void);
  particle
    updateParticle(uint16_t i, boolean respawn),
    newParticle();
  float
    stripPosition,
    maxVelocity;
  uint16_t
    numPixels,    // Number of RGB LEDs in strip
    numParticles;

 private:

  particle
    particles[MAX_PARTICLES];
};
