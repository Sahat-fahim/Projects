#include <FastLED.h>
#include <BluetoothSerial.h>

#define NUM_LEDS 130                
const byte  right_window = 16;       
const byte left_window = 19;
#define LED_TYPE WS2812B           
#define BRIGHTNESS 80              
#define SATURATION 255              
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND 100
CRGB leds[NUM_LEDS];


// const for Running effect 
#define updateLEDS 8        
#define COLOR_SHIFT 1800  

// ***variables for running effect***
unsigned long setTime = COLOR_SHIFT;
int shiftC = 0;
int mulC = 2;

// Define color structure for rgb
struct color {
  int r;
  int g;
  int b;
};
typedef struct color Color;

// variables for rainbow effect
// AUDIO INPUT SETUP
int strobe = 4;
int reset = 5;
int audio1 = 36;
int audio2 = 39;
int left[7];
int right[7];
int band;
int audio_input = 0;
int freq = 0;

// STANDARD VISUALIZER VARIABLES
int midway = NUM_LEDS / 2; 
int loop_max = 0;
int k = 255; 
int decay = 0; 
int decay_check = 0;
long pre_react = 0; 
long react = 0; 
long post_react = 0;

// RAINBOW WAVE SETTINGS
int wheel_speed = 2;

const int numberOfSamples = 128;
//volatile int state= 0; 
//int counter= 0;

void Music_running();
void Music_rainbow();


BluetoothSerial ESP_LED;
char incoming_command; 


void setup() {
  Serial.begin(9600);
  FastLED.addLeds<LED_TYPE, right_window, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, left_window, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );

  // SPECTRUM SETUP
  pinMode(audio1, INPUT);
  pinMode(audio2, INPUT);
  pinMode(strobe, OUTPUT);
  pinMode(reset, OUTPUT);
  digitalWrite(reset, LOW);
  digitalWrite(strobe, HIGH);
  delay( 3000 ); // power-up safety delay
  FastLED.setBrightness(  BRIGHTNESS );

   // CLEAR LEDS
  for (int i = 0; i < NUM_LEDS; i++)
    leds[i] = CRGB(0, 0, 0);
  FastLED.show();
  
  ESP_LED.begin("LED control");
 
}

void loop(){

     while(ESP_LED.available()){

     incoming_command= ESP_LED.read();
     Serial.println(incoming_command);}

    switch(incoming_command){
         case '0': 
          LED_off();
          break;
           
         case '1': 
          Rainbow();
          break;
           
         case '2': 
          Music_rainbow();
          break; 
          
         case '3': 
          Music_running();
          break; 
    }
}


void Rainbow() {
  
  for (int j = 0; j < 255; j++) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(i - (j * 2), SATURATION, BRIGHTNESS); /* The higher the value 4 the less fade there is and vice versa */ 
    }
    FastLED.show();     
    delay(200); /* Change this to your hearts desire, the lower the value the faster your colors move (and vice versa) */
    }
}

void LED_off(){
   
   for(int i=0; i<NUM_LEDS; i++){
   leds[i] = CRGB::Black;
   FastLED.show();
   } 
}


// Music reacting stars from here


CRGB Scroll(int pos) {
  pos = abs(pos);
  CRGB color (0,0,0);
  if(pos < 85) {
    color.g = 0;
    color.r = ((float)pos / 85.0f) * 255.0f;
    color.b = 255 - color.r;
  } else if(pos < 170) {
    color.g = ((float)(pos - 85) / 85.0f) * 255.0f;
    color.r = 255 - color.g;
    color.b = 0;
  } else if(pos < 256) {
    color.b = ((float)(pos - 170) / 85.0f) * 255.0f;
    color.g = 255 - color.b;
    color.r = 1;
  }

  return color;
}

void singleRainbow()
{
  for(int i = NUM_LEDS - 1; i >= 0; i--) {
    if (i < react)
      leds[i] = Scroll((i * 256 / 50 + k) % 256);
    else
      leds[i] = CRGB(0, 0, 0);      
  }
  FastLED.show(); 
}

// FUNCTION TO MIRRORED VISUALIZER
void doubleRainbow()
{
  for(int i = NUM_LEDS - 1; i >= midway; i--) {
    if (i < react + midway) {
      //Serial.print(i);
      //Serial.print(" -> ");
      leds[i] = Scroll((i * 256 / 50 + k) % 256);
      //Serial.print(i);
      //Serial.print(" -> ");
      leds[(midway - i) + midway] = Scroll((i * 256 / 50 + k) % 256);
    }
    else
      leds[i] = CRGB(0, 0, 0);
      leds[midway - react] = CRGB(0, 0, 0);
  }
  FastLED.show();
}

void readMSGEQ7()
// Function to read 7 band equalizers
{
  digitalWrite(reset, HIGH);
  digitalWrite(reset, LOW);
  long sample = 0;
  for (int i=0; i<numberOfSamples; i++) { // take many readings and average them
    sample += analogRead(audio1)/3;          // take a reading
    }

   sample = sample/numberOfSamples;

    Serial.println(sample);
   
  for(band=0; band <7; band++)
  {
    digitalWrite(strobe, LOW); 
    delayMicroseconds(30); 
    left[band] = sample;   
    right[band] = sample; 
    digitalWrite(strobe, HIGH); 
  }
    
}

void convertSingle()
{
  if (left[freq] > right[freq])
    audio_input = left[freq];
  else
    audio_input = right[freq];

  if (audio_input > 80)
  {
    pre_react = ((long)NUM_LEDS * (long)audio_input) / 1023L; // TRANSLATE AUDIO LEVEL TO NUMBER OF LEDs

    if (pre_react > react) // ONLY ADJUST LEVEL OF LED IF LEVEL HIGHER THAN CURRENT LEVEL
      react = pre_react;

  }
}

void convertDouble()
{
  if (left[freq] > right[freq])
    audio_input = left[freq];
  else
    audio_input = right[freq];

  if (audio_input > 80)
  {
    pre_react = ((long)midway * (long)audio_input) / 1023L; // TRANSLATE AUDIO LEVEL TO NUMBER OF LEDs

    if (pre_react > react) // ONLY ADJUST LEVEL OF LED IF LEVEL HIGHER THAN CURRENT LEVEL
      react = pre_react;
  }
}

// FUNCTION TO VISUALIZE WITH A SINGLE LEVEL
void singleLevel()
{
  readMSGEQ7();

  convertSingle();

  singleRainbow(); // APPLY COLOR

  k = k - wheel_speed; // SPEED OF COLOR WHEEL
  if (k < 0) // RESET COLOR WHEEL
    k = 255;

  // REMOVE LEDs
  decay_check++;
  if (decay_check > decay)
  {
    decay_check = 0;
    if (react > 0)
      react--;
  }
}

// FUNCTION TO VISUALIZE WITH MIRRORED LEVELS
void Music_rainbow()
{
  readMSGEQ7();

  convertDouble();

  doubleRainbow();

  k = k - wheel_speed; // SPEED OF COLOR WHEEL
  if (k < 0) // RESET COLOR WHEEL
    k = 255;

  // REMOVE LEDs
  decay_check++;
  if (decay_check > decay)
  {
    decay_check = 0;
    if (react > 0)
      react--;
  }
}

void Music_running() { 
  unsigned long time = millis();

  // Shift the color spectrum by 200 on set intervals (setTime)
  if(time / (double)setTime >= 1) {
    setTime = time + COLOR_SHIFT;
    Serial.println(setTime);
    shiftC += 200;
    mulC++;
    if(shiftC >= 600) {
      shiftC = 0;
    }
    if(mulC > 3) {
      mulC = 2;
    }
  }

  // Shift all LEDs to the right by updateLEDS number each time
  for(int i = NUM_LEDS - 1; i >= updateLEDS; i--) {
    leds[i] = leds[i - updateLEDS];
  }

  long sample = 0;
  for (int i=0; i<numberOfSamples; i++) { // take many readings and average them
    sample += analogRead(audio1)/2;          // take a reading
    }

   sample = sample/numberOfSamples;

    Serial.println(sample);

  // Get the pitch and brightness to compute the new color
  int newPitch = (sample*2) + shiftC;
  Color nc = pitchConv(newPitch, sample/2);

  // Set the left most updateLEDs with the new color
  for(int i = 0; i < updateLEDS; i++) {
    leds[i] = CRGB(nc.r, nc.g, nc.b);
  }
  FastLED.show();

  delay(1);
}

double convBrightness(int b) {
  double c = b / 614.0000;
  if( c < 0.2 ) {
    c = 0;
  }
  else if(c > 1) {
    c = 1.00;
  }
  return c;
}

Color pitchConv(int p, int b) {
  Color c;
  double bright = convBrightness(b);

  if(p < 40) {
    setColor(&c, 255, 0, 0);
  }
  else if(p >= 40 && p <= 77) {
    int b = (p - 40) * (255/37.0000);
    setColor(&c, 255, 0, b);
  }
  else if(p > 77 && p <= 205) {
    int r = 255 - ((p - 78) * 2);
    setColor(&c, r, 0, 255);
  }
  else if(p >= 206 && p <= 238) {
    int g = (p - 206) * (255/32.0000);
    setColor(&c, 0, g, 255);
  }
  else if(p <= 239 && p <= 250) {
    int r = (p - 239) * (255/11.0000);
    setColor(&c, r, 255, 255);
  }
  else if(p >= 251 && p <= 270) {
    setColor(&c, 255, 255, 255);
  }
  else if(p >= 271 && p <= 398) {
    int rb = 255-((p-271)*2);
    setColor(&c, rb, 255, rb);
  }
  else if(p >= 398 && p <= 653) {
    setColor(&c, 0, 255-(p-398), (p-398));
  }
  else {
    setColor(&c, 255, 0, 0);
  }
  setColor(&c, c.r * bright, c.g * bright, c.b * bright);
  return c;
}

void setColor(Color *c, int r, int g, int b) {
  c->r = r;
  c->g = g;
  c->b = b;
}
