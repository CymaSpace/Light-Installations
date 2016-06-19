#define NUM_PARTICLES 14
#define IDLE_MIC_ROTATIONS_PER_SEC 1
#define IDLE_AMPLITUDE ((MIN_AMPLITUDE + MAX_AMPLITUDE) / 3)
#define FADE_TIME 3

struct Particle {
  int brightness;
  bool direction;
  int position;
};

Particle particles[NUM_PARTICLES];
bool positions[NUM_LEDS];
float fade_timer = FADE_TIME;

bool shouldIdle() {
  for (int i = 0; i < SOUND_BUFFER_LENGTH; i++) {
    if (sound_buffer[i] > IDLE_AMPLITUDE) {
      return false;
    }
  }
  return true;
}

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

  fade_timer = FADE_TIME;

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

  int val = 0;
  for (int i = 0; i < NUM_MIC_LEDS; i++) {
    mic_leds_val[i] = val;
    val += 255 / NUM_MIC_LEDS;
    val %= 255;
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

  fade_timer -= ANIMATE_SECS_PER_TICK;
  float dimmer = (FADE_TIME - fade_timer) / FADE_TIME;
  dimmer = min(dimmer, 1.0);

  // Blank all LEDs
  for (int i = 0; i < NUM_LEDS; i++) {
    leds_inner_values[i].r = 0;
    leds_inner_values[i].g = 0;
    leds_inner_values[i].b = 0;

    leds_outer_values[i].r = 0;
    leds_outer_values[i].g = 0;
    leds_outer_values[i].b = 0;
  }

  // Apply particles to LEDs
  for (int i = 0; i < NUM_PARTICLES; i++) {
    Particle p = particles[i];

    int brightness = p.brightness * dimmer;

    leds_inner_values[p.position].r = brightness;
    leds_inner_values[p.position].g = brightness;
    leds_inner_values[p.position].b = brightness;

    leds_outer_values[p.position].r = brightness;
    leds_outer_values[p.position].g = brightness;
    leds_outer_values[p.position].b = brightness;
  }

  // Apply values to the FastLED array
  for (int i = 0; i < NUM_MIC_LEDS; i++) {
    mic_leds_val[i] += 255 * ANIMATE_SECS_PER_TICK * IDLE_MIC_ROTATIONS_PER_SEC;
    if (mic_leds_val[i] > 255) { mic_leds_val[i] = 0; }
    mic_leds_rgb[i] = CHSV(0, 0, mic_leds_val[i] * dimmer);
  }
}
