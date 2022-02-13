#include <Arduino.h>
#include <EasyButton.h>
#include <CAN.h> 
#include <Pinout.h>
#include <Functions.cpp>

TaskHandle_t Task1;                         //Hauptschleife Kern 0
TaskHandle_t Task2;                         //Hauptschleife Kern 1


#define Addr            31        // CAN Adresse HEX
#define Rollo1time      200       // in zehntel Sekunden
#define Rollo2time      300       // in zehntel Sekunden

void BTN1onPressed();
void BTN2onPressed();
void BTN3onPressed();
void BTN4onPressed();
void Task1code(void * pvParameters);
void Task2code(void * pvParameters);
void CANcalc();

EasyButton BTN1(S1, 150); 
EasyButton BTN2(S2, 150); 
EasyButton BTN3(S3, 150); 
EasyButton BTN4(S4, 150); 

int CANread1[8]={0,0,0,0,0,0,0,0};
int CANread2[8]={0,0,0,0,0,0,0,0};
int CANold1[8]={0,0,0,0,0,0,0,0};
int CANold2[8]={0,0,0,0,0,0,0,0};

bool BTN1stop, BTN2stop, BTN3stop, BTN4stop;
int Bytecount, Rollo1state, Rollo2state, Rollo1stateCAN = 0, Rollo2stateCAN = 0, Cur1StateCAN = 0, Cur2StateCAN = 0;
bool running1=false, running2=false, waitthenUP1=false, waitthenDOWN1=false, waitthenUP2=false, waitthenDOWN2=false;
int Rollo1CAN, Rollo1CANold, Rollo2CAN, Rollo2CANold;




void setup() {
  

  CAN.setPins(canRX, canTX);                                                                                  //CAN Initialisieren
    if(!CAN.begin(125E3)){
      Serial.println("Start CAN Failed");                                                               //Abbruch bei CAN Fehler
      while (1); //Freeze
    }
  //InitCan();

  Serial.begin(115200);
  delay(100);


  pinMode(K1, OUTPUT);
  pinMode(K2, OUTPUT);
  pinMode(K3, OUTPUT);
  pinMode(K4, OUTPUT);
  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);
  //pinMode(S4, INPUT_PULLUP);

  digitalWrite(K1, false);
  digitalWrite(K2, false);
  digitalWrite(K3, false);
  digitalWrite(K4, false);
  
  BTN1.onPressed(BTN1onPressed);
  BTN2.onPressed(BTN2onPressed);
  BTN3.onPressed(BTN3onPressed);
  BTN4.onPressed(BTN4onPressed);
  BTN1.begin();
  BTN2.begin();
  BTN3.begin();
  BTN4.begin();

  Serial.println("booted");
  disableCore0WDT();
  disableCore1WDT();
  delay(1000); 

  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500); 

xTaskCreatePinnedToCore(
                    Task2code,   /* Task function. */
                    "Task2",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
    delay(500); 
}

void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
 //--------------------------------------------- Loop auf CPU Kern 0 ---------------------------------------------------------
 //Eigentliches Programm, CAN Kommunikation aus Timinggründen auf Kern 1 ausgelagert
  BTN1.read();
  BTN2.read();
  BTN3.read();
  BTN4.read();
  BTN1stop=BTN1.pressedFor(800);
  BTN2stop=BTN2.pressedFor(800);
  BTN3stop=BTN3.pressedFor(800);
  BTN4stop=BTN4.pressedFor(800);
  RolloMovement1();
  RolloMovement2();
  CANcalc();



  
    

  }
  }

void Task2code( void * pvParameters ){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
 //--------------------------------------------- Loop auf CPU Kern 1 ---------------------------------------------------------


    if (Rollo1Stateold != Rollo1stateCAN || Rollo2Stateold != Rollo2stateCAN || Cur1State != Cur1StateCAN || Cur2State != Cur2StateCAN){
        Serial.print("Sending packet ... ");
//------------Rollo1 SEND-------------------------
        CAN.beginPacket(Addr);
        if(Cur1State != Cur1StateCAN){
          CAN.write(Cur1State);
        }
        else{
          CAN.write(Rollo1Stateold);      
        }
        CAN.write(Rollo1CAN);
        CAN.endPacket();

//------------Rollo2 Send--------------------------
        
        CAN.beginPacket(Addr+1);
        if(Cur2State != Cur2StateCAN){
          CAN.write(Cur2State);
        }
        else{
          CAN.write(Rollo2Stateold);
        
        }
        CAN.write(Rollo2CAN);
        CAN.endPacket();

        Rollo1stateCAN = Rollo1Stateold;
        Rollo2stateCAN = Rollo2Stateold;
        Cur1StateCAN = Cur1State;
        Cur2StateCAN = Cur2State;
        Serial.println("done!");
    }

      int packetSize = CAN.parsePacket();                                     //Empfangen von CAN BUS
      Bytecount=0;

          if (packetSize) {
            // received a packet
            Serial.print("Received packet with id 0x");
            Serial.println(CAN.packetId(), HEX);

            if(CAN.packetId()== Addr){
                Serial.println("stored as self Addr");
                while (CAN.available()) {
                CANread1[Bytecount]=(int)CAN.read();
                Serial.println(CANread1[Bytecount]);
                Bytecount++;
                }
                Serial.println("");
              }

            if(CAN.packetId()== Addr+1){
                Serial.println("stored as self Addr");
                while (CAN.available()) {
                CANread2[Bytecount]=(int)CAN.read();
                Serial.println(CANread2[Bytecount]);
                Bytecount++;
                }
                Serial.println("");
              }

            else{
                Serial.print("Message Ignored: ");
                while (CAN.available()) {
                Serial.print((int)CAN.read());
                }
                Serial.println();                
            }
          }
              
    Rollo1CAN = CANread1[1];
    Rollo2CAN = CANread2[1];

  }
  }




void loop() {
}



void BTN1onPressed(){
  Serial.println("BTN1 pressed");
  if(digitalRead(K2)==false && digitalRead(K1)==false && BTN1stop==false)
  {
    Rollo1State=100;
  }
  
  else if(digitalRead(K1)==true){
    Rollo1State = Rollo1Stateold + map(millis() - Rollo_Timer_1, 0, RolloDriveTime1, 0, 100);
  }

}

void BTN2onPressed(){
  Serial.println("BTN2 pressed");
  if(digitalRead(K1)==false && digitalRead(K2)==false && BTN2stop==false)
  {
    Rollo1State=0;
  }

  else if(digitalRead(K2)==true){
    Rollo1State = Rollo1Stateold + map(millis() - Rollo_Timer_1, 0, RolloDriveTime1, 0, 100);
  }
}


//  ------------------------------------ Copy from BTN1 + 2 when working ------------------
void BTN3onPressed(){
  Serial.println("BTN3 pressed");
  if(digitalRead(K4)==false && digitalRead(K3)==false && BTN3stop==false)
  {
    Rollo2State=100;
  }

  else if(digitalRead(K3)==true){
     Rollo2State = Rollo2Stateold + map(millis() - Rollo_Timer_2, 0, RolloDriveTime2, 0, 100);
  }
}

void BTN4onPressed(){
  Serial.println("BTN4 pressed");
  if(digitalRead(K3)==false && digitalRead(K3)==false && BTN4stop==false)
  {
    Rollo2State=0;
  }

  else if(digitalRead(K4)==true && digitalRead(K3)==false){
      Rollo2State = Rollo2Stateold + map(millis() - Rollo_Timer_2, 0, RolloDriveTime2, 0, 100);
  }
}





void CANcalc(){
  if (CANread1[1]!= CANold1[1]){
    Rollo1State = CANread1[1];
    CANold1[1] = CANread1[1];
  }
  if (CANread2[1]!= CANold2[1]){
    Rollo2State = CANread2[1];
    CANold2[1] = CANread2[1];
  }
}