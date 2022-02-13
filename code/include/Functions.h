#ifndef Functions_h
#define Functions_h

#include <Arduino.h>
#include <Pinout.h>

//--------------------------------------------------


//-------Enum for Movement -----------
enum E_RolloMovement
{
    MovementIDLE,
    Rollo1_up,
    Rollo1_down,
    Rollo1_change,
    Rollo2_up,
    Rollo2_down,
    Wait,


};
//--------------------------------------------------





//----------------Globals Variables-----------------
//extern int CANread[];
//extern int CANold[];
//extern int Rollo_1_Can_old;
//extern int Rollo_2_Can_old;
//extern int Rollo1State;
//extern int Rollo2State;
extern int Rollo1Stateold;
extern int Rollo2Stateold;


//--------------------------------------------------

//--------------Function Prototypes-----------------
bool InitCan(void);
bool RolloMovement1(void);
//bool CheckButtons();
//bool ControlSegments();

//--------------------------------------------------

#endif   //#define Functions_h