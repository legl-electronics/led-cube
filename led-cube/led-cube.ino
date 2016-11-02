/*
8x8x8 RGB-LED-Cube
 The code runs on a Arduino Due with ethernet shield
 Sadly Ethernet is not working yet, I am trying to find the problem
 For further informations please visit http://www.legl.de/led-cube.html
 If you have any questions, suggestions for improvements or other remarks please write an e-mail. 
 Special thaks to Kevin Darrah who inspired me to built a cube.
 I used his cube as role model and of course there are many accordances, so please visit also his website http://www.kevindarrah.com/
 
 This project was my first big project. As a result of this not everything is coded in the best way. I have to apologize this.
 
 
 License:
      Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License http://creativecommons.org/licenses/by-nc-sa/4.0/
      Please be honest and follow these rules
 
      legl electronics
 */



//Including all the necessary files
#include <Debouncing.h>
#include <Rotary2.h>
#include <Fft.h>
#include "Menu.h"
#include "Cube.h"
#include "AandChandler.h"
#include <DueTimer.h>        //https://github.com/ivanseidel/DueTimer thank you ivanseidel
#include <SPI.h>
#include <LiquidCrystal.h>

//Definign the pins of the lcd
#define LCD_RS 24            //pin of register select
#define LCD_ENABLE 26        //pin of lcd enable
#define LCD_D4 28            //pin of lcd data 4
#define LCD_D5 30            //pin of lcd data 5
#define LCD_D6 32            //pin of lcd data 6
#define LCD_D7 34            //pin of lcd data 7

#define STORAGE 22           //pin of the storage clock
#define OUTPUT_ENABLE 23     //pin of output enable (aktiv low)
#define CS_REGISTER 52       //a virtual Cs of the shift register, only used to have other SPI-settings for the shift registers and the ehternet shield

//the pins of the rotary
#define ROTARY_A 25            //pin of rotary a
#define ROTARY_B 27            //pin of rotary b
#define ROTARY_BUTTON 29       //pin of rotary button
#define DEBOUNCING_SAMPLES 10         //how often the same state must be read

//Running the constructors of everything
Debouncing rotaryA(ROTARY_A, DEBOUNCING_SAMPLES);
Debouncing rotaryB(ROTARY_B, DEBOUNCING_SAMPLES);
Debouncing rotaryButton(ROTARY_BUTTON, DEBOUNCING_SAMPLES);
Rotary rotary;
AandChandler AandC;
Menu menu;
Cube cube(STORAGE, OUTPUT_ENABLE, CS_REGISTER);
LiquidCrystal lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
Fft fft(20000u, FFT_FORWARD, FFT_BIT_NORMAL);

//for saving Fft data
volatile int adcData[FFT_SIZE];
volatile uint32_t currentDataPosition;
float32_t *data;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
All this funktion does, is to run the AandC.dataShiftOut.
It did not work by letting the interrupt start the AandC.dataShiftOut.
Because of this, I choose this way.
 */
void interrupt()
{
     AandC.dataShiftOut();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//the interrupt calls the data sampler

//This interrupt implementation assumes that currentDataPosition is handled outside
void interruptSampling()
{    
     // get data and store it in the current bin; please note that we overwrite the last value if not read fast enough.
     if (currentDataPosition<(FFT_SIZE)) {
          adcData[currentDataPosition++] =  analogRead(MICPIN);
     }
}

// This function is closely related the the interrup handler. It checks whether sampling is complete.
// If so, it stops the interrupt and pushes the data into the fft input data container.
// Returns 1 if sampling is complete
uint8_t samplingDone(void) {
     // all done?
     if (currentDataPosition >= (FFT_SIZE)) {
          currentDataPosition = 0;
          TIMERSAMPLER.stop();
          fft.pushDataAndConvert(adcData);
          return 1;
     }
     // not all done yet.
     return 0;
}

// Initialize the sampler.
void initSampler(void) {
     currentDataPosition = 0;
     analogReadResolution(ADC_RESOLUTION);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
     //a small delay so everything can boot up proper
     //I had some problems with the user characters on the lcd
     delay(1000);
     //Serial.begin(9600);
     //deselect the SD-Card and Ethernet
     digitalWrite(4, HIGH);
     digitalWrite(10, HIGH);
     //start everything
     menu.begin(16, 2);
     rotary.begin();
     AandC.begin();

     //start the timer with 8kHz
     //8kHz are 66fps 8000/(15*8)= 66 (15 times the 8 levels represent one picture)
     //first I had 6kHz (50fps) but then you saw it flickering
     Timer3.attachInterrupt(interrupt);
     Timer3.setFrequency(8000);
     Timer3.start();

     initSampler();

     // Attacht the data collection funciton to the other timer.
     TIMERSAMPLER.attachInterrupt(interruptSampling);
     TIMERSAMPLER.setFrequency(fft.getSamplingFrequency());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop()
{
     //run the menu and check the rotary
     menu.interactRotary();
     //shows the actuell animation and handels the backlight
     menu.actuell(false);
     //run the animation
     AandC.animationsHandler();
     //check fft
     if(samplingDone() == true && AandC.returnFFTOn() == true)
     {
          TIMERSAMPLER.start();
          if (fft.isFftDataAvailable() == FFT_DATA_AVAILABLE)
          {
               fft.calcFftForward(FFT_OCTAVE, OCTAVE_NORMALIZING_OFF);
          }
          // Save octave data
          AandC.saveOctaveData(fft.getOctaveDataPtr());
          fft.resetInputData();
          fft.resetOutputData();
     }
}



