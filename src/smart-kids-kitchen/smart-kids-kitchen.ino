// Include libraries
#include <Adafruit_NeoPixel.h>
#include <SD.h>
#include <TMRpcm.h>
#include <RTClib.h>

// Define pins
#define sdChipSelectPin 4
#define loudSpeakerPin 9
#define neoPixelPin 6
#define numberOfLeds 56

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

// Setup class for real time clock;
RTC_DS3231 rtc;

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
  // If the Serial communication is enabled the tmrpcm playback does NOT work! Maybe baud port issue?
  //Serial.begin(9600);

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
 
  if (! rtc.begin()) {
    //Serial.println("Couldn't find RTC");
    //Serial.flush();
    abort();
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
     //rtc.adjust(DateTime(2021, 2, 16, 6, 59,0));
  }
   // Comment in to set the time
  //rtc.adjust(DateTime(2021, 2, 15, 22, 27, 0));
}

void loop() {

  int ovenHeatKnob = analogRead(A0) / 256;
  int ovenModeKnob = analogRead(A1) / 256;
  int microwaveKnob = analogRead(A2) / 256;
  int lightSwitch = digitalRead(2);

  // Lesson learnt: Don't call the stripe.show() method while the tmrpcm is playing or the sound will be distorted!

  if (currentOvenHeatKnobState > 0 && currentOvenModeKnobState > 0 && currentMicrowaveKnobState == 0 && !tmrpcm.isPlaying()) {
    //Serial.println("Play oven on sound");
    tmrpcm.play(const_cast<char*>("oven.wav"), 1);
  } else if ((currentOvenHeatKnobState == 0 || currentOvenModeKnobState == 0) && currentMicrowaveKnobState > 0 && !tmrpcm.isPlaying()) {
    //Serial.println("Play microwave on sound");
    tmrpcm.play(const_cast<char*>("mcrwv.wav"), 1);
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

   DateTime now = rtc.now();
   ringTheBell(now);

}

// Set one of three parts to a color
void colorPart(byte part, uint32_t color) {
  byte firstLed, lastLed;
  switch (part) {
    case 1:
      firstLed = 0;
      lastLed = 15;
      break;
    case 2:
      firstLed = 16;
      lastLed = 35;
      break;
    case 3:
      firstLed = 36;
      lastLed = 55;
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

void ringTheBell (DateTime now) {
    uint8_t month = now.month();
    uint8_t day = now.day();
    // Hour is 0-23 based
    uint8_t hour = now.hour();
    uint8_t minute = now.minute();
    uint8_t second = now.second();
    //Serial.print(now.year(), DEC);
    //Serial.print('/');
    //Serial.print(now.month(), DEC);
    //Serial.print('/');
    //Serial.print(now.day(), DEC);
    //Serial.print(" at ");
    //Serial.print(now.hour(), DEC);
    //Serial.print(':');
    //Serial.print(now.minute(), DEC);
    //Serial.print(':');
    //Serial.print(now.second(), DEC);
    //Serial.println();
    if (hour >= 7 && hour <= 19 && minute == 0 && second == 0) {
       uint8_t soundHour = hour;
       if (isDaylightSavingTime(now)) {
         soundHour++;  
       }
      // Serial.println(soundHour);
       playAnimalSound(soundHour);
    }
  
}

void playAnimalSound (uint8_t soundHour) {
  if (soundHour == 7 && !tmrpcm.isPlaying()) {
    //Serial.println("Play 7 hour sound");
    tmrpcm.play(const_cast<char*>("7.wav"), 1);
  } else if (soundHour == 8 && !tmrpcm.isPlaying()) {
    tmrpcm.play(const_cast<char*>("8.wav"), 1);
  } else if (soundHour == 9 && !tmrpcm.isPlaying()) {
    tmrpcm.play(const_cast<char*>("9.wav"), 1);
  } else if (soundHour == 10 && !tmrpcm.isPlaying()) {
    tmrpcm.play(const_cast<char*>("10.wav"), 1);
  } else if (soundHour == 11 && !tmrpcm.isPlaying()) {
    tmrpcm.play(const_cast<char*>("11.wav"), 1);
  } else if (soundHour == 12 && !tmrpcm.isPlaying()) {
    tmrpcm.play(const_cast<char*>("12.wav"), 1);
  } else if (soundHour == 13 && !tmrpcm.isPlaying()) {
    tmrpcm.play(const_cast<char*>("13.wav"), 1);
  } else if (soundHour == 14 && !tmrpcm.isPlaying()) {
    tmrpcm.play(const_cast<char*>("14.wav"), 1);
  } else if (soundHour == 15 && !tmrpcm.isPlaying()) {
    tmrpcm.play(const_cast<char*>("15.wav"), 1);
  } else if (soundHour == 16 && !tmrpcm.isPlaying()) {
    tmrpcm.play(const_cast<char*>("16.wav"), 1);
  } else if (soundHour == 17 && !tmrpcm.isPlaying()) {
    tmrpcm.play(const_cast<char*>("17.wav"), 1);
  } else if (soundHour == 18 && !tmrpcm.isPlaying()) {
    tmrpcm.play(const_cast<char*>("18.wav"), 1);
  } else if (soundHour == 19 && !tmrpcm.isPlaying()) {
    tmrpcm.play(const_cast<char*>("19.wav"), 1);
  }    
}

boolean isDaylightSavingTime(DateTime now) {
  if (now >= DateTime(2021, 3, 28, 2, 0, 0) && now <= DateTime(2021, 10, 31, 2, 0, 0 )) {
    return true;
  } else if (now >= DateTime(2022, 3, 27, 2, 0, 0) && now <= DateTime(2022, 10, 30, 2, 0, 0 )) {
    return true;
  } else if (now >= DateTime(2023, 3, 26, 2, 0, 0) && now <= DateTime(2023, 10, 29, 2, 0, 0 )) {
    return true;
  } else if (now >= DateTime(2024, 3, 31, 2, 0, 0) && now <= DateTime(2024, 10, 27, 2, 0, 0 )) {
    return true;
  } else if (now >= DateTime(2025, 3, 30, 2, 0, 0) && now <= DateTime(2025, 10, 26, 2, 0, 0 )) {
    return true;
  } else if (now >= DateTime(2026, 3, 29, 2, 0, 0) && now <= DateTime(2026, 10, 25, 2, 0, 0 )) {
    return true;
  } else if (now >= DateTime(2027, 3, 28, 2, 0, 0) && now <= DateTime(2027, 10, 31, 2, 0, 0 )) {
    return true;
  } else {
    return false;
  }  
}
