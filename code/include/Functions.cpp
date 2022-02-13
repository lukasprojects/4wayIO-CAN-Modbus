#include <Arduino.h>
#include <Functions.h>
#include <Pinout.h>
#include <EasyButton.h>
#include <CAN.h>




//--------Statemachine Variables--------------------
static E_RolloMovement State_Rollo_1 = MovementIDLE;
static E_RolloMovement State_Rollo_2 = MovementIDLE;
//--------------------------------------------------

//--------Global / External Variables---------------
extern int CANread[];
extern int CANold[];
extern int Rollo_1_Can_old;
extern int Rollo_2_Can_old;
extern bool BTN1stop, BTN2stop, BTN3stop, BTN4stop;
int Rollo1State;
int Rollo1Stateold;
int Rollo2State;
int Rollo2Stateold;
int Rollo1statstart;
int Rollo2statstart;
// int  time_minutes = 0;
// bool time_changed = false;
//--------------------------------------------------


//--------------Functions---------------------------

//Init Can

bool InitCan(void)
{
    bool return_b = false;
    CAN.setPins( canRX, canTX );
    if ( CAN.begin( 500E3 ) )
    {
        return_b = true;
    }

    return return_b;
}

//Checks if Button was pressed, increments defined time --> time setting

    bool                 return_b                 = false;
    bool                 MinutesInput             = false;
    bool                 HoursInput               = false;
    static int           LastState1               = MovementIDLE;
    static int           LastState2               = MovementIDLE;
    int                  Rollo_time1              = 0; 
    int                  Rollo_time2              = 0;    
    int                  Cur1State; 
    int                  Cur2State; 
    //----------Create Timer for Tasking---------------
    unsigned long Rollo_Timer_1              = 0;
    unsigned long Rollo_Timer_2              = 0;
    static unsigned long State_Time;
    unsigned long Wait_Timer_1;
    unsigned long Wait_Timer_2;  


    //----------Define Timer Delays--------------------
    static unsigned long RolloDriveTime1            = 26000;
    static unsigned long RolloDriveTime2            = 20000;
    const unsigned long CheckButtons_Timer2_delay   = 250;
    static unsigned long WaitTime                   = 1000;
    //--------------------------------------------------
bool RolloMovement1()
{
    
    switch (State_Rollo_1)
    {

    case MovementIDLE:    
    if (Rollo1State != Rollo1Stateold)
    {
        Serial.print("Rollo 1 state changed");
        
        if (Rollo1State >= Rollo1Stateold)                                                  //UP
        {
            State_Rollo_1 = Rollo1_up;
            Rollo_time1 = map((Rollo1State - Rollo1Stateold), 0, 100, 0, RolloDriveTime1);
            Serial.print(" driving UP for ");
            Serial.print(Rollo_time1);
            Serial.println(" ms");
            digitalWrite(K2, false);
            digitalWrite(K1, true);
            Rollo_Timer_1 = millis();
            Rollo1statstart = Rollo1State;
        }
        else if (Rollo1State <= Rollo1Stateold)                                             //DOWN
        {
            State_Rollo_1=Rollo1_down;              
            Rollo_time1 = map((Rollo1Stateold - Rollo1State), 0, 100, 0, RolloDriveTime1);
            Serial.print(" driving DOWN for ");
            Serial.print(Rollo_time1);
            Serial.println(" ms");
            digitalWrite(K1, false);
            digitalWrite(K2, true);
            Rollo_Timer_1 = millis();
            Rollo1statstart = Rollo1State;
        }
    }
    break;
    
    case Rollo1_up:
        if (millis() - Rollo_Timer_1 >= Rollo_time1 || BTN1stop==true){        //Rolladen 1 AUF
            digitalWrite(K1, false);
            Rollo1State = Rollo1Stateold + map(millis() - Rollo_Timer_1, 0, RolloDriveTime1, 0, 100);
            Rollo1Stateold = Rollo1State;
            Serial.print("Motor stopped new status: ");
            Serial.print(Rollo1State);
            Serial.println("%");
            State_Rollo_1 = MovementIDLE;
        }
        else if (millis() - State_Time >= 2000){
            Cur1State = Rollo1Stateold + map(millis() - Rollo_Timer_1, 0, RolloDriveTime1, 0, 100);
            Serial.print("going UP, current Val: ");
            Serial.println(Cur1State);
            State_Time = millis();
        }
        else if(Rollo1State != Rollo1statstart){
            Serial.print("Motor stopped because of statechange at current Val:");
            if (Rollo1State<=Rollo1Stateold){
                digitalWrite(K1, false);
                State_Rollo_1 = Wait;
                LastState1 = Rollo1_up;
                Wait_Timer_1 = millis();
                Rollo1Stateold = Rollo1Stateold + map(millis() - Rollo_Timer_1, 0, RolloDriveTime1, 0, 100);
                Rollo_time1 = map((Rollo1Stateold - Rollo1State), 0, 100, 0, RolloDriveTime1);
                Rollo_Timer_1 = millis();
                Serial.println(Rollo1Stateold);
            }
            else{
                State_Rollo_1 = MovementIDLE;
                Rollo1Stateold = Rollo1Stateold + map(millis() - Rollo_Timer_1, 0, RolloDriveTime1, 0, 100);
            }
        }
        else{
            digitalWrite(K1, true);
            //Rollo1statstart = Rollo1State;
        }
        

    break;

    case Rollo1_down:
        if (millis() - Rollo_Timer_1 >= Rollo_time1  || BTN2stop==true){        //Rolladen 1 ZU
            digitalWrite(K2, false);
            Rollo1State = Rollo1Stateold - map(millis() - Rollo_Timer_1, 0, RolloDriveTime1, 0, 100);
            Rollo1Stateold = Rollo1State;
            Serial.print("Motor stopped new status: ");
            Serial.print(Rollo1State);
            Serial.println("%");
            State_Rollo_1 = MovementIDLE;
           
        }
        else if (millis() - State_Time >= 2000){
            Cur1State = Rollo1Stateold - map(millis() - Rollo_Timer_1, 0, RolloDriveTime1, 0, 100);
            Serial.print("going DOWN, current Val: ");
            Serial.println((Cur1State));
            State_Time = millis();
        }
        else if(Rollo1State != Rollo1statstart){
            Serial.print("Motor stopped because of statechange at current Val: ");
            if (Rollo1State>=Rollo1Stateold){
                digitalWrite(K2, false);
                State_Rollo_1 = Wait;
                LastState1 = Rollo1_down;
                Wait_Timer_1 = millis();
                Rollo1Stateold = Rollo1Stateold - map(millis() - Rollo_Timer_1, 0, RolloDriveTime1, 0, 100);
                Rollo_time1 = map((Rollo1State - Rollo1Stateold), 0, 100, 0, RolloDriveTime1);
                Rollo_Timer_1 = millis();
                Serial.println(Rollo1Stateold);
            }
            else{
                State_Rollo_1 = MovementIDLE;
                Rollo1Stateold = Rollo1Stateold - map(millis() - Rollo_Timer_1, 0, RolloDriveTime1, 0, 100);
            }
        }
        else{
            digitalWrite(K2, true);
            //Rollo1statstart = Rollo1State;
        }
        
    break;

    case Wait:
        if (millis() - Wait_Timer_1 >= WaitTime){
            if(LastState1==Rollo1_up){
               State_Rollo_1 = Rollo1_down; 
               Rollo1statstart=Rollo1State;
            }
            else{
                State_Rollo_1 = Rollo1_up;
                Rollo1statstart=Rollo1State;
            }

        }

    break;
    
    default:
    break;
    
    }
    return return_b;
} 



bool RolloMovement2()
{
    
    switch (State_Rollo_2)
    {

    case MovementIDLE:    
    if (Rollo2State != Rollo2Stateold)
    {
        Serial.print("Rollo 2 state changed");
        
        if (Rollo2State >= Rollo2Stateold)                                                  //UP
        {
            State_Rollo_2 = Rollo2_up;
            Rollo_time2 = map((Rollo2State - Rollo2Stateold), 0, 100, 0, RolloDriveTime2);
            Serial.print(" driving UP for ");
            Serial.print(Rollo_time2);
            Serial.println(" ms");
            digitalWrite(K4, false);
            digitalWrite(K3, true);
            Rollo_Timer_2 = millis();
            Rollo2statstart = Rollo2State;
        }
        else if (Rollo2State <= Rollo2Stateold)                                             //DOWN
        {
            State_Rollo_2=Rollo2_down;              
            Rollo_time2 = map((Rollo2Stateold - Rollo2State), 0, 100, 0, RolloDriveTime2);
            Serial.print(" driving DOWN for ");
            Serial.print(Rollo_time2);
            Serial.println(" s");
            digitalWrite(K3, false);
            digitalWrite(K4, true);
            Rollo_Timer_2 = millis();
            Rollo2statstart = Rollo2State;
        }
    }
    break;
    
    case Rollo2_up:
        if (millis() - Rollo_Timer_2 >= Rollo_time2  || BTN3stop==true){        //Rolladen 1 AUF
            digitalWrite(K3, false);
            Rollo2State = Rollo2Stateold + map(millis() - Rollo_Timer_2, 0, RolloDriveTime2, 0, 100);
            Rollo2Stateold = Rollo2State;
            Serial.print("Motor stopped new status: ");
            Serial.print(Rollo2State);
            Serial.println("%");
            State_Rollo_2 = MovementIDLE; 
        }
        else if (millis() - State_Time >= 2000){
            Cur2State = Rollo2Stateold + map(millis() - Rollo_Timer_2, 0, RolloDriveTime2, 0, 100);
            Serial.print("going UP, current Val: ");
            Serial.println(Cur2State);
            State_Time = millis();
        }
        else if(Rollo2State != Rollo2statstart){
            Serial.print("Motor stopped because of statechange at current Val:");
            if (Rollo2State<=Rollo2Stateold){
                digitalWrite(K3, false);
                State_Rollo_2 = Wait;
                LastState2 = Rollo2_up;
                Wait_Timer_2 = millis();
                Rollo2Stateold = Rollo2Stateold + map(millis() - Rollo_Timer_2, 0, RolloDriveTime2, 0, 100);
                Rollo_time2 = map((Rollo2Stateold - Rollo2State), 0, 100, 0, RolloDriveTime2);
                Rollo_Timer_2 = millis();
                Serial.println(Rollo2Stateold);
            }
            else{
                State_Rollo_2 = MovementIDLE;
                Rollo2Stateold = Rollo2Stateold + map(millis() - Rollo_Timer_2, 0, RolloDriveTime2, 0, 100);
            }
        }
        else{
            digitalWrite(K3, true);
            //Rollo1statstart = Rollo1State;
        }
        

    break;

    case Rollo2_down:
        if (millis() - Rollo_Timer_2 >= Rollo_time2 || BTN4stop==true){        //Rolladen 1 ZU
            digitalWrite(K4, false);
            Rollo2State = Rollo2Stateold - map(millis() - Rollo_Timer_2, 0, RolloDriveTime2, 0, 100);
            Rollo2Stateold = Rollo2State;
            Serial.print("Motor stopped new status: ");
            Serial.print(Rollo2State);
            Serial.println("%");
            State_Rollo_2 = MovementIDLE;
           
        }
        else if (millis() - State_Time >= 2000){
            Cur2State = Rollo2Stateold - map(millis() - Rollo_Timer_2, 0, RolloDriveTime2, 0, 100);
            Serial.print("going DOWN, current Val: ");
            Serial.println((Cur2State));
            State_Time = millis();
        }
        else if(Rollo2State != Rollo2statstart){
            Serial.print("Motor stopped because of statechange at current Val: ");
            if (Rollo2State>=Rollo2Stateold){
                digitalWrite(K4, false);
                State_Rollo_2 = Wait;
                LastState2 = Rollo2_down;
                Wait_Timer_2 = millis();
                Rollo2Stateold = Rollo2Stateold - map(millis() - Rollo_Timer_2, 0, RolloDriveTime2, 0, 100);
                Rollo_time2 = map((Rollo2State - Rollo2Stateold), 0, 100, 0, RolloDriveTime2);
                Rollo_Timer_2 = millis();
                Serial.println(Rollo2Stateold);
            }
            else{
                State_Rollo_2 = MovementIDLE;
                Rollo2Stateold = Rollo2Stateold - map(millis() - Rollo_Timer_2, 0, RolloDriveTime2, 0, 100);
            }
        }
        else{
            digitalWrite(K4, true);
            //Rollo1statstart = Rollo1State;
        }

    break;


    case Wait:
        if (millis() - Wait_Timer_2 >= WaitTime){
            if(LastState2==Rollo2_up){
               State_Rollo_2 = Rollo2_down;
               Rollo1statstart=Rollo1State; 
            }
            else{
                State_Rollo_2 = Rollo2_up;
                Rollo1statstart=Rollo1State;
            }
        }
    break;
    
    default:
    break;
    
    }
    return return_b;
} 
  
//--------------------------------------------------

