/*
     Cube.cpp - Libary for things in the cube
     created by legl-electroinics
     known bucks -
*/


#include "Cube.h"
#include <LiquidCrystal.h>
#include "AandChandler.h"

extern LiquidCrystal lcd;
extern AandChandler AandC;

Cube::Cube(int storage, int outputEnable, int csRegister)
{
     _STORAGE = storage;
     _OUTPUT_ENABLE = outputEnable;
     _CS_REGISTER = csRegister;
}

//an error methode
//stops the cube and prints an error
void Cube::error(String s)
{
     //stop the interrupt
     Timer3.stop();
     //clear the display and print the error
     lcd.clear();
     lcd.print("Error");
     lcd.setCursor(0,1);
     lcd.print(s);
     
      digitalWrite(_STORAGE, LOW);
      //disable the output
      digitalWrite(_OUTPUT_ENABLE, HIGH);
      //turn all LEDs off directly (without using the blue, red and green arrays)
      //I use this to really ensure that all LEDs are off
      for(int i = 0; i <= 7; i++)
      {    
           SPI.transfer(_CS_REGISTER,B0000000);
      }
      //turn all levels of
      SPI.transfer(_CS_REGISTER,B0000000);
      //let the data save
      digitalWrite(_STORAGE, HIGH);
      digitalWrite(_STORAGE, LOW);
      //enable the output
      digitalWrite(_OUTPUT_ENABLE, LOW);
     
     //stay here for ever
     while(true)
     {
          delay(10000);
     }
}
