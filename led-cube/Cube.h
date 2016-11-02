/*
     Cube.cpp - Libary for things in the cube
     created by legl-electroinics
     known bucks -
*/

#ifndef Cube_h
#define Cube_h

#include "Arduino.h"

class Cube
{
     public:
          Cube(int storage, int outputEnable, int csRegister);
          //stops the cube and prints the error-string s
          void error(String s);    
		
     private:
          int _STORAGE;
          int _OUTPUT_ENABLE;
          int _CS_REGISTER;
};


#endif


/* List off all errors

In main:

In AandChandler:

     red, green, blue, red2, green2, blue2:     The color you wanted to set was not in the boundary
     color type:                                You tried to set a non existing color type
     LED column:                                The led you wanted to set is not existing because of it´s column eg set a led in the 10th column
     LED level:                                 The led you wanted to set is not existing because of it´s level
     LED row:                                   The led you wanted to set is not existing because of it´s row
     LED red, LED green, LED blue:              The color you want to set was not in the boundary
     shiftOut:                                  something wrong with the BAM_Bit
     randomC:                                   something wrong with the random color function
     randomC2:                                  something wrong with the random color 2 function
     anim set:                                  The animation set function is not in the animationSet handler If you wrote a new annimation, maybe you forgot to put the setMethode in the handler
     animation:                                 you tried to run the set function of a non existing animation
     a Handler:                                 The animation is not in the animation handler If you wrote a new annimation, maybe you forgot to put the annimation in the handler / you tried to run a non existing animation
     colorCube:                                 something wrong with swiper in colorCube
     S not imp:                                 stantandard color type is not implementet (maybe added a new animation?)
     new Color:                                 something wrong with color type
     letter:                                    You tried to show a not implementet letter
     text too long:                             the text you want to show is too long for the array to save it (maybe increase MAX_TEXT_LENGTH)
     depth:                                     the depth of you letter is too small or too big
     folder:                                    something wrong in the folder animation
     colorWheel:                                something wrong in the colorWheel animation
     0 group:                                   no animation in the group of animations you wanted to run
     Bounce Plate:                              something wrong with the swiper in bounce plate   
                                                
In Menu:                                        
                                                
     length an:                                array is to small, maybe wrote a new animation and forget to make the array size larger
     length gr:                                array is to small, maybe wrote a new group and forget to make the array size larger
     rotary:                                   rotary returned something unexpected
     Menu 1:                                   something wrong in menu 1/ main menu
     Menu 2:                                   something wrong in menu 2/ animations
     Menu 3:                                   something wrong in menu 3/ color type
     Menu 4:                                   something wrong in menu 4/ user color set
     Menu 5:                                   something wrong in menu 5/ setting
     Menu 6:                                   something wrong in menu 6/ group set
     whichMenu:                                something wrong with whichMenu, tried to change to a non existing menu
     Wrong color                               color does not exist in color set
     not existing menu                         the menu you wanted to change in (with change(whichMenu)) does not exist

*/
