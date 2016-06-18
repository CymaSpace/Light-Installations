#define NUM_PARTICLES 14

struct Particle {
  int brightness;
  bool direction;
  int position;
};

Particle particles[NUM_PARTICLES];
bool positions[NUM_LEDS];

int randomFreePosition(int position) {
  positions[position] = false;

  bool found = false;
  while (!found) {
    position = random(0, NUM_LEDS - 1);
    if (positions[position] == false) {
      found = true;
    }
  }

  positions[position] = true;

  return position;
}

void setupIdleAnimation() {
  randomSeed(analogRead(0));

  for (int i = 0; i < NUM_LEDS; i++) {
    positions[i] = false;
  }

  for (int i = 0; i < NUM_PARTICLES; i++) {
    int brightness = random(0, 255);
    bool direction = random(0, 10) < 5 ? false : true;
    int position = randomFreePosition(0);

    particles[i] = Particle {
      brightness,
      direction,
      position
    };
  }
}

void setSignalLED(CRGB color) {
  leds_outer_values[NUM_LEDS / 2] = color;
}

void updateParticles() {
  for (int i = 0; i < NUM_PARTICLES; i++) {
    Particle* p = particles + i;

    if (p->direction) {
      p->brightness += 4;
      if (p->brightness > 255) {
        p->brightness = 255;
        p->direction = false;
      }
    } else {
      p->brightness -= 4;

      if (p->brightness <= 0) {
        int new_position = randomFreePosition(p->position);

        p->brightness = 0;
        p->direction = true;
        randomSeed(analogRead(0));
        p->position = new_position;
      }
    }
  }
}

void animateIdle() {
  updateParticles();

  for (int i = 0; i < NUM_PARTICLES; i++) {
    Particle p = particles[i];
    leds_inner_values[p.position].r = p.brightness;
    leds_inner_values[p.position].g = p.brightness;
    leds_inner_values[p.position].b = p.brightness;

    leds_outer_values[p.position].r = p.brightness;
    leds_outer_values[p.position].g = p.brightness;
    leds_outer_values[p.position].b = p.brightness;
  }
}
