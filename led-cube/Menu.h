/*
     Menu.cpp - Libary for menu
     created by legl-electroinics
     known bucks -
 */


#ifndef Menu_h
#define Menu_h

#include "Arduino.h"

//TODO rename defines oder errechnen lassen
#define LENGTH_MENU_MAIN 3
#define LENGTH_MENU_ANIMATION 20
#define LENGTH_MENU_COLOR 4
#define LENGTH_MENU_COLOR_SETTING 7
#define LENGTH_MENU_SETTINGS 2
#define LENGTH_MENU_GROUP 4

typedef enum {COLOR_RED, COLOR_GREEN, COLOR_BLUE, WHICH_COLOR_QUANTITY} WHICH_COLOR;
typedef enum {MENU_MAIN, MENU_ANIMATION, MENU_COLOR, MENU_COLOR_SETTING, MENU_SETTINGS, MENU_GROUP, MENU_QUANTITY} WHICH_MENU;

class Menu
{
     public:
          //starts the menu
          void begin(byte cols, byte rows);
          //handles the changes from the rotary
          void interactRotary();
          //shows the actuell animation and handels the backlight
          void actuell(boolean fromSet);
		
     private:
          //shows the menu
          void show();
          //shows the Cursor
          void showCursor(boolean pressed, boolean force);
          //changes the menu and does the changes at animation, colour...
          void handler();
          //for changing between the menus 
          void change(WHICH_MENU menu);
          //sets the animation
          void setAnimation(byte animation);
          //for color setting
          byte setColor(byte red, byte green, byte blue, WHICH_COLOR whichColor);
          void showChangedColor(byte red, byte green, byte blue, WHICH_COLOR whichColor, byte tmpColor, byte tmpPos);
          
          //saves whether the cursor is on the top of the display or on the buttom
          boolean _cursorTop;
          //saves the position in the menu
          int _pos;
          //saves in which menu we are
          WHICH_MENU _whichMenu;
          //saves wether the rotary is pressed or not
          boolean _pressed;
          
          //saves the length of the different menus
          //by using _length[WHICH_MENU] we get the starting position of the WHICH_MENU
          byte _length[MENU_QUANTITY + 1];
          //saves the content of every menu
          String _menu[LENGTH_MENU_MAIN + LENGTH_MENU_ANIMATION + LENGTH_MENU_COLOR + LENGTH_MENU_COLOR_SETTING + LENGTH_MENU_SETTINGS + LENGTH_MENU_GROUP];
          
          //for backlight and actuell
          //should be the backlight on
          boolean _backlightOn;
          //is the backlight on
          boolean _isOn;
          //should we show the current animation
          boolean _showActuell;
          //is the current animation showed
          boolean _isShowed;
          //timer for timing
          unsigned long _timer;
};

#endif


