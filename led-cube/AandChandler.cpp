/*
     AandChandler.cpp - Libary for Animation and Colour handling
     created by legl-electroinics
     known bucks -
 */
 
 
 #include "AandChandler.h"
 #include "Cube.h"
 #include "Menu.h"
 
 //TODO
 #define STORAGE 22           //pin of the storage clock
 #define OUTPUT_ENABLE 23     //pin of output enable (aktiv low)
 #define CS_REGISTER 52       //a virtual Cs of the shift register, only used to have other SPI-settings for the shift registers and the ehternet shield
 
 
 extern Cube cube;
 extern Menu menu;
 
 void AandChandler::begin()
 {
      //begins the spi with special settings only for our cs
      SPI.begin(CS_REGISTER);
      //Most Significant Bit First
      SPI.setBitOrder(CS_REGISTER,MSBFIRST);
      //Spi mode 0
      SPI.setDataMode(CS_REGISTER,SPI_MODE0);
      //set clock divider, dataspeed 84MHz / 12 = 7MHz is enough for the cube (Max F of 74HC595 55MHz)
      //in my case I can´t run the clock faster, because then the shift registers don´t get the data corectly (is a result of the long wires, capacity, ...)
      SPI.setClockDivider(CS_REGISTER, 12);
      
      //set the pins as outputs
      pinMode(STORAGE, OUTPUT);
      digitalWrite(STORAGE, LOW);
      pinMode(OUTPUT_ENABLE, OUTPUT);
      //disable the output
      digitalWrite(OUTPUT_ENABLE, HIGH);
      
      //turn all LEDs off directly (without using the blue, red and green arrays)
      //I use this to ensure that during the init all LEDs are off
      for(int i = 0; i <= 7; i++)
      {    
           SPI.transfer(CS_REGISTER,B0000000);
      }
      //turn all levels of
      SPI.transfer(CS_REGISTER,B0000000);
      //let the data save
      digitalWrite(STORAGE, HIGH);
      digitalWrite(STORAGE, LOW);
      //enable the output
      digitalWrite(OUTPUT_ENABLE, LOW);
      
      //take the pin with our mic for random seed
      //I use this because the mic pin creates a bit random values --> the first random color the cube creats is not every time the same
      randomSeed(analogRead(A0));
      
      //intit all the variables
      _red = 0;
      _green = 0;
      _blue = 0;
      _red2 = 0;
      _green2 = 0;
      _blue2 = 0;
      _colorType = STANDARD;
      _BAM_Bit = 0;
      _BAM_Counter = 0;
      _shift_out = 0;
      _anodelevel = 0;
      _level = 0;
      _anode[0]=B00000001;
      _anode[1]=B00000010;
      _anode[2]=B00000100;
      _anode[3]=B00001000;
      _anode[4]=B00010000;
      _anode[5]=B00100000;
      _anode[6]=B01000000;
      _anode[7]=B10000000;
      _animation = AN_OFF;
      _fftNew = false;
      _fftOn = false;
      
      //defines the standard color for the animation
      //TODO nach .h verlegen
      _standardC[AN_OFF] = STANDARD_RANDOM;
      _standardC[AN_GROUP] = STANDARD_RANDOM;
      _standardC[AN_1BALL] = STANDARD_RANDOM;
      _standardC[AN_2BALLS] = STANDARD_RANDOM;
      _standardC[AN_FOLDER] = STANDARD_RANDOM;
      _standardC[AN_CO_CUBE] = STANDARD_RANDOM;
      _standardC[AN_PLATE] = STANDARD_RANDOM;
      _standardC[AN_SNAKE] = STANDARD_RANDOM;
      _standardC[AN_ELASTIC] = STANDARD_RANDOM;
      _standardC[AN_BOUNCE_PLATE] = STANDARD_RANDOM;
      _standardC[AN_SINWAVE] = STANDARD_RANDOM;
      _standardC[AN_RANDOM_CUBE] = STANDARD_RANDOM;
      _standardC[AN_EXPLOSION] = STANDARD_RANDOM;
      _standardC[AN_EDGING] = STANDARD_RANDOM;
      _standardC[AN_STROBE] = STANDARD_RANDOM;
      //uses hard coded colors --> does not matter what we set here
      _standardC[AN_RAIN] = STANDARD_RANDOM;
      _standardC[AN_FFT] = STANDARD_SINGLE_RANDOM;
      _standardC[AN_COLOR_WHEEL] = STANDARD_RANDOM;
      _standardC[AN_RTLTEXT] = STANDARD_SINGLE_RANDOM;

      //start the cube with the group animation
      _whichGroup = GROUP_NORMAL;
      setAnimation(AN_GROUP);
      //run group once
      group();
      //show actuell
      menu.actuell(true);
 }
 
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//sets the color 
 void AandChandler::setRed(byte red)
 {
      if(red <= 15)
      {
           _red = red;
      }
      else
      {
           cube.error("red");
      }
 }
 
 void AandChandler::setGreen(byte green)
 {
      if(green <= 15)
      {
           _green = green;
      }
      else
      {
           cube.error("green");
      }
 }

 void AandChandler::setBlue(byte blue)
 {
      if(blue <= 15)
      {
           _blue = blue;
      }
      else
      {
           cube.error("blue");
      }
 }
 
 void AandChandler::setRed2(byte red)
 {
      if(red <= 15)
      {
           _red2 = red;
      }
      else
      {
           cube.error("red 2");
      }
 }
 
 void AandChandler::setGreen2(byte green)
 {
      if(green <= 15)
      {
           _green2 = green;
      }
      else
      {
           cube.error("green 2");
      }
 }

 void AandChandler::setBlue2(byte blue)
 {
      if(blue <= 15)
      {
           _blue2 = blue;
      }
      else
      {
           cube.error("blue 2");
      }
 }
 
 //returns the value of the colors (is only needed because the color variables are private)
 byte AandChandler::getRed()
 {
     return _red;
 }
 
 byte AandChandler::getBlue()
 {
     return _blue;
 }
 byte AandChandler::getGreen()
 {
     return _green;
 }
  byte AandChandler::getRed2()
 {
     return _red2;
 }
  byte AandChandler::getBlue2()
 {
     return _blue2;
 }
  byte AandChandler::getGreen2()
 {
     return _green2;
 }
 
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
 //sets the color type
 void AandChandler::setColorType(byte colorType)
 {
      if(colorType < COLOR_TYPE_QUANTITY)
      {
           _colorType = colorType;
      }
      else
      {
           cube.error("color type");
      }
 }
 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//constrains the position of the LEDs, only use this when you really want the effect
//if not the normal LED methode is a lot more efficient and you get the respons (by an error) wether the LED relly exist (the LED you wanted to do something with)
void AandChandler::onlyWhenInsideLED(int column, int level, int row, byte red, byte green, byte blue)
{
     column = constrain(column, 0, 7);
     level = constrain(level, 0, 7);
     row = constrain(row, 0, 7);
     //use the normal LED methode with constrained values     
     LED(column, level, row, red, green, blue);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
 void AandChandler::LED(byte column, byte level, byte row, byte red, byte green, byte blue)
 {                     //    x     , y         , z centre
      //check the column, level, row and the colors if they are in the limits. If not we print an error so we recognize that there is buck and can fix it
      if(column > 7)
      {
           cube.error("LED column");
      }
      if(column < 0)
      {
           cube.error("LED column");
      }      
      if(level > 7)
      {
           cube.error("LED level");
      }
      if(level < 0)
      {
           cube.error("LED level");
      }      
      if(row > 7)
      {
           cube.error("LED row");
      }
      if(row < 0)
      {
           cube.error("LED row");
      }      
      if(red > 15)
      {
           cube.error("LED red");
      }
      if(red < 0)
      {
           cube.error("LED red");
      }      
      if(green > 15)
      {
           cube.error("LED green");
      }
      if(green < 0)
      {
           cube.error("LED green");
      }      
      if(blue > 15)
      {
           cube.error("LED blue");
      }
      if(blue < 0)
      {
           cube.error("LED blue");
      }      
      
      //There are 512 LEDs in the cube, so when we write to column 5, level 2, row 4, that needs to be translated into a number from 0 to 511
      int whichbyte = int(((level*64)+(row*8)+column)/8);
  
      //The column counts left to right 0-7 and the row is front to back 0-7
      //This means that if you had level 0, row 0, the bottom front row would count from 0-7, 
      //so if you look down on the cube, and only look at the bottom level 
 
      //        back
      // 56 57 58 59 60 61 62 63
      // 48 49 50 51 52 53 54 55 
      // 40 41 42 43 44 45 46 47 
      // 32 33 34 35 36 37 38 39
      // 24 25 26 27 28 29 30 31
      // 16 17 18 19 20 21 22 23
      // 08 09 10 11 12 13 14 15
      // 00 01 02 03 04 05 06 07
      //         front
      
      //The first level LEDs are first in the sequence, then 2nd level, then third, and so on
      //the (level*64) is what indexes the level's starting place, so level 0 are LEDs 0-63, level 1 are LEDs 64-127, and so on
      //looking on the cube from the front
      
      //             top
      // 448 449 450 451 452 453 454 455
      // 384 385 386 387 388 389 340 341
      // 320 321 322 323 324 325 326 327
      // 256 256 258 259 260 261 262 263
      // 192 193 194 195 196 197 198 199
      // 128 129 130 131 132 133 134 135
      // 064 065 066 067 068 069 070 071
      // 000 001 002 003 004 005 006 007
      //            buttom
      
      
      //Now, what about the divide by 8 in there?
      //...well, we have 8 bits per byte, and we have 64 bytes in memory for all 512 bits needed for each LED, so
      //we divide the number we just found by 8, and take the integer of it, so we know which byte, that bit is located
      //confused? that's ok, let's take an example, if we wanted to write to the LED to the last LED in the cube, we would write a 7, 7, 7
      // giving (7*64)+(7*8)=7 = 511, which is right, but now let's divide it by 8, 511/8 = 63.875, and take the int of it so, we get 63,
      //this is the last byte in the array, which is right since this is the last LED
      
      // This next variable is the same thing as before, but here we don't divide by 8, so we get the LED number 0-511
      int wholebyte=(level*64)+(row*8)+column;
      //This will all make sense in a sec
      
       //This is 4 bit color resolution, so each color contains 4 x 64 byte arrays, explanation below:
       bitWrite(red0[whichbyte], wholebyte-(8*whichbyte), bitRead(red, 0));
       bitWrite(red1[whichbyte], wholebyte-(8*whichbyte), bitRead(red, 1));
       bitWrite(red2[whichbyte], wholebyte-(8*whichbyte), bitRead(red, 2)); 
       bitWrite(red3[whichbyte], wholebyte-(8*whichbyte), bitRead(red, 3)); 

       bitWrite(green0[whichbyte], wholebyte-(8*whichbyte), bitRead(green, 0));
       bitWrite(green1[whichbyte], wholebyte-(8*whichbyte), bitRead(green, 1));
       bitWrite(green2[whichbyte], wholebyte-(8*whichbyte), bitRead(green, 2)); 
       bitWrite(green3[whichbyte], wholebyte-(8*whichbyte), bitRead(green, 3));

       bitWrite(blue0[whichbyte], wholebyte-(8*whichbyte), bitRead(blue, 0));
       bitWrite(blue1[whichbyte], wholebyte-(8*whichbyte), bitRead(blue, 1));
       bitWrite(blue2[whichbyte], wholebyte-(8*whichbyte), bitRead(blue, 2)); 
       bitWrite(blue3[whichbyte], wholebyte-(8*whichbyte), bitRead(blue, 3));
       
       //Are you now more confused?  You shouldn't be!  It's starting to make sense now.  Notice how each line is a bitWrite, which is
       //bitWrite(the byte you want to write to, the bit of the byte to write, and the 0 or 1 you want to write)
       //This means that the 'whichbyte' is the byte from 0-63 in which the bit coresponding to the LED from 0-511
       //Is making sense now why we did that? taking a value from 0-511 and converting it to a value from 0-63, since each LED represents a bit in 
       //an array of 64 bytes.
       //Then next line is which bit 'wholebyte-(8*whichbyte)'  
       //This is simply taking the LED's value of 0-511 and subracting it from the BYTE its bit was located in times 8
       //Think about it, byte 63 will contain LEDs from 504 to 511, so if you took 505-(8*63), you get a 1, meaning that,
       //LED number 505 is is located in bit 1 of byte 63 in the array
     
       //is that it?  No, you still have to do the bitRead of the brightness 0-15 you are trying to write,
       //if you wrote a 15 to RED, all 4 arrays for that LED would have a 1 for that bit, meaning it will be on 100%
       //This is why the four arrays read 0-4 of the value entered in for RED, GREEN, and BLUE
       //hopefully this all makes some sense?
 }
 
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//shifts the data out
//is the interrupt
 void AandChandler::dataShiftOut()
 {
      //turn the LEDs off
      digitalWrite(OUTPUT_ENABLE, HIGH);
      
      //This is 4 bit 'Bit angle Modulation' or BAM, There are 8 levels, so when a '1' is written to the color brightness, 
      //each level will have a chance to light up for 1 cycle, the BAM bit keeps track of which bit we are modulating out of the 4 bits
      //Bam counter is the cycle count, meaning as we light up each level, we increment the BAM_Counter
      //first bit
      if(_BAM_Counter==8)
      _BAM_Bit++;
      else
      //second bit 8+(2*8) = 24
      if(_BAM_Counter==24)
      _BAM_Bit++;
      else
      //third bit 24+(4*8) = 56
      if(_BAM_Counter==56)
      _BAM_Bit++;
       
      _BAM_Counter++;//Here is where we increment the BAM counter    
      
      //look which array we want to shift out
      switch (_BAM_Bit)
      {
           case 0:
                for(_shift_out=_level; _shift_out<_level+8; _shift_out++)
                {
                     SPI.transfer(CS_REGISTER, red0[_shift_out]);
                     SPI.transfer(CS_REGISTER, green0[_shift_out]); 
                     SPI.transfer(CS_REGISTER, blue0[_shift_out]);
                }
                break;
           
           case 1:
                for(_shift_out=_level; _shift_out<_level+8; _shift_out++)
                {
                     SPI.transfer(CS_REGISTER, red1[_shift_out]);
                     SPI.transfer(CS_REGISTER, green1[_shift_out]); 
                     SPI.transfer(CS_REGISTER, blue1[_shift_out]);
                }
                break;
           
           case 2:
                for(_shift_out=_level; _shift_out<_level+8; _shift_out++)
                {
                     SPI.transfer(CS_REGISTER, red2[_shift_out]);
                     SPI.transfer(CS_REGISTER, green2[_shift_out]); 
                     SPI.transfer(CS_REGISTER, blue2[_shift_out]);
                }
                break;

           case 3:
                for(_shift_out=_level; _shift_out<_level+8; _shift_out++)
                {
                     SPI.transfer(CS_REGISTER, red3[_shift_out]);
                     SPI.transfer(CS_REGISTER, green3[_shift_out]); 
                     SPI.transfer(CS_REGISTER, blue3[_shift_out]);
                } 
                 
               //Here is where the BAM_Counter is reset back to 0, it's only 4 bit, but since each cycle takes 8 counts,
               //it goes 0 8 24 56, and when BAM_counter hits 120 we reset the BAM
               if(_BAM_Counter==120)
               {
                    _BAM_Counter=0;
                    _BAM_Bit=0;
               }
               break;
               
           default:
                cube.error("shiftOut");
                break;
     }
     
     SPI.transfer(CS_REGISTER, _anode[_anodelevel]);//finally, send out the anode level byte
     
     //storage pin HIGH
     digitalWrite(STORAGE, HIGH);
     
     //storage pin LOW
     digitalWrite(STORAGE, LOW);
     
     //output enable pin LOW to turn on the LEDs with the new data
     digitalWrite(OUTPUT_ENABLE, LOW);

     _anodelevel++;//inrement the anode level
     _level = _level+8;//increment the level variable by 8, which is used to shift out data, since the next level would be the next 8 bytes in the arrays

     if(_anodelevel==8)//go back to 0 if the top is reached
     {
          _anodelevel=0;
     }
     if(_level==64)//if you hit 64 on level, this means you just sent out all 63 bytes, so go back
     {
          _level=0;
     }
           
 }
 
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
 //creates a new color
 //it considers the actuel color type
 void AandChandler::newColor(byte animation)
 {
      switch(_colorType)
      {
           case USER:
                //if we have a user color, we don´t want to change the color
                break;
                
           case RANDOM:
                randomColor();
                break;
               
           case STANDARD:
                //look what the standard color is
                switch(_standardC[animation])
                {
                     case STANDARD_RANDOM:
                          randomColor();
                          break;
                          
                     case STANDARD_SINGLE_RANDOM:
                          singleRandomColor();
                          break;
                          
                     default:
                          cube.error("S not imp");
                          break;
                }
                break;
                
           default:
                cube.error("new Color");
                return;
      }
 }
 //the same for the second color
 void AandChandler::newColor2(byte animation)
 {
      switch(_colorType)
      {
           case USER:
                break;
                
           case RANDOM:
                randomColor2();
                break;
               
           case STANDARD:
                switch(_standardC[animation])
                {
                     case STANDARD_RANDOM:
                          randomColor2();
                          break;
                          
                     case STANDARD_SINGLE_RANDOM:
                          singleRandomColor2();
                          break;
                          
                     default:
                          cube.error("S not imp");
                          break;
                }
                break;
                
           default:
                cube.error("new Color");
                return;
      }
 } 
           
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
 //creates a random color
 void AandChandler::randomColorX(byte &red, byte &green, byte &blue)
 {
      byte select = random(0,7);
      switch(select)
      {
           case 0:
                red = 15;
                green = 0;
                blue = 0;
                break;
           
           case 1:
                red = 0;
                green = 15;
                blue = 0;
                break;
                
           case 2:
                red = 0;
                green = 0;
                blue = 15;
                break;
                
           case 3:
                red = random(10,16);
                green = random(10,16);
                blue = 0;
                break;
           
           case 4:
                red = 0;
                green = random(10,16);
                blue = random(10,16);
                break;
            
           case 5:
                red = random(10,16);
                green = 0;
                blue = random(10,16);
                break;
                
           case 6:
                red = random(5,11);
                green = random(5,11);
                blue = random(5,11);
                break;
                
           default:
                cube.error("randomC");
                break;
      } 
}      

void AandChandler::randomColor()
{
     randomColorX(_red, _green, _blue);                
}
 
void AandChandler::randomColor2()
{
     randomColorX(_red2, _green2, _blue2);                
} 
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
 //creates randomly red, green or blue     
 void AandChandler::singleRandomColorX(byte &red, byte &green, byte &blue)
 {
      byte select = random(0,3);
      switch(select)
      {
           case 0:
                red = 15;
                green = 0;
                blue = 0;
                break;
           
           case 1:
                red = 0;
                green = 15;
                blue = 0;
                break;
                
           case 2:
                red = 0;
                green = 0;
                blue = 15;
                break;
                
           default:
                cube.error("randomC");
                break;
      }                
 }
 
 void AandChandler::singleRandomColor()
 {
      singleRandomColorX(_red, _green, _blue);                    
 }
 
 void AandChandler::singleRandomColor2()
 {
      singleRandomColorX(_red2, _green2, _blue2);                    
 }
 
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
 //inits our animations
  void AandChandler::setAnimation(byte animation)
  {
      //if we are in the boundary
      if(animation < AN_QUANTITY)
      {
           //and have a different new animation we want to init the animation
           if( (_animation != animation) || (animation == AN_GROUP) )
           {
                //says the menu that there is a new animation running
                menu.actuell(true);
                //clear the color arrays
                all(0,0,0);
                //turn of the fft
                _fftOn = false;
                //look which animation should be innit
                switch(animation)
                {
                     case AN_OFF:
                          setOff();
                          break;
                          
                     case AN_GROUP:
                          setGroup();
                          break;
                          
                     case AN_1BALL:
                          //if we had the 2 balls animation running we don´t want to inint the ball 1 again
                          if(_animation != AN_2BALLS)
                          { 
                               setBall1();
                          }
                          break;
                          
                     case AN_2BALLS:
                          //if we had the 1 ball animation running we don´t want to inint the ball 1 again
                          if(_animation != AN_1BALL)
                          {
                                    setBall1();
                          }
                          setBall2();
                          break;
                          
                     case AN_FOLDER:
                          setFolder();
                          break;
                          
                     case AN_CO_CUBE:
                          setColorCube();
                          break;     
                        
                     case AN_PLATE:
                          setPlate();
                          break;
                          
                     case AN_SNAKE:
                          setSnake();
                          break;  
                       
                     case AN_ELASTIC:
                          setElastic();
                          break;
                          
                     case AN_BOUNCE_PLATE:
                          setBouncePlate();
                          break;
                          
                     case AN_SINWAVE:
                          setSinwave();
                          break;
                          
                     case AN_RANDOM_CUBE:
                          setRandomCube();
                          break;
                          
                     case AN_EXPLOSION:
                          setExplosion();
                          break;
                          
                     case AN_EDGING:
                          setEdging();
                          break;
                          
                     case AN_STROBE:
                          setStrobe();
                          break;
                          
                     case AN_RAIN:
                          setRain();
                          break;
                          
                     case AN_FFT:
                          setFft();
                          break;
                          
                     case AN_COLOR_WHEEL:
                          setColorWheel();
                          break;                          
                          
                     case AN_RTLTEXT:
                          setRtLText(8,"legl-electronics");
                          break; 
                          
                     default:
                          cube.error("anim set");
                          break;
                }
                //write the new animation to _animation
                _animation = animation;
           }
      }
      //if we have a false animation we want to print an error
      else
      {
           cube.error("animation");
      }
 }
 
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
 void AandChandler::setWhichGroup(GROUP_TYPE whichGroup)
 {
      _whichGroup = whichGroup;
 }     
 
 GROUP_TYPE AandChandler::returnWhichGroup()
{
     return _whichGroup;
}
 
 //animation all runs some different animations
 void AandChandler::setGroup()
 {
      _animationsInGroup = 0;
      _newState = 0;
      for(int i=0; i < AN_QUANTITY; i++)
      {
           if(_groupArray[_whichGroup][i] == true)
           {
                _animationsInGroup ++;
           }     
      }
      if(_animationsInGroup == 0)
      {
           cube.error("0 group");
      }
      resetGroup();
 }
 
 void AandChandler::resetGroup()
 {
      _lastChange = millis();
      _randomDelay = random(_groupTime[_whichGroup][0], _groupTime[_whichGroup][1] + 1);
      
      //check whether the group runs in order or in random order
      if(_groupArray[_whichGroup][AN_QUANTITY] == true)
      {     
           _newState = random(1, _animationsInGroup + 1);
      }
      else
      {
           _newState ++;
           if(_newState > _animationsInGroup)
           {
                _newState = 1;
           }
      }
      int counter = 0;
      for(int i=0; i < AN_QUANTITY; i++)
      {
           if(_groupArray[_whichGroup][i] == true)
           {
                counter ++;
                if(counter == _newState)
                {
                     //if we already running the animation we want to set as new one, we don´t want to reset it
                     if(_state != i)
                     {
                          _state = i;
                          setAnimation(_state);
                     }     
                     return;
                }
           }
      }
 }
 void AandChandler::group()
 {  
      if(_lastChange+_randomDelay < millis())
      {
           resetGroup();
      }
      else
      {
           //if we didn´t increase the state we set the animation directly so the set methode isn´t called
           //if we won´t change the _animation variable the group animation would be called again and not the animation we want to run
           _animation = _state;
      }
      //run the animation
      animationsHandler();
      //set the animation to this(group animation)
      //if we won´t do this, the group animation (this animation) wouldn´t be called any more because we set the animation to the state (animation which lets the group animation run)
      _animation = AN_GROUP;
 }
 

 
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
 //are used in the menu, that we can show the running animation
 byte AandChandler::returnState()
 {
      return _state;
 }
 
 byte AandChandler::returnAnimation()
 {
      return _animation;
 }
 
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
 //looks which animation should be running
 void AandChandler::animationsHandler()
 {
      switch(_animation)
      {
           case AN_OFF:
                off();
                break;
                
           case AN_GROUP:
                group();
                break;
                
           case AN_1BALL:
                ball1();
                break;
                
           case AN_2BALLS:
                ball1();
                ball2();
                break;
                
           case AN_FOLDER:
                folder();
                break;
           
           case AN_CO_CUBE:
                colorCube();
                break;
              
           case AN_PLATE:
                plate();
                break;
                
           case AN_SNAKE:
                snake();
                break;
           
           case AN_ELASTIC:
                elastic();
                break; 
      
           case AN_BOUNCE_PLATE:
                bouncePlate();
                break;          
                
           case AN_SINWAVE:
                sinwave();
                break;
                
           case AN_RANDOM_CUBE:
                randomCube();
                break;
                
           case AN_EXPLOSION:
                explosion();
                break;
                
           case AN_EDGING:
                edging();
                break;
                
           case AN_STROBE:
                strobe();
                break;

           case AN_RAIN:
                rain();
                break;           
                
           case AN_FFT:
                fft();
                break;
                
           case AN_COLOR_WHEEL:
                colorWheel();
                break;                
                
           case AN_RTLTEXT:
                rtLText();
                break;    
                
           default:
                cube.error("a Handler");
                break;
      }
 }
                
 
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
 //turns all LEDs off
 void AandChandler::setOff()
 {
      all(0,0,0);
 }
 
 // if we run the off animation nothing should be done
 void AandChandler::off()
 {
      ;
 }
 
 //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//this method is used for ball 1 and 2 
void AandChandler::setBall(int &X, int &Y, int &Z, int &Xo, int &Yo, int &Zo, int &Xm, int &Ym, int &Zm)
{
     //reset the timer
     _timer = 0;
     //create a random position for the ball
     X = random(0,8);
     Y = random(0,8);
     Z = random(0,8);
     //set the old position
     //(you could write anything to them, but it has to be in the boundary 0-7)
     Xo = X;
     Yo = Y;
     Zo = Z;
     //set the direction to which it will move
     Xm = 1;
     Ym = 1;
     Zm = 1;
}

//this method is used for ball 1 and 2 
 void AandChandler::ball(int &X, int &Y, int &Z, int &Xo, int &Yo, int &Zo, int &Xm, int &Ym, int &Zm, byte &red, byte &green, byte &blue)
 {
      if(_timer + 20 < millis() || _timer == 0)
      {
           //reset the timer
          _timer = millis();
          //clear the old ball
          LED(Xo, Yo, Zo, 0, 0, 0);
          //we have to look if the LED exist we want to clear
          if(Zo < 7)
          {
               LED(Xo, Yo, Zo+1, 0, 0, 0);
          }
          if(Zo > 0)
          {
               LED(Xo, Yo, Zo-1, 0, 0, 0);
          }
          if(Xo < 7)
          {
               LED(Xo+1, Yo, Zo, 0, 0, 0);
          }
          if(Xo > 0)
          {
               LED(Xo-1, Yo, Zo, 0, 0, 0);
          }
          if(Yo < 7)
          {
               LED(Xo, Yo+1, Zo, 0, 0, 0);
          }
          if(Yo > 0)
          {
               LED(Xo, Yo-1, Zo, 0, 0, 0);
          }     
          
          //if we don´t do this, the ball would bounce every time the same
          //so we let it move only in one direction
          int direct = random(0,3);
          if(direct == 0)
               X= X + Xm;
          if(direct == 1)
               Y= Y + Ym;  
          if(direct == 2)
               Z= Z + Zm;
          
          //look that the position is in the cube and set the directions     
          if(X<0){
              X=0; Xm=1;}
          if(X>7){
              X=7; Xm=-1;}  
          if(Y<0){
              Y=0; Ym=1;}
          if(Y>7){
              Y=7; Ym=-1;}    
          if(Z<0){
              Z=0; Zm=1;}
          if(Z>7){
              Z=7; Zm=-1;}  
      
          //draw the new ball
          LED(X, Y, Z, red, green, blue);
          //of course we have to check first wether the LED exist
          //(we are only looking at the center of the ball)
          //the ball looks like
          //    X
          //  X C X
          //    X
          //because of this the x LEDs could be outside the cube
          if(Z < 7)
          {
               LED(X, Y, Z+1, red, green, blue);
          }
          if(Z > 0)
          {
               LED(X, Y, Z-1, red, green, blue);
          }
          if(X < 7)
          {
               LED(X+1, Y, Z, red, green, blue);
          }
          if(X > 0)
          {
               LED(X-1, Y, Z, red, green, blue);
          }
          if(Y <7)
          {
               LED(X, Y+1, Z, red, green, blue);
          }
          if(Y > 0)
          {
               LED(X, Y-1, Z, red, green, blue);
          }
          
          //the new positions are the new old    
          Xo=X;
          Yo=Y;
          Zo=Z; 
      }
}

//the set ball methode with the variables of ball 1
void AandChandler::setBall1()
{
     setBall(_X1, _Y1, _Z1, _Xo1, _Yo1, _Zo1, _Xm1, _Ym1, _Zm1);
     newColor(AN_1BALL);     
}

//the ball methode with the variables of ball 1
void AandChandler::ball1()
{
     //if we run the 2 balls-animation we have to put the timer to it´s old state, otherwise ball 2 wouldn´t run
     if(_animation == AN_2BALLS)
     {
          unsigned long timer = _timer;
          ball(_X1, _Y1, _Z1, _Xo1, _Yo1, _Zo1, _Xm1, _Ym1, _Zm1, _red, _green, _blue);
          _timer = timer;
     }
     else
     {
          ball(_X1, _Y1, _Z1, _Xo1, _Yo1, _Zo1, _Xm1, _Ym1, _Zm1, _red, _green, _blue);
     }
}

//the set ball methode with the variables of ball 2
void AandChandler::setBall2()
{
     setBall(_X2, _Y2, _Z2, _Xo2, _Yo2, _Zo2, _Xm2, _Ym2, _Zm2);
     newColor2(AN_2BALLS);     
}

//the ball methode with the variables of ball 2
void AandChandler::ball2()
{
     ball(_X2, _Y2, _Z2, _Xo2, _Yo2, _Zo2, _Xm2, _Ym2, _Zm2, _red2, _green2, _blue2);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AandChandler::setPlate()
{
     _Yo = 0;
     _timer = 0;
}

void AandChandler::plate()
{
     if(_timer+200<millis() || _timer == 0)
     {
          //reset the timer
          _timer = millis();
          
          //remove the old plate
          for(int x=0; x<8; x++)
          {
               for(int z=0; z<8; z++)
                    {
                         LED(x, _Yo, z, 0,0,0);
                    }
          }
          
          //a random plate and color  
          byte y = random(0,8);
          newColor(AN_PLATE);
          
          //draw the new
          for(int x=0; x<8; x++)
          {
               for(int z=0; z<8; z++)
               {
                    LED(x, y, z, _red, _green, _blue);
               }
          }
          //save the new plate to the new old
          _Yo = y;
       }
}    

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AandChandler::setSinwave()
{
     for (int i=0; i<8; i++)
     {
          sinewave[i]=i;
          sinewaveM[i]=1;
     }
     _timer = 0;
}

void AandChandler::sinwave()
{
    if(_timer+50 < millis() || _timer == 0)
    {
         //reset the timer
         _timer = millis();
         
         //change the directions if it is at the margin
         for(int i=0; i<8; i++)
         {
              if(sinewave[i]==7)
              {
                   sinewaveM[i]=-1;
              }
              if(sinewave[i]==0)
              {
                   sinewaveM[i]=1;     
              }
              //make the new positions
              sinewave[i] = sinewave[i] + sinewaveM[i];
         }
         //create a new color
         if(sinewave[0]==7)
         {
              newColor(AN_SINWAVE);
         }    
         
         
         for(int i=0; i<8; i++)
         {
              //clear the old
              LED(sinewaveOLD[i], i, 0, 0, 0, 0);
              LED(sinewaveOLD[i], 0, i, 0, 0, 0);
              LED(sinewaveOLD[i], 7-i, 7, 0, 0, 0);
              LED(sinewaveOLD[i], 7, 7-i, 0, 0, 0);
              //draw the new     
              LED(sinewave[i], i, 0, _red, _green, _blue);
              LED(sinewave[i], 0, i, _red, _green, _blue);
              LED(sinewave[i], 7-i,7, _red, _green, _blue);
              LED(sinewave[i], 7, 7-i, _red, _green, _blue);
         }
         
         for(int i=1; i<7; i++)
         {   
              LED(sinewaveOLD[i], i, 1, 0, 0, 0);
              LED(sinewaveOLD[i], 1, i, 0, 0, 0);
              LED(sinewaveOLD[i], 7-i, 6, 0, 0, 0);
              LED(sinewaveOLD[i], 6, 7-i, 0, 0, 0);  
              LED(sinewave[i], i, 1, _red, _green, _blue);
              LED(sinewave[i], 1, i, _red, _green, _blue);
              LED(sinewave[i], 7-i,6, _red, _green, _blue);
              LED(sinewave[i], 6, 7-i, _red, _green, _blue);
       }
       
       for(int i=2; i<6; i++)
       {   
            LED(sinewaveOLD[i], i, 2, 0, 0, 0);
            LED(sinewaveOLD[i], 2, i, 0, 0, 0);
            LED(sinewaveOLD[i], 7-i, 5, 0, 0, 0);
            LED(sinewaveOLD[i], 5, 7-i, 0, 0, 0);  
            LED(sinewave[i], i, 2, _red, _green, _blue);
            LED(sinewave[i], 2, i, _red, _green, _blue);
            LED(sinewave[i], 7-i,5, _red, _green, _blue);
            LED(sinewave[i], 5, 7-i, _red, _green, _blue);
       }  
       
       for(int i=3; i<5; i++)
       {   
            LED(sinewaveOLD[i], i, 3, 0, 0, 0);
            LED(sinewaveOLD[i], 3, i, 0, 0, 0);
            LED(sinewaveOLD[i], 7-i, 4, 0, 0, 0);
            LED(sinewaveOLD[i], 4, 7-i, 0, 0, 0);  
            LED(sinewave[i], i, 3, _red, _green, _blue);
            LED(sinewave[i], 3, i, _red, _green, _blue);
            LED(sinewave[i], 7-i,4, _red, _green, _blue);
            LED(sinewave[i], 4, 7-i, _red, _green, _blue);
       }      
       
       //save the new positions as the new old
       for(int i=0; i<8; i++)
       {
            sinewaveOLD[i]=sinewave[i];
       }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AandChandler::setColorCube()
{
     _timer = 0;
     //saves the direction
     _swiper=random(0,6);
     //if we increas the old position we start at 0
     if(_swiper <=2)
     {
          _pos = 0;
     }
     //if we decrease it we start at 7
     else
     {
          _pos = 7;
     }  
     newColor(AN_CO_CUBE);
}

void AandChandler::colorCube()
{
     if(_timer+40 < millis() || _timer == 0)
     {
          //reset the timer
          _timer=millis();
          
          //the cube has run through the animation
          //because of that we reset the animation    
          if(_swiper<=2 && _pos == 8)
          {
               setColorCube();
          }
        
          if(_swiper>=3 && _pos == -1)
          {
               setColorCube();
          }
          
          switch(_swiper)
          {
             case 0: 
                  //left to right
                  for(int x=0; x<8; x++)
                  {
                       for(int z=0;z<8;z++)
                       {
                            LED(x, _pos, z,  _red, _green, _blue);
                       }
                  }
                  _pos++;
                  break;
    
             case 1:
                  //bot to top
                  for(int y=0;y<8;y++)
                  {
                       for(int z=0;z<8;z++)
                       {
                            LED(_pos, y, z,  _red, _green, _blue);
                       }
                  }
                  _pos++;
                  break;
             
             case 2:
                  //front to back
                  for(int x=0;x<8;x++)
                  {
                       for(int y=0;y<8;y++)
                       {
                            LED(x, y, _pos,  _red, _green, _blue);
                       }
                  }
                  _pos++;
                  break;
             
             case 3:
                  //right to left
                  for(int x=0;x<8;x++)
                  {
                       for(int z=0;z<8;z++)
                       {
                            LED(x, _pos, z,  _red, _green, _blue);
                       }
                  }
                  _pos--;
                  break;
             
             case 4:
                  //top to bot
                  for(int y=0;y<8;y++)
                  {
                       for(int z=0;z<8;z++)
                       {
                            LED(_pos, y, z,  _red, _green, _blue);
                       }
                  }
                  _pos--;
                  break;
            
             case 5:
                  //back to front
                  for(int x=0; x<8; x++)
                  {
                       for(int y=0; y<8; y++)
                       {
                            LED(x, y, _pos,  _red, _green, _blue);
                       }
                  }
                  _pos--;
                  break;
                  
             default:
                  cube.error("colorCube");
                  break;
        }
   }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AandChandler::setSnake()
{
     newColor(AN_SNAKE);
     int Random = random(0,8);
     for(int i=0; i<20; i++)
     {
          //saving the start position and writing possible values in the arrays
          _x[i]=Random;
          _y[i]=Random;
          _z[i]=Random;
          _redA[i]=_red;
          _greenA[i]=_green;
          _blueA[i]=_blue;
     }
     _Xm=1; 
     _Ym=1; 
     _Zm=1;
     _timer=0;
     //running the snake animation 20 times without delay to create a snake
     for(int i=0; i<20; i++)
     {
          snake();
          _timer = 0;
     }
}

void AandChandler::snake()
{
     if(_timer+50 < millis() || _timer == 0)
     {
          //reset the timer
          _timer = millis();
          
          //clear the last LED
          LED(_x[19],_y[19], _z[19], 0, 0, 0);
          
          //the first LED is now the second, the third now the fourth, ... 
          for(int i=19; i>0; i--)
          {
               _x[i] = _x[i-1];
               _y[i] = _y[i-1];
               _z[i] = _z[i-1];
               _redA[i] = _redA[i-1];
               _greenA[i] = _greenA[i-1];
               _blueA[i] = _blueA[i-1];
          }
          
          //which direction
          int Random = random(3);
          if(Random==0)
               _x[0]= _x[0]+_Xm;
          if(Random==1)
               _y[0]= _y[0]+_Ym;
          if(Random==2)
               _z[0]= _z[0]+_Zm;
     
          //is the snake at the margin?
          //if it is, change the direction, set the position to a possible position and create a new color
          if(_x[0]>7)
          {
               _Xm=-1;
               _x[0]=7;
               newColor(AN_SNAKE);
               _redA[0]=_red;
               _greenA[0]=_green;
               _blueA[0]=_blue;
          }
          if(_x[0]<0)
          {
               _Xm=1;
               _x[0]=0;
               newColor(AN_SNAKE);
               _redA[0]=_red;
               _greenA[0]=_green;
               _blueA[0]=_blue;
          }
          if(_y[0]>7)
          {
               _Ym=-1;
               _y[0]=7;
               newColor(AN_SNAKE);
               _redA[0]=_red;
               _greenA[0]=_green;
               _blueA[0]=_blue;
          }
          if(_y[0]<0)
          {
               _Ym=1;
               _y[0]=0;
               newColor(AN_SNAKE);
               _redA[0]=_red;
               _greenA[0]=_green;
               _blueA[0]=_blue;
          }
          if(_z[0]>7)
          {
               _Zm=-1;
               _z[0]=7;
               newColor(AN_SNAKE);
               _redA[0]=_red;
               _greenA[0]=_green;
               _blueA[0]=_blue;
          }
          if(_z[0]<0)
          {
               _Zm=1;
               _z[0]=0;
               newColor(AN_SNAKE);
               _redA[0]=_red;
               _greenA[0]=_green;
               _blueA[0]=_blue; 
          }
          
          //draw the snake
          for(int i=0; i<20; i++)
          {
               LED(_x[i], _y[i], _z[i], _redA[i], _greenA[i], _blueA[i]);
          }
     }
}
         
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AandChandler::setExplosion()
{
     _pos = -1;
     _direct = 1;    
     _timer = 0;
}

void AandChandler::explosion()
{  
     if(_timer+150 < millis() || _timer == 0)
     {
          //reset the timer
          _timer = millis();
          
          //when we run the set-methode the position is -1
          //than we don´t want to clear the old
          if(_pos != -1)
          {
               //clear the old
               for(int x = _pos; x <= (7-_pos); x++)
               {
                    for(int y = _pos; y <= (7-_pos); y++)
                    {
                         //front
                         LED(x,y,_pos,0,0,0);
                         //back
                         LED(x,y,(7-_pos),0,0,0);
                    }
               }
               
               for(int x = _pos; x <= (7-_pos); x++)
               {
                    for(int z = _pos; z <= (7-_pos); z++)
                    {
                         //top
                         LED(x,_pos,z,0,0,0);
                         //buttom
                         LED(x,(7-_pos),z,0,0,0);
                     }
               }  
         
               for(int y = _pos; y <= (7-_pos); y++)
               {
                    for(int z = _pos; z <= (7-_pos); z++)
                    {
                         //left
                         LED(_pos,y,z,0,0,0);
                         //right
                         LED((7-_pos),y,z,0,0,0);
                     }
                }
           }
           
           //calculate new position
           _pos = _pos + _direct;
           
           //if we are at the middle we have to change the direction
           if(_pos == 4)
           {
                _pos = 2;
                _direct = -1;
           }
           //and when we are at the outside we have to do it too
           if(_pos == -1)
           {
                newColor(AN_EXPLOSION);
                _direct = 1;
                _pos = 0;
           }
           
          
          //draw the cube 
          for(int x = _pos; x <= (7-_pos); x++)
          {
               for(int y = _pos; y <= (7-_pos); y++)
               {
                    //front
                    LED(x,y,_pos,_red,_green,_blue);
                    //back
                    LED(x,y,(7-_pos),_red,_green,_blue);
               }
          }
          
          for(int x = _pos; x <= (7-_pos); x++)
          {
               for(int z = _pos; z <= (7-_pos); z++)
               {
                    //top
                    LED(x,_pos,z,_red,_green,_blue);
                    //buttom
                    LED(x,(7-_pos),z,_red,_green,_blue);
               }
          }  
              
          for(int y = _pos; y <= (7-_pos); y++)
          {
               for(int z = _pos; z <= (7-_pos); z++)
               {
                    //left
                    LED(_pos,y,z,_red,_green,_blue);
                    //right
                    LED((7-_pos),y,z,_red,_green,_blue);
               }
          } 


  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//saves the fft octave values and saves that there are new ones
void AandChandler::saveOctaveData(float32_t *data)
{
     for (int i=0; i<NUM_OCTAVES; i++)
     {
          //this is just to make the output nicer
          if( (i > 1) && (i != 7) )
          {
               data[i] += data[i]*0.1*(float(i)-1.0);
          }
          
          //constrain the data
          //here you have to set the min and max value
          //just try some values until it looks cool (they depend on the loudness of your music)
          data[i] = constrain(data[i], -5, 30);
          
          //map the value to the cube
          //i use the -1 because all values below your min are constrained to your min and then would be shown on the cube
          //so the lowest plate would always be on
          //with the -1 there are also some tones to silent to light the lowest plate
          _octaveArray[i] = map(data[i], -5, 30, -1, 7);
     }
     //save that there are new values
     _fftNew = true;   
}

void AandChandler::setFft()
{ 
     _fftNew = false;
     //start fft
     TIMERSAMPLER.start();
     newColor(AN_FFT);
     _fftOn = true;
}

boolean AandChandler::returnFFTOn()
{
     return _fftOn;
} 

void AandChandler::fft()
{
     //if there are new values
     if(_fftNew == true)
     {
          //save that we used the new values
          _fftNew = false;
          //x direction
          for(int x = 0; x <= 7; x++)
          {
               //z direction
               for(int z = 0; z <= 7; z++)
               {
                    //y direction
                    for(int y = 0; y <= 7; y++)
                    {
                         //if the value in the octaveArray is bigger or same as the value of y we have to light the led
                         if(y <= _octaveArray[x])
                         {
                              LED(x, y, z, _red, _green, _blue);
                         }
                         //else we have to turn it off
                         else
                         {
                              LED(x, y, z, 0, 0, 0);
                         }
                    }
               }     
          }
     }
}
          
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AandChandler::setFolder()
{
     _folderaddr[0] = -7;
     _folderaddr[1] = -6;
     _folderaddr[2]= -5;
     _folderaddr[3] = -4;
     _folderaddr[4] = -3;
     _folderaddr[5] = -2;
     _folderaddr[6] = -1;
     _folderaddr[7] = 0;
     
     _fromSide = 5;
     _toSide = random(0,3);
  
     for(int i=0; i<8; i++)
     {
          _oldpullback[i] = 0;
          _pullback[i] = 0;
          _LED_Old[i] = 0;
     }
     newColor(AN_FOLDER);
     _timer = 0;
}

void AandChandler::folder()
{
     if(_timer + 15 < millis() || _timer == 0)
     {
          //reset timer
          _timer = millis();
          //we look first from which side we start
          //then to wich side we want
          switch(_fromSide)
          {
               case 0:
                    switch(_toSide)
                    {
                         case 2:
                             for(int i=0; i<8; i++)
                             {
                                  for(int j=0; j<8; j++)
                                  {
                                       onlyWhenInsideLED(j, _LED_Old[i],i-_oldpullback[i], 0, 0, 0);
                                       onlyWhenInsideLED(j,_folderaddr[i], i-_pullback[i], _red, _green, _blue);
                                  }
                             }
                             break;
                         
                         case 3:     
                              for(int i=0; i<8; i++)
                              {
                                   for(int j=0; j<8; j++)
                                   {
                                        onlyWhenInsideLED(j, _LED_Old[7-i],i+_oldpullback[i], 0, 0, 0);
                                        onlyWhenInsideLED(j,_folderaddr[7-i], i+_pullback[i], _red, _green, _blue);
                                   }
                              }
                              break;                         
                             
                         case 4:               
                              for(int i=0; i<8; i++)
                              {
                                   for(int j=0; j<8; j++)
                                   {
                                        onlyWhenInsideLED(i+_oldpullback[i],_LED_Old[7-i],j , 0, 0, 0);
                                        onlyWhenInsideLED( i+_pullback[i],_folderaddr[7-i],j , _red, _green, _blue);
                                   }
                              }
                              break;
                        
                        case 5:     
                             for(int i=0; i<8; i++)
                             {
                                  for(int j=0; j<8; j++)
                                  {
                                       onlyWhenInsideLED(i-_oldpullback[i],_LED_Old[i],j , 0, 0, 0);
                                       onlyWhenInsideLED( i-_pullback[i],_folderaddr[i],j , _red, _green, _blue);
                                  }
                             }
                             break;
                             
                        default:
                             cube.error("folder");
                             break;
                   }
                   break;
                   //fromSide 0
               
               case 1:
                    switch(_toSide)
                    {    
                         case 2:     
                              for(int i=0; i<8; i++)
                              {
                                   for(int j=0; j<8; j++)
                                   {
                                        onlyWhenInsideLED(j, 7-_LED_Old[i],i-_oldpullback[i], 0, 0, 0);
                                        onlyWhenInsideLED(j,7-_folderaddr[i], i-_pullback[i], _red, _green, _blue);
                                   }
                              }
                              break;
                              
                         case 3:
                              for(int i=0; i<8; i++)
                              {
                                   for(int j=0; j<8; j++)
                                   {
                                        onlyWhenInsideLED(j, 7-_LED_Old[7-i],i+_oldpullback[i], 0, 0, 0);
                                        onlyWhenInsideLED(j,7-_folderaddr[7-i], i+_pullback[i], _red, _green, _blue);
                                   }
                              }
                              break;                              
                              
                         case 4:
                              for(int i=0; i<8; i++)
                              {
                                   for(int j=0; j<8; j++)
                                   {
                                        onlyWhenInsideLED(i+_oldpullback[7-i],7-_LED_Old[7-i],j , 0, 0, 0);
                                        onlyWhenInsideLED( i+_pullback[7-i],7-_folderaddr[7-i],j , _red, _green, _blue);
                                   }
                              }
                              break;     
                              
                         case 5:     
                              for(int i=0; i<8; i++)
                              {
                                   for(int j=0; j<8; j++)
                                   {
                                        onlyWhenInsideLED(i-_oldpullback[i],7-_LED_Old[i],j , 0, 0, 0);
                                        onlyWhenInsideLED( i-_pullback[i],7-_folderaddr[i],j , _red, _green, _blue);
                                   }
                              }
                              break;
                              
                        default:
                             cube.error("folder");
                             break;
                    }
                    break;
                    //fromSide 1
        
             case 2:
                  switch(_toSide)
                  {    
                       case 0:     
                            for(int i=0; i<8; i++)
                            {
                                 for(int j=0; j<8; j++)
                                 {
                                      onlyWhenInsideLED(j, i-_oldpullback[i],_LED_Old[i], 0, 0, 0);
                                      onlyWhenInsideLED(j, i-_pullback[i],_folderaddr[i], _red, _green, _blue);
                                 }
                            }
                            break;                       
                       
                       case 1:
                            for(int i=0; i<8; i++)
                            {
                                 for(int j=0; j<8; j++)
                                 {
                                      onlyWhenInsideLED(j, i+_oldpullback[i],_LED_Old[7-i], 0, 0, 0);
                                      onlyWhenInsideLED(j, i+_pullback[i],_folderaddr[7-i], _red, _green, _blue);
                                 }
                            }
                            break;
                            
                       case 4:     
                            for(int i=0; i<8; i++)
                            {
                                 for(int j=0; j<8; j++)
                                 {
                                      onlyWhenInsideLED(i+_oldpullback[i],j,_LED_Old[7-i] , 0, 0, 0);
                                      onlyWhenInsideLED( i+_pullback[i],j,_folderaddr[7-i] , _red, _green, _blue);
                                 }
                            }
                            break;
                       
                       case 5:     
                            for(int i=0; i<8; i++)
                            {
                                 for(int j=0; j<8; j++)
                                 {
                                      onlyWhenInsideLED(i-_oldpullback[i],j,_LED_Old[i] , 0, 0, 0);
                                      onlyWhenInsideLED( i-_pullback[i],j,_folderaddr[i] , _red, _green, _blue);
                                 }
                            }
                            break;
                            
                        default:
                             cube.error("folder");
                             break;                            
                  }
                  break;
                  //fromSide 2
               
               case 3:
                    switch(_toSide)
                    {
                       case 0:
                            for(int i=0; i<8; i++)
                            {
                                 for(int j=0; j<8; j++)
                                 {
                                      onlyWhenInsideLED(j, i-_oldpullback[i],7-_LED_Old[i], 0, 0, 0);
                                      onlyWhenInsideLED(j, i-_pullback[i],7-_folderaddr[i], _red, _green, _blue);
                                 }
                            }
                            break;
                            
                       case 1:     
                            for(int i=0; i<8; i++)
                            {
                                 for(int j=0; j<8; j++)
                                 {
                                      onlyWhenInsideLED(j, i+_oldpullback[i],7-_LED_Old[7-i], 0, 0, 0);
                                      onlyWhenInsideLED(j, i+_pullback[i],7-_folderaddr[7-i], _red, _green, _blue);
                                 }
                            }
                            break;                            
                            
                       case 4:     
                            for(int i=0; i<8; i++)
                            {
                                 for(int j=0; j<8; j++)
                                 {
                                      onlyWhenInsideLED(i+_oldpullback[i],j,7-_LED_Old[7-i] , 0, 0, 0);
                                      onlyWhenInsideLED(i+_pullback[i],j,7-_folderaddr[7-i] , _red, _green, _blue);
                                 }
                            }
                            break;                       
                       
                       case 5:     
                            for(int i=0; i<8; i++)
                            {
                                 for(int j=0; j<8; j++)
                                 {
                                      onlyWhenInsideLED(i-_oldpullback[i],j,7-_LED_Old[i] , 0, 0, 0);
                                      onlyWhenInsideLED( i-_pullback[i],j,7-_folderaddr[i] , _red, _green, _blue);
                                 }
                            }
                            break;
                            
                        default:
                             cube.error("folder");
                             break;                              
                  }
                  break;
                  //fromSide 3
                            
               case 4:
                    switch(_toSide)
                    {
                         case 0:
                              for(int i=0; i<8; i++)
                              {
                                   for(int j=0; j<8; j++)
                                   {
                                        onlyWhenInsideLED(7-_LED_Old[i], i-_oldpullback[i],j , 0, 0, 0);
                                        onlyWhenInsideLED(7-_folderaddr[i], i-_pullback[i],j , _red, _green, _blue);
                                   }
                              }
                              break;
                              
                         case 1:     
                              for(int i=0; i<8; i++)
                              {
                                   for(int j=0; j<8; j++)
                                   {
                                        onlyWhenInsideLED(7-_LED_Old[7-i], i+_oldpullback[i],j , 0, 0, 0);
                                        onlyWhenInsideLED(7-_folderaddr[7-i], i+_pullback[i],j , _red, _green, _blue);
                                   }
                              } 
                              break;                              
                              
                         case 2:     
                              for(int i=0; i<8; i++)
                              {
                                   for(int j=0; j<8; j++)
                                   {
                                        onlyWhenInsideLED(7-_LED_Old[i], j, i-_oldpullback[i], 0, 0, 0);
                                        onlyWhenInsideLED(7-_folderaddr[i], j, i-_pullback[i], _red, _green, _blue);
                                   }
                              }
                              break;
                              
                         case 3:     
                              for(int i=0; i<8; i++)
                              {
                                   for(int j=0; j<8; j++)
                                   {
                                        onlyWhenInsideLED(7-_LED_Old[7-i], j, i+_oldpullback[i], 0, 0, 0);
                                        onlyWhenInsideLED(7-_folderaddr[7-i], j, i+_pullback[i], _red, _green, _blue);
                                   }
                              }
                              break;
                              
                        default:
                             cube.error("folder");
                             break;                               
                    }
                    break;
                    //fromSide 4
                    
             case 5:
                  switch(_toSide)
                  {
                       case 1:
                            for(int i=0; i<8; i++)
                            {
                                 for(int j=0; j<8; j++)
                                 {
                                      onlyWhenInsideLED(_LED_Old[7-i], i+_oldpullback[i],j , 0, 0, 0);
                                      onlyWhenInsideLED(_folderaddr[7-i], i+_pullback[i],j , _red, _green, _blue);
                                 }
                            }
                            break;
                            
                       case 3:     
                            for(int i=0; i<8; i++)
                            {
                                 for(int j=0; j<8; j++)
                                 {
                                      onlyWhenInsideLED(_LED_Old[7-i], j, i+_oldpullback[i], 0, 0, 0);
                                      onlyWhenInsideLED(_folderaddr[7-i], j, i+_pullback[i], _red, _green, _blue);
                                 }
                            }
                            break;
                            
                       case 2:     
                            for(int i=0; i<8; i++)
                            {
                                 for(int j=0; j<8; j++)
                                 {
                                      onlyWhenInsideLED(_LED_Old[i], j, i-_oldpullback[i], 0, 0, 0);
                                      onlyWhenInsideLED(_folderaddr[i], j, i-_pullback[i], _red, _green, _blue);
                                 }
                            }
                            break;
                            
                       case 0:     
                            for(int i=0; i<8; i++)
                            {
                                 for(int j=0; j<8; j++)
                                 {
                                      onlyWhenInsideLED(_LED_Old[i], i-_oldpullback[i],j , 0, 0, 0);
                                      onlyWhenInsideLED(_folderaddr[i], i-_pullback[i],j , _red, _green, _blue);
                                 }
                            }
                            break;
                            
                        default:
                             cube.error("folder");
                             break;                             
                  }
                  break;
                  //fromSide 5
                  
             default:
                  cube.error("folder");
                  break;
        }
        
        //save the old values
        for(int i=0; i<8; i++)
        {
             _LED_Old[i]=_folderaddr[i];
             _oldpullback[i]=_pullback[i];
        }
 
        if(_folderaddr[7]==7)
        {
             for(int i=0; i<8; i++)
             {
                  _pullback[i] = _pullback[i]+1;
             }
        }
 
        if(_pullback[7]==8)
        {
             //finished with fold
             //should stay in the finished state a bit
             _timer += 150;
             //new color
             newColor(AN_FOLDER);
             
             //creat a new toSide
             int sideSelect=random(4);
     
             //change fromSide to the actuel side and toSide to the new
             switch(_toSide)
             {
                  case 0:
                       _fromSide=0;
                       if(sideSelect==0) _toSide=2;
                       else if(sideSelect==1) _toSide=3;
                       else if(sideSelect==2) _toSide=4;
                       else if(sideSelect==3) _toSide=5;
                       break;
                       
                  case 1:     
                       _fromSide=1;
                       if(sideSelect==0) _toSide=2;
                       else if(sideSelect==1) _toSide=3;
                       else if(sideSelect==2) _toSide=4;
                       else if(sideSelect==3) _toSide=5;
                       break; 
                      
                   case 2:    
                       _fromSide=2;
                       if(sideSelect==0) _toSide=0;
                       else if(sideSelect==1) _toSide=1;
                       else if(sideSelect==2) _toSide=4;
                       else if(sideSelect==3) _toSide=5;
                       break; 
                   
                   case 3:    
                       _fromSide=3;
                       if(sideSelect==0) _toSide=0;
                       else if(sideSelect==1) _toSide=1;
                       else if(sideSelect==2) _toSide=4;
                       else if(sideSelect==3) _toSide=5;
                       break;
                       
                  case 4:     
                       _fromSide=4;
                       if(sideSelect==0) _toSide=0;
                       else if(sideSelect==1) _toSide=1;
                       else if(sideSelect==2) _toSide=2;
                       else if(sideSelect==3) _toSide=3;
                       break;
                    
                  case 5:   
                       _fromSide=5;
                       if(sideSelect==0) _toSide=0;
                       else if(sideSelect==1) _toSide=1;
                       else if(sideSelect==2) _toSide=2;
                       else if(sideSelect==3) _toSide=3;
                       break;
                       
                  default:
                       cube.error("folder");
                       break;
            }
            for(int i=0; i<8; i++)
            {
                 _oldpullback[i]=0;
                 _pullback[i]=0;
            }
           
            _folderaddr[0]=-8;
            _folderaddr[1]=-7;
            _folderaddr[2]=-6;
            _folderaddr[3]=-5;
            _folderaddr[4]=-4;
            _folderaddr[5]=-3;
            _folderaddr[6]=-2;
            _folderaddr[7]=-1;
          
        }
        if(_folderaddr[7]!=7)
        {
             for(int zz=0; zz<8; zz++)
             {
                  _folderaddr[zz] = _folderaddr[zz]+1;
             }
        }
     }    
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AandChandler::setStrobe()
{
     _light = true;
     _timer = 0;
     _intervall = 1000;
     //get a new color
     newColor(AN_STROBE);
     _strobeCount = 0;     
}

void AandChandler::strobe()
{
     if( ( (_timer + _intervall) < millis() ) || (_timer == 0) )
     {
          _timer = millis();
          if(_light == true)
          {
               all(_red, _green, _blue);
               _light = false;
          }
          else
          {
               all(0,0,0);
               _light = true;
          }
          _intervall = float(_intervall * 0.95);
     }
     if(_intervall < 75)
     {
          _intervall = 75;
          if(_strobeCount >= 30)
          {
               setStrobe();
          }
          else
          {
               _strobeCount ++;
          }
     }     
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AandChandler::setColorWheel()
{
     _red = 15;
     _blue = 0;
     _green = 0;
     _colorWheelState = 0;
     _timer = 0;
}

void AandChandler::colorWheel()
{
     if( ( (_timer + 250) < millis() ) || (_timer == 0) )
     {
          _timer = millis();
          switch(_colorWheelState)
          {
               case 0:
                    if(_red > 1)
                    {
                         _red --;
                         _green ++;
                    }
                    else
                    {
                         _red = 0;
                         _green = 15;
                         _colorWheelState = 1;
                    }
                    break;
               
               case 1:
                    if(_green > 1)
                    {
                         _green --;
                         _blue ++;
                    }
                    else
                    {
                         _green = 0;
                         _blue = 15;
                         _colorWheelState = 2;
                    }          
                    break;
               
               case 2:
                    if(_blue > 1)
                    {
                         _blue --;
                         _red ++;
                    }
                    else
                    {
                         _blue = 0;
                         _red = 15;
                         _colorWheelState = 0;
                    }                   
                    break;
               
               default:
                    cube.error("colorWheel");
                    break;
          }
          all(_red, _green, _blue);
     }     
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AandChandler::setRain()
{
     _timer = 0;
     for(int i = 0; i < 64; i++)
     {
          //when we are starting we want many raindrops --> max height is 25
          _rainPos[i] = random(0, 25);
     }     
}

void AandChandler::rain()
{
     if( ( (_timer + 15) < millis() ) || (_timer == 0) )
     {
          _timer = millis();
          
          for(int i = 0; i < 64; i++)
          {
               //clear old LED
               if( (_rainPos[i] < 8) && (_rainPos[i] > 0) )
               {
                    LED(i%8, _rainPos[i], i/8, 0, 0, 0);
               }
               
               //calculate new position
               _rainPos[i] = _rainPos[i] - 1;
               
               //show the new raindrops
               //while they are falling they change their color
               if(_rainPos[i] == 7)
               {
                    LED(i%8, 7, i/8, 0, 5, 15);
               }
               if(_rainPos[i] == 6)
               {
                    LED(i%8, 6, i/8, 0, 1, 9);
               }
               if(_rainPos[i] == 5)
               {
                    LED(i%8, 5, i/8, 0, 0, 10);
               }
               if(_rainPos[i] == 4)
               {
                    LED(i%8, 4, i/8, 1, 0, 11);
               }
               if(_rainPos[i] == 3)
               {
                    LED(i%8, 3, i/8, 3, 0, 12);
               }
               if(_rainPos[i] == 2)
               {
                    LED(i%8, 2, i/8, 10, 0, 15);
               }
               if(_rainPos[i] == 1)
               {
                    LED(i%8, 1, i/8, 10, 0, 10);
               }
               if(_rainPos[i] <= 0)
               {
                    //if the raindrop is below a random value (-100 to -1) we want to create a new one
                    if(_rainPos[i] < random(-100, 0))
                    {
                         //reset it
                         LED(i%8, 0, i/8, 0, 0, 0);
                         //create the new one
                         _rainPos[i] = random(8, 100);     
                    }
                    else
                    {
                         //we want to show the raindrops always on the lowest plate, because of this the z coordinate stays 0
                         LED(i%8, 0, i/8, 10, 0, 1);
                    }
               }               
          }
     }
}     

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AandChandler::setRandomCube()
{
     _randomCounter = 256;
     for(int i = 0; i < 256; i++)
     {
               newColor(AN_RANDOM_CUBE);
               LED(random(8), random(8), random(8), _red, _green, _blue);
     }     
     _timer = 0;
}

void AandChandler::randomCube()
{
     if( ( (_timer + 15) < millis() ) || (_timer == 0) )
     {
          _timer = millis();
          if(_randomCounter < 256)
          {
               newColor(AN_RANDOM_CUBE);
               LED(random(8), random(8), random(8), _red, _green, _blue);
               _randomCounter ++;
          }
          else if(_randomCounter < 512)
          {
               LED(random(8), random(8), random(8), 0, 0, 0);
               _randomCounter ++;
          }
          if(_randomCounter >= 512)
          {
               _randomCounter = 0;
          }
     }     
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AandChandler::setElastic()
{
     _elasticPos = -2;
     _timer = 0;
     _elasticDirect = 1;
     newColor(AN_ELASTIC);
}
    

void AandChandler::elastic()
{
     if( ( (_timer + 20) < millis() ) || (_timer == 0) )
     {
          //reset the timer
          _timer = millis();

          //clear the old leds
          int tmpPos = _elasticPos;
          int tmpPosShow = constrain(tmpPos, 0, 7);
          for(int i=3; i<5; i++)
          {
               LED(3, tmpPosShow, i, 0, 0, 0);
               LED(4, tmpPosShow, i, 0, 0, 0);
               LED(i, tmpPosShow, 3, 0, 0, 0);
               LED(i, tmpPosShow, 4, 0, 0, 0);
          }
          tmpPos = tmpPos + _elasticDirect;
          tmpPosShow = constrain(tmpPos, 0, 7);
          for(int i=3; i<5; i++)
          {
               LED(3, tmpPosShow, i, 0, 0, 0);
               LED(4, tmpPosShow, i, 0, 0, 0);
               LED(i, tmpPosShow, 3, 0, 0, 0);
               LED(i, tmpPosShow, 4, 0, 0, 0);
          }
          tmpPos = tmpPos - _elasticDirect * 3;
          tmpPosShow = constrain(tmpPos, 0, 7);
          for(int i=2; i<6; i++)
          {
               LED(2, tmpPosShow, i, 0, 0, 0);
               LED(5, tmpPosShow, i, 0, 0, 0);
               LED(i, tmpPosShow, 2, 0, 0, 0);
               LED(i, tmpPosShow, 5, 0, 0, 0);
          }
          tmpPos = tmpPos + _elasticDirect;
          tmpPosShow = constrain(tmpPos, 0, 7); 
          for(int i=2; i<6; i++)
          {
               LED(2, tmpPosShow, i, 0, 0, 0);
               LED(5, tmpPosShow, i, 0, 0, 0);
               LED(i, tmpPosShow, 2, 0, 0, 0);
               LED(i, tmpPosShow, 5, 0, 0, 0);
          }
          tmpPos = tmpPos - _elasticDirect * 4;
          tmpPosShow = constrain(tmpPos, 0, 7);
          for(int i=1; i<7; i++)
          {
               LED(1, tmpPosShow, i, 0, 0, 0);
               LED(6, tmpPosShow, i, 0, 0, 0);
               LED(i, tmpPosShow, 1, 0, 0, 0);
               LED(i, tmpPosShow, 6, 0, 0, 0);
          }
          tmpPos = tmpPos + _elasticDirect;
          tmpPosShow = constrain(tmpPos, 0, 7); 
          for(int i=1; i<7; i++)
          {
               LED(1, tmpPosShow, i, 0, 0, 0);
               LED(6, tmpPosShow, i, 0, 0, 0);
               LED(i, tmpPosShow, 1, 0, 0, 0);
               LED(i, tmpPosShow, 6, 0, 0, 0);
          }
          tmpPos = tmpPos - _elasticDirect * 5;
          tmpPosShow = constrain(tmpPos, 0, 7);
          for(int i=0; i<8; i++)
          {
               LED(0, tmpPosShow, i, 0, 0, 0);
               LED(7, tmpPosShow, i, 0, 0, 0);
               LED(i, tmpPosShow, 0, 0, 0, 0);
               LED(i, tmpPosShow, 7, 0, 0, 0);
          }          
          tmpPos = tmpPos + _elasticDirect;
          tmpPosShow = constrain(tmpPos, 0, 7);
          for(int i=0; i<8; i++)
          {
               LED(0, tmpPosShow, i, 0, 0, 0);
               LED(7, tmpPosShow, i, 0, 0, 0);
               LED(i, tmpPosShow, 0, 0, 0, 0);
               LED(i, tmpPosShow, 7, 0, 0, 0);
          }
          
          //calculate the new position
          _elasticPos = _elasticPos + _elasticDirect;
          
          //turn on the new LEDs
          tmpPos = _elasticPos;
          tmpPosShow = constrain(tmpPos, 0, 7);
          for(int i=3; i<5; i++)
          {
               LED(3, tmpPosShow, i, _red, _green, _blue);
               LED(4, tmpPosShow, i, _red, _green, _blue);
               LED(i, tmpPosShow, 3, _red, _green, _blue);
               LED(i, tmpPosShow, 4, _red, _green, _blue);
          }
          tmpPos = tmpPos + _elasticDirect;
          tmpPosShow = constrain(tmpPos, 0, 7);
          for(int i=3; i<5; i++)
          {
               LED(3, tmpPosShow, i, _red, _green, _blue);
               LED(4, tmpPosShow, i, _red, _green, _blue);
               LED(i, tmpPosShow, 3, _red, _green, _blue);
               LED(i, tmpPosShow, 4, _red, _green, _blue);
          }
          tmpPos = tmpPos - _elasticDirect * 3;
          tmpPosShow = constrain(tmpPos, 0, 7);
          for(int i=2; i<6; i++)
          {
               LED(2, tmpPosShow, i, _red, _green, _blue);
               LED(5, tmpPosShow, i, _red, _green, _blue);
               LED(i, tmpPosShow, 2, _red, _green, _blue);
               LED(i, tmpPosShow, 5, _red, _green, _blue);
          }
          tmpPos = tmpPos + _elasticDirect;
          tmpPosShow = constrain(tmpPos, 0, 7); 
          for(int i=2; i<6; i++)
          {
               LED(2, tmpPosShow, i, _red, _green, _blue);
               LED(5, tmpPosShow, i, _red, _green, _blue);
               LED(i, tmpPosShow, 2, _red, _green, _blue);
               LED(i, tmpPosShow, 5, _red, _green, _blue);
          }
          tmpPos = tmpPos - _elasticDirect * 4;
          tmpPosShow = constrain(tmpPos, 0, 7);
          for(int i=1; i<7; i++)
          {
               LED(1, tmpPosShow, i, _red, _green, _blue);
               LED(6, tmpPosShow, i, _red, _green, _blue);
               LED(i, tmpPosShow, 1, _red, _green, _blue);
               LED(i, tmpPosShow, 6, _red, _green, _blue);
          }
          tmpPos = tmpPos + _elasticDirect;
          tmpPosShow = constrain(tmpPos, 0, 7); 
          for(int i=1; i<7; i++)
          {
               LED(1, tmpPosShow, i, _red, _green, _blue);
               LED(6, tmpPosShow, i, _red, _green, _blue);
               LED(i, tmpPosShow, 1, _red, _green, _blue);
               LED(i, tmpPosShow, 6, _red, _green, _blue);
          }
          tmpPos = tmpPos - _elasticDirect * 5;
          tmpPosShow = constrain(tmpPos, 0, 7);
          for(int i=0; i<8; i++)
          {
               LED(0, tmpPosShow, i, _red, _green, _blue);
               LED(7, tmpPosShow, i, _red, _green, _blue);
               LED(i, tmpPosShow, 0, _red, _green, _blue);
               LED(i, tmpPosShow, 7, _red, _green, _blue);
          }          
          tmpPos = tmpPos + _elasticDirect;
          tmpPosShow = constrain(tmpPos, 0, 7);
          for(int i=0; i<8; i++)
          {
               LED(0, tmpPosShow, i, _red, _green, _blue);
               LED(7, tmpPosShow, i, _red, _green, _blue);
               LED(i, tmpPosShow, 0, _red, _green, _blue);
               LED(i, tmpPosShow, 7, _red, _green, _blue);
          }

          //reached the top
          if(_elasticPos == 16)
          {
               _elasticPos = 8;
               _elasticDirect = -1;
               _timer = _timer + 100;
          }
          //reached the bottom
          if(_elasticPos == -9)
          {
               _elasticPos = -1;
               _elasticDirect = 1;
               _timer = _timer + 100;
               newColor(AN_ELASTIC);
          }          
     }
} 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AandChandler::setBouncePlate()
{
     _timer = 0;
     //saves the direction
     _swiper = random(0,6);
     //if we increas the old position we start at 0
     if(_swiper <=2)
     {
          _pos = 0;
     }
     //if we decrease it we start at 7
     else
     {
          _pos = 7;
     }  
     newColor(AN_BOUNCE_PLATE);
     _firstRun = true;
}

void AandChandler::resetBouncePlate()
{
     if(_firstRun == true)
     {
          switch(_swiper)
          {
               case 0:
                    _swiper = 3;
                    _pos = 6;
                    break;
                    
               case 1:
                    _swiper = 4;
                    _pos = 6;
                    break;
                    
               case 2:
                    _swiper = 5;
                    _pos = 6;
                    break;
                    
               case 3:
                    _swiper = 0;
                    _pos = 1;
                    break;
               
               case 4:
                    _swiper = 1;
                    _pos = 1;
                    break;
               
               case 5:
                    _swiper = 2;
                    _pos = 1;
                    break;     
                    
               default:
                    cube.error("Bounce Plate");
                    break;
          }
          _firstRun = false;
     }
     else
     {
          setBouncePlate();
          _timer = millis() + 120;
     }
}


void AandChandler::bouncePlate()
{
     if( (_timer+40 < millis() ) || (_timer == 0) )
     {
          //reset the timer
          _timer = millis();
          
          //the cube has run through the animation
          //because of that we reset the animation    
          if( ( _swiper <= 2 && _pos == 9 && _firstRun == false) || (_swiper<=2 && _pos == 8 && _firstRun == true) )
          {
               resetBouncePlate();
          }
          else if( (_swiper>=3 && _pos == -2 && _firstRun == false) || (_swiper>=3 && _pos == -1 && _firstRun == true))
          {
               resetBouncePlate();
          }
          else
          {
               switch(_swiper)
               {
                  case 0: 
                       //left to right
                       for(int x=0; x<8; x++)
                       {
                            for(int z=0;z<8;z++)
                            {
                                 if(_pos-1 >= 0 && _firstRun == false)
                                 {
                                      LED(x, _pos-1, z,  0, 0, 0);
                                 }
                                 if(_pos <= 7)
                                 {     
                                      LED(x, _pos, z,  _red, _green, _blue);
                                 }
                            }
                       }
                       _pos++;
                       break;
         
                  case 1:
                       //bot to top
                       for(int y=0;y<8;y++)
                       {
                            for(int z=0;z<8;z++)
                            {
                                 if(_pos-1 >= 0 && _firstRun == false)
                                 {                            
                                      LED(_pos-1, y, z,  0, 0, 0);
                                 }
                                 if(_pos <= 7)
                                 {      
                                      LED(_pos, y, z,  _red, _green, _blue);
                                 }
                            }
                       }
                       _pos++;
                       break;
                  
                  case 2:
                       //front to back
                       for(int x=0;x<8;x++)
                       {
                            for(int y=0;y<8;y++)
                            {
                                 if(_pos-1 >= 0 && _firstRun == false)
                                 {                            
                                      LED(x, y, _pos - 1,  0, 0, 0);
                                 }
                                 if(_pos <= 7)
                                 { 
                                      LED(x, y, _pos,  _red, _green, _blue);
                                 }
                            }
                       }
                       _pos++;
                       break;
                  
                  case 3:
                       //right to left
                       for(int x=0;x<8;x++)
                       {
                            for(int z=0;z<8;z++)
                            {
                                 if(_pos+1 <= 7 && _firstRun == false)
                                 { 
                                      LED(x, _pos+1, z,  0, 0, 0);
                                 }
                                 if(_pos >= 0)
                                 {      
                                      LED(x, _pos, z,  _red, _green, _blue);
                                 }
                            }
                       }
                       _pos--;
                       break;
                  
                  case 4:
                       //top to bot
                       for(int y=0;y<8;y++)
                       {
                            for(int z=0;z<8;z++)
                            {
                                 if(_pos+1 <= 7 && _firstRun == false)
                                 {
                                      LED(_pos+1, y, z,  0, 0, 0);
                                 }
                                 if(_pos >= 0)
                                 {     
                                      LED(_pos, y, z,  _red, _green, _blue);
                                 }
                            }
                       }
                       _pos--;
                       break;
                 
                  case 5:
                       //back to front
                       for(int x=0; x<8; x++)
                       {
                            for(int y=0; y<8; y++)
                            {
                                 if(_pos+1 <= 7 && _firstRun == false)
                                 {
                                      LED(x, y, _pos+1,  0, 0, 0);
                                 }
                                 if(_pos >= 0)
                                 {     
                                      LED(x, y, _pos,  _red, _green, _blue);
                                 }
                            }
                       }
                       _pos--;
                       break;
                       
                  default:
                       cube.error("Bounce Plate");
                       break;
               }
          }             
     }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AandChandler::setEdging()
{
     _timer = 0;
     _counter = 0;
     newColor(AN_EDGING);
     resetEdging();
}

void AandChandler::resetEdging()
{
     //take one of the 8 edges
     byte _edge = random(0,9);
     _edgeCoordinate[0] = bitRead(_edge, 0) * 7;
     _edgeCoordinate[1] = bitRead(_edge, 1) * 7;
     _edgeCoordinate[2] = bitRead(_edge, 2) * 7;
     for(int i=0; i<3; i++)
     {
          if(_edgeCoordinate[i] == 0)
          {
               _edgeDirection[i] = false;
          }
          else
          {
               _edgeDirection[i] = true;
          }
     }
}     

void AandChandler::edging()
{
     if( (_timer+35 < millis() ) || (_timer == 0) )
     {
          //TODO
          all(0,0,0);
          if( (_counter == 8) || (_counter == -1) )
          {
               if(_firstRun == true)
               {
                    _firstRun = false;
                    resetEdging();
                    _counter = 7;
               }
               else
               { 
                    _firstRun = true;
                    _counter = 0;
                    newColor(AN_EDGING);
               }                    
          }
          _timer = millis();
          
          for(int i=0; i<=_counter; i++)
          {
               if(_edgeDirection[0] == false)
               {
                    LED(_edgeCoordinate[0] + i, _edgeCoordinate[1], _edgeCoordinate[2], _red, _green, _blue);
                    if(_edgeDirection[1] == false)
                    {
                         LED(_edgeCoordinate[0], _edgeCoordinate[1] + i, _edgeCoordinate[2], _red, _green, _blue);
                         LED(_edgeCoordinate[0] + i, _edgeCoordinate[1] + _counter, _edgeCoordinate[2], _red, _green, _blue);
                         LED(_edgeCoordinate[0] + _counter, _edgeCoordinate[1] + i, _edgeCoordinate[2], _red, _green, _blue);
                         if(_edgeDirection[2] == false)
                         {
                              LED(_edgeCoordinate[0] + i, _edgeCoordinate[1] + _counter, _edgeCoordinate[2] + _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0] + _counter, _edgeCoordinate[1] + i, _edgeCoordinate[2] + _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0] + i, _edgeCoordinate[1], _edgeCoordinate[2] + _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1] + i, _edgeCoordinate[2] + _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1], _edgeCoordinate[2] + i, _red, _green, _blue);
                              LED(_edgeCoordinate[0] + _counter, _edgeCoordinate[1], _edgeCoordinate[2] + i, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1] + _counter, _edgeCoordinate[2] + i, _red, _green, _blue);
                              LED(_edgeCoordinate[0] + _counter, _edgeCoordinate[1] + _counter, _edgeCoordinate[2] + i, _red, _green, _blue);
                         }
                         else
                         {
                              //evt das problem
                              LED(_edgeCoordinate[0] + i, _edgeCoordinate[1] + _counter, _edgeCoordinate[2] - _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0] + _counter, _edgeCoordinate[1] + i, _edgeCoordinate[2] - _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0] + i, _edgeCoordinate[1], _edgeCoordinate[2] - _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1] + i, _edgeCoordinate[2] - _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1], _edgeCoordinate[2] - i, _red, _green, _blue);
                              LED(_edgeCoordinate[0] + _counter, _edgeCoordinate[1], _edgeCoordinate[2] - i, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1] + _counter, _edgeCoordinate[2] - i, _red, _green, _blue); 
                              LED(_edgeCoordinate[0] + _counter, _edgeCoordinate[1] + _counter, _edgeCoordinate[2] - i, _red, _green, _blue);
                         }     
                    }
                    else
                    {
                         LED(_edgeCoordinate[0], _edgeCoordinate[1] - i, _edgeCoordinate[2], _red, _green, _blue);
                         LED(_edgeCoordinate[0] + i, _edgeCoordinate[1] - _counter, _edgeCoordinate[2], _red, _green, _blue);
                         LED(_edgeCoordinate[0] + _counter, _edgeCoordinate[1] - i, _edgeCoordinate[2], _red, _green, _blue);
                         if(_edgeDirection[2] == false)
                         {
                              LED(_edgeCoordinate[0] + i, _edgeCoordinate[1] - _counter, _edgeCoordinate[2] + _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0] + _counter, _edgeCoordinate[1] - i, _edgeCoordinate[2] + _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0] + i, _edgeCoordinate[1], _edgeCoordinate[2] + _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1] - i, _edgeCoordinate[2] + _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1], _edgeCoordinate[2] + i, _red, _green, _blue);
                              LED(_edgeCoordinate[0] + _counter, _edgeCoordinate[1], _edgeCoordinate[2] + i, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1] - _counter, _edgeCoordinate[2] + i, _red, _green, _blue);
                              LED(_edgeCoordinate[0] + _counter, _edgeCoordinate[1] - _counter, _edgeCoordinate[2] + i, _red, _green, _blue);
                              
                         }
                         else
                         {
                              LED(_edgeCoordinate[0] + i, _edgeCoordinate[1] - _counter, _edgeCoordinate[2] - _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0] + _counter, _edgeCoordinate[1] - i, _edgeCoordinate[2] - _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0] + i, _edgeCoordinate[1], _edgeCoordinate[2] - _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1] - i, _edgeCoordinate[2] - _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1], _edgeCoordinate[2] - i, _red, _green, _blue);
                              LED(_edgeCoordinate[0] + _counter, _edgeCoordinate[1], _edgeCoordinate[2] - i, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1] - _counter, _edgeCoordinate[2] - i, _red, _green, _blue);
                              LED(_edgeCoordinate[0] + _counter, _edgeCoordinate[1] - _counter, _edgeCoordinate[2] - i, _red, _green, _blue);
                         }
                    }     
               }
               else
               {
                    LED(_edgeCoordinate[0] - i, _edgeCoordinate[1], _edgeCoordinate[2], _red, _green, _blue);
                    if(_edgeDirection[1] == false)
                    {
                         LED(_edgeCoordinate[0], _edgeCoordinate[1] + i, _edgeCoordinate[2], _red, _green, _blue);
                         LED(_edgeCoordinate[0] - i, _edgeCoordinate[1] + _counter, _edgeCoordinate[2], _red, _green, _blue);
                         LED(_edgeCoordinate[0] - _counter, _edgeCoordinate[1] + i, _edgeCoordinate[2], _red, _green, _blue);
                         if(_edgeDirection[2] == false)
                         {
                              LED(_edgeCoordinate[0] - i, _edgeCoordinate[1] + _counter, _edgeCoordinate[2] + _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0] - _counter, _edgeCoordinate[1] + i, _edgeCoordinate[2] + _counter, _red, _green, _blue);                              
                              LED(_edgeCoordinate[0] - i, _edgeCoordinate[1], _edgeCoordinate[2] + _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1] + i, _edgeCoordinate[2] + _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1], _edgeCoordinate[2] + i, _red, _green, _blue);
                              LED(_edgeCoordinate[0] - _counter, _edgeCoordinate[1], _edgeCoordinate[2] + i, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1] + _counter, _edgeCoordinate[2] + i, _red, _green, _blue);
                              LED(_edgeCoordinate[0] - _counter, _edgeCoordinate[1] + _counter, _edgeCoordinate[2] + i, _red, _green, _blue);
                         }
                         else
                         {
                              LED(_edgeCoordinate[0] - i, _edgeCoordinate[1] + _counter, _edgeCoordinate[2] - _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0] - _counter, _edgeCoordinate[1] + i, _edgeCoordinate[2] - _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0] - i, _edgeCoordinate[1], _edgeCoordinate[2] - _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1] + i, _edgeCoordinate[2] - _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1], _edgeCoordinate[2] - i, _red, _green, _blue);
                              LED(_edgeCoordinate[0] - _counter, _edgeCoordinate[1], _edgeCoordinate[2] - i, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1] + _counter, _edgeCoordinate[2] - i, _red, _green, _blue);
                              LED(_edgeCoordinate[0] - _counter, _edgeCoordinate[1] + _counter, _edgeCoordinate[2] - i, _red, _green, _blue);
                         }     
                    }
                    else
                    {
                         LED(_edgeCoordinate[0], _edgeCoordinate[1] - i, _edgeCoordinate[2], _red, _green, _blue);
                         LED(_edgeCoordinate[0] - i, _edgeCoordinate[1] - _counter, _edgeCoordinate[2], _red, _green, _blue);
                         LED(_edgeCoordinate[0] - _counter, _edgeCoordinate[1] - i, _edgeCoordinate[2], _red, _green, _blue);
                         if(_edgeDirection[2] == false)
                         {
                              LED(_edgeCoordinate[0] - i, _edgeCoordinate[1] - _counter, _edgeCoordinate[2] + _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0] - _counter, _edgeCoordinate[1] - i, _edgeCoordinate[2] + _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0] - i, _edgeCoordinate[1], _edgeCoordinate[2] + _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1] - i, _edgeCoordinate[2] + _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1], _edgeCoordinate[2] + i, _red, _green, _blue);
                              LED(_edgeCoordinate[0]- _counter, _edgeCoordinate[1], _edgeCoordinate[2] + i, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1] - _counter, _edgeCoordinate[2] + i, _red, _green, _blue);
                              LED(_edgeCoordinate[0] - _counter, _edgeCoordinate[1] - _counter, _edgeCoordinate[2] + i, _red, _green, _blue);
                              
                         }
                         else
                         {
                              LED(_edgeCoordinate[0] - i, _edgeCoordinate[1] - _counter, _edgeCoordinate[2] - _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0] - _counter, _edgeCoordinate[1] - i, _edgeCoordinate[2] - _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0] - i, _edgeCoordinate[1], _edgeCoordinate[2] - _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1] - i, _edgeCoordinate[2] - _counter, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1], _edgeCoordinate[2] - i, _red, _green, _blue); 
                              LED(_edgeCoordinate[0] - _counter, _edgeCoordinate[1], _edgeCoordinate[2] - i, _red, _green, _blue);
                              LED(_edgeCoordinate[0], _edgeCoordinate[1] - _counter, _edgeCoordinate[2] - i, _red, _green, _blue);
                              LED(_edgeCoordinate[0] - _counter, _edgeCoordinate[1] - _counter, _edgeCoordinate[2] - i, _red, _green, _blue);
                         }
                    }     
               }
          }
          if(_firstRun == true)
          {
               _counter ++;
          }
          else
          {
               _counter --;
          }     
     }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//draws a letter
void AandChandler::font(int letter, int pos, int depth)
{
     font(letter, pos, depth, 0);
}

//draws a letter with an offset in x-direction
void AandChandler::font(int letter, int pos, int depth, int offsetX)
{
     if( (depth < 1) || (depth > 8) )
     {
          cube.error("depth");
     }     
     //z-achis
     for (int z = pos; z< (pos+depth); z++)
     {
          //check if we are still in the cube
          if (z <= 7)
          {
               //y-achis
               for (int y=0; y<8; y++)
               {
                    //x-achis
                    for (int x=0; x<8; x++)
                    {
                         if (bitRead(font8x8[letter][7-y], x) == 1)
                         {
                              if( x+offsetX >= 0 && x+offsetX <= 7)
                              {
                                   LED(x+offsetX, y, z, _red, _green, _blue);
                              }
                         }
                    }
               }
          }
     }
}

//clears a letter
void AandChandler::clearFont(int letter, int pos, int depth)
{
     clearFont(letter, pos, depth, 0);
}

void AandChandler::clearFont(int letter, int pos, int depth, int offsetX)
{
     //z-achis
     for (int z = pos; z< (pos+depth); z++)
     {
          //check if we are still in the cube
          if (z <= 7)
          {
               //y-achis
               for (int y=0; y<8; y++)
               {
                    //x-achis
                    for (int x=0; x<8; x++)
                    {
                         if (bitRead(font8x8[letter][7-y], x) == 1)
                         {
                              if( x+offsetX >= 0 && x+offsetX <= 7)
                              {
                                   LED(x+offsetX, y, z, 0, 0, 0);
                              }
                         }
                    }
               }
          }
     }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//shows a text running from right to left on the cube
void AandChandler::setRtLText(int depth, String text)
{
     //converts our string and saves it
     textToInt(text);
     //init the variables
     _depth = depth;
     _textPos = 0;
     _posLetter = 8;
     _timer = 0;
     //get a new color
     newColor(AN_FFT);
}

void AandChandler::rtLText()
{
     if(_timer + 500 < millis() || _timer == 0)
     {
          //reset timer
          _timer = millis();
          
          //clear the old letter
          clearFont(_text[_textPos],0,_depth,_posLetter);
          //if there is an other letter after the current we have to clear this following one
          if( (_textPos+1) <= _textLength)
          { 
               clearFont(_text[_textPos+1],0,_depth,_posLetter+10);
          }
          //otherwise we start at the beginning of our text because of that we have to clear the first letter in the text 
          else
          {
               clearFont(_text[0],0,_depth,_posLetter+10);
          }
          //decreas the position
          _posLetter--;
          //draw the new letter
          font(_text[_textPos],0,_depth,_posLetter);
          //is it the last letter in our text or is there still one following 
          if( (_textPos+1) <= _textLength)
          { 
               font(_text[_textPos+1],0,_depth,_posLetter+10);
          }else
          {
               font(_text[0],0,_depth,_posLetter+10);
          }
          //if we have reached -10 (the letter is not longer to see, the following letter is exactly in the middle of the cube)
          //we want to set the position at 0 (is where the following letter is at the moment)
          //and we have to increase our text pos to get the next letter
          if(_posLetter == -10)
          {
               _posLetter = 0;
               _textPos++;
               if(_textPos > _textLength)
               {
                    //if we have finished our text we want to start at the beginning
                    _textPos = 0;
               } 
          }
     }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//takes a String and converts it to the position in our font8x8 matrix
void AandChandler::textToInt(String text)
{
     //converts the string to a string with only capital letter (they are easier to read)
     text.toUpperCase();
     for(int i=0; i<text.length(); i++)
     {
          //check if we are inside max length
          if(i < MAX_TEXT_LENGTH)
          {
               //writes the char in our arry
               _text[i] = text.charAt(i);
               //we don´t have every caracter in our font array we have to get the place where in our font array the character is 
               _text[i] = _text[i] - 32;
               //do we really have a font for this character
               if((_text[i] < 0) || (_text[i] > 64))
               {
                    cube.error("letter");
               }
          }
          else
          {
               cube.error("text too long");
          }
     }
     //save the new tect length
     _textLength = text.length() -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//sets all LEDs to red green blue
void AandChandler::all(byte red, byte green, byte blue)
{
     for(int z = 0; z<=7; z++)
     {
          for(int y = 0; y<=7; y++)
          {
               for(int x = 0; x<=7; x++)
               {
                    LED(x,y,z,red,green,blue);
               }
          }
     }
}
      
