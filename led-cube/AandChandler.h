/*
     AandChandler.cpp - Libary for Animation and Colour handling
     created by legl-electroinics
     known bucks -
 */
 
#ifndef AandChandler_h
#define AandChandler_h
 
#include "Arduino.h"
#include <SPI.h>
#include <DueTimer.h>
#include <Fft.h>

typedef enum {AN_OFF, AN_GROUP, AN_1BALL, AN_2BALLS, AN_FOLDER, AN_CO_CUBE, AN_PLATE, AN_SNAKE, AN_ELASTIC, AN_BOUNCE_PLATE, AN_SINWAVE, AN_RANDOM_CUBE, AN_EXPLOSION, AN_EDGING, AN_STROBE, AN_RAIN, AN_FFT, AN_COLOR_WHEEL, AN_RTLTEXT, AN_QUANTITY} AN_NUMBER;
typedef enum {USER, RANDOM, STANDARD, COLOR_TYPE_QUANTITY} COLOR_TYPE;
typedef enum {STANDARD_RANDOM, STANDARD_SINGLE_RANDOM, STANDARD_COLOR_TYPE_QUANTITY} STANDARD_COLOR_TYPE;
typedef enum {GROUP_NORMAL, GROUP_ALL, GROUP_PARTY, GROUP_TYPE_QUANTITY} GROUP_TYPE;
#define MAX_TEXT_LENGTH 20

class AandChandler
{
     public:
          //sets everythig for starting
          void begin();
          //sets the animation
          void setAnimation(byte animation);
          //sets the color
          void setRed(byte red);
          void setGreen(byte green);
          void setBlue(byte blue);
          void setRed2(byte red);
          void setGreen2(byte green);
          void setBlue2(byte blue);
          //returns the color
          byte getRed();
          byte getBlue();
          byte getGreen();
          byte getRed2();
          byte getBlue2();
          byte getGreen2();
          //set the color type
          void setColorType(byte colorType);
          //shifts the data out
          void dataShiftOut();
          //runs the set(setup/init) methodes of the animations
          void setAnimation();
          //runs the animation
          void animationsHandler();
          //returns the state of all-animation (is needet to show the actuell animation on the display)
          byte returnState();
          //returns the current animation
          byte returnAnimation();
          //sets all LEDs to red, green, blue
          void all(byte red, byte green, byte blue);
          
          //saves the octave data
          void saveOctaveData(float32_t *data);
          boolean returnFFTOn();
          
          //for group
          void setWhichGroup(GROUP_TYPE whichGroup);
          GROUP_TYPE returnWhichGroup();
          
		
     private:
          //saves if the led is on or off
          //for each bit (4 bit angle modulation) one array
          byte red0[64], red1[64], red2[64], red3[64];
          byte blue0[64], blue1[64], blue2[64], blue3[64];
          byte green0[64], green1[64], green2[64], green3[64];
          
          //user set, random and normal/preset
          byte _colorType;
          //saves the actuel animation
          byte _animation;
          //the colors
          byte _red;
          byte _green;
          byte _blue;
          //the second colors if an animation uses two different
          byte _red2;
          byte _green2;
          byte _blue2;
          
          //for the bit angle modulation
          byte _BAM_Bit;
          byte _BAM_Counter;
          
          //for shifting out the data
          //we don´t have to do it like this we could make the variable new every time we use it in shiftOut()
          //but I think it is faster(because it is used in the interrupt very often) to use this variable every time than making every time a new one 
          byte _shift_out;
          
          //for shift out
          byte _anodelevel;
          byte _level;
          byte _anode[8];
          
          //creates new colors
          void newColor(byte animation);
          void newColor2(byte animatio);
          
          //saves the standard colors
          STANDARD_COLOR_TYPE _standardC[AN_QUANTITY];
          
          //creates random colors
          void randomColorX(byte &red, byte &green, byte &blue);
          void randomColor();
          void randomColor2();
          
           //creates randomly red, green or blue
           void singleRandomColorX(byte &red, byte &green, byte &blue);     
           void singleRandomColor();
           void singleRandomColor2();
           
          
          
          //variables for the animations
          
          //for timing the animations
          unsigned long _timer;
          
          //timer for all-animation
          unsigned long _lastChange;
          
          //set the state of the LED
          void LED(byte column, byte level, byte row, byte red, byte green, byte blue);
          //sets the state of the LED with constrained values
          void onlyWhenInsideLED(int column, int level, int row, byte red, byte green, byte blue);
          
          //Every animation has a set methode. This is called once in setAnimation and inits the variables. It could be also called out of the normal animation to reset it.
          //The normal methode is called in animationHandler and runs the animation.
          //if you want you can use the same variable in different annimations
          //I don´t because the 96kb of the due are big enought to hold plenty of variables
          //because of this every animation has it own variables with a name easy to understand --> code gets easyer to read
          
          //turns the LEDs off 
          void setOff();
          void off();
          
          //runs all animation in a row
          void setAll();
          void all();
          
          //for ball 1
          int _X1, _Y1, _Z1, _Xo1, _Yo1, _Zo1, _Xm1, _Ym1, _Zm1;
          //for ball 2
          int _X2, _Y2, _Z2, _Xo2, _Yo2, _Zo2, _Xm2, _Ym2, _Zm2;
          //I wrote the animation like this, because then we can use it for ball 1 and ball 2
          void setBall(int &X, int &Y, int &Z, int &Xo, int &Yo, int &Zo, int &Xm, int &Ym, int &Zm);
          void ball(int &X, int &Y, int &Z, int &Xo, int &Yo, int &Zo, int &Xm, int &Ym, int &Zm, byte &red, byte &green, byte &blue);
          //ball 1
          void setBall1();
          void ball1();
          //ball 2
          void setBall2();
          void ball2();
          
          //for plate
          byte _Yo;
          
          void setPlate();
          void plate();
          
          //for sinwave
          byte sinewave[8];
          int sinewaveM[8];
          byte sinewaveOLD[8];
          
          void setSinwave();
          void sinwave();
          
          //for colorCube
          byte _swiper;
          //already have a variable named like this
          //int _pos;
          
          void setColorCube();
          void colorCube();
          
          //for snake
          int _x[20];
          int _y[20];
          int _z[20];
          byte _redA[20];
          byte _greenA[20];
          byte _blueA[20];
          int _Xm;
          int _Ym;
          int _Zm;
          
          void setSnake();
          void snake();
          
          //for explosion
          int _direct;
          int _pos;
          
          void setExplosion();
          void explosion();
          
          boolean _fftNew;
          float32_t _octaveArray[NUM_OCTAVES];
          boolean _fftOn;
          
          void setFft();
          void fft();
          
          //for folder
          int _folderaddr[8];
          int _oldpullback[8];
          int _pullback[8];
          int _LED_Old[8];
          byte _fromSide;
          byte _toSide;
          
          void setFolder();
          void folder();
          
          //for strobe
          boolean _light;
          int _intervall;
          byte _strobeCount;
          
          void setStrobe();
          void strobe();
          
          //for colorWheel
          byte _colorWheelState;
          
          void setColorWheel();
          void colorWheel();
          
          //for rain
          int _rainPos[64];
          
          void setRain();
          void rain();
          
          //for randomCube
          int _randomCounter;
          
          void setRandomCube();
          void randomCube();
          
          //for elastic
          int _elasticPos;
          int _elasticDirect;
          
          void setElastic();
          void elastic();
          
          //for bouncePlate
          //already have a variable named like this
          //byte _swiper;
          //already have a variable named like this
          //int _pos;
          boolean _firstRun;
          
          void setBouncePlate();
          void resetBouncePlate();
          void bouncePlate();
          
          void font(int letter, int pos, int depth);
          void font(int letter, int pos, int depth, int offsetX);
          void clearFont(int letter, int pos, int depth);
          void clearFont(int letter, int pos, int depth, int offsetX);
          
          //holds the text to be shown
          byte _text[MAX_TEXT_LENGTH];
          //how long is the text
          byte _textLength;
          //converts the string into our font system
          void textToInt(String text);
          //at which position we are in our text
          byte _textPos;
          //where the letter is
          int _posLetter;
          //to save the depth of the letter
          byte _depth;
          
          //sets the right to left text
          void setRtLText(int depth, String text);
          void rtLText();
          
          //for group
          byte _state;
          byte _newState;
          byte _animationsInGroup;
          unsigned long _randomDelay;
          GROUP_TYPE _whichGroup;
          
          void setGroup();
          void resetGroup();
          void group();
          
          //for edging
          byte _edgeCoordinate[3];
          boolean _edgeDirection[3];
          int _counter;
          //boolean _firstRun;
          
          void setEdging();
          void resetEdging();
          void edging();
          
};

     //this is where we save our font
     //it is based on this: 8x8 monochrome bitmap fonts for rendering Author: Daniel Hepper <daniel@hepper.net>
     //I first used his font, but then created my own
     //the tool I wrote and used is online at www.legl.de
     const byte font8x8[][8] = 
          {
              { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0020 (space)
              { 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18}, // U+0021 (!)
              { 0x14, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0022 (")
              { 0x24, 0x24, 0xFF, 0x24, 0x24, 0xFF, 0x24, 0x24}, // U+0023 (#)
              { 0x7E, 0x19, 0x19, 0x7E, 0x98, 0x98, 0x7E, 0x18}, // U+0024 ($)
              { 0x86, 0x46, 0x20, 0x10, 0x08, 0x04, 0x62, 0x61}, // U+0025 (%)
              { 0x00, 0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E}, // U+0026 (&)
              { 0x06, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0027 (')
              { 0x18, 0x0C, 0x06, 0x06, 0x06, 0x06, 0x0C, 0x18}, // U+0028 (()
              { 0x18, 0x30, 0x60, 0x60, 0x60, 0x60, 0x30, 0x18}, // U+0029 ())
              { 0x00, 0x2A, 0x1C, 0x3E, 0x1C, 0x2A, 0x00, 0x00}, // U+002A (*)
              { 0x00, 0x18, 0x18, 0x7E, 0x7E, 0x18, 0x18, 0x00}, // U+002B (+)
              { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x06}, // U+002C (,)
              { 0x00, 0x00, 0x00, 0x7E, 0x7E, 0x00, 0x00, 0x00}, // U+002D (-)
              { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03}, // U+002E (.)
              { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01}, // U+002F (/)
              { 0x3E, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x3E}, // U+0030 (0)
              { 0x18, 0x1C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18}, // U+0031 (1)
              { 0x3C, 0x66, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x7E}, // U+0032 (2)
              { 0x3C, 0x66, 0x60, 0x38, 0x60, 0x60, 0x66, 0x3C}, // U+0033 (3)
              { 0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x30, 0x30}, // U+0034 (4)
              { 0x7E, 0x06, 0x06, 0x3E, 0x60, 0x60, 0x66, 0x3C}, // U+0035 (5)
              { 0x38, 0x0C, 0x06, 0x3E, 0x66, 0x66, 0x66, 0x3C}, // U+0036 (6)
              { 0x7E, 0x7E, 0x60, 0x60, 0x30, 0x18, 0x18, 0x18}, // U+0037 (7)
              { 0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x66, 0x3C}, // U+0038 (8)
              { 0x3C, 0x66, 0x66, 0x7C, 0x60, 0x30, 0x18, 0x0C}, // U+0039 (9)
              { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00}, // U+003A (:)
              { 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x06}, // U+003B (;)
              { 0x30, 0x18, 0x0C, 0x06, 0x06, 0x0C, 0x18, 0x30}, // U+003C (<)
              { 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00}, // U+003D (=)
              { 0x0C, 0x18, 0x30, 0x60, 0x60, 0x30, 0x18, 0x0C}, // U+003E (>)
              { 0x3C, 0x66, 0x60, 0x30, 0x18, 0x00, 0x18, 0x18}, // U+003F (?)
              { 0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x7B, 0x03, 0x1E}, // U+0040 (@)
              { 0x18, 0x3C, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66}, // U+0041 (A)
              { 0x3E, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x66, 0x3E}, // U+0042 (B)
              { 0x3C, 0x7E, 0x43, 0x03, 0x03, 0x43, 0x7E, 0x3C}, // U+0043 (C)
              { 0x1F, 0x3F, 0x63, 0x63, 0x63, 0x63, 0x3F, 0x1F}, // U+0044 (D)
              { 0x7E, 0x7E, 0x06, 0x1E, 0x1E, 0x06, 0x7E, 0x7E}, // U+0045 (E)
              { 0x7E, 0x7E, 0x06, 0x1E, 0x1E, 0x06, 0x06, 0x06}, // U+0046 (F)
              { 0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x3C, 0x18}, // U+0047 (G)
              { 0x66, 0x66, 0x66, 0x7E, 0x7E, 0x66, 0x66, 0x66}, // U+0048 (H)
              { 0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C}, // U+0049 (I)
              { 0xF0, 0x60, 0x60, 0x60, 0x66, 0x66, 0x3C, 0x18}, // U+004A (J)
              { 0x66, 0x66, 0x36, 0x1E, 0x1E, 0x36, 0x66, 0x66}, // U+004B (K)
              { 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x7E, 0x7E}, // U+004C (L)
              { 0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x63}, // U+004D (M)
              { 0x63, 0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63}, // U+004E (N)
              { 0x1C, 0x36, 0x63, 0x63, 0x63, 0x63, 0x36, 0x1C}, // U+004F (O)
              { 0x3E, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x06, 0x06}, // U+0050 (P)
              { 0x3C, 0x66, 0x66, 0x66, 0x66, 0x76, 0x3C, 0x70}, // U+0051 (Q)
              { 0x3E, 0x66, 0x66, 0x3E, 0x3E, 0x66, 0x66, 0x66}, // U+0052 (R)
              { 0x3C, 0x66, 0x06, 0x1C, 0x38, 0x60, 0x66, 0x3C}, // U+0053 (S)
              { 0x7E, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18}, // U+0054 (T)
              { 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7E, 0x3C}, // U+0055 (U)
              { 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0x66, 0x3C, 0x18}, // U+0056 (V)
              { 0x63, 0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63}, // U+0057 (W)
              { 0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x63}, // U+0058 (X)
              { 0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x18}, // U+0059 (Y)
              { 0x7F, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x7F}, // U+005A (Z)
              { 0x3C, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x3C}, // U+005B ([)
              { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80}, // U+005C (\)
              { 0x3C, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3C}, // U+005D (])
              { 0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00}, // U+005E (^)
              { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF}, // U+005F (_)
              { 0x0C, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00}, // U+0060 (`) insgesamt: 65
          };
          
          //stores whether a animation should be run in the group or not 
          const boolean _groupArray[GROUP_TYPE_QUANTITY][AN_QUANTITY+1] =
          {
               //AN_OFF, AN_GROUP, AN_1BALL, AN_2BALLS, AN_FOLDER, AN_CO_CUBE, AN_PLATE, AN_SNAKE, AN_ELASTIC, AN_BOUNCE_PLATE, AN_SINWAVE, AN_RANDOM_CUBE, AN_EXPLOSION, AN_EDGING, AN_STROBE, AN_RAIN, AN_FFT, AN_COLOR_WHEEL, AN_RTLTEXT, random order
                {false,  false,    false,    true,      true,      true,       true,     true,     true,       true,            true,       true,           false,        true,      false,     true,    false,  true,           false,      true}, //GROUP_NORMAL
                {false,  false,    true,     true,      true,      true,       true,     true,     true,       true,            true,       true,           true,         true,      true,      true,    true,   true,           true,       false}, //GROUP_ALL
                {false,  false,    false,    true,      true,      true,       true,     false,    true,       true,            true,       false,          true,         true,      true,      false,   false,  false,          false,      true}, //GROUP_PARTY
          };
          
          //sotores the time the animations in the group should be run
          const unsigned long _groupTime[GROUP_TYPE_QUANTITY][2]
          {
               {20000, 30000},
               {15000, 15000},
               {60000, 90000},
          };
#endif

/*
     instructions for a new animation
     
          1.write the animation and the set animation function in AandChandler.cpp
          2.write the needed variables and the header of the animation in AandChandler.h
          3.change the typedef AN_NUMBER
          4.insert the function in the animationHandler and setAnimation
          5.define the standard color in AandChandler::begin()
          6.update the _groupArray
          7.enlarge the menu array by increasing the LENGHT2
          8.write the name of the function in the menu begin
          9.have fun :) 
*/

