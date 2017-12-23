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

// Initialize led stripe parts
byte microwaveLedStripPart = 1;
byte lightLedStripPart = 2;
byte ovenLedStripPart = 3;

// Define colors
uint32_t darkRed = strip.Color(63, 0, 0);
uint32_t mediumRed = strip.Color(127, 0, 0);
uint32_t brightRed = strip.Color(255, 0, 0);

uint32_t darkOrange = strip.Color(255, 60, 0);
uint32_t mediumOrange = strip.Color(255, 83, 0);
uint32_t brightOrange = strip.Color(255, 106, 0);

uint32_t darkYellow = strip.Color(255, 205, 0);
uint32_t mediumYellow = strip.Color(255, 225, 0);
uint32_t brightYellow = strip.Color(255, 255, 0);

uint32_t darkGreen = strip.Color(0, 63, 0);
uint32_t mediumGreen = strip.Color(0, 127, 0);
uint32_t brightGreen = strip.Color(0, 255, 0);

uint32_t mediumWhite = strip.Color(127, 127, 127);
uint32_t white = strip.Color(255, 255, 255);

uint32_t black = strip.Color(0, 0, 0);

uint32_t ovenColorsRed[] = {black, darkRed, mediumRed, brightRed};
uint32_t ovenColorsOrange[] = {black, darkOrange, mediumOrange, brightOrange};
uint32_t ovenColorsYellow[] = {black, darkYellow, mediumYellow, brightYellow};
uint32_t microwaveColors[] = {black, darkGreen, mediumGreen, brightGreen};

// Initialize knob states
byte currentOvenHeatKnobState = 0;
byte currentOvenModeKnobState = 0;
byte currentMicrowaveKnobState = 0;
byte currentLightSwitchState = 0;

void setup() {

  // Enable debug logging only if needed!
  // If the Serial communication is enabled the tmrpcm playback does NOT work!
  // Serial.begin(9600);

  // Setup stripe
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // setup light switch
  pinMode(2, INPUT_PULLUP);

  digitalWrite(LED_BUILTIN, LOW);

  // Setup wav file play back
  tmrpcm.speakerPin = loudSpeakerPin;
  if (!SD.begin(sdChipSelectPin)) {
    digitalWrite(LED_BUILTIN, HIGH);
    return;
  }
  tmrpcm.volume(7);
  tmrpcm.quality(0);

}

void loop() {

  int ovenHeatKnob = analogRead(A0) / 256;
  int ovenModeKnob = analogRead(A1) / 256;
  int microwaveKnob = analogRead(A2) / 256;
  int lightSwitch = digitalRead(2);

  // Lesson learnt: Don't call the stripe.show() method while the tmrpcm is playing or the sound will be distorted!

  if (currentOvenHeatKnobState > 0 && currentOvenModeKnobState > 0 && currentMicrowaveKnobState == 0 && !tmrpcm.isPlaying()) {
    //Serial.println("Play oven on sound");
    tmrpcm.play("1.wav", 1);
  } else if ((currentOvenHeatKnobState == 0 || currentOvenModeKnobState == 0) && currentMicrowaveKnobState > 0 && !tmrpcm.isPlaying()) {
    //Serial.println("Play microwave on sound");
    tmrpcm.play("2.wav", 1 );
  } else if (currentOvenHeatKnobState > 0 && currentOvenModeKnobState > 0 && currentMicrowaveKnobState > 0 && !tmrpcm.isPlaying()) {
    //Serial.println("Play oven and microwave on sound");
    tmrpcm.play("4.wav", 1);
  }

  switch (ovenHeatKnob) {
    case 0:
      if (currentOvenHeatKnobState != 0) {
        //Serial.println("Oven heat 0");
        colorPart(ovenLedStripPart, getOvenColor(currentOvenModeKnobState, ovenHeatKnob));
        currentOvenHeatKnobState = 0;
      }
      break;
    case 1:
      if (currentOvenHeatKnobState != 1) {
        //Serial.println("Oven heat 1");
        colorPart(ovenLedStripPart, getOvenColor(currentOvenModeKnobState, ovenHeatKnob));
        currentOvenHeatKnobState = 1;
      }
      break;
    case 2:
      if (currentOvenHeatKnobState != 2) {
        //Serial.println("Oven heat 2");
        colorPart(ovenLedStripPart, getOvenColor(currentOvenModeKnobState, ovenHeatKnob));
        currentOvenHeatKnobState = 2;
      }
      break;
    case 3:
      if (currentOvenHeatKnobState != 3) {
        //Serial.println("Oven heat 3");
        colorPart(ovenLedStripPart, getOvenColor(currentOvenModeKnobState, ovenHeatKnob));
        currentOvenHeatKnobState = 3;
      }
      break;
    default:
      return;
  }

  switch (ovenModeKnob) {
    case 0:
      if (currentOvenModeKnobState != 0) {
        //Serial.println("Oven mode 0");
        colorPart(ovenLedStripPart, getOvenColor(ovenModeKnob, currentOvenHeatKnobState));
        currentOvenModeKnobState = 0;
      }
      break;
    case 1:
      if (currentOvenModeKnobState != 1) {
        //Serial.println("Oven mode 1");
        colorPart(ovenLedStripPart, getOvenColor(ovenModeKnob, currentOvenHeatKnobState));
        currentOvenModeKnobState = 1;
      }
      break;
    case 2:
      if (currentOvenModeKnobState != 2) {
        //Serial.println("Oven mode 2");
        colorPart(ovenLedStripPart, getOvenColor(ovenModeKnob, currentOvenHeatKnobState));
        currentOvenModeKnobState = 2;
      }
      break;
    case 3:
      if (currentOvenModeKnobState != 3) {
        //Serial.println("Oven mode 3");
        colorPart(ovenLedStripPart, getOvenColor(ovenModeKnob, currentOvenHeatKnobState));
        currentOvenModeKnobState = 3;
      }
      break;
    default:
      return;
  }

  switch (microwaveKnob) {
    case 0:
      if (currentMicrowaveKnobState != 0) {
        //Serial.println("Microwave mode 0");
        colorPart(microwaveLedStripPart, microwaveColors[0]);
        currentMicrowaveKnobState = 0;
      }
      break;
    case 1:
      if (currentMicrowaveKnobState != 1) {
        //Serial.println("Microwave mode 1");
        colorPart(microwaveLedStripPart, microwaveColors[1]);
        currentMicrowaveKnobState = 1;
      }
      break;
    case 2:
      if (currentMicrowaveKnobState != 2) {
        //Serial.println("Microwave mode 2");
        colorPart(microwaveLedStripPart, microwaveColors[2]);
        currentMicrowaveKnobState = 2;
      }
      break;
    case 3:
      if (currentMicrowaveKnobState != 3) {
        //Serial.println("Microwave mode 3");
        colorPart(microwaveLedStripPart, microwaveColors[3]);
        currentMicrowaveKnobState = 3;
      }
      break;
    default:
      return;
  }

  if (lightSwitch == LOW && currentLightSwitchState != 0) {
    //Serial.println("Light on");
    colorPart(lightLedStripPart, mediumWhite);
    currentLightSwitchState = 0;
  } else if (lightSwitch == HIGH && currentLightSwitchState != 1) {
    //Serial.println("Light off");
    colorPart(lightLedStripPart, black);
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

uint32_t getOvenColor(byte currentOvenMode, byte currentOvenHeat) {
  uint32_t ovenColor;
  //Serial.print("Get oven color: currentOvenMode:");
  //Serial.print(currentOvenMode);
  //Serial.print(" currentOvenHeat: ");
  //Serial.print(currentOvenHeat);
  //Serial.println("");

  switch (currentOvenMode) {
    case 0:
      ovenColor = black;
      //Serial.print("Get oven color: black, heat: ");
      //Serial.print(currentOvenHeat);
      //Serial.println("");
      break;
    case 1:
      ovenColor = ovenColorsRed[currentOvenHeat];
      //Serial.print("Get oven color: red, heat: ");
      //Serial.print(currentOvenHeat);
      //Serial.println("");
      break;
    case 2:
      ovenColor = ovenColorsOrange[currentOvenHeat];
      //Serial.print("Get oven color: orange, heat: ");
      //Serial.print(currentOvenHeat);
      //Serial.println("");
      break;
    case 3:
      ovenColor = ovenColorsYellow[currentOvenHeat];
      //Serial.print("Get oven color: yellow, heat: ");
      //Serial.print(currentOvenHeat);
      //Serial.println("");
      break;
  }
  return ovenColor;
}

