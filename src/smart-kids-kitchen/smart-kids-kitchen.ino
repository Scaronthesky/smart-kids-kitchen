// Include libraries
#include <Adafruit_NeoPixel.h>
#include <SD.h>
#include <TMRpcm.h>

// Define pins
#define sdChipSelectPin 4
#define loudSpeakerPin 9
#define neoPixelPin 6
#define numberOfLeds 60

// Setup Adafruit NeoPixel stripe
// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numberOfLeds, neoPixelPin, NEO_GRB + NEO_KHZ800);

// Setup class for Wav file playback
TMRpcm tmrpcm;

uint32_t darkRed = strip.Color(63, 0, 0);
uint32_t mediumRed = strip.Color(127, 0, 0);
uint32_t brightRed = strip.Color(255, 0, 0);
uint32_t darkGreen = strip.Color(0, 63, 0);
uint32_t mediumGreen = strip.Color(0, 127, 0);
uint32_t brightGreen = strip.Color(0, 255, 0);
uint32_t mediumWhite = strip.Color(127, 127, 127);
uint32_t white = strip.Color(255, 255, 255);

uint32_t black = strip.Color(0, 0, 0);

uint32_t ovenColors[] = {black, darkRed, mediumRed, brightRed};
uint32_t microwaveColors[] = {black, darkGreen, mediumGreen, brightGreen};

byte currentOvenKnobState = 0;
byte currentMicrowaveKnobState = 0;
byte currentLightSwitchState = 0;

void setup() {

  // Setup stripe
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // Setup wav file play back
  tmrpcm.speakerPin = loudSpeakerPin;
  if (!SD.begin(sdChipSelectPin)) {
    return;
  }
  tmrpcm.volume(4);
  tmrpcm.quality(0);

  // setup light switch
  pinMode(2,INPUT_PULLUP);

}

void loop() {

  int ovenKnob = analogRead(A0) / 256;
  int microwaveKnob = analogRead(A1) / 256;
  int lightSwitch = digitalRead(2);

  // Lesson learnt: Don't call the stripe.show() method while the tmrpcm is playing or the sound will be distorted!

  if (currentOvenKnobState > 0 && currentMicrowaveKnobState == 0 && !tmrpcm.isPlaying()) {
    tmrpcm.play("1.wav", 1);
  } else if (currentOvenKnobState == 0 && currentMicrowaveKnobState > 0 && !tmrpcm.isPlaying()) {
    tmrpcm.play("2.wav", 1 );
  } else if (currentOvenKnobState > 0 && currentMicrowaveKnobState > 0 && !tmrpcm.isPlaying()) {
    tmrpcm.play("4.wav", 1);
  }

  switch (ovenKnob) {
    case 0:
      if (currentOvenKnobState != 0) {
        colorPart(1, ovenColors[0]);
        currentOvenKnobState = 0;
      }
      break;
    case 1:
      if (currentOvenKnobState != 1) {
        colorPart(1, ovenColors[1]);
        currentOvenKnobState = 1;
      }
      break;
    case 2:
      if (currentOvenKnobState != 2) {
        colorPart(1, ovenColors[2]);
        currentOvenKnobState = 2;
      }
      break;
    case 3:
      if (currentOvenKnobState != 3) {
        colorPart(1, ovenColors[3]);
        currentOvenKnobState = 3;
      }
      break;
    default:
      return;
  }

  switch (microwaveKnob) {
    case 0:
      if (currentMicrowaveKnobState != 0) {
        colorPart(2, microwaveColors[0]);
        currentMicrowaveKnobState = 0;
      }
      break;
    case 1:
      if (currentMicrowaveKnobState != 1) {
        colorPart(2, microwaveColors[1]);
        currentMicrowaveKnobState = 1;
      }
      break;
    case 2:
      if (currentMicrowaveKnobState != 2) {
        colorPart(2, microwaveColors[2]);
        currentMicrowaveKnobState = 2;
      }
      break;
    case 3:
      if (currentMicrowaveKnobState != 3) {
        colorPart(2, microwaveColors[3]);
        currentMicrowaveKnobState = 3;
      }
      break;
    default:
      return;
  }

  if (lightSwitch == LOW && currentLightSwitchState != 0) {
    colorPart(3, mediumWhite);
    currentLightSwitchState = 0;
  } else if (lightSwitch == HIGH && currentLightSwitchState != 1) {
    colorPart(3, black);
    currentLightSwitchState = 1;
  }

}

void playSound(char fileName) {
  tmrpcm.play(fileName);
}

// Set one of three parts to a color
void colorPart(byte part, uint32_t color) {
  byte firstLed, lastLed;
  switch (part) {
    case 1:
      firstLed = 0;
      lastLed = 19;
      break;
    case 2:
      firstLed = 20;
      lastLed = 39;
      break;
    case 3:
      firstLed = 40;
      lastLed = 59;
      break;
    default:
      return;
  }
  for (uint16_t i = firstLed; i <= lastLed; i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

// Set all the LEDs to one color
void colorAll(uint32_t c) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}

