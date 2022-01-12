# 4wayIO-CAN-Modbus
4 230V Input 4 Relay Output CAN Bus Modbus SmartHome Project

This projects has started because i wanted to make my windowblinds smart. I allready got a RaspberryPi with IOBroker running, wich interfaces my heating control via CAN Bus. 
I designed this PCB to go on the same bus, but added a RS485 Transceiver to also be able to interface the Modbus standard.
Because i dont wanted to change my existing wiring the PCB got 4 230V inputs, so you can wire your switches directly to the pcb. In the software you can choose between simple IO extention - switch 1 changes relaystate 1, switch 2 changes relaystate 2 - or to use it as a windowblind actor. In this case 2 Relays and 2 switch input are grouped together. Relay 1 (blind 1 UP) and Relay2 (blind 1 DOWN) will not opperate simultainusly!
The brian of the PCB is an ESP32 wroom Module. So it is possible to use WIFI instead of CAN or Modbus, since this is not my intended usecase feel free to just use the PCB-LAyout and write the code by yourself.
For the CAN transceiver i used a way to small HVSON8 footprint TJA1057GTK/3 it can directly interface with an 3.3V microcontroller. For Modbus i used MAX22028FAWA+ IC from the manufacturer Maxim Integrated, it is an isolateted RS485 transceiver and fully covers the Modbus specification.
