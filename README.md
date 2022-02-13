# 4wayIO-CAN-Modbus
4x 230V Switchinput 4x Relay Output (250V/5A) CAN Bus or Modbus SmartHome Project

This projects has started because i wanted to make my windowblinds smart. I allready got a RaspberryPi with IOBroker running, wich interfaces my heating control via CAN Bus. 
I designed this PCB to go on the same bus, but added an additional RS485 Transceiver to also be able to interface Modbus.
Because i dont wanted to change my existing wiring the pcb got 4x 230V switchinputs, so you can wire your switches directly to the pcb. In the software you can choose between simple IO extention - switch 1 changes relaystate 1, switch 2 changes relaystate 2 etc.. - or to use it as a windowblind actor. In this case 2 Relays and 2 switch input are grouped together. Relay 1 (blind 1 UP) and Relay2 (blind 1 DOWN) will not opperate simultainusly! For use as a windowblind-actor there is an additional current transformer footprint on the pcb. With its help you dont need to specify the time your Blind needs to travel a full way, it is messured and corrected if it changes.

The brain of the pcb is an ESP32 wroom Module. So it is possible to use WIFI instead of CAN or Modbus, since this is not my intended usecase feel free to just use the pcb-Layout and write the code by yourself :). 
As the CAN transceiver i used a way to small HVSON8 footprint TJA1057GTK/3 it can directly interface with an 3.3V microcontroller. For Modbus a MAX22028FAWA+ IC from the manufacturer Maxim Integrated is used, it is an isolateted RS485 transceiver and fully covers the Modbus specification.
The pcb is desigend in KiCAD, the code written in VScode with platformio.

Currently this Projekt is work in progress and the code is not perfect, consider it as proof of concept ;) because there are currently no RS485 transceivers available the code is only written for CAN Bus. Control Board connector is for future extension of the user interface.
