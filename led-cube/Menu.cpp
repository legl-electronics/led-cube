/*
     Menu.cpp - Libary for menu
     created by legl-electroinics
     known bucks -
 */

#include "Menu.h"
#include "Rotary2.h"
#include "Cube.h"
#include "AandChandler.h"
#include <LiquidCrystal.h>

extern Cube cube;
extern AandChandler AandC;
extern LiquidCrystal lcd;
extern Rotary rotary;

#define BACKLIGHT 36

//////////////////////////////////////////////////////////////////////////////////////

//starts the menu and inits everything
void Menu::begin(byte cols, byte rows)
{
     //sets variables to init values
     _cursorTop = true;
     _pos = 0;
     _whichMenu = MENU_MAIN;
     _pressed = true;
     _timer = millis();
     _showActuell = true;
     _isShowed = false;
     _backlightOn = true;
     _isOn = true;
     pinMode(BACKLIGHT, OUTPUT);
     //turn the backlight on
     digitalWrite(BACKLIGHT, HIGH);
     
     
     //saves the length of the different menus
     _length[MENU_MAIN] = 0;
     _length[MENU_ANIMATION] = _length[MENU_MAIN] + LENGTH_MENU_MAIN;
     _length[MENU_COLOR] = _length[MENU_ANIMATION] + LENGTH_MENU_ANIMATION;
     _length[MENU_COLOR_SETTING] = _length[MENU_COLOR] + LENGTH_MENU_COLOR;
     _length[MENU_SETTINGS] = _length[MENU_COLOR_SETTING] + LENGTH_MENU_COLOR_SETTING;
     _length[MENU_GROUP] = _length[MENU_SETTINGS] + LENGTH_MENU_SETTINGS;
     _length[MENU_QUANTITY] = _length[MENU_GROUP] + LENGTH_MENU_GROUP;
     
     //saves the text of the menu
     _menu[0] = "animation";
     _menu[1] = "colour";
     _menu[2] = "settings";
     
     _menu[_length[MENU_ANIMATION] + AN_OFF] = "OFF";
     _menu[_length[MENU_ANIMATION] + AN_GROUP] = "Groups";
     _menu[_length[MENU_ANIMATION] + AN_1BALL] = "Ball";
     _menu[_length[MENU_ANIMATION] + AN_2BALLS] = "2 Balls";
     _menu[_length[MENU_ANIMATION] + AN_FOLDER] = "Folder";
     _menu[_length[MENU_ANIMATION] + AN_CO_CUBE] = "ColorCube";
     _menu[_length[MENU_ANIMATION] + AN_PLATE] = "Plate";
     _menu[_length[MENU_ANIMATION] + AN_SNAKE] = "Snake";
     _menu[_length[MENU_ANIMATION] + AN_ELASTIC] = "Elastic";
     _menu[_length[MENU_ANIMATION] + AN_BOUNCE_PLATE] = "Bounce Plate";
     _menu[_length[MENU_ANIMATION] + AN_SINWAVE] = "SinWave";
     _menu[_length[MENU_ANIMATION] + AN_RANDOM_CUBE] = "Random";
     _menu[_length[MENU_ANIMATION] + AN_EXPLOSION] = "Explosion";
     _menu[_length[MENU_ANIMATION] + AN_EDGING] = "Edging";
     _menu[_length[MENU_ANIMATION] + AN_STROBE] = "Strobe";
     _menu[_length[MENU_ANIMATION] + AN_RAIN] = "Rain";
     _menu[_length[MENU_ANIMATION] + AN_FFT] = "FFT";
     _menu[_length[MENU_ANIMATION] + AN_COLOR_WHEEL] = "ColorWheel";
     _menu[_length[MENU_ANIMATION] + AN_RTLTEXT] = "right to left T";
     _menu[_length[MENU_ANIMATION] + AN_QUANTITY] = "back";
     
     _menu[_length[MENU_COLOR] + 0] = "user";
     _menu[_length[MENU_COLOR] + 1] = "random";
     _menu[_length[MENU_COLOR] + 2] = "standard";
     _menu[_length[MENU_COLOR] + 3] = "back";
     
     _menu[_length[MENU_COLOR_SETTING] + 0] = "red";
     _menu[_length[MENU_COLOR_SETTING] + 1] = "green";
     _menu[_length[MENU_COLOR_SETTING] + 2] = "blue";
     _menu[_length[MENU_COLOR_SETTING] + 3] = "red2";
     _menu[_length[MENU_COLOR_SETTING] + 4] = "green2";
     _menu[_length[MENU_COLOR_SETTING] + 5] = "blue2";
     _menu[_length[MENU_COLOR_SETTING] + 6] = "back";
     
     _menu[_length[MENU_SETTINGS] + 0] = "inter f";
     _menu[_length[MENU_SETTINGS] + 1] = "back";
     
     _menu[_length[MENU_GROUP] + GROUP_NORMAL] = "Normal";
     _menu[_length[MENU_GROUP] + GROUP_ALL] = "All";
     _menu[_length[MENU_GROUP] + GROUP_PARTY] = "Party";
     _menu[_length[MENU_GROUP] + GROUP_TYPE_QUANTITY] = "back";

     byte arrow[8] =
     {
          B10000,
          B11000,
          B11100,
          B11110,
          B11110,
          B11100,
          B11000,
          B10000,
     };

     byte circle[8] =
     {
          B00000,
          B00100,
          B01110,
          B11111,
          B11111,
          B01110,
          B00100,
          B00000,
     };

     //creates user set symbols
     lcd.createChar(0, arrow);
     lcd.createChar(1, circle);
     //I had some problems with the user set symbols(because of that the delay)
     delay(500);
     //start lcd
     lcd.begin(cols, rows);
     delay(500);

     //shows the menu
     show();
     //shows the cursor
     showCursor(false,true);
     
     //checks LENGTH_MENU_ANIMATION
     if(AN_QUANTITY > LENGTH_MENU_ANIMATION)
     {
          cube.error("length an");
     }
     
     //checks LENGTH_MENU_GROUP
     if(GROUP_TYPE_QUANTITY > LENGTH_MENU_GROUP)
     {
          cube.error("length gr");
     }
}

////////////////////////////////////////////////////////////////////////////////////////

//shows the menu
void Menu::show()
{
     lcd.clear();
     lcd.setCursor(1,0);
     lcd.print(_menu[_pos]);
     lcd.setCursor(1,1);
     lcd.print(_menu[_pos+1]);
}

////////////////////////////////////////////////////////////////////////////////////////

//shows the cursor
void Menu::showCursor(boolean pressed, boolean force)
{
     //if there was not a change with the cursor we want not that it is updated, with fore we can update it without any change
     //if the button is pressed and it was not pressed before print the arrow
     if(pressed == true && (_pressed == false || force == true) )
     {
          //we have to check were we want to print the cursor
          if(_cursorTop == true)
          {
               lcd.setCursor(0,0);
          }
          else
          {
               lcd.setCursor(0,1);
          }
          lcd.write(byte(1));
          _pressed = true;
     }
     //if the button is not pressed and it was pressed before print the circle
     if(pressed == false && (_pressed == true || force == true) )
     {
          if(_cursorTop == true)
          {
               lcd.setCursor(0,0);
          }
          else
          {
               lcd.setCursor(0,1);
          }
          lcd.write(byte(0));
          _pressed = false;
     }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

//interact with the rotary
void Menu::interactRotary()
{
     //checks the rotary for changes
     byte change = rotary.check();
     
     //if there was a change we want to look what to do
     if(change != ROT_NOTHING)
     {
          //first we check the backlight
          //if it was off we want to turn it on and don´t do anything
          if(_backlightOn == false)
          {
               _backlightOn = true;
               _isOn = false;
               _timer = millis();
               return;
          }
          //then we check if the menu is shown or the actuell animation
          //if the actuell animation is shown, we want to show the menu
          else if(_showActuell == true)
          {
               _showActuell = false;
               _isShowed = false;
               //show the menu and cursor
               show();
               showCursor(false, true);
               _timer = millis();
               return;
          }
          //reset the timer for showing the actuell animation and backlight
          _timer=millis();
     }     
     
     //what has changed with the rotary
     switch(change)
     {
          case ROT_NOTHING:
               break;
     
          //if the cursor is in the upper position we want to set it down
          //if not we want to increment the position, but we have to look whether it would be out of the boundary
          case ROT_CLOCKWISE:
               if(_cursorTop == true)
               {
                    _cursorTop = false;
               }
               else
               {
                    _pos++;
                    if(_pos > _length[_whichMenu + 1] - 2)
                    {
                         _pos = _length[_whichMenu + 1] - 2;
                    }
               }
               //show the menu and cursor
               show();
               showCursor(false, true);
               break;
     
          //the same but in the other direction     
          case ROT_COUNTERCLOCKWISE:
               if(_cursorTop == false)
               {
                    _cursorTop = true;
               }
               else
               {
                    _pos--;
                    if(_pos < _length[_whichMenu] )
                    {
                         _pos = _length[_whichMenu];
                    }
               }
               show();
               showCursor(false, true);
               break;
     
          //we want to change the cursor to the arrow     
          case ROT_PRESSED:
               showCursor(true, false);
               break;
     
          //when it was released we want to change the menu, colour ...     
          case ROT_RELEASED:
               //handels the menu changes
               handler();
               break;
     
          default:
               cube.error("rotary");
               break;  
     }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//handles the changes after the button was pressed and released
void Menu::handler()
{
     //we want the curser position in the actual menu not in the whole menu
     byte tmpPos = _pos - (_length[_whichMenu] - 1);

     //the pos is every time the element on the top, it is equal when cursor is on the upper or lower position
     //if the cursor is in the lower position we have to increase the pos
     if(_cursorTop == false)
     {
          tmpPos++;
     }

     //first we have to look in which menu we are
     switch (_whichMenu)
     {
     case MENU_MAIN:
          //then we have to look at which position
          //and then do what to do
          switch (tmpPos)
          {
          case 1:
               change(MENU_ANIMATION);
               break;
               
          case 2:
               change(MENU_COLOR);
               break;
               
          case 3:
               change(MENU_SETTINGS);
               break;

          default:
               cube.error("Menu 1");
               break;
          }
          break;

     case MENU_ANIMATION:
          if(tmpPos == 2)
          {
               change(MENU_GROUP);
          }
          else if(tmpPos <= AN_QUANTITY)
          {
               setAnimation(tmpPos);
          }
          else if(tmpPos == AN_QUANTITY + 1)
          {
               change(MENU_MAIN);
          }
          else
          {
               cube.error("Menu 2");
          }
          break;
          
     case MENU_COLOR:
          if(tmpPos == 1)
          {
               AandC.setColorType(USER);
               change(MENU_COLOR_SETTING);
          }
          else if( (tmpPos == 2) || (tmpPos == 3) )
          {
               AandC.setColorType(tmpPos-1);
               change(MENU_MAIN);
          }
          else if(tmpPos == 4)
          {
               change(MENU_MAIN);
          }
          else
          {
               cube.error("Menu 3");
          }
          break;
          
     case MENU_COLOR_SETTING:
          switch(tmpPos)
          {
               case 1:
                    AandC.setRed(setColor(AandC.getRed(), AandC.getGreen(), AandC.getBlue(), COLOR_RED));
                    change(MENU_COLOR_SETTING);
                    break;
               case 2:
                    AandC.setGreen(setColor(AandC.getRed(), AandC.getGreen(), AandC.getBlue(), COLOR_GREEN));
                    change(MENU_COLOR_SETTING);
                    break;
               case 3:
                    AandC.setBlue(setColor(AandC.getRed(), AandC.getGreen(), AandC.getBlue(), COLOR_BLUE));
                    change(MENU_COLOR_SETTING);
                    break;
               case 4:
                    AandC.setRed2(setColor(AandC.getRed2(), AandC.getGreen2(), AandC.getBlue2(), COLOR_RED));
                    change(MENU_COLOR_SETTING);
                    break;
               case 5:
                    AandC.setGreen2(setColor(AandC.getRed2(), AandC.getGreen2(), AandC.getBlue2(), COLOR_RED));
                    change(MENU_COLOR_SETTING);
                    break;
               case 6:
                    AandC.setBlue2(setColor(AandC.getRed2(), AandC.getGreen2(), AandC.getBlue2(), COLOR_RED));
                    change(MENU_COLOR_SETTING);
                    break;
               case 7:
                    change(MENU_COLOR);
                    break;               
               default:
                    cube.error("Menu 4");
                    break;
          }
          break;
          
     case MENU_SETTINGS:
          switch(tmpPos)
          {
               case 1:
                    if(Timer3.getFrequency() == 8000)
                    {
                         Timer3.stop();
                         Timer3.setFrequency(8);
                         Timer3.start();
                    }
                    else
                    {
                         Timer3.stop();
                         Timer3.setFrequency(8000);
                         Timer3.start();
                    }
                    change(MENU_MAIN);     
                    break;
                    
               case 2:
                    change(MENU_MAIN);
                    break;
                    
               default:
                    cube.error("Menu 5");
                    break;
          }
          break;

     case MENU_GROUP:
          if(tmpPos <= GROUP_TYPE_QUANTITY)
          {
               AandC.setWhichGroup(GROUP_TYPE(tmpPos-1));
               AandC.setAnimation(AN_GROUP);
               _showActuell = true;
               _isShowed = false;
               actuell(false);
          }
          else if(tmpPos == (GROUP_TYPE_QUANTITY + 1) )
          {
               change(MENU_ANIMATION);
          }
          else
          {
               cube.error("Menu 6");
          }
          break;    
     
     default:
          cube.error("whichMenu");
          break;
     }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//changes between two menus
void Menu::change(WHICH_MENU menu)
{
     //set new menu
     if(menu < MENU_QUANTITY)
     {
          _whichMenu = menu;
     }
     else
     {
          cube.error("not existing menu");
     }
     //set the new position
     _pos = _length[_whichMenu];
     //set the cursor to the top
     _cursorTop = true;
     //update menu and cursor
     show();
     showCursor(false, false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//sets the new animation and shows the actuell one
void Menu::setAnimation(byte animation)
{
     AandC.setAnimation(animation - 1);
     _showActuell = true;
     _isShowed = false;
     actuell(false);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//for setting the color
byte Menu::setColor(byte red, byte green, byte blue, WHICH_COLOR whichColor)
{
      byte tmpColor = 0;
      switch(whichColor)
      {
           case COLOR_RED:
                tmpColor = red;
                break;
                
           case COLOR_GREEN:
                tmpColor = green;
                break;

           case COLOR_BLUE:
                tmpColor = blue;
                break;
          default:
               cube.error("Wrong color");
               break;
      }          
      byte change = 0;
      //save the old position
      byte tmpPos = _pos;
      if(_cursorTop == false)
      {
           tmpPos++;
      }
      //print which color you are going to change
      showChangedColor(red, green, blue, whichColor, tmpColor, tmpPos);
      
      while(true)
      {
           //check the rotary for changes
           change = rotary.check();
           if(change != ROT_NOTHING)
           {
               //reset the timer for showing the actuell animation and backlight
               _timer=millis();
          }
           switch(change)
           {
                //we want to increment
                case ROT_CLOCKWISE:
                     //we have to look that we stay in the boundary
                     if(tmpColor < 15)
                     {
                          tmpColor ++;
                          //show the new value
                          showChangedColor(red, green, blue, whichColor, tmpColor, tmpPos);
                     }
                     break;
               //decreasing
                case ROT_COUNTERCLOCKWISE:
                    if(tmpColor > 0)
                    {
                          tmpColor --;
                          //show the new value
                          showChangedColor(red, green, blue, whichColor, tmpColor, tmpPos);                          
                    }
                    break;
                
                //if the button is released after pressing we want to set the new color
                case ROT_RELEASED:
                    AandC.all(0,0,0);
                    return tmpColor;
                    break;
           }
      }
}     


void Menu::showChangedColor(byte red, byte green, byte blue, WHICH_COLOR whichColor, byte tmpColor, byte tmpPos)
{
           lcd.clear();
           lcd.print(_menu[tmpPos]);
           lcd.setCursor(0,1);
           lcd.print(tmpColor);
           switch(whichColor)
           {
                case COLOR_RED:
                     AandC.all(tmpColor, green, blue);
                     break;
                     
                case COLOR_GREEN:
                     AandC.all(red, tmpColor, blue);
                     break;
     
                case COLOR_BLUE:
                     AandC.all(red, green, tmpColor);
                     break;
               default:
                    cube.error("Wrong color");
                    break;
           }
}     


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Menu::actuell(boolean fromSet)
{
     //after 5s when nothing happend we want to show the actuell animaton and then turn the backlight off
     if(_timer+5000 < millis() && (_showActuell == false || _backlightOn == true)  )
     {
          //reset the timer
          _timer = millis();
          if(_showActuell == false)
          {
               _showActuell = true;
          }
          else
          {
               _backlightOn = false;
          }
     }
     //if the actuell animation should be showed and isn´t show
     //or we use it from our set methode out of AandC and it should be showed
     if( (_isShowed == false && _showActuell == true) || (fromSet == true && _showActuell == true))
     {
          lcd.clear();
          //if we are running the all-animation we want to see wich animations all runs
          if(AandC.returnAnimation() == AN_GROUP)
          {
               lcd.print(_menu[_length[MENU_GROUP] + AandC.returnWhichGroup()]);
               lcd.setCursor(0,1);
               lcd.print( _menu[_length[MENU_ANIMATION] + AandC.returnState()]);
          }
          else
          {
               //shows the animation
               lcd.print( _menu[_length[MENU_ANIMATION] + AandC.returnAnimation()]);     
          }
          _isShowed = true;
          //if we change something we reset the timer but not when it was called out of the set methode
          if(fromSet == false)
          {
               _timer = millis();
          }
     }
     //looks for changes in the backlight
     if(_backlightOn == true && _isOn == false)
     {
          digitalWrite(BACKLIGHT, HIGH);
          _isOn = true;
          _timer = millis();
     }
     if(_backlightOn == false && _isOn == true)
     {
          digitalWrite(BACKLIGHT, LOW);
          _isOn = false;
          _timer = millis();
     }
}
          
