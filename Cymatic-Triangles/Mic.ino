#include "Wisp.h" /* Import Wisp class */
#include <FastLED.h>
#include "SPI.h" // Comment out this line if using Trinket or Gemma

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include <math.h>

#define NUM_MIC_LEDS 28
#define MIC_DATA_PIN 8

#define MAX_ROTATIONS_PER_SEC 2
#define MAX_SPEED (NUM_MIC_LEDS * MAX_ROTATIONS_PER_SEC) /* LEDs per second */

#define ROTATION_ACCEL (0.1 / ANIMATE_SECS_PER_TICK)
#define ROTATION_FRICTION (0.10 / ANIMATE_SECS_PER_TICK)
/* Between 0 and 1, when the particle starts to accelerate on the amplitude
   scale */
#define ACCELERATION_THRESHOLD 0.1

uint8_t dataPin = MIC_DATA_PIN;
CRGB mic_leds_rgb[NUM_MIC_LEDS];
float mic_leds_hue[NUM_MIC_LEDS];
float mic_leds_val[NUM_MIC_LEDS];

float wave = 0;
float amplitude = 0;

void setupMic() {
  // Instantiate FastLED
  FastLED.addLeds<NEOPIXEL, MIC_DATA_PIN>(mic_leds_rgb, NUM_MIC_LEDS);

  // Start with full brightness LEDs that fade to test that code works
  for (int i = 0; i < NUM_MIC_LEDS; i++) {
    mic_leds_val[i] = 255.0;
    mic_leds_hue[i] = 255;
  }
}

// Returns a number between 0 and 1 that represents current amplitude of sound
float normalize(float x) {
  x = x - min_amplitude;
  x = x / (max_amplitude - min_amplitude);
  if (x > 1) { x = 1; }
  if (x < 0) { x = 0; }
  return x;
}

void animateMic() {
  // Dim all LEDs and set hue to current sound
  for (int i = 0; i < NUM_MIC_LEDS; i++) {
    mic_leds_val[i] /= 2;
    mic_leds_hue[i] = color.hue;
  }

  // Apply changes in amplitude to wave
  float amplitude_coeff = normalize((float)amp_sum_L);

  // Calculate new wave position
  int last_wave = (int)wave;
  float max_wave = (float)(NUM_MIC_LEDS - 1);
  float max_wave_half = max_wave / 2.0;
  float curr_amplitude = max_wave_half * amplitude_coeff;
  if (curr_amplitude > amplitude) {
    amplitude = curr_amplitude;
  } else {
    amplitude *= 0.95;
  }

  if (amplitude < 1) { amplitude = 0; }

  wave = max_wave_half + sin(2.0 * 3.14 * (millis() % 333) / 333.0) * amplitude;
  int curr_wave = (int)wave;
  while (curr_wave > NUM_MIC_LEDS - 1) { curr_wave = NUM_MIC_LEDS; }
  while (curr_wave < 0) { curr_wave = 0; }

  // Light all LEDs between last and current position
  int diff = curr_wave - last_wave;
  int direction = (int)(diff / abs(diff));

  while (last_wave != curr_wave) {
    last_wave += (int)direction;
    last_wave = abs(last_wave % NUM_MIC_LEDS);
    mic_leds_val[last_wave] = 255;
  }

  // Apply values to the FastLED array
  for (int i = 0; i < NUM_MIC_LEDS; i++) {
    mic_leds_rgb[i] = CHSV(mic_leds_hue[i], 255, mic_leds_val[i]);
  }
}
