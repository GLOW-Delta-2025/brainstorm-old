#include <Arduino.h>
#include <FastLED.h>

// First LED strip
#define NUM_LEDS_1 64
#define DATA_PIN_1 3

// Second LED strip
#define NUM_LEDS_2 17  // Change this to your desired number of LEDs
#define DATA_PIN_2 2    // Change this to your desired pin

// Create arrays for both strips
CRGB leds1[NUM_LEDS_1];
CRGB leds2[NUM_LEDS_2];

// Colors for the lightning
#define DARK_BLUE   CRGB(0, 0, 40)
#define LIGHT_BLUE  CRGB(90, 90, 140)
#define BRIGHT_WHITE CRGB(255, 255, 255)
#define THUNDER_YELLOW CRGB(255, 230, 150)
#define FULL_YELLOW CRGB(255, 255, 0)

const byte boltPatterns[][12] = {
  {0, 9, 10, 19, 28, 37, 45, 54},  // Zigzag pattern 1
  {7, 14, 21, 28, 35, 42, 49, 56}, // Straight down right side
  {3, 12, 21, 30, 37, 44, 53, 60}, // Zigzag pattern 2
  {4, 11, 18, 25, 34, 43, 52, 61}, // Zigzag pattern 3
  {0, 1, 2, 10, 18, 26, 34, 42},   // Diagonal pattern 1
  {24, 33, 42, 51, 52, 53, 54, 63} // L-shaped pattern
};

const byte boltPatterns2[][12] = {
  {0, 9, 10, 9, 17, 17, 15, 14},  // Zigzag pattern 1
  {7, 1, 2, 8, 3, 2, 4, 16}, // Straight down right side
  {3, 2, 1, 3, 7, 4, 3, 6}, // Zigzag pattern 2
  {4, 1, 1, 2, 3, 4, 5, 6}, // Zigzag pattern 3
  {0, 1, 2, 10, 17, 6, 4, 2},   // Diagonal pattern 1
  {4, 3, 2, 1, 2, 3, 5, 3} // L-shaped pattern
};

void setup() {
  // Initialize both LED strips
  FastLED.addLeds<WS2812B, DATA_PIN_1, GRB>(leds1, NUM_LEDS_1);
  FastLED.addLeds<WS2812B, DATA_PIN_2, GRB>(leds2, NUM_LEDS_2);
  FastLED.setBrightness(255);
  pinMode(8, OUTPUT);

  // Initialize with dark background on both strips
  fill_solid(leds1, NUM_LEDS_1, CRGB::Black);
  fill_solid(leds2, NUM_LEDS_2, CRGB::Black);
  FastLED.show();

  randomSeed(analogRead(0));  // Seed the random number generator
}

void transitionToColor(CRGB startColor, CRGB endColor) {
  int transitionSteps = 1;

  for (int step = 0; step <= transitionSteps; step++) {
    float progress = (float)step / transitionSteps;

    // Update both strips
    for (int i = 0; i < NUM_LEDS_1; i++) {
      leds1[i] = CRGB(
        startColor.r + (endColor.r - startColor.r) * progress,
        startColor.g + (endColor.g - startColor.g) * progress,
        startColor.b + (endColor.b - startColor.b) * progress
      );
    }

    for (int i = 0; i < NUM_LEDS_2; i++) {
      leds2[i] = CRGB(
        startColor.r + (endColor.r - startColor.r) * progress,
        startColor.g + (endColor.g - startColor.g) * progress,
        startColor.b + (endColor.b - startColor.b) * progress
      );
    }

    FastLED.show();
    delay(50);
  }
}

void fullGridFlash() {
  // Bright flash across both entire grids
  int oldBrightness = FastLED.getBrightness();

  // First white flash on both strips
  fill_solid(leds1, NUM_LEDS_1, BRIGHT_WHITE);
  fill_solid(leds2, NUM_LEDS_2, BRIGHT_WHITE);
  FastLED.setBrightness(255);
  FastLED.show();
  delay(random(10, 50));

  // Return to dark on both strips
  fill_solid(leds1, NUM_LEDS_1, DARK_BLUE);
  fill_solid(leds2, NUM_LEDS_2, DARK_BLUE);
  FastLED.setBrightness(oldBrightness);
  FastLED.show();
  delay(random(10, 30));
}

void drawLightningBolt(int patternIndex, int intensity) {
  // Clear all LEDs first on both strips
  fill_solid(leds1, NUM_LEDS_1, DARK_BLUE);
  fill_solid(leds2, NUM_LEDS_2, DARK_BLUE);

  // Random choice between white and yellow for the lightning
  CRGB boltColor = random(10) > 3 ? BRIGHT_WHITE : THUNDER_YELLOW;

  // Draw speed increases with intensity
  int drawDelay = map(intensity, 10, 100, 40, 5);

  // Branch probability increases with intensity
  int branchChance = map(intensity, 10, 100, 2, 8);

  // Draw the bolt with potential branches on first strip
  for (int i = 0; i < 8; i++) {
    if (boltPatterns[patternIndex][i] != 0 || i == 0) { // Skip any 0 entries except the first position
      byte ledPosition = boltPatterns[patternIndex][i];
      leds1[ledPosition] = boltColor;

      // Chance to add a branch
      if (random(10) > (10 - branchChance) && i > 0 && i < 7) {
        // Add a branch 1-2 LEDs away from the main bolt
        int branchPos = ledPosition + random(-16, 16);
        if (branchPos >= 0 && branchPos < NUM_LEDS_1) {
          leds1[branchPos] = boltColor;
        }
      }
    }
  }

  // Draw a different bolt pattern on the second strip
  int patternIndex2 = random(0, sizeof(boltPatterns2) / sizeof(boltPatterns2[0]));
  for (int i = 0; i < 8; i++) {
    if (boltPatterns2[patternIndex2][i] != 0 || i == 0) {
      byte ledPosition = boltPatterns2[patternIndex2][i];
      leds2[ledPosition] = boltColor;

      // Chance to add a branch
      if (random(10) > (10 - branchChance) && i > 0 && i < 7) {
        // Add a branch, adapt this based on the second strip's layout
        int branchPos = ledPosition + random(-10, 10);
        if (branchPos >= 0 && branchPos < NUM_LEDS_2) {
          leds2[branchPos] = boltColor;
        }
      }
    }
  }

  // Show each step of the bolt forming
  FastLED.show();
  delay(drawDelay);

  // Hold the complete bolt for a moment
  delay(random(10, 50));
}

void distantLightning() {
  // Subtle flash to simulate distant lightning on both strips
  int oldBrightness = FastLED.getBrightness();
  fill_solid(leds1, NUM_LEDS_1, LIGHT_BLUE);
  fill_solid(leds2, NUM_LEDS_2, LIGHT_BLUE);
  FastLED.setBrightness(random(20, 50));
  FastLED.show();
  delay(random(10, 40));
  FastLED.setBrightness(oldBrightness);
  fill_solid(leds1, NUM_LEDS_1, DARK_BLUE);
  fill_solid(leds2, NUM_LEDS_2, DARK_BLUE);
  FastLED.show();
}

void normalLightning(int intensity) {
  // Initial distant flash (more likely at lower intensities)
  if (random(100) > intensity) {
    distantLightning();
    delay(random(100, 300));
  }

  // Main lightning bolt strike
  int patternIndex = random(0, sizeof(boltPatterns) / sizeof(boltPatterns[0]));

  // Flash probability and speed increases with intensity
  if (random(100) < intensity) {
    fullGridFlash();
  }

  // Draw the zigzag pattern
  drawLightningBolt(patternIndex, intensity);

  // Secondary flashes
  int numFlashes = map(intensity, 10, 100, 1, 3);
  for (int i = 0; i < numFlashes; i++) {
    // Return to dark background on both strips
    fill_solid(leds1, NUM_LEDS_1, DARK_BLUE);
    fill_solid(leds2, NUM_LEDS_2, DARK_BLUE);
    FastLED.show();
    delay(random(10, 100 - intensity));

    // Flash again with slightly lower intensity
    if (random(100) < intensity) {
      // Full grid flash on both strips
      int brightness = 255 - (i * 40);
      FastLED.setBrightness(constrain(brightness, 50, 255));
      fill_solid(leds1, NUM_LEDS_1, BRIGHT_WHITE);
      fill_solid(leds2, NUM_LEDS_2, BRIGHT_WHITE);
      FastLED.show();
      delay(random(5, 30));
    } else {
      // Just the bolt
      drawLightningBolt(patternIndex, intensity);
      delay(random(10, 30));
    }
  }

  // Return to dark background on both strips
  FastLED.setBrightness(255);
  fill_solid(leds1, NUM_LEDS_1, DARK_BLUE);
  fill_solid(leds2, NUM_LEDS_2, DARK_BLUE);
  FastLED.show();
}

void crazyLightning(int crazyLevel) {
  // Rapid and intense lightning for the finale on both strips
  // Multiple bolts and full flashes in quick succession

  // Number of flashes increases with crazy level
  int numFlashes = map(crazyLevel, 0, 9, 3, 8);

  for (int i = 0; i < numFlashes; i++) {
    // Extremely rapid flashes on both strips
    fill_solid(leds1, NUM_LEDS_1, BRIGHT_WHITE);
    fill_solid(leds2, NUM_LEDS_2, BRIGHT_WHITE);
    FastLED.show();
    delay(random(5, 15));

    fill_solid(leds1, NUM_LEDS_1, DARK_BLUE);
    fill_solid(leds2, NUM_LEDS_2, DARK_BLUE);
    FastLED.show();
    delay(random(5, 15));
  }

  // Multiple simultaneous lightning bolts on both strips
  fill_solid(leds1, NUM_LEDS_1, DARK_BLUE);
  fill_solid(leds2, NUM_LEDS_2, DARK_BLUE);

  // Draw 2-3 bolts at the same time on first strip
  int numBolts = map(crazyLevel, 0, 9, 2, 3);

  for (int b = 0; b < numBolts; b++) {
    int patternIndex = random(0, sizeof(boltPatterns) / sizeof(boltPatterns[0]));

    // Draw each bolt with high intensity on first strip
    for (int i = 0; i < 8; i++) {
      if (boltPatterns[patternIndex][i] != 0 || i == 0) {
        byte ledPosition = boltPatterns[patternIndex][i];
        leds1[ledPosition] = (random(10) > 5) ? BRIGHT_WHITE : THUNDER_YELLOW;
      }
    }

    // Draw bolts on second strip
    int patternIndex2 = random(0, sizeof(boltPatterns2) / sizeof(boltPatterns2[0]));
    for (int i = 0; i < 8; i++) {
      if (boltPatterns2[patternIndex2][i] != 0 || i == 0) {
        byte ledPosition = boltPatterns2[patternIndex2][i];
        leds2[ledPosition] = (random(10) > 5) ? BRIGHT_WHITE : THUNDER_YELLOW;
      }
    }
  }

  FastLED.show();
  delay(random(10, 30));

  // Flash the whole grid again on both strips
  fill_solid(leds1, NUM_LEDS_1, BRIGHT_WHITE);
  fill_solid(leds2, NUM_LEDS_2, BRIGHT_WHITE);
  FastLED.show();
  delay(5);

  // Quick return to dark on both strips
  fill_solid(leds1, NUM_LEDS_1, DARK_BLUE);
  fill_solid(leds2, NUM_LEDS_2, DARK_BLUE);
  FastLED.show();
  delay(random(5, 10 * (10 - crazyLevel))); // Gets shorter as we approach the end
}

void beginStage() {
  // Start with white on both strips
  for (int i = 0; i < NUM_LEDS_1; i++) {
    leds1[i] = BRIGHT_WHITE;
  }
  for (int i = 0; i < NUM_LEDS_2; i++) {
    leds2[i] = BRIGHT_WHITE;
  }

  // Pulse effect on both strips
  for (int i = 0; i < 10; i++) {
    for (int brightness = 1; brightness < 255; brightness++) {
      FastLED.setBrightness(brightness);
      FastLED.show();
      delay(5);
    }
    for (int brightness = 255; brightness > 1; brightness--) {
      FastLED.setBrightness(brightness);
      FastLED.show();
      delay(5);
    }
  }
  FastLED.setBrightness(255);
}

void middleStage() {
  // Set dark stormy background on both strips
  fill_solid(leds1, NUM_LEDS_1, DARK_BLUE);
  fill_solid(leds2, NUM_LEDS_2, DARK_BLUE);
  FastLED.show();
  delay(300);
  digitalWrite(8, HIGH);


  // Total number of flash sequences (50 as requested)
  const int TOTAL_SEQUENCES = 50;

  for (int seq = 0; seq < TOTAL_SEQUENCES; seq++) {
    // Calculate intensity based on progress (0-100)
    int intensity = map(seq, 0, TOTAL_SEQUENCES - 1, 10, 100);

    // Calculate delay time - gets shorter as we progress
    int delayBetweenSequences = map(seq, 0, TOTAL_SEQUENCES - 1, 500, 20);

    // For last 10 sequences, make it really crazy with multiple simultaneous bolts
    bool crazyMode = (seq >= TOTAL_SEQUENCES - 10);

    if (crazyMode) {
      // Crazy final sequence (multiple bolts, rapid flashes)
      crazyLightning(seq - (TOTAL_SEQUENCES - 10));
    } else {
      // Regular lightning with increasing intensity
      normalLightning(intensity);
    }

    // Wait between lightning sequences (gets shorter as storm progresses)
    if (!crazyMode) {
      delay(delayBetweenSequences);
    }
  }

  // Final flash on both strips
  for (int i = 0; i < 5; i++) {
    fill_solid(leds1, NUM_LEDS_1, BRIGHT_WHITE);
    fill_solid(leds2, NUM_LEDS_2, BRIGHT_WHITE);
    FastLED.show();
    delay(10);
    fill_solid(leds1, NUM_LEDS_1, DARK_BLUE);
    fill_solid(leds2, NUM_LEDS_2, DARK_BLUE);
    FastLED.show();
    delay(10);
  }

  digitalWrite(8, LOW);


  // Fill with yellow at the end on both strips
  fill_solid(leds1, NUM_LEDS_1, FULL_YELLOW);
  fill_solid(leds2, NUM_LEDS_2, FULL_YELLOW);
  FastLED.show();
  delay(2000); // Keep yellow for 2 seconds
}

void endStage() {
  CRGB startColor = CRGB::Yellow;
  CRGB endColor = CRGB::White;
  int transitionSteps = 5;

  for (int step = 0; step <= transitionSteps; step++) {
    float progress = (float)step / transitionSteps;

    // Update both strips
    for (int i = 0; i < NUM_LEDS_1; i++) {
      leds1[i] = CRGB(
        startColor.r + (endColor.r - startColor.r) * progress,
        startColor.g + (endColor.g - startColor.g) * progress,
        startColor.b + (endColor.b - startColor.b) * progress
      );
    }

    for (int i = 0; i < NUM_LEDS_2; i++) {
      leds2[i] = CRGB(
        startColor.r + (endColor.r - startColor.r) * progress,
        startColor.g + (endColor.g - startColor.g) * progress,
        startColor.b + (endColor.b - startColor.b) * progress
      );
    }

    FastLED.show();
    delay(20);  // Adjust speed of transition
  }
}

// Optional: Create a function for synchronized or alternating effects
void syncedEffect() {
  // Example of synchronized or alternating effect between the two strips
  // This could be called during any stage for added visual interest

  // Alternating flash between strips
  fill_solid(leds1, NUM_LEDS_1, BRIGHT_WHITE);
  fill_solid(leds2, NUM_LEDS_2, DARK_BLUE);
  FastLED.show();
  delay(100);

  fill_solid(leds1, NUM_LEDS_1, DARK_BLUE);
  fill_solid(leds2, NUM_LEDS_2, BRIGHT_WHITE);
  FastLED.show();
  delay(100);

  // Return to normal
  fill_solid(leds1, NUM_LEDS_1, DARK_BLUE);
  fill_solid(leds2, NUM_LEDS_2, DARK_BLUE);
  FastLED.show();
}

void loop() {
  beginStage();
  middleStage();
  endStage();

  // Optional: Add a call to syncedEffect() somewhere if you want that additional effect
}